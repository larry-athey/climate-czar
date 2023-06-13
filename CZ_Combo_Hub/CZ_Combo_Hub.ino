// Written by Larry Athey (Panhandle Ponics - https://3dgtower.com) v1.0.1 released Feb 6, 2023
//
// Climate Czar Combo Hub is based on the Teyleten Robot ESP32S developer board which has 38 pins
// https://www.amazon.com/dp/B09J95SMG7 but will work with any ESP32 developer board with at least
// 18 physical GPIO pins. This board was chosen just in case future expansion is ever necessary.
//
// The combo hub provides 8 analog to digial input pins for variable value or binary value input
// sensors, 8 digital outputs for driving solid state relays, an on-board DHT22 temp and humidity
// sensor, and a OneWire bus for connecting as many DS18B20 temperature sensors as you want (just
// keep in mind that this is dependent upon the current output of your 5 volt power supply).
//
// Since the same ports are used for both variable value and binary value sensors, a script will
// need to be used in binary value use cases. Your script returns zero when zero and returns one
// if the value is greater than zero. This value can be anything from 1 to 4095 depending on the
// voltage that you send to the input sensor ports of the hub up to 3.3 volts. Depending on your
// implementation, a pull-down (bias) resistor may be needed to obtain a solid zero.
//
// Temperatures are output in both Celcius and Farrenheit depending on the URL that is requested.
// http://device-ip/temperature/c or http://device-ip/temperature/f reads from the DHT22.
// The same method also applies to any OneWire temperature sensors attached to the combo hub.
//
// Keep in mind that OneWire devices have a unique address (8 hexadecimal octets, a 64 bit number)
// and there's no way to force their position in the array. The easiest way to determine which one
// is which is to put them in ice water one by one and read the /onewire/c-or-f URL, then actually
// label them. Remember, since this address is just a 64 bit number, replacing just one sensor can
// change the position of all devices if its address is lower than all of the others in the array.
//
// Reading the OneWire bus returns all of the temperatures in a space-delimited list, for example
// 66.8 65.2 59.6 69.7 71.4 58.8 67.3 70.6
// If you want to get the third sensor's F reading in a Climate Czar read command, you would use
// curl --http0.9 http://device-ip/onewire/f | awk '{print $3}'
//
// Since long distance sensors tend to be randomly undependable, you may want to read sensors by
// their address instead. Make a call to http://device-ip/onewire/list to list all sensors along
// with their address. Then call http://device-ip/onewire/f?<address> to read sensors individually.
//
// Since we don't want to create any authority conflicts, NVRAM in the ESP32 isn't used to save
// and restore your output switch states. Therefore, you should disable One-Shot in all of your
// output switch device configurations that point to your Combo Hub. That way it won't matter if
// your Combo Hub loses power, Climate Czar will make sure the output switch states are in sync.
//------------------------------------------------------------------------------------------------
#include "WiFi.h" // ESP32-WROOM-DA will allow the blue on-board LED to react to WiFi traffic.
#include "ESP32Ping.h"
#include "DHTesp.h"
#include "OneWire.h"
#include "DallasTemperature.h"
//------------------------------------------------------------------------------------------------
const char* ssid = "Wokwi-GUEST"; // Your WiFi network name (replace Wokwi-GUEST)
const char* password = ""; // Your WiFi password

// Comment out the following 4 lines if you want to use DHCP (see setup section as well)
IPAddress staticIP(10,20,30,160);
IPAddress gateway(10,20,30,254);
IPAddress subnet(255,255,255,0);
IPAddress dns(10,20,30,254);

// IP to ping every 30 seconds to verify network connectivity and reboot the hub if no response.
const char* PingerIP = "10.20.30.254";

