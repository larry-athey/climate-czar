//------------------------------------------------------------------------------------------------
// Climate Czar Combo Hub v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//
// Use picocom to configure under Linux
// picocom /dev/ttyUSB0 -b 115200
//
// While there are many other terminal programs out there for Linux, I can only recommend picocom
// because that's what I use. You could just as easily use minicom, but I've found that it doesn't
// handle backspace keypresses well.
//
// Windows and Mac OS users should use their favorite search engine to find guides for connecting
// to an ESP32 with a serial terminal program. ChatGPT and Grok are even better options for this.
// You may also wish to find options for using your cell phone with a serial terminal connection.
//
//------------------------------------------------------------------------------------------------
inline void ConfigMenu() { // Display the main configuration menu
  Serial.print(F("\033[2J\033[H\n"));
  Serial.println(F("-=> Some settings require a reboot to become effective <=-\n"));
  Serial.println(F("Main Configuration Menu:\n"));
  Serial.println(F("  1. TCP/IP Settings"));
  Serial.println(F("  2. LoRa WAN Settings"));
  Serial.println(F("  3. Combo Hub Settings"));
  Serial.println(F("  4. Show DS18B20 sensors"));
  Serial.println(F("  5. Debug LoRa WAN Data"));
  Serial.println(F("  6. Reboot Combo Hub"));
  Serial.print(F("\nChoose an option: "));
}
//------------------------------------------------------------------------------------------------
inline void NetworkMenu() { // Display the network configuration menu
  Serial.print(F("\033[2J\033[H\n"));
  if (Net_useWifi == 1) {
    Serial.println(F("WiFi Enabled  : Yes"));
    Serial.print(F("WiFi SSID     : ")); Serial.println(Net_wifiSSID);
    Serial.print(F("WiFi Password : ")); Serial.println(Net_wifiPW);
    Serial.println("WiFi Channel  : " + String(WiFi.channel()));
    Serial.println("WiFi Signal   : " + String(WiFi.RSSI()) + "\n");
  } else {
    Serial.println(F("WiFi Enabled  : No"));
    if (ethConnected) {
      Serial.println(F("Ethernet Up   : Yes\n"));
    } else {
      Serial.println(F("Ethernet Up   : No\n"));
    }
  }
  if (Net_useDHCP == 1) {
    Serial.println(F("Use DHCP      : Yes"));
  } else {
    Serial.println(F("Use DHCP      : No"));
  }
  Serial.print(F("IP Address    : ")); Serial.println(Net_IP);
  Serial.print(F("Subnet Mask   : ")); Serial.println(Net_Mask);
  Serial.print(F("Def Gateway   : ")); Serial.println(Net_Gateway);
  Serial.print(F("DNS Resolver  : ")); Serial.println(Net_DNS);
  Serial.println(F("\nNetwork Configuration Menu:\n"));
  Serial.println(F("  1. Enable/Disable WiFi"));
  Serial.println(F("  2. Set WiFi SSID"));
  Serial.println(F("  3. Set WiFi Password"));
  Serial.println(F("  4. Set DHCP or Static IP"));
  Serial.println(F("  5. Restart Network Services"));
  Serial.println(F("  Q. Quit this menu"));
  Serial.print(F("\nChoose an option: "));
}
//------------------------------------------------------------------------------------------------
inline void LoRaMenu() { // Display the LoRa WAN configuration menu
  Serial.print(F("\033[2J\033[H\n"));
  if (LoRa_Mode == 1) {
    Serial.println(F("LoRa Mode    : Slave"));
  } else {
    Serial.println(F("LoRa Mode    : Master"));
  }
  Serial.print(F("LoRa Network : ")); Serial.println(LoRa_Network);
  Serial.print(F("LoRa Address : ")); Serial.println(LoRa_Address);
  Serial.println(F("\nLoRa WAN Configuration Menu:\n"));
  Serial.println(F("  1. Set Master/Slave Mode"));
  Serial.println(F("  2. Set Network ID"));
  Serial.println(F("  3. Set Network Address"));
  Serial.println(F("  4. Set Domain Password"));
  Serial.println(F("  Q. Quit this menu"));
  Serial.print(F("\nChoose an option: "));
}
//------------------------------------------------------------------------------------------------
inline void HubMenu() { // Display the Combo Hub configuration menu
  Serial.print(F("\033[2J\033[H\n"));
  Serial.print(F("Device Name     : ")); Serial.println(CZ_deviceName);
  Serial.print(F("Watchdog Host   : ")); Serial.println(CZ_Watchdog);
  Serial.print(F("Ping Fail Limit : ")); Serial.println(CZ_pingFailures);
  Serial.println(F("\nCombo Hub Configuration Menu:\n"));
  Serial.println(F("  1. Set Device Name"));
  Serial.println(F("  2. Set Watchdog Host"));
  Serial.println(F("  3. Set Ping Fail Limit"));
  Serial.println(F("  Q. Quit this menu"));
  Serial.print(F("\nChoose an option: "));
}
//------------------------------------------------------------------------------------------------
inline void PurgeBuffer() { // Purge anything waiting in the receive buffer
  while (Serial.available()) Serial.read();
}
//------------------------------------------------------------------------------------------------
inline String ReadInput() { // Read from the serial port until the user presses the ENTER key
  String input = "";
  char c;
  // Teminal program should have ANSI emulation enabled, VT100 may not handle backspaces correctly
  while (true) {
    if (Serial.available() > 0) {
      c = Serial.read(); // Read the incoming character
      // Echo the character back to the serial monitor
      Serial.print(c);
      if (c == '\r' || c == '\n') { // Check if it's a newline
        return input; // Return the collected input when Enter is pressed
      } else if ((c == '\b' || c == 0x7F) && input.length() > 0) { // Handle backspace
        // Remove the last character from input
        input.remove(input.length() - 1);
        // Clear the last character from the display
        Serial.print(F("\b \b")); // Backspace, space, backspace
        //Serial.print("\b\033[K"); // Backspace + Ye old ANSI CLREOL
      } else {
        // Append the character to the input string
        input += c;
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------
inline void get_czDeviceName() { // Get the combo hub device name
  Serial.print(F("Enter the device name: "));
  CZ_deviceName = ReadInput();
  CZ_deviceName = sanitizeHostname(CZ_deviceName);
}
//-----------------------------------------------------------------------------------------------
inline void get_czPingFailures() { // Get the ping failure count to trigger a reboot
  Serial.print(F("Enter the number of ping failures to reboot the hub: "));
  CZ_pingFailures = ReadInput().toInt();
  if (CZ_pingFailures < 1) CZ_pingFailures = 1;
}
//-----------------------------------------------------------------------------------------------
inline void get_czWatchdog() { // Get the ping watchdog host name or IP address
  Serial.print(F("Enter the ping watchdog host name or IP (0.0.0.0 to disable): "));
  CZ_Watchdog = ReadInput();
  CZ_Watchdog = sanitizeHostname(CZ_Watchdog);
}
//-----------------------------------------------------------------------------------------------
inline void get_enableWifi() { // Get the WiFi on/off switch setting
  Serial.print(F("Enter 1 to enable WiFi or 0 for ethernet only: "));
  Net_useWifi = ReadInput().toInt();
  if (Net_useWifi != 0) Net_useWifi = 1;
}
//-----------------------------------------------------------------------------------------------
inline void get_ipMode() { // Get the network IP address mode (DHCP or Static IP)
  Serial.print(F("Enter 0 for DHCP or 1 for Static IP: "));
  int Input = ReadInput().toInt();
  if (Input > 0) {
    Net_useDHCP = 0;
    Serial.print(F("\n\nEnter Static IP Address: "));
    Net_IP = ReadInput();
    if (! isValidIP(Net_IP)) Net_IP = "0.0.0.0";
    Serial.print(F("\nEnter Subnet Mask: "));
    Net_Mask = ReadInput();
    if (! isValidIP(Net_Mask)) Net_Mask = "255.255.255.0";
    Serial.print(F("\nEnter Default Gateway: "));
    Net_Gateway = ReadInput();
    if (! isValidIP(Net_Gateway)) Net_Gateway = "0.0.0.0";
    Serial.print(F("\nEnter DNS Resolver: "));
    Net_DNS = ReadInput();
    if (! isValidIP(Net_DNS)) Net_DNS = "0.0.0.0";
  } else {
    Net_useDHCP = 1;
    Net_IP = "";
    Net_Mask = "";
    Net_Gateway = "";
    Net_DNS = "";
  }
}
//-----------------------------------------------------------------------------------------------
inline void get_LoRaAddress() { // Get the LoRa network address
  Serial.print(F("Enter the LoRa address [1..65535]: "));
  LoRa_Address = ReadInput().toInt();
  if (LoRa_Address < 1) LoRa_Address = 1;
  if (LoRa_Address > 65535) LoRa_Address = 65535;
}
//-----------------------------------------------------------------------------------------------
inline void get_LoRaMode() { // Get the LoRa master/slave mode
  Serial.print("Enter 1 to set slave mode or 0 to be the master: ");
  LoRa_Mode = ReadInput().toInt();
  if (LoRa_Mode != 0) LoRa_Mode = 1;
  if (LoRa_Mode == 0) LoRa_Address = 1;
  if ((LoRa_Mode == 1) && (LoRa_Address == 1)) LoRa_Address = 100;
}
//-----------------------------------------------------------------------------------------------
inline void get_LoRaNetwork() { // Get the LoRa network ID
  Serial.print(F("Enter the LoRa network ID [0..15]: "));
  LoRa_Network = ReadInput().toInt();
  if (LoRa_Network < 0) LoRa_Network = 0;
  if (LoRa_Network != 18) { // Valid network ID, but mostly unknown to others
    if (LoRa_Network > 15) LoRa_Network = 15;
  }
}
//-----------------------------------------------------------------------------------------------
inline void get_LoRaPW() { // Get the LoRa domain password
  Serial.print(F("Enter the LoRa domain password (8 characters, hex): "));
  LoRa_PW = ReadInput().substring(0,8);
  LoRa_PW.toUpperCase();
}
//-----------------------------------------------------------------------------------------------
inline void get_wifiPassword() { // Get the WiFi password
  Serial.print(F("Enter WiFi Password: "));
  Net_wifiPW = ReadInput();
}
//-----------------------------------------------------------------------------------------------
inline void get_wifiSSID() { // Get the WiFi SSID (network name)
  Serial.print(F("Enter WiFi SSID: "));
  Net_wifiSSID = ReadInput();
}
//-----------------------------------------------------------------------------------------------
inline void showDS18B20() { // Show all DS18B20 sensor addresses and their current temperatures
  Serial.print(F("\033[2J\033[H\n"));

  int deviceCount = DT.getDeviceCount();
  Serial.print(F("Found "));
  Serial.print(deviceCount);
  Serial.println(F(" DS18B20 sensor(s)"));

  if (deviceCount == 0) {
    Serial.println(F("\nNo sensors detected. Check wiring and pull-up resistor."));
    return;
  }

  Serial.println(F("\nSensor List:\n"));
  Serial.println(F("Address                Temp (°C)  Temp (°F)"));
  Serial.println(F("-------------------------------------------"));

  DT.requestTemperatures();

  for (int i = 0; i < deviceCount; i++) {
    uint8_t deviceAddress[8];
    if (DT.getAddress(deviceAddress,i)) {
      // Get colon-delimited address
      String addressStr = addressToString(deviceAddress);

      // Read temperatures
      float tempC = DT.getTempC(deviceAddress);
      float tempF = DT.getTempF(deviceAddress);

      // Check for valid readings
      if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.print(addressStr);
        Serial.println(F("  Error: Disconnected"));
        continue;
      }

      char buffer[80];
      sprintf(buffer,"%-22s %8.1f  %8.1f",addressStr.c_str(),tempC,tempF);
      Serial.println(buffer);
    } else {
      Serial.print(F("Sensor "));
      Serial.print(i + 1);
      Serial.println(F(": Unable to retrieve address"));
    }
  }
}
//-----------------------------------------------------------------------------------------------
inline void showLoRa() { // Show all outbound and inbound RYLR998 communications
  Serial.print(F("\033[2J\033[H"));
}
//-----------------------------------------------------------------------------------------------
inline void SerialConfigInput() { // Handle user configuration via the serial console
  String Option = ReadInput();
  PurgeBuffer();
  Serial.println(F("\n\n"));

  if (ActiveMenu == 0) { // Main configuration menu
    if (Option == "1") {
      ActiveMenu = 1;
      NetworkMenu();
    } else if (Option == "2") {
      ActiveMenu = 2;
      LoRaMenu();
    } else if (Option == "3") {
      ActiveMenu = 3;
      HubMenu();
    } else if (Option == "4") {
      ActiveMenu = 4;
      showDS18B20();
      Serial.print(F("\n\nPress ENTER to refresh, or Q to return to the main menu: "));
    } else if (Option == "5") {
      ActiveMenu = 5;
      showLoRa();
      Serial.println(F("Press Q to return to the main menu:\n"));
    } else if (Option == "6") {
      Serial.print(F("Are you sure that you want to reboot this device? [Y/N] "));
      String Confirm = ReadInput();
      Confirm.toLowerCase();
      if (Confirm == "y") {
        ESP.restart();
      } else {
        ConfigMenu();
      }
    } else {
      ConfigMenu();
    }
  } else if (ActiveMenu == 1) { // Network Configuration Menu
    if (Option == "1") {
      get_enableWifi();
      NetworkMenu();
    } else if (Option == "2") {
      get_wifiSSID();
      NetworkMenu();
    } else if (Option == "3") {
      get_wifiPassword();
      NetworkMenu();
    } else if (Option == "4") {
      get_ipMode();
      NetworkMenu();
    } else if (Option == "5") {
      if (! StartNetwork()) {
        Serial.println(F("\r\nNo TCP/IP network available"));
        delay(2000);
      }
      NetworkMenu();
    } else {
      Option.toLowerCase();
      if (Option == "q") {
        ActiveMenu = 0;
        ConfigMenu();
      } else {
        NetworkMenu();
      }
    }
  } else if (ActiveMenu == 2) { // LoRa WAN configuration menu
    if (Option == "1") {
      get_LoRaMode();
      LoRaMenu();
    } else if (Option == "2") {
      get_LoRaNetwork();
      resetRYLR998();
      LoRa_Init();
      LoRaMenu();
    } else if (Option == "3") {
      get_LoRaAddress();
      resetRYLR998();
      LoRa_Init();
      LoRaMenu();
    } else if (Option == "4") {
      get_LoRaPW();
      resetRYLR998();
      LoRa_Init();
      LoRaMenu();
    } else {
      Option.toLowerCase();
      if (Option == "q") {
        ActiveMenu = 0;
        ConfigMenu();
      } else {
        LoRaMenu();
      }
    }
  } else if (ActiveMenu == 3) { // Combo Hub configuration menu
    if (Option == "1") {
      get_czDeviceName();
      HubMenu();
    } else if (Option == "2") {
      get_czWatchdog();
      HubMenu();
    } else if (Option == "3") {
      get_czPingFailures();
      HubMenu();
    } else {
      Option.toLowerCase();
      if (Option == "q") {
        ActiveMenu = 0;
        ConfigMenu();
      } else {
        HubMenu();
      }
    }
  } else if (ActiveMenu == 4) { // Show all connected DS18B20 sensors
    Option.toLowerCase();
    if (Option == "q") {
      ActiveMenu = 0;
      ConfigMenu();
    } else {
      showDS18B20();
      Serial.print(F("\n\nPress ENTER to refresh, or Q to return to the main menu: "));
    }
  } else if (ActiveMenu == 5) { // Show all RYLR998 communications
    Option.toLowerCase();
    if (Option == "q") {
      ActiveMenu = 0;
      ConfigMenu();
    } else {
      showLoRa();
      Serial.println(F("Press Q to return to the main menu:\n"));
    }
  }
  SetMemory();
}
//------------------------------------------------------------------------------------------------
