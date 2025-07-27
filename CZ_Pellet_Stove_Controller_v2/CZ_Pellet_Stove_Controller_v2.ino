//------------------------------------------------------------------------------------------------
// CZ Pellet Stove Controller v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Tested up to Espressif ESP32 v2.0.14 due to LilyGo requirements
//
// This code requires the LilyGo T-Display-S3 ESP32 board. You will first need to properly set up
// your Arduino IDE to upload to that board. See: https://github.com/Xinyuan-LilyGO/T-Display-S3
// Make sure that you only install the Espressif ESP32 v2.0.14 board library since the 3.x stuff
// is not backward compatible and will throw syntax errors all over the place. <Nice fkin upgrade>
//
// This project is an easy drop-in replacement for any common pellet stove controller that brings
// WiFi connectivity to it and eliminates the need for an add-on thermostat. Aside from the stove
// now having a web UI that you can control from any mobile phone or desktop/laptop, a web API is
// also included. This allows Climate Czar Server and most any home automation software to run it.
//
// Best of all, this controller can be built for well under $100 which beats the replacement cost
// of ANY factory built controller. Even the controller for my basic England's Stove Works pellet
// stove is $299 and has no WiFi connectivity. This inflated price shouldn't comes as a surprise,
// they call it a "specialized device", even though they all work EXACTLY the same way. <facepalm>
//
// Unlike the vast majority of pellet stove controllers, this one doesn't use a cheap thermistor
// to read the stove body temperature. Thermistors are inaccurate and their accuracy changes over
// time as they age. This controller uses a digital infrared non-contact temperature sensor which
// will still have the same accuracy years from now as it has today.
//
// The optional built-in thermostat also uses a digital temperature sensor (a DS18B20) and can be
// placed up to 50 feet away, so long as unshielded twisted pair wire is used (Cat 5+ ethernet).
// This goes against their documented standards, but I use these in my greenhouse and there is no
// need for line drivers. Just run the data line and ground on a pair and run the +5v on another.
//
// As with the previous Raspberry Pi Zero based CZ Pellet Stove Controller, this version also has
// power loss recovery. If the power is lost while the stove is running, it will attempt to start
// up the stove from scratch when the power is restored. This version doesn't use SQLite to store
// settings and other data, which eliminates the possibility of data corruption if power is lost.
//
// Theory of Operation:
//
// The pellet stove starts up with a countdown timer running where constant power is sent to the
// combustion blower, bottom auger, and ignitor. The pellet feed rate is set to high burn mode
// in the startup phase. The stove body temperature is monitored during this countdown to check
// for a minimum operating temperature. The ignitor is diabled when that temperature is met and
// the room air blower is activated. The pellet feed rate then falls back to idle mode.
//
// The pellet feed operation is fairly simple. Your bottom auger spins all the time during a run.
// The top auger feeds pellets from the hopper into the bottom auger feed tube every 15 seconds.
// The top auger pulse time has two different durations for idle mode and high burn mode.
//
// If the stove body temperature fails to reach the minimum operating temperature in time, this
// initiates the stove shutdown procedure. This disables the top auger feed timer. The power to
// the combustion blower, bottom auger, and room air blower is left on for twice the length of
// the startup timer to make sure that all pellets and coals have completely burned out.
//
// The countdown timer for the shutdown process cannot be interrupted from the web UI or using
// the buttons on the ESP32. The only way to interrupt this is to make a web API call to reset
// the pellet stove controller. Complicating this is intentional so a person can't accidentally
// cause a burn-back situation and ignite the pellets in the hopper.
//------------------------------------------------------------------------------------------------
//sqlite3 $DB "CREATE TABLE Stats (ID INTEGER PRIMARY KEY,Temperature FLOAT,Probe1 FLOAT,Probe2 FLOAT,Probe3 FLOAT,Probe4 FLOAT,Probe5 FLOAT,Probe6 FLOAT,Probe7 FLOAT,Probe8 FLOAT,Combustion INT,RoomBlower INT,Ignitor INT,OpMode INT,StoveOn INT,HighBurn INT,CountDown INT,Restart INT,LowTempFail TEXT,ShutDownTime TEXT)"
//sqlite3 $DB "INSERT INTO Stats (Temperature,Probe1,Probe2,Probe3,Probe4,Probe5,Probe6,Probe7,Probe8,Combustion,RoomBlower,Ignitor,OpMode,StoveOn,HighBurn,CountDown,Restart,LowTempFail,ShutDownTime) VALUES (32.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'','')"

