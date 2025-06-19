//------------------------------------------------------------------------------------------------
// Climate Czar Combo Hub v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// You must be using a v2.x ESP32 library to compile this code. It appears that v3.x libraries do
// not contain compatible headers for certain legacy libraries that I rely on. You should also use
// the following URL in your preferences under Additional Boards Manager URLs.
//
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
//
// This project uses any 38 pin ESP32 WROOM-DA / WROOM-32
//
// If you are a user of Climate Czar Combo Hub 1.1.0 there is nothing to worry about, your units
// will still continue to work with Climate Czar Server perfectly fine. This version is a complete
// rewrite from the ground up and utilizes a completely different web API.
//
// The biggest change in this version is the addition of a master/slave topography where the slave
// units are controlled via LoRa WAN. Climate Czar Server still communicates with the master unit
// via TCP/IP and HTTP calls. However, when you make an API call to anything other than unit zero,
// the API call is sent to the slave as a LoRa WAN message and the result returns to the master as
// LoRa WAN message, which is then returned to Climate Czar Server via HTTP.
//
// http://ip-address/0/humidity (get the humidity from the master unit)
// http://ip-address/2/humidity (get the humidity from the first slave unit)
// http://ip-address/3/humidity (get the humidity from the second slave unit)
//
// Aside from the local temperature & humidity at the hub via DHT22, a local BH1750 ambient light
// sensor has been added as well. This allows you to trigger daylight extension lights, or monitor
// ambient light in an indoor grow operation to trigger an alarm if your have a grow light failure.
//
// A local OLED display has also been added where you can cycle through status screens using the
// GPIO zero button on the ESP32. Support for W5500 ethernet interfaces has also been added since
// most people prefer a hard-wired network connection over a WiFi-only connection.
//
// The unit still supports unlimited DS18B20 remote temperature sensors, 8 digital inputs (ground
// the pin to trigger), and now 16 digital outputs via MCP23017 for controlling solid state relays.
// What's more, you can now configure all hub settings using a serial terminal program over USB.
//
// Climate Czar Combo Hub is now a far more sophisticated device and much easier to implement in
// your greenhouse or indoor grow operation. Until a printed circuit board is available, they're a
// bit of a lesson in patience to build because of about 40 wires that need to be cut & soldered.
// Luckily, there's no complicated electronics work involved, it's just point-to-point wiring.
//------------------------------------------------------------------------------------------------
#include "Preferences.h"         // ESP32 Flash memory read/write library
#include "SPI.h"                 // SPI bus library
#include "Wire.h"                // I2C bus library
#include "Adafruit_SSD1306.h"    // SSD1306 OLED display library
#include "Adafruit_MCP23X17.h"   // MCP23017 I2C 16 port GPIO expansion module library
#include "DHTesp.h"              // DHT22 temperature/humidity sensor library
#include "BH1750.h"              // BH1750 light sensor library
#include "OneWire.h"             // OneWire Network communications library
#include "DallasTemperature.h"   // Dallas Semiconductor DS18B20 temperature sensor library
#include "UIPEthernet.h"         // Ethernet interface library compatible with W5500 modules
#include "WiFi.h"                // WiFi interface library
#include "ESP32Ping.h"           // ICMP (ping) library from https://github.com/marian-craciunescu/ESP32Ping
//------------------------------------------------------------------------------------------------
// Flash memory initialization
Preferences preferences;
// Server instances
WiFiServer wifiServer(80);
EthernetServer ethServer(80);
// SSD1306 configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,-1);
// I2C pins
#define SDA_PIN 21
#define SCL_PIN 22
// MCP23017 I2C address
#define MCP_ADDR 0x20
Adafruit_MCP23X17 mcp;
// DS18B20 (1-Wire)
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DT(&oneWire);
// DHT22
#define DHT_PIN 15
DHTesp dhtSensor;
// W5500 Ethernet (SPI)
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18
#define ETH_CS 5
// Status LED
#define LED 2
// Screen page button
#define BTN 0
// Inputs (digital switches)
const int inputPins[8] = {32,33,25,26,27,14,12,13};
//------------------------------------------------------------------------------------------------
bool spiStarted = false;         // Work-around since "if (SPI)" doesn't convert to a boolean value
bool ethConnected = false;       // Used for tracking the ethernet port connected status
byte ethMAC[6];                  // Placeholder for the ethernet interface MAC address
int ActiveMenu = 0;              // Used for tracking which serial config menu is in use
int ActivePage = 0;              // Used for tracking which OLED screen page to display
int CZ_pingFailures = 5;         // How many consecutive ping failures required for a reboot
int FlashInit = 0;               // Used to indicate whether the flash memory has been initialized
int LoRa_Mode = 0;               // LoRa operation mode 0=Master, 1=Slave
int LoRa_Network = 0;            // LoRa network ID 0-16 (zero is the public network)
int LoRa_Address = 1;            // LoRa network address 1-65535 (think of it like an IP address)
int Net_useDHCP = 1;             // Network use DHCP 0=False, 1=True
int Net_useWifi = 0;             // Network use WiFi 0=False, 1=True
int PingFailures = 0;            // Total number of watchdog host ping failures
long PingTimer = 0;              // Milliseconds since the last watchdog host ping test
long ScreenTimer = 0;            // Milliseconds since the last OLED screen update
String CZ_deviceName = "null";   // Network host name and LoRa device name
String CZ_Watchdog = "x.com";    // Watchdog host to ping to verify network connectivity
String Net_DNS = "";             // Network static DNS resolver
String Net_Gateway = "";         // Network static default gateway
String Net_IP = "";              // Network static IP address
String Net_Mask = "";            // Network static subnet mask
String Net_wifiSSID = "";        // Network WiFi SSID
String Net_wifiPW = "";          // Network WiFi Password
String Uptime = "00:00:00";      // Current system uptime
String Version = "2.0.1";        // Current release version of the project
//------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200,SERIAL_8N1,16,17);
  delay(500);
  GetMemory();
  if (FlashInit == 0) {
    SetMemory();
    GetMemory();
  }

  // Initialize inputs/outputs
  for (int i = 0; i <= 7; i ++) {
    pinMode(inputPins[i],INPUT_PULLUP); // Switch grounds the pin to register 1=true in the web API
  }                                     // 4.7K to 10K pullup resistors to +3.3 may still be needed
  pinMode(BTN,INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

  // Initialize MCP23017
  mcp.begin_I2C(MCP_ADDR);
  for (int i = 0; i <= 15; i ++) {
    mcp.pinMode(i,OUTPUT);
    mcp.digitalWrite(i,LOW);
  }

  // Inidialize I2C
  Wire.begin(SDA_PIN,SCL_PIN);

  // Initialize the DHT22
  dhtSensor.setup(DHT_PIN,DHTesp::DHT22);

  // Initialize SSD1306
  if (! display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDRESS)) {
    Serial.println(F("SSD1306 init failed!"));
  }

  // Initialize Dallas Temperature bus
  DT.begin();

  // Copy the WiFi MAC address for use with the W5500
  WiFi.macAddress(ethMAC);

  // Initialize the RYLR998 modem
  digitalWrite(LED,HIGH);
  Serial2.println(F("AT+RESET"));
  delay(500);
  Serial2.println("AT+NETWORKID=" + LoRa_Network);
  delay(100);
  Serial2.println("AT+ADDRESS=" + LoRa_Address);
  delay(100);
  digitalWrite(LED,LOW);

  // Connect to the network (TCP/IP)
  if (! StartNetwork()) {
    Serial.println(F("\r\nNo TCP/IP network available"));
  }

  // Splash screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("CZ Combo Hub v" + Version);
  display.println(CZ_deviceName);
  display.display();

  PingTimer = millis();
  ScreenTimer = PingTimer;
}
//------------------------------------------------------------------------------------------------
// External function includes are used here to reduce the overall size of the main sketch.
// Go ahead and call it non-standard, but I don't like spaghetti code that goes on forever.
#include "helpers.h"  // Inline function library for data validation/formatting helpers
//------------------------------------------------------------------------------------------------
void GetMemory() { // Get the configuration settings from flash memory on startup
  preferences.begin("prefs",true);
  CZ_deviceName    = preferences.getString("cz_devicename",sanitizeHostname(""));
  CZ_pingFailures  = preferences.getUInt("cz_pingfailures",5);
  CZ_Watchdog      = preferences.getString("cz_watchdog","x.com");

  FlashInit        = preferences.getUInt("flash_init",0);

  LoRa_Mode        = preferences.getUInt("lora_mode",0);
  LoRa_Network     = preferences.getUInt("lora_network",1);
  LoRa_Address     = preferences.getUInt("lora_address",1);

  Net_useWifi      = preferences.getUInt("net_usewifi",0);
  Net_wifiSSID     = preferences.getString("net_wifissid","");
  Net_wifiPW       = preferences.getString("net_wifipw","");

  Net_useDHCP      = preferences.getUInt("net_usedhcp",1);
  Net_IP           = preferences.getString("net_ip","0.0.0.0");
  Net_Mask         = preferences.getString("net_mask","255.255.255.0");
  Net_Gateway      = preferences.getString("net_gateway","0.0.0.0");
  Net_DNS          = preferences.getString("net_dns","0.0.0.0");
  preferences.end();
}
//------------------------------------------------------------------------------------------------
void SetMemory() { // Update flash memory with the current configuration settings
  preferences.begin("prefs",false);
  preferences.putString("cz_devicename",CZ_deviceName);
  preferences.putUInt("cz_pingfailures",CZ_pingFailures);
  preferences.putString("cz_watchdog",CZ_Watchdog);

  preferences.putUInt("flash_init",65535);

  preferences.putUInt("lora_mode",LoRa_Mode);
  preferences.putUInt("lora_network",LoRa_Network);
  preferences.putUInt("lora_address",LoRa_Address);

  preferences.putUInt("net_usewifi",Net_useWifi);
  preferences.putString("net_wifissid",Net_wifiSSID);
  preferences.putString("net_wifipw",Net_wifiPW);

  preferences.putUInt("net_usedhcp",Net_useDHCP);
  preferences.putString("net_ip",Net_IP);
  preferences.putString("net_mask",Net_Mask);
  preferences.putString("net_gateway",Net_Gateway);
  preferences.putString("net_dns",Net_DNS);
  preferences.end();
}
//------------------------------------------------------------------------------------------------
bool StartNetwork() {
  bool Result = true;
  IPAddress staticIP;
  IPAddress subnet;
  IPAddress gateway;
  IPAddress dns;

  if (wifiServer) wifiServer.end();
  if (ethServer) ethServer.end();

  if (Net_useDHCP == 0) {
    int ipSegments[4];
    int maskSegments[4];
    int gwSegments[4];
    int dnsSegments[4];
    sscanf(Net_IP.c_str(),"%d.%d.%d.%d",&ipSegments[0],&ipSegments[1],&ipSegments[2],&ipSegments[3]);
    sscanf(Net_Mask.c_str(),"%d.%d.%d.%d",&maskSegments[0],&maskSegments[1],&maskSegments[2],&maskSegments[3]);
    sscanf(Net_Gateway.c_str(),"%d.%d.%d.%d",&gwSegments[0],&gwSegments[1],&gwSegments[2],&gwSegments[3]);
    sscanf(Net_DNS.c_str(),"%d.%d.%d.%d",&dnsSegments[0],&dnsSegments[1],&dnsSegments[2],&dnsSegments[3]);
    staticIP = IPAddress(ipSegments[0],ipSegments[1],ipSegments[2],ipSegments[3]);
    subnet = IPAddress(maskSegments[0],maskSegments[1],maskSegments[2],maskSegments[3]);
    gateway = IPAddress(gwSegments[0],gwSegments[1],gwSegments[2],gwSegments[3]);
    dns = IPAddress(dnsSegments[0],dnsSegments[1],dnsSegments[2],dnsSegments[3]);
    if (Net_useWifi == 1) {
      WiFi.config(staticIP,gateway,subnet,dns,dns);
    }
  }

  if (Net_useWifi == 1) {
    WiFi.begin(Net_wifiSSID,Net_wifiPW);
    unsigned long startTime = millis();
    Serial.print("\nConnecting to " + Net_wifiSSID + ":");
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
      delay(500);
      Serial.print(F("."));
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print(F("\nWiFi connection failed"));
      Result = false;
    } else {
      Serial.print(F("\r\nWiFi connected"));
      Net_IP = WiFi.localIP().toString();
      Net_Mask = WiFi.subnetMask().toString();
      Net_Gateway = WiFi.gatewayIP().toString();
      Net_DNS = WiFi.dnsIP(0).toString();
      wifiServer.begin();
    }
    delay(1000);
  } else {
    if (spiStarted) {
      spiStarted = false;
      SPI.end();
    }
    if (! spiStarted) {
      spiStarted = true;
      SPI.begin(SPI_SCK,SPI_MISO,SPI_MOSI,ETH_CS);
    }
    Ethernet.init(ETH_CS);
    if (Ethernet.linkStatus() == LinkON) {
      ethConnected = true;
      if (Net_useDHCP == 0) {
        Ethernet.begin(ethMAC,staticIP,dns,gateway,subnet);
      } else {
        if (Ethernet.begin(ethMAC) == 0) {
          Result = false;
          ethConnected = false;
        }
      }
      if (ethConnected) {
        Serial.print(F("\nEthernet connected"));
        Net_IP = Ethernet.localIP().toString();
        Net_Mask = Ethernet.subnetMask().toString();
        Net_Gateway = Ethernet.gatewayIP().toString();
        Net_DNS = Ethernet.dnsServerIP().toString();
        ethServer.begin();
      }
    } else {
      Result = false;
      ethConnected = false;
      spiStarted = false;
      SPI.end();
    }
    if (! ethConnected) Serial.print(F("\nEthernet connection failed"));
    delay(1000);
  }
  return Result;
}
//------------------------------------------------------------------------------------------------
String GetDHT22(byte WhichOne) { // Get humidity and temperature from the DHT22
  TempAndHumidity DHT = dhtSensor.getTempAndHumidity();
  float T = DHT.temperature;
  float H = DHT.humidity;
  if (isnan(T)) T = 0;
  if (isnan(H)) H = 0;
  if (WhichOne == 0) {
    return String(H,1);
  } else if (WhichOne == 1) {
    return String(T,1);
  } else {
    return String(T * 9 / 5 + 32,1);
  }
}
//------------------------------------------------------------------------------------------------
String GetDS18B20(String Address, String Format) { // Get DS18B20 sensor temperature by address
  uint8_t Addr[8];
  if (stringToDeviceAddress(Address.c_str(),Addr)) {
    DT.requestTemperatures();
    oneWire.reset_search();
    float Temp = DT.getTempC(Addr);
    if (Temp == -127.0) Temp = 0.0;
    if (Format =="c") {
      return String(Temp,1);
    } else {
      return String(Temp * 9 / 5 + 32,1);
    }
  }
}
//------------------------------------------------------------------------------------------------
String GetBH1750(byte WhichOne) { // Get ambient light level from the BH1750
  BH1750 lightMeter(0x23);
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    delay(200);
    float lux = lightMeter.readLightLevel();
    if (lux < 0 || lux > 65535) {
      return "-1"; // Indicate sensor error
    }
    if (WhichOne == 0) {
      return String(lux,1);
    } else {
      //float light = .12 * lux;
      //if (light > 100) light = 100;
      float light = (lux / 54612.5) * 100;
      return String(light,1);
    }
  }
}
//------------------------------------------------------------------------------------------------
String GetRelay(byte WhichOne) { // Get the on/off status of one of the 16 relay ports
  WhichOne --;
  if (WhichOne > 15) return jsonFailure;
  return String(mcp.digitalRead(WhichOne));
}
//------------------------------------------------------------------------------------------------
String GetSwitch(byte WhichOne) { // Get the on/off status of one of the eight switch ports
  WhichOne --;
  if (WhichOne > 7) return jsonFailure;
  byte State = digitalRead(inputPins[WhichOne]);
  if (State == 0) {
    State = 1;
  } else {
    State = 0;
  }
  return String(State);
}
//------------------------------------------------------------------------------------------------
String SetRelay(byte WhichOne, byte State) { // Set one of the eight relays ports on or off
  WhichOne --;
  if (WhichOne > 15) return jsonFailure;
  mcp.digitalWrite(WhichOne,State);
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
void ScreenUpdate() {
  display.clearDisplay();
  display.setCursor(0,0);
  if (ActivePage == 0) {
    display.println(F("System Uptime"));
    display.println(Uptime);
  } else if (ActivePage == 1) {
    display.println(F("LoRa WAN Mode"));
    if (LoRa_Mode == 0) {
      display.println("Master - ID " + LoRa_Address);
    } else {
      display.println("Slave - ID " + LoRa_Address);
    }
  } else if (ActivePage == 2) {
    display.println("IP Address: " + Net_IP);
    if (Net_useWifi == 1) {
      display.println("Channel: " + String(WiFi.channel()) + " - " + "Signal: " + String(WiFi.RSSI()));
    } else {
      if (ethConnected) {
        display.println(F("Ethernet link is up"));
      } else {
        display.println(F("Ethernet link is down"));
      }
    }
  } else if (ActivePage == 3) {
    display.println(F("Local Temperature"));
    display.println(String(GetDHT22(1)) + "C, " + String(GetDHT22(2)) + "F");
  } else if (ActivePage == 4) {
    display.println(F("Local Humidity"));
    display.println(String(GetDHT22(0)) + "%");
  } else if (ActivePage == 5) {
    display.println(F("Local Light Level"));
    display.println(String(GetBH1750(0)) + " Lux, " + String(GetBH1750(1) + "%"));
  } else if (ActivePage == 6) {
    int deviceCount = DT.getDeviceCount();
    display.println(F("DS18B20 Temp Sensors"));
    display.println("Total Detected: " + deviceCount);
  }
  display.display();
}
//------------------------------------------------------------------------------------------------
// External function includes are used here to reduce the overall size of the main sketch.
// Go ahead and call it non-standard, but I don't like spaghetti code that goes on forever.
#include "web_api.h"             // Inline function library for the web API functions
#include "lora_wan.h"            // Inline function library for the LoRa WAN functions
#include "serial_config.h"       // Inline function library for system configutation via serial
//------------------------------------------------------------------------------------------------
void handleClient(Client& client) {
  String Msg = "";
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          break;
        } else {
          if (c != '\r') Msg += c;
        }
      }
    }
  }
  if ((Msg.length() > 0) && (Msg.indexOf("GET ") == 0)) {
    Msg.remove(0,4); // Delete the "GET " from the beginning
    Msg.remove(Msg.indexOf(" HTTP/1.1"),9); // Delete the " HTTP/1.1" from the end
    String Result = handleWebRequest(Msg);
    client.println(Result);
    if ((LoRa_Mode == 0) && (Result == "Rebooting...")) {
      delay(2000);
      ESP.restart();
    } else if ((LoRa_Mode == 1) && (Result == "Restarting...")) {
      delay(2000);
      ESP.restart();
    }
  }
}
//------------------------------------------------------------------------------------------------
void loop() {
  long CurrentTime = millis();
  Uptime = formatMillis(CurrentTime);
  if (CurrentTime > 4200000000) {
    // Reboot the system if we're reaching the maximum long integer value of CurrentTime (49 days)
    ESP.restart();
  }
  // Check for page button presses and switch the OLED screen page
  if (digitalRead(BTN) == 0) {
    while (digitalRead(BTN) == 0) delay(100);
    ActivePage ++;
    if (ActivePage > 6) ActivePage = 0;
    ScreenUpdate();
    ScreenTimer = CurrentTime;
  }
  // Check for user input from the serial configuration system
  if ((Serial) && (Serial.available())) SerialConfigInput();

  // Check for web API calls from Climate Czar Server
  if (Net_useWifi == 1) {
    WiFiClient client = wifiServer.available();
    handleClient(client);
  } else {
    EthernetClient client = ethServer.available();
    handleClient(client);
    if (Net_useDHCP == 1) {
      Ethernet.maintain();
    }
  }

  // Check for LoRa slave API calls from the master hub
  if ((Serial2) && (Serial2.available())) {
    String Msg = handleSlaveRequest();
    Serial2.println("AT+SEND=1," + String(Msg.length()) + "," + Msg);
  }

  // Update the OLED screen every 5 seconds
  if (CurrentTime - ScreenTimer >= 5000) {
    ScreenUpdate();
    ScreenTimer = millis();
  }
  // If this is a master unit, ping test the watchdog host every minute and reboot the hub if necessary
  if (CurrentTime - PingTimer >= 60000) {
    if (Net_IP != "0.0.0.0") {
      bool PingTest = Ping.ping(CZ_Watchdog.c_str(),2);
      if (! PingTest) {
        PingFailures ++;
      } else {
        PingFailures = 0;
      }
      if (PingFailures == CZ_pingFailures) ESP.restart();
    }
    PingTimer = millis();
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