#define OW_PIN 4   // OneWire (or 1-Wire) data bus for DS18B20 temperature sensors
#define DHT_PIN 15 // DHT-22 data pin
#define SENSOR1 36 // Climate Czar variable value or binary value sensors 1..8
#define SENSOR2 39 // " "
#define SENSOR3 34 // " "
#define SENSOR4 35 // " "
#define SENSOR5 32 // " "
#define SENSOR6 33 // " "
#define SENSOR7 25 // " "
#define SENSOR8 26 // " "
#define SWITCH1 23 // Climate Czar output switch signal for solid state relays 1..8
#define SWITCH2 22 // " "
#define SWITCH3 21 // " "
#define SWITCH4 19 // " "
#define SWITCH5 18 // " "
#define SWITCH6 5  // " "
#define SWITCH7 17 // " "
#define SWITCH8 16 // " "
#define LED 2      // Onboard blue LED for WiFi connection status
//------------------------------------------------------------------------------------------------
WiFiServer Server(80);
DHTesp dhtSensor;
OneWire oneWire(OW_PIN);
DallasTemperature DT(&oneWire);
//------------------------------------------------------------------------------------------------
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (2 seconds)
const long timeoutTime = 2000;
// Used for WiFi reconnect check time keeping
unsigned long LoopCounter = 0;
// Used to force a restart every 21,600,000 loops (roughly every 6 hours)
unsigned long Uptime = 0;
// Check WiFi connection every 30000 loops (reading millis messes with the WiFi for some reason)
const long wifiCheck = 30000;
// Variable to store the HTTP request
String Header;
//------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN,DHTesp::DHT22);
  DT.begin();

  pinMode(SWITCH1,OUTPUT);
  digitalWrite(SWITCH1,0);
  pinMode(SWITCH2,OUTPUT);
  digitalWrite(SWITCH2,0);
  pinMode(SWITCH3,OUTPUT);
  digitalWrite(SWITCH3,0);
  pinMode(SWITCH4,OUTPUT);
  digitalWrite(SWITCH4,0);
  pinMode(SWITCH5,OUTPUT);
  digitalWrite(SWITCH5,0);
  pinMode(SWITCH6,OUTPUT);
  digitalWrite(SWITCH6,0);
  pinMode(SWITCH7,OUTPUT);
  digitalWrite(SWITCH7,0);
  pinMode(SWITCH8,OUTPUT);
  digitalWrite(SWITCH8,0);
  pinMode(LED,OUTPUT);

  // Comment out the following 3 lines if you want to use DHCP
  if (! WiFi.config(staticIP,gateway,subnet,dns,dns)) {
    Serial.println("WiFi static IP configuration failed.");
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED,1);
    delay(250);
    digitalWrite(LED,0);
    delay(250);
    Serial.print(".");
    LoopCounter ++;
    if (LoopCounter == 60) ESP.restart();
  }
  digitalWrite(LED,1);

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Server.begin();
}
//------------------------------------------------------------------------------------------------
String ReadOneWireBus(byte WhichOne) {
  String Readings = "";
  byte Addr[8];
  if (DT.getDeviceCount() > 0) {
    DT.requestTemperatures(); 
    oneWire.reset_search();
    delay(250);
    while (oneWire.search(Addr)) {
      if (WhichOne == 0) {
        Readings += String(DT.getTempC(Addr),1) + " ";
      } else {
        Readings += String(DT.getTempF(Addr),1) + " ";
      }
    }
    return Readings;
  } else {
    return "No Sensors Found";
  }
}
//------------------------------------------------------------------------------------------------
String ReadOneWireSensor(String Header,byte WhichOne) {
  String HexStr = "0x";
  char Buffer[5];
  byte x,y;
  byte Addr[8];
  if (DT.getDeviceCount() > 0) {
    Header.remove(0,15);
    Header.remove(23,(Header.length() - 23));
    y = 0;
    for (x = 0; x <= 22; x ++) {
      if (Header.charAt(x) != ':') HexStr += Header.charAt(x);
      if ((Header.charAt(x) == ':') || (x == 22)) {
        Serial.println(HexStr);
        HexStr.toCharArray(Buffer,5);
        Addr[y] = strtol(Buffer,NULL,0);
        HexStr = "0x";
        y ++;
      }
    }
    DT.requestTemperatures();
    oneWire.reset_search();
    delay(250);
    if (WhichOne == 0) {
      return String(DT.getTempC(Addr),1);
    } else {
      return String(DT.getTempF(Addr),1);
    } 
  } else {
    return "No Sensors Found";
  }
}
//------------------------------------------------------------------------------------------------
String ListOneWireDevices() {
  String Devices = "";
  byte x;
  byte Addr[8];
  if (DT.getDeviceCount() > 0) {
    DT.requestTemperatures(); 
    oneWire.reset_search();
    delay(250);
    while (oneWire.search(Addr)) {
      for (x = 0; x < 8; x ++) {
        if (x < 7) {
          Devices += String(Addr[x],HEX) + ":";
        } else {
          Devices += String(Addr[x],HEX);
        }
      }
      Devices += " " + String(DT.getTempC(Addr),1) + " " + String(DT.getTempF(Addr),1) + "\n";
    }
    return Devices;
  } else {
    return "No Sensors Found";
  }
}
//------------------------------------------------------------------------------------------------
void loop() {
  TempAndHumidity DHT = dhtSensor.getTempAndHumidity();
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
            Serial.println("\n" + Header);
            if (Header.indexOf("GET /humidity") >= 0) { // Read DHT-22 humidity
              Client.println(String(DHT.humidity,1));
            } else if (Header.indexOf("GET /temperature/c") >= 0) { // Read DHT-22 temperature in C
              Client.println(String(DHT.temperature,1));
            } else if (Header.indexOf("GET /temperature/f") >= 0) { // Read DHT-22 temperature in F
              Client.println(String(DHT.temperature * 9 / 5 + 32,1));
            } else if (Header.indexOf("GET /onewire/c?") >= 0) { // Read a specific OneWire sensor by address in C
              Client.println(ReadOneWireSensor(Header,0));
            } else if (Header.indexOf("GET /onewire/c") >= 0) { // Read all OneWire sensors in C
              Client.println(ReadOneWireBus(0));
            } else if (Header.indexOf("GET /onewire/f?") >= 0) { // Read a specific OneWire sensor by address in F
              Client.println(ReadOneWireSensor(Header,1));
            } else if (Header.indexOf("GET /onewire/f") >= 0) { // Read all OneWire sensors in F
              Client.println(ReadOneWireBus(1));
            } else if (Header.indexOf("GET /onewire/list") >= 0) { // List all OneWire sensors as "Address C F"
              Client.println(ListOneWireDevices());
            } else if (Header.indexOf("GET /sensor1") >= 0) { // Read all sensor 1..8 GPIO ports
              Client.println(String(analogRead(SENSOR1),0));
            } else if (Header.indexOf("GET /sensor2") >= 0) { // " "
              Client.println(String(analogRead(SENSOR2),0));
            } else if (Header.indexOf("GET /sensor3") >= 0) { // " "
              Client.println(String(analogRead(SENSOR3),0));
            } else if (Header.indexOf("GET /sensor4") >= 0) { // " "
              Client.println(String(analogRead(SENSOR4),0));
            } else if (Header.indexOf("GET /sensor5") >= 0) { // " "
              Client.println(String(analogRead(SENSOR5),0));
            } else if (Header.indexOf("GET /sensor6") >= 0) { // " "
              Client.println(String(analogRead(SENSOR6),0));
            } else if (Header.indexOf("GET /sensor7") >= 0) { // " "
              Client.println(String(analogRead(SENSOR7),0));
            } else if (Header.indexOf("GET /sensor8") >= 0) { // " "
              Client.println(String(analogRead(SENSOR8),0));
            } else if (Header.indexOf("GET /switch1/0") >= 0) { // Toggle output switch 1..8 GPIO ports
              digitalWrite(SWITCH1,0);
              Client.println("Switch1=0");
            } else if (Header.indexOf("GET /switch1/1") >= 0) { // " "
              digitalWrite(SWITCH1,1);
              Client.println("Switch1=1");
            } else if (Header.indexOf("GET /switch2/0") >= 0) { // " "
              digitalWrite(SWITCH2,0);
              Client.println("Switch2=0");
            } else if (Header.indexOf("GET /switch2/1") >= 0) { // " "
              digitalWrite(SWITCH2,1);
              Client.println("Switch2=1");
            } else if (Header.indexOf("GET /switch3/0") >= 0) { // " "
              digitalWrite(SWITCH3,0);
              Client.println("Switch3=0");
            } else if (Header.indexOf("GET /switch3/1") >= 0) { // " "
              digitalWrite(SWITCH3,1);
              Client.println("Switch3=1");
            } else if (Header.indexOf("GET /switch4/0") >= 0) { // " "
              digitalWrite(SWITCH4,0);
              Client.println("Switch4=0");
            } else if (Header.indexOf("GET /switch4/1") >= 0) { // " "
              digitalWrite(SWITCH4,1);
              Client.println("Switch4=1");
            } else if (Header.indexOf("GET /switch5/0") >= 0) { // " "
              digitalWrite(SWITCH5,0);
              Client.println("Switch5=0");
            } else if (Header.indexOf("GET /switch5/1") >= 0) { // " "
              digitalWrite(SWITCH5,1);
              Client.println("Switch5=1");
            } else if (Header.indexOf("GET /switch6/0") >= 0) { // " "
              digitalWrite(SWITCH6,0);
              Client.println("Switch6=0");
            } else if (Header.indexOf("GET /switch6/1") >= 0) { // " "
              digitalWrite(SWITCH6,1);
              Client.println("Switch6=1");
            } else if (Header.indexOf("GET /switch7/0") >= 0) { // " "
              digitalWrite(SWITCH7,0);
              Client.println("Switch7=0");
            } else if (Header.indexOf("GET /switch7/1") >= 0) { // " "
              digitalWrite(SWITCH7,1);
              Client.println("Switch7=1");
            } else if (Header.indexOf("GET /switch8/0") >= 0) { // " "
              digitalWrite(SWITCH8,0);
              Client.println("Switch8=0");
            } else if (Header.indexOf("GET /switch8/1") >= 0) { // " "
              digitalWrite(SWITCH8,1);
              Client.println("Switch8=1");
            } else if (Header.indexOf("GET /restart") >= 0) { // Reboot the hub
              Client.println("Restarting in 1 second");
              Client.stop();
              delay(1000);
              ESP.restart();
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
  if (LoopCounter >= wifiCheck) {
    bool Test = Ping.ping(PingerIP,3);
    if (! Test) {
      ESP.restart();
    } else {
      LoopCounter = 0;
    }
  }
  delay(10);
  LoopCounter ++;
  Uptime ++;
  if (Uptime >= 21600000) ESP.restart();
}
//------------------------------------------------------------------------------------------------
