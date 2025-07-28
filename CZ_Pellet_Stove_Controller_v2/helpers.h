//------------------------------------------------------------------------------------------------
// CZ Pellet Stove Controller v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------
inline String jsonSuccess = "{\"status\": \"success\",\"message\": \"Operation completed successfully\"}";
inline String jsonFailure = "{\"status\": \"error\",\"message\": \"Operation failed\"}";
//------------------------------------------------------------------------------------------------
inline float roundToOneDecimal(float value) { // Limits the precision of CtoF anf FtoC to one decimal place
  return (float)(int)(value * 10.0 + 0.5) / 10.0;
}
//------------------------------------------------------------------------------------------------
inline float CtoF(float celsius) { // Convert Celsius to Fahrenheit
  float result = (celsius * 9.0 / 5.0) + 32.0;
  return roundToOneDecimal(result);
}
//------------------------------------------------------------------------------------------------
inline float FtoC(float fahrenheit) { // Convert Fahrenheit to Celsius
  float result = (fahrenheit - 32.0) * 5.0 / 9.0;
  return roundToOneDecimal(result);
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
inline bool isValidIP(String ip) { // Validates IPv4 addresses used in static IP configuration
  std::string ipStr = ip.c_str();
  if (ipStr.find('.') != std::string::npos) {
    IPAddress ipv4Addr;
    if (ipv4Addr.fromString(ip)) {
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
    String Default = "CZ-Pellet-Stove-" + WiFi.macAddress();
    Default.replace(":","-");
    return Default;
  }

  return String(sanitized);
}
//------------------------------------------------------------------------------------------------