//sqlite3 $DB "CREATE TABLE Settings (ID INTEGER PRIMARY KEY,Deadline INT,OpTemp INT,MaxTemp INT,HighFeed FLOAT,LowFeed FLOAT,CookTemp INT)"
//sqlite3 $DB "INSERT INTO Settings (Deadline,OpTemp,MaxTemp,HighFeed,LowFeed,CookTemp) VALUES (1500,90,400,4.5,1.6,225)"
//------------------------------------------------------------------------------------------------
#include "WiFi.h"                // WiFi network connectivity library
#include "ESP32Ping.h"           // ICMP (ping) library from https://github.com/marian-craciunescu/ESP32Ping
#include "Arduino_GFX_Library.h" // Standard GFX library for Arduino, built with version 1.6.0
#include "FreeSans9pt7b.h"       // https://github.com/moononournation/ArduinoFreeFontFile.git 
#include "Preferences.h"         // ESP32 Flash memory read/write library
#include "Wire.h"                // I2C communications library
#include "Adafruit_MLX90614.h"   // MLX90614 non-contact temperature sensor library
#include "OneWire.h"             // OneWire Network communications library
#include "DallasTemperature.h"   // Dallas Semiconductor DS18B20 temperature sensor library
//------------------------------------------------------------------------------------------------
#define ONE_WIRE 13              // 1-Wire network pin for the DS18B20 temperature sensor
#define SCL 12                   // I2C clock pin
#define SDA 11                   // I2C data pin
#define SCREEN_BACKLIGHT 38      // Screen backlight LED pin
#define SCREEN_POWER_ON 15       // Screen power on/off pin
#define START_BTN 0              // Stove start/stop button (press and hold for 5 seconds)
#define BURN_BTN 14              // High burn mode toggle button
#define TOP_AUGER 44             // Output to the top auger SSR
#define BOTTOM_AUGER 18          // Output to the bottom auger SSR
#define COMBUSTION_BLOWER 17     // Output to the combustion blower SSR
#define ROOM_BLOWER 21           // Output to the room air blower SSR
#define IGNITOR 16               // Output to the ignitor SSR
#define HIGH_BURN 1              // Optional external high burn mode toggle pin (ground to activate)
#define FAULT 2                  // Optional vacuum switch fault detection pin (ground to activate)
//------------------------------------------------------------------------------------------------
Arduino_DataBus *bus = new Arduino_ESP32PAR8Q(7 /* DC */, 6 /* CS */, 8 /* WR */, 9 /* RD */,39 /* D0 */, 40 /* D1 */, 41 /* D2 */, 42 /* D3 */, 45 /* D4 */, 46 /* D5 */, 47 /* D6 */, 48 /* D7 */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 5 /* RST */, 0 /* rotation */, true /* IPS */, 170 /* width */, 320 /* height */, 35 /* col offset 1 */, 0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */);
Arduino_Canvas_Indexed *canvas = new Arduino_Canvas_Indexed(320 /* width */, 170 /* height */, gfx);
OneWire oneWire(ONE_WIRE);
DallasTemperature DT(&oneWire);
WiFiServer Server(80);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Preferences preferences;
hw_timer_t *timer = NULL;
//------------------------------------------------------------------------------------------------
bool FaultState = false;         // True if a critical fault is detected
bool UpToTemp = false;           // True if the run startup has reached operating temperature
bool UseThermostat = true;       // Use the internal thermostat routines Y/N
byte TemperatureMode = 0;        // 0=Fahrenheit, 1=Celcius
byte OpMode = 0;                 // 0=Off, 1=Startup, 2=Running, 3=Shutdown, 4=TempFail, 5=Fault
byte wifiCheckCounter = 0;       // Used to check the WiFi connection once per minute
byte wifiMode = 0;               // DHCP (0) or manual configuration (1)
long LoopCounter = 0;            // Timekeeper for the loop to eliminate the need to delay it
long StartTime = 0;              // Start time of the current stove run
float feedRateLow = 1.6;         // Top auger feed time in seconds (idle mode)
float feedRateHigh = 4.5;        // Top auger feed time in seconds (high burn mode)
float roomTempC = 0.0;           // Room temperature in Celcius
float roomTempF = 0.0;           // Room temperature in Fahrenheit
float stoveTempC = 0.0;          // Stove body temperature in Celcius
float stoveTempF = 0.0;          // Stove body temperature in Fahrenheit
float targetTempF = 0.0;         // Thermostat target temperature in Celcius
float targetTempC = 0.0;         // Thermostat target temperature in Fahrenheit

