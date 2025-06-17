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
  Serial.write("\033[2J\033[H\n");
  Serial.println("-=> NOTE: Power cylce the hub after disconnecting your USB connection! <=-\n\n");
  Serial.println("Main Configuration Menu:\n");
  Serial.println("  1. TCP/IP Settings");
  Serial.println("  2. LoRa WAN Settings");
  Serial.println("  3. Combo Hub Settings");
  Serial.println("  4. Show DS18B20 sensors");
  Serial.println("  5. Reboot Combo Hub");
  Serial.print("\nChoose an option: ");
}
//------------------------------------------------------------------------------------------------
inline void NetworkMenu() { // Display the network configuration menu
  Serial.write("\033[2J\033[H\n");
  if (Net_useWifi == 1) {
    Serial.println("WiFi Enabled  : Yes");
    Serial.println("WiFi Channel  : " + String(WiFi.channel()));
    Serial.println("WiFi Signal   : " + String(WiFi.RSSI()));
  } else {
    Serial.println("WiFi Enabled  : No");
  }
  Serial.print("WiFi SSID     : "); Serial.println(Net_wifiSSID);
  Serial.print("WiFi Password : "); Serial.println(Net_wifiPW + "\n");
  if (Net_useDHCP == 1) {
    Serial.println("Use DHCP      : Yes");
  } else {
    Serial.println("Use DHCP      : No");
  }
  Serial.print("IP Address    : "); Serial.println(Net_IP);
  Serial.print("Subnet Mask   : "); Serial.println(Net_Mask);
  Serial.print("Def Gateway   : "); Serial.println(Net_Gateway);
  Serial.print("DNS Resolver  : "); Serial.println(Net_DNS);
  Serial.println("\nNetwork Configuration Menu:\n");
  Serial.println("  1. Enable/Disable WiFi");
  Serial.println("  2. Set WiFi SSID");
  Serial.println("  3. Set WiFi Password");
  Serial.println("  4. Set DHCP or Static IP");
  Serial.println("  5. Restart Network Services");
  Serial.println("  Q. Quit this menu");
  Serial.print("\nChoose an option: ");
}
//------------------------------------------------------------------------------------------------
inline void LoRaMenu() { // Display the LoRa WAN configuration menu
  Serial.write("\033[2J\033[H\n");
  if (LoRa_Mode == 1) {
    Serial.println("LoRa Mode    : Slave");
  } else {
    Serial.println("LoRa Mode    : Master");
  }
  Serial.print("LoRa Network : "); Serial.println(LoRa_Network);
  Serial.print("LoRa Address : "); Serial.println(LoRa_Address);
  Serial.println("\nLoRa WAN Configuration Menu:\n");
  Serial.println("  1. Set Master/Slave Mode");
  Serial.println("  2. Set Network ID");
  Serial.println("  3. Set Network Address");
  Serial.println("  Q. Quit this menu");
  Serial.print("\nChoose an option: ");
}
//------------------------------------------------------------------------------------------------
inline void HubMenu() { // Display the Combo Hub configuration menu
  Serial.write("\033[2J\033[H\n");
  Serial.print("Device Name     : "); Serial.println(CZ_deviceName);
  Serial.print("Watchdog Host   : "); Serial.println(CZ_Watchdog);
  Serial.print("Ping Fail Limit : "); Serial.println(CZ_pingFailures);
  Serial.println("\nCombo Hub Configuration Menu:\n");
  Serial.println("  1. Set Device Name");
  Serial.println("  2. Set Watchdog Host");
  Serial.println("  3. Set Ping Fail Limit");
  Serial.println("  Q. Quit this menu");
  Serial.print("\nChoose an option: ");
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
        Serial.print("\b \b"); // Backspace, space, backspace
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
  Serial.print("Enter the device name: ");
  CZ_deviceName = ReadInput();
  CZ_deviceName = sanitizeHostname(CZ_deviceName);
}
//-----------------------------------------------------------------------------------------------
inline void get_czPingFailures() { // Get the ping failure count to trigger a reboot
  Serial.print("Enter the number of ping failures to reboot the hub: ");
  CZ_pingFailures = ReadInput().toInt();
  if (CZ_pingFailures < 1) CZ_pingFailures = 1;
}
//-----------------------------------------------------------------------------------------------
inline void get_czWatchdog() { // Get the ping watchdog host name or IP address
  Serial.print("Enter the ping watchdog host name or IP: ");
  CZ_Watchdog = ReadInput();
  CZ_Watchdog = sanitizeHostname(CZ_Watchdog);
}
//-----------------------------------------------------------------------------------------------
inline void get_enableWifi() { // Get the WiFi on/off switch setting
  Serial.print("Enter 1 to enable WiFi or 0 for ethernet only: ");
  Net_useWifi = ReadInput().toInt();
  if (Net_useWifi != 0) Net_useWifi = 1;
}
//-----------------------------------------------------------------------------------------------
inline void get_ipMode() { // Get the network IP address mode (DHCP or Static IP)
  Serial.print("Enter 1 for DHCP or 0 for Static IP: ");
  Net_useDHCP = ReadInput().toInt();
  if (Net_useDHCP == 0) {
    Net_useDHCP = 0;
    Serial.print("\n\nEnter Static IP Address: ");
    Net_IP = ReadInput();
    if (! isValidIP(Net_IP)) Net_IP = "0.0.0.0";
    Serial.print("\nEnter Subnet Mask: ");
    Net_Mask = ReadInput();
    if (! isValidIP(Net_Mask)) Net_Mask = "255.255.255.0";
    Serial.print("\nEnter Default Gateway: ");
    Net_Gateway = ReadInput();
    if (! isValidIP(Net_Gateway)) Net_Gateway = "0.0.0.0";
    Serial.print("\nEnter DNS Resolver: ");
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
  Serial.print("Enter the LoRa address [1..65535]: ");
  LoRa_Address = ReadInput().toInt();
  if (LoRa_Address < 1) LoRa_Address = 1;
  if (LoRa_Address > 65535) LoRa_Address = 65535;
}
//-----------------------------------------------------------------------------------------------
inline void get_LoRaMode() { // Get the LoRa master/slave mode
  Serial.print("Enter 1 to set slave mode or 0 to be the master: ");
  LoRa_Mode = ReadInput().toInt();
  if (LoRa_Mode != 0) LoRa_Mode = 1;
}
//-----------------------------------------------------------------------------------------------
inline void get_LoRaNetwork() { // Get the LoRa network ID
  Serial.print("Enter the LoRa network ID [0..16]: ");
  LoRa_Network = ReadInput().toInt();
  if (LoRa_Network < 0) LoRa_Network = 0;
  if (LoRa_Network > 16) LoRa_Network = 16;
}
//-----------------------------------------------------------------------------------------------
inline void get_wifiPassword() { // Get the WiFi password
  Serial.print("Enter WiFi Password: ");
  Net_wifiPW = ReadInput();
}
//-----------------------------------------------------------------------------------------------
inline void get_wifiSSID() { // Get the WiFi SSID (network name)
  Serial.print("Enter WiFi SSID: ");
  Net_wifiSSID = ReadInput();
}
//-----------------------------------------------------------------------------------------------
inline void showDS18B20() { // Show all DS18B20 sensor addresses and their current temperatures
  Serial.write("\033[2J\033[H\n");

  int deviceCount = DT.getDeviceCount();
  Serial.print("Found ");
  Serial.print(deviceCount);
  Serial.println(" DS18B20 sensor(s)");

  if (deviceCount == 0) {
    Serial.println("\nNo sensors detected. Check wiring and pull-up resistor.");
    return;
  }

  Serial.println("\nSensor List:\n");
  Serial.println("Address                Temp (°C)  Temp (°F)");
  Serial.println("-------------------------------------------");

  DT.requestTemperatures();

  for (int i = 0; i < deviceCount; i++) {
    uint8_t deviceAddress[8];
    if (DT.getAddress(deviceAddress, i)) {
      // Get colon-delimited address
      String addressStr = addressToString(deviceAddress);

      // Read temperatures
      float tempC = DT.getTempC(deviceAddress);
      float tempF = DT.getTempF(deviceAddress);

      // Check for valid readings
      if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.print(addressStr);
        Serial.println("  Error: Disconnected");
        continue;
      }

      char buffer[80];
      sprintf(buffer, "%-22s %8.1f  %8.1f", addressStr.c_str(), tempC, tempF);
      Serial.println(buffer);
    } else {
      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.println(": Unable to retrieve address");
    }
  }
}
//-----------------------------------------------------------------------------------------------
inline void SerialConfigInput() { // Handle user configuration via the serial console
  String Option = ReadInput();
  PurgeBuffer();
  Serial.println("\n\n");

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
      Serial.print("\n\nPress ENTER to refresh, or Q to return to the main menu: ");
    } else if (Option == "5") {
      Serial.print("Are you sure that you want to reboot this device? [Y/N] ");
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
      StartNetwork();
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
      LoRa_Init();
      LoRaMenu();
    } else if (Option == "3") {
      get_LoRaAddress();
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
      Serial.print("\n\nPress ENTER to refresh, or Q to return to the main menu: ");
    }
  }
  SetMemory();
}
//------------------------------------------------------------------------------------------------
