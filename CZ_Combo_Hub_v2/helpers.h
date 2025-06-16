//------------------------------------------------------------------------------------------------
// Climate Czar Combo Hub v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------
inline String addressToString(uint8_t deviceAddress[8]) { // Converts DS18B20 8 byte array address to a string
  String addressStr = "";
  for (int i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) addressStr += "0";
    addressStr += String(deviceAddress[i],HEX);
    if (i < 7) addressStr += ":";
  }
  return addressStr;
}
//------------------------------------------------------------------------------------------------
inline String formatMillis(unsigned long millisValue) { // Converts a timestamp to a HH:MM:SS formated time
  unsigned long seconds = millisValue / 1000; // Convert milliseconds to seconds
  unsigned long minutes = seconds / 60;       // Convert seconds to minutes
  unsigned long hours = minutes / 60;         // Convert minutes to hours

  // Calculate the remaining seconds, minutes after accounting for hours and minutes
  seconds %= 60;
  minutes %= 60;

  char buffer[10]; // Enough space for "HH:MM:SS" + null terminator
  snprintf(buffer,sizeof(buffer),"%02ld:%02ld:%02ld",hours,minutes,seconds);

  return String(buffer);
}
//------------------------------------------------------------------------------------------------
inline String getBaseMacString() { // Returns the ESP32 MAC address as a hyphenated string
  byte mac[6];
  char macStr[18];
  esp_efuse_mac_get_default(mac);
  snprintf(macStr,sizeof(macStr),"%02X-%02X-%02X-%02X-%02X-%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  return String(macStr);
}
//------------------------------------------------------------------------------------------------
inline bool isValidIP(String ip) { // Validates both IPv4 and IPv6 addresses
  // Convert String to std::string for easier manipulation
  std::string ipStr = ip.c_str();

  if (ipStr.find('.') != std::string::npos) {
    // Check for IPv4
    // IPv4 validation
    IPAddress ipv4Addr;
    if (ipv4Addr.fromString(ip)) {
      return true;
    }
  } else if (ipStr.find(':') != std::string::npos) {
    // Check for IPv6
    // IPv6 validation using the Client.h library which supports IPv6
    Client *client;
    if (client->connect(ipStr.c_str(),80)) { // Try connecting to the IP's web server
      client->stop();
      return true;
    }
  }
  return false;
}
//------------------------------------------------------------------------------------------------
inline String sanitizeHostname(String hostname) { // Sanitizes host names of invalid characters
  char sanitized[64] = {0}; // Max hostname length is 63 chars + null terminator
  int j = 0;
  bool lastWasPeriod = false; // Track consecutive periods

  for (int i = 0; i < hostname.length() && j < 63; i++) {
    char c = hostname[i];
    if (c == ' ') c = '-';
    // Allow letters (a-z, A-Z), digits (0-9), hyphen (-), and period (.)
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '.') {
      // Prevent consecutive periods or period after hyphen
      if (c == '.' && (lastWasPeriod || (j > 0 && sanitized[j-1] == '-'))) {
        continue; // Skip invalid period
      }
      // Prevent hyphen after period
      if (c == '-' && lastWasPeriod) {
        continue; // Skip invalid hyphen
      }
      sanitized[j++] = tolower(c); // Convert to lowercase
      lastWasPeriod = (c == '.');
    }
  }

  // Remove leading or trailing hyphen or period
  while (j > 0 && (sanitized[j-1] == '-' || sanitized[j-1] == '.')) {
    sanitized[--j] = '\0';
  }
  if (j > 0 && (sanitized[0] == '-' || sanitized[0] == '.')) {
    memmove(sanitized,sanitized + 1,j--);
  }

  // If empty after sanitization, return the default hostname
  if (j == 0) {
    return "cz-combo-hub-" + getBaseMacString();
  }

  return String(sanitized);
}
//------------------------------------------------------------------------------------------------
inline bool stringToDeviceAddress(const char* addressStr, uint8_t deviceAddress[8]) { // Converts DS18B20 colon delimited address string to an 8 byte array
  char temp[24];
  strncpy(temp, addressStr, sizeof(temp));
  temp[sizeof(temp) - 1] = '\0';

  char* token = strtok(temp, ":");
  int i = 0;
  while (token != NULL && i < 8) {
    deviceAddress[i] = (uint8_t)strtol(token, NULL, 16);
    token = strtok(NULL, ":");
    i++;
  }
  return (i == 8); // True if exactly 8 bytes parsed
}
//------------------------------------------------------------------------------------------------
