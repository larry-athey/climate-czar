// Written by Larry Athey (Panhandle Ponics - https://3dgtower.com) v1.0.1 released June 18, 2023
//
// This is an external outdoor monitor to provide easy comparison of temperature and humidity to
// the environment in the greenhouse and provide an ambient light meter which can be used to turn
// lights on and off via Climate Czar. This is a regular task when you run a greenhouse all year
// long in a place that has wicked cold winters. For example, in my case, if it's above freezing
// at night, I don't have to run my pellet stove to keep my fish tanks warm. If it actually drops
// below zero, then I have a blast heater to deal with that keeps frost off the inner skin of the
// greenhouse which will migrate to plants. If Climate Czar knows what's happening outside, then
// it can take care of what needs to be done inside.
//
// This unit is based on the Xinyuan-LilyGO T-Display-S3 ESP32 powered development board that has
// a built-in 1.9 inch color LCD display. I had an extra one of these laying around and the little
// screen really wasn't of much use to me for any other projects. So I figured it would make for
// a nice outdoor thermometer if it was put in a weatherproof case. The code can be easily edited
// to work with any other ESP32 board without a display.
//
// While there may be some concern about the DHT-22 being exposed, This really isn't a problem if
// you mount it to the bottom of the enclosure away from the edges. A person could also put some
// form of open bottom shell around it to help protect it even more. If you have a 3D printer you
// can find plenty of shells on Thingiverse that you can print up for this purpose. Since I live
// in Nebraska, storms always come from the west. In my situation, I just mounted the unit to the
// east wall of the greenhouse and it's perfectly safe from rain and snow blowing into it.
//
// The ambient light sensor is a BH1750 which uses I2C communications, so everything used in this
// project works without any external components. The sensor just needs to be pointed downward so
// it's less affected by direct sunlight. The output to the screen and the web API is a 0 to 100
// value so it can be connected to Climate Czar without the need for a script. Just a simple curl
// command to pull any of the sensor readings. This value can be used to control supplemental
// grow lights in a greenhouse so you can maintain summertime light every day all year long.
//
// The web API for this unit uses the same call structure as the Climate Czar Combo Hub, but is
// very much reduced in size due to the limited number of sensors.
//------------------------------------------------------------------------------------------------
#include "Arduino_GFX_Library.h"
#include "FreeSans10pt7b.h"      // https://github.com/moononournation/ArduinoFreeFontFile.git 
#include "FreeSans60pt7b.h"      // https://github.com/moononournation/ArduinoFreeFontFile.git
#include "WiFi.h"
#include "ESP32Ping.h"           // https://github.com/marian-craciunescu/ESP32Ping.git
#include "DHTesp.h"
#include "Wire.h"
#include "BH1750.h"
//------------------------------------------------------------------------------------------------
#define OW_PIN 1 // This is only here because I have a pull-up resistor on GPIO 1 "just in case"
#define BH_PIN 2
#define DHT_PIN 3
#define PIN_LCD_BL 38
#define PIN_POWER_ON 15
//------------------------------------------------------------------------------------------------
WiFiServer Server(80);
DHTesp dhtSensor;
BH1750 lightMeter;
Arduino_DataBus *bus = new Arduino_ESP32LCD8(7 /* DC */, 6 /* CS */, 8 /* WR */, 9 /* RD */, 39 /* D0 */, 40 /* D1 */, 41 /* D2 */, 42 /* D3 */,
                                             45 /* D4 */, 46 /* D5 */, 47 /* D6 */, 48 /* D7 */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 5 /* RST */, 0 /* rotation */, true /* IPS */, 170 /* width */, 320 /* height */, 35 /* col offset 1 */,
                                      0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */);

// Comment out the following 4 lines if you want to use DHCP (see setup section as well)
IPAddress staticIP(10,20,30,161);
IPAddress gateway(10,20,30,254);
IPAddress subnet(255,255,255,0);
IPAddress dns(10,20,30,254);
//------------------------------------------------------------------------------------------------
const char* SSID = "Wokwi-GUEST"; // Your WiFi network name (replace Wokwi-GUEST)
const char* Password = ""; // Your WiFi password
// IP to ping every screen update to verify network connectivity and reboot the unit if no response.
const char* PingerIP = "10.20.30.254";

