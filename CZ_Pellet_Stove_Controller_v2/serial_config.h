//------------------------------------------------------------------------------------------------
// CZ Pellet Stove Controller v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------
inline void ShowConfig() { // Send an ANSI clear screen sequence and display all configuration settings
  Serial.print(F("\033[2J\033[H\n\n"));
  Serial.println("DeviceName    : " + DeviceName);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("wifiConnected : Yes"));
    Serial.println("WiFi Channel  : " + String(WiFi.channel()));
    Serial.println("WiFi Signal   : " + String(WiFi.RSSI()) + "\n");
  } else {
    Serial.println(F("wifiConnected : No\n"));
  }
  if (wifiMode == 0) {
    Serial.println(F("wifiMode      : DHCP"));
  } else {
    Serial.println(F("wifiMode      : Static IP"));
  }
  Serial.println("wifiSSID      : " + wifiSSID);
  Serial.println("wifiPassword  : " + wifiPassword);
  Serial.println("wifiIP        : " + wifiIP);
  Serial.println("wifiMask      : " + wifiMask);
  Serial.println("wifiGateway   : " + wifiGateway);
  Serial.println("wifiDNS       : " + wifiDNS);
  Serial.println(F("\nNetwork Configuration Menu:\n"));
  Serial.println(F("  1. Set Device/Host Name"));
  Serial.println(F("  2. Set WiFi SSID"));
  Serial.println(F("  3. Set WiFi Password"));
  Serial.println(F("  4. Set DHCP or Static IP"));
  Serial.println(F("  5. Restart Network Services"));
  Serial.println(F("  6. Reboot Device"));
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
//------------------------------------------------------------------------------------------------
inline void get_DeviceName() { // Get the controller device/host name
  Serial.print(F("Enter the device/host name: "));
  DeviceName = ReadInput();
  DeviceName = sanitizeHostname(DeviceName);
}
//------------------------------------------------------------------------------------------------
inline void get_ipMode() { // Get the network IP address mode (DHCP or Static IP)
  Serial.print(F("Enter 0 for DHCP or 1 for Static IP: "));
  wifiMode = ReadInput().toInt();
  if (wifiMode > 0) {
    wifiMode = 1;
    Serial.print(F("\n\nEnter Static IP Address: "));
    wifiIP = ReadInput();
    if (! isValidIP(wifiIP)) wifiIP = "0.0.0.0";
    Serial.print(F("\nEnter Subnet Mask: "));
    wifiMask = ReadInput();
    if (! isValidIP(wifiMask)) wifiMask = "255.255.255.0";
    Serial.print(F("\nEnter Default Gateway: "));
    wifiGateway = ReadInput();
    if (! isValidIP(wifiGateway)) wifiGateway = "0.0.0.0";
    Serial.print(F("\nEnter DNS Resolver: "));
    wifiDNS = ReadInput();
    if (! isValidIP(wifiDNS)) wifiDNS = "0.0.0.0";
  } else {
    wifiMode = 0;
    wifiIP = "";
    wifiMask = "";
    wifiGateway = "";
    wifiDNS = "";
  }
}
//------------------------------------------------------------------------------------------------
inline void get_wifiPassword() { // Get the WiFi password
  Serial.print(F("Enter WiFi Password: "));
  wifiPassword = ReadInput();
}
//------------------------------------------------------------------------------------------------
inline void get_wifiSSID() { // Get the WiFi SSID (network name)
  Serial.print(F("Enter WiFi SSID: "));
  wifiSSID = ReadInput();
}
//------------------------------------------------------------------------------------------------
inline void SerialConfigInput() { // Handle user configuration via the serial console
  String Option = ReadInput();
  PurgeBuffer();
  Serial.println(F("\n\n"));

  if (Option == "1") {
    get_DeviceName();
  } else if (Option == "2") {
    get_wifiSSID();
  } else if (Option == "3") {
    get_wifiPassword();
  } else if (Option == "4") {
    get_ipMode();
  } else if (Option == "5") {
    ConnectWiFi();
  } else if (Option == "6") {
    ESP.restart();
  }

  ShowConfig();
  SetMemory();
}
//------------------------------------------------------------------------------------------------