String DeviceName = "";          // Network host name and device name to be displayed in the web UI
String Runtime = "00:00:00";     // Current stove runtime
String Uptime = "00:00:00";      // Current system uptime
String Version = "2.0.1";        // Current release version of the project
String wifiSSID = "none";        // WiFi network SSID
String wifiPassword;             // WiFi network password
String wifiIP;                   // WiFi network IPV4 address
String wifiMask;                 // WiFi network subnet mask
String wifiGateway;              // WiFi network default gateway address
String wifiDNS;                  // WiFi network DNS resolver address
//------------------------------------------------------------------------------------------------
// Top auger interrupt driven timer variables
volatile bool augerState = false;
volatile uint32_t cycleCount = 0;
const uint32_t CYCLE_PERIOD = 15000;
uint32_t FEED_TIME = feedRateLow * 1000;
//------------------------------------------------------------------------------------------------
void IRAM_ATTR onTimer() { // Interrupt driven top auger timer (15 second wide PWM)
  cycleCount += 100;  // Increment by 100ms
  
  if (cycleCount >= CYCLE_PERIOD) {
    augerState = true;
    digitalWrite(TOP_AUGER,HIGH);  // Turn top auger ON
    cycleCount = 0;  // Reset cycle
  } else if (cycleCount >= FEED_TIME && augerState) {
    augerState = false;
    digitalWrite(TOP_AUGER,LOW);   // Turn top auger OFF
  }
}
//------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(1000);
  //PurgeBuffer();
  if (Serial) Serial.println("");

  // Get the last user settings from flash memory
  GetMemory();
  if (wifiSSID == "none") {
    SetMemory();
  } else {
    //if ((wifiSSID != "none") && (wifiPassword != "")) ConnectWiFi();
  }

  // Initialize the GPIO pins
  pinMode(START_BTN,INPUT_PULLUP);
  pinMode(BURN_BTN,INPUT_PULLUP);
  pinMode(HIGH_BURN,INPUT_PULLUP);
  pinMode(FAULT,INPUT_PULLUP);
  pinMode(TOP_AUGER,OUTPUT); digitalWrite(TOP_AUGER,LOW);
  pinMode(BOTTOM_AUGER,OUTPUT); digitalWrite(TOP_AUGER,LOW);
  pinMode(COMBUSTION_BLOWER,OUTPUT); digitalWrite(COMBUSTION_BLOWER,LOW);
  pinMode(ROOM_BLOWER,OUTPUT); digitalWrite(ROOM_BLOWER,LOW);
  pinMode(IGNITOR,OUTPUT); digitalWrite(IGNITOR,LOW);

  // Configure the top auger low speed PWM
  timer = timerBegin(0,80,true);  // Prescaler 80 (1µs per tick)
  timerAttachInterrupt(timer,&onTimer,true);
  timerAlarmWrite(timer,100000,true);  // Trigger every 100ms (100,000µs)

  // Enable top auger PWM
  //timerAlarmEnable(timer);

  // Disable top auger PWM
  //timerAlarmDisable(timer);
  //digitalWrite(TOP_AUGER,LOW);

  // Initialize the OneWire and I2C buses
  DT.begin();
  Wire.begin(SDA,SCL);

  // Power up the screen and backlight
  pinMode(SCREEN_POWER_ON,OUTPUT);
  digitalWrite(SCREEN_POWER_ON,HIGH);
  ledcSetup(0,2000,8);
  ledcAttachPin(SCREEN_BACKLIGHT,0);
  ledcWrite(0,255); // Screen brightness (0-255)

  // Initialize the graphics library and blank the screen
  gfx->begin();
  gfx->setRotation(3);
  gfx->fillScreen(BLACK);

  // In order to eliminate screen flicker, everything is plotted to an off-screen buffer and popped onto the screen when done
  canvas->begin();
  ScreenUpdate();

  if (! mlx.begin()) {
    Serial.println(F("Error initializing the stove body temperature sensor"));
    FaultState = true;
  };

  LoopCounter = millis();
}
//------------------------------------------------------------------------------------------------
void ConnectWiFi() { // Connect to WiFi network, must be WPA2-PSK, not WPA3
  byte x = 0;
  if (Server) Server.end();
  WiFi.mode(WIFI_STA);
  if (wifiMode == 1) {
    bool Passed = true;
    int segCount = 0;
    int ipSegments[4];
    int maskSegments[4];
    int gwSegments[4];
    int dnsSegments[4];
    segCount = sscanf(wifiIP.c_str(),"%d.%d.%d.%d",&ipSegments[0],&ipSegments[1],&ipSegments[2],&ipSegments[3]);
    if (segCount != 4) {
      if (Serial) Serial.println(F("\nCannot parse static IP address!"));
      Passed = false;
    }
    segCount = sscanf(wifiMask.c_str(),"%d.%d.%d.%d",&maskSegments[0],&maskSegments[1],&maskSegments[2],&maskSegments[3]);
    if (segCount != 4) {
      if (Serial) Serial.println(F("\nCannot parse subnet mask!"));
      Passed = false;
    }
    segCount = sscanf(wifiGateway.c_str(),"%d.%d.%d.%d",&gwSegments[0],&gwSegments[1],&gwSegments[2],&gwSegments[3]);
    if (segCount != 4) {
      if (Serial) Serial.println(F("\nCannot parse gateway address!"));
      Passed = false;
    }
    segCount = sscanf(wifiDNS.c_str(),"%d.%d.%d.%d",&dnsSegments[0],&dnsSegments[1],&dnsSegments[2],&dnsSegments[3]);
    if (segCount != 4) {
      if (Serial) Serial.println(F("\nCannot parse DNS resolver address!"));
      Passed = false;
    }
    if (Passed) {
      IPAddress staticIP(ipSegments[0],ipSegments[1],ipSegments[2],ipSegments[3]);
      IPAddress subnet(maskSegments[0],maskSegments[1],maskSegments[2],maskSegments[3]);
      IPAddress gateway(gwSegments[0],gwSegments[1],gwSegments[2],gwSegments[3]);
      IPAddress dns(dnsSegments[0],dnsSegments[1],dnsSegments[2],dnsSegments[3]);
      if (! WiFi.config(staticIP,gateway,subnet,dns,dns)) {
        if (Serial) Serial.println(F("\nWiFi static IP configuration failed!"));
        delay(2000);
      }
    } else {
      delay(2000);
    }
  }
  WiFi.setHostname(DeviceName.c_str());
  WiFi.begin(wifiSSID,wifiPassword);
  if (Serial) Serial.print(F("\nConnecting to WiFi .."));
  while (WiFi.status() != WL_CONNECTED) {
    if (Serial) Serial.print('.');
    delay(1000);
    x ++;
    if (x == 15) break;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Server.begin();
    wifiIP = WiFi.localIP().toString();
    wifiMask = WiFi.subnetMask().toString();
    wifiGateway = WiFi.gatewayIP().toString();
    wifiDNS = WiFi.dnsIP(0).toString();
  } else {
    wifiIP = "";
    wifiMask = "";
    wifiGateway = "";
    wifiDNS = "";
    if (Serial) Serial.println(F("\nConnection Failed!"));
    delay(2000);
  }
}
//------------------------------------------------------------------------------------------------
void GetMemory() { // Get the configuration settings from flash memory on startup
  /*
  preferences.begin("prefs",true);
  wifiMode         = preferences.getUInt("wifi_mode",0);
  wifiSSID         = preferences.getString("wifi_ssid","none");
  wifiPassword     = preferences.getString("wifi_password","");
  wifiIP           = preferences.getString("wifi_ip","");
  wifiMask         = preferences.getString("wifi_mask","");
  wifiGateway      = preferences.getString("wifi_gateway","");
  wifiDNS          = preferences.getString("wifi_dns","");
  DeviceName       = preferences.getString("device_name","Boilermaker-" + WiFi.macAddress());
  OpMode           = preferences.getUInt("op_mode",1);
  TargetTemp       = preferences.getFloat("target_temp",80.0);
  preferences.end();
  */
}
//------------------------------------------------------------------------------------------------
void SetMemory() { // Update flash memory with the current configuration settings
  /*
  preferences.begin("prefs",false);
  preferences.putUInt("wifi_mode",wifiMode);
  preferences.putString("wifi_ssid",wifiSSID);
  preferences.putString("wifi_password",wifiPassword);
  preferences.putString("wifi_ip",wifiIP);
  preferences.putString("wifi_mask",wifiMask);
  preferences.putString("wifi_gateway",wifiGateway);
  preferences.putString("wifi_dns",wifiDNS);
  preferences.putString("device_name",DeviceName);
  preferences.putUInt("op_mode",OpMode);
  preferences.putUInt("op_mode",OpMode);
  preferences.end();
  */
}
//------------------------------------------------------------------------------------------------
void GetStoveTemp() { // Gets the stove body temperature
  stoveTempC = mlx.readObjectTempC();
  stoveTempF = mlx.readObjectTempF();
}
//------------------------------------------------------------------------------------------------
void GetRoomTemp () { // Gets the room temperature if using the built-in thermostat
  DT.requestTemperatures();
  float Test = DT.getTempCByIndex(0); // Returns -127.00 if the device reading is missed
  if (Test > -127.00) {
    roomTempC = Test;
    roomTempF = DT.getTempFByIndex(0);
  }
}
//------------------------------------------------------------------------------------------------
void ScreenUpdate() { // Update the LCD screen

}
//------------------------------------------------------------------------------------------------
void loop() {
  long CurrentTime = millis();
  //Uptime = formatMillis(CurrentTime);
  wifiCheckCounter ++;
  if (CurrentTime > 4200000000) {
    // Reboot the system if we're reaching the maximum long integer value of CurrentTime (49 days)
    ESP.restart();
  }

  // Check the external control GPIO pins
  if (digitalRead(FAULT) == 0) FaultState = true;
  if (digitalRead(HIGH_BURN) == 0) FEED_TIME = feedRatehigh * 1000;

  // Check for button presses

  // Check for HTTP API calls and handle as necessary
  WiFiClient Client = Server.available();
  if (Client) {
    CurrentTime = millis(); // Grab this again in case we just processed button presses
    long PreviousTime = CurrentTime;
    String Header = "";
    while (Client.connected() && CurrentTime - PreviousTime <= 2000) { // 2 second connection timeout
      CurrentTime = millis();
      if (Client.available()) {
        char c = Client.read();
        if ((c != '\r') && (c != '\n')) Header += c;
        if (c == '\n') {
          if (Header.indexOf("GET ") == 0) {
            //String Content = HandleAPI(Header);
            //Client.println(Content);
            //if (Content == "Rebooting...") { // Need to wait to reboot until after the response is sent
            //  Client.stop();
            //  delay(1000);
            //  ESP.restart();
            //}
            break;
          }
        }
      }
    }
    Client.stop();
  }

  // Check for serial console input and handle as necessary
  //if ((Serial) && (Serial.available())) HandleSerialInput();
  if (CurrentTime - LoopCounter >= 1000) {
    GetStoveTemp();
    GetRoomTemp();
    if (! FaultState) {


    }
    if (wifiCheckCounter >= 60) {
      bool PingTest = Ping.ping(wifiGateway.c_str(),2);
      if ((WiFi.status() != WL_CONNECTED) || (! PingTest)) { // Reconnect WiFi if we got dropped
        WiFi.disconnect();
        if ((wifiSSID != "none") && (wifiPassword != "")) {
          delay(500);
          ConnectWiFi();
        }
      }
      wifiCheckCounter = 0;
    }
    ScreenUpdate();
    LoopCounter = CurrentTime;
  }
}
//------------------------------------------------------------------------------------------------
/*
// Create & run a new sketch with the following code to fully erase the flash memory of an ESP32

#include <nvs_flash.h>

void setup() {
  nvs_flash_erase(); // erase the NVS partition and...
  nvs_flash_init();  // initialize the NVS partition.
  while(true);
}

void loop() {

}
*/
//------------------------------------------------------------------------------------------------