// Used for centering the large font readings on the screen
int ScreenWidth,ScreenCenter;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define web server timeout time in milliseconds (2 seconds)
const long timeoutTime = 2000;
// Used to force a restart every 21,600,000 loops (roughly every 6 hours without the LCD lag)
unsigned long Uptime = 0;
// Used to count loops between screen updates
unsigned long ScreenTimer = 0;
// Determines what reading the screen displays every 5 seconds
byte ScreenIndex = 1;
// Ping failure counter, reboot the device after 30 failures
byte PingFail = 0;
// Global placeholders for the DHT sensor readings
float Humidity = 0;
float TempC = 0;
float TempF = 0;
// Global placeholder for the light level reading (0%..100%)
float Light = 0;
// Variable to store the HTTP request
String Header;
//------------------------------------------------------------------------------------------------
void setup() {
  byte LoopCounter = 0;
  dhtSensor.setup(DHT_PIN,DHTesp::DHT22);
  Wire.begin(BH_PIN,43);
  lightMeter.begin();

  pinMode(PIN_POWER_ON,OUTPUT);
  digitalWrite(PIN_POWER_ON,HIGH);
  ledcSetup(0,2000,8);
  ledcAttachPin(PIN_LCD_BL,0);
  ledcWrite(0,255); // Screen brightness (0-255)

  gfx->begin();
  gfx->setRotation(1);
  gfx->fillScreen(BLACK);

  ScreenWidth  = gfx->width();
  ScreenCenter = round(ScreenWidth / 2);

  // Comment out the following conditional code block if you want to use DHCP
  if (! WiFi.config(staticIP,gateway,subnet,dns,dns)) {
    gfx->setFont(&FreeSans10pt7b);
    gfx->setCursor(0,25);
    gfx->setTextColor(RED);
    gfx->println("WiFi static IP\nconfiguration failed.");
    gfx->setCursor(0,135);
    gfx->println("Restarting...");
    delay(3000);
    ESP.restart();
  }

  gfx->setFont(&FreeSans10pt7b);
  gfx->setCursor(0,25);
  gfx->setTextColor(BLUE);
  gfx->print("Connecting to: ");
  gfx->println(SSID);
  WiFi.begin(SSID,Password);
  while (WiFi.status() != WL_CONNECTED) {
    gfx->print(".");
    delay(500);
    LoopCounter ++;
    if (LoopCounter == 60) {
      gfx->setTextColor(RED);
      gfx->setCursor(0,135);
      gfx->println("WiFi failed, restarting...");
      delay(3000);
      ESP.restart();
    }
  }

  gfx->setCursor(0,80);
  gfx->setTextColor(CYAN);
  gfx->println("Connected!");
  gfx->print("IP Address: ");
  gfx->println(WiFi.localIP());
  Server.begin();
}
//------------------------------------------------------------------------------------------------
void SetFG(float Temp) {
  if (Temp <= 32) {
    gfx->setTextColor(BLUE);
  } else if ((Temp > 32) && (Temp <= 58)) {
    gfx->setTextColor(CYAN);
  } else if ((Temp > 58) && (Temp <= 78)) {
    gfx->setTextColor(GREEN);
  } else if ((Temp > 78) && (Temp <= 85)) {
    gfx->setTextColor(YELLOW);
  } else if ((Temp > 88) && (Temp <= 95)) {
    gfx->setTextColor(MAGENTA);
  } else {
    gfx->setTextColor(RED);
  }
}
//------------------------------------------------------------------------------------------------
void ScreenUpdate() {
  int16_t nX = 0, nY = 0, TextX;
  uint16_t nWidth = 0, nHeight = 0;
  String strReading;
  gfx->fillScreen(BLACK);
  gfx->setFont(&FreeSans10pt7b);
  gfx->setCursor(0,25);
  if (ScreenIndex == 1) {
    SetFG(TempF);
    gfx->println("Temperature: Celcius");
    strReading = String(TempC,1);
  } else if (ScreenIndex == 2) {
    SetFG(TempF);
    gfx->println("Temperature: Farrenheit");
    strReading = String(TempF,1);
  } else if (ScreenIndex == 3) {
    SetFG(Humidity);
    gfx->println("Relative Humidity %");
    strReading = String(Humidity,1);
  } else if (ScreenIndex == 4) {
    SetFG(Light);
    gfx->println("Ambient Light Level %");
    strReading = String(Light,1);
  }
  gfx->setFont(&FreeSans60pt7b);
  gfx->getTextBounds(strReading,0,0,&nX,&nY,&nWidth,&nHeight);
  TextX = round(nWidth / 2);
  gfx->setCursor((ScreenCenter - TextX),125);
  gfx->println(strReading);

  gfx->setFont(&FreeSans10pt7b);
  gfx->setTextColor(LIGHTGREY);
  gfx->setCursor(0,155);
  gfx->print("WiFi Signal: ");
  gfx->print(WiFi.RSSI());

  bool Test = Ping.ping(PingerIP,1);
  if (! Test) {
    gfx->print(" (Not Pinging)");
    PingFail ++;
    if (PingFail == 30) ESP.restart();
  } else {
    gfx->print(" (Pinging)");
    PingFail = 0;
  }

  ScreenIndex ++;
  if (ScreenIndex > 4) ScreenIndex = 1;
}
//------------------------------------------------------------------------------------------------
String GetStats(String LineEnd) {
  String Stats = "";
  Stats += "System Uptime: " + String(millis() / 1000) + " seconds" + LineEnd;
  Stats += "WiFi Network: " + String(WiFi.SSID()) + LineEnd;
  Stats += "WiFi Channel: " + String(WiFi.channel()) + LineEnd;
  Stats += "WiFi Signal: " + String(WiFi.RSSI());
  return Stats;
}
//------------------------------------------------------------------------------------------------
void loop() {
  TempAndHumidity DHT = dhtSensor.getTempAndHumidity();
  Humidity = DHT.humidity;
  TempC = DHT.temperature;
  TempF = TempC * 9 / 5 + 32;
  float Lux = lightMeter.readLightLevel();
  Light = (100 / 65536) * Lux;

  if (ScreenTimer == 1000) {
    ScreenUpdate();
    ScreenTimer = 0;
  }

  WiFiClient Client = Server.available();
  if (Client) {
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";
    while (Client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();
      if (Client.available()) {
        char c = Client.read();
        Header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (Header.indexOf("GET /humidity") >= 0) { // DHT-22 humidity
              Client.println(String(Humidity,1));
            } else if (Header.indexOf("GET /temperature/c") >= 0) { // DHT-22 temperature in C
              Client.println(String(TempC,1));
            } else if (Header.indexOf("GET /temperature/f") >= 0) { // DHT-22 temperature in F
              Client.println(String(TempF,1));
            } else if (Header.indexOf("GET /light") >= 0) { // Ambient light level
              Client.println(String(Light,1));
            } else if (Header.indexOf("GET /restart") >= 0) { // Reboot the device
              Client.println("Restarting in 1 second");
              Client.stop();
              delay(1000);
              ESP.restart();
            } else if (Header.indexOf("GET /statshtml") >= 0) { // Get system stats with HTML line breaks
              Client.println(GetStats("<br>\n"));
            } else if (Header.indexOf("GET /stats") >= 0) { // Get system stats as raw text
              Client.println(GetStats("\n"));
            } else {
              Client.println("Unrecognized Request");
            }
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    Header = "";
    Client.stop();
  }
  delay(10);
  ScreenTimer ++;
  Uptime ++;
  if (Uptime == 21600000) ESP.restart();
}
//------------------------------------------------------------------------------------------------