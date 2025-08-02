//------------------------------------------------------------------------------------------------
// CZ Pellet Stove Controller v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------
inline String czStats() {
  String Content = "";
  Content += "<div class=\"container\" style=\"font-size: 0.93em;\">\n";
  Content += LiveData(1);
  Content += "</div>\n";
  return Content;
}
//------------------------------------------------------------------------------------------------
inline String getRoomTemp() {
  if (TemperatureMode == 0) {
    return String(roomTempF);
  } else {
    return String(roomTempC);
  }
}
//------------------------------------------------------------------------------------------------
inline String getStoveTemp() {
  if (TemperatureMode == 0) {
    return String(stoveTempF);
  } else {
    return String(stoveTempC);
  }
}
//------------------------------------------------------------------------------------------------
inline String getWifiStats() {
  return "WiFi Channel: " + String(WiFi.channel()) + "\n" + "WiFi Signal: " + String(WiFi.RSSI());
}
//------------------------------------------------------------------------------------------------
inline String jsonStats() {
  String Content = "";
  Content += "{\"op_mode\":\"" + String(OpMode) + "\",";
  //Content += "\"var\":\"" + String(var) + "\",";
  Content += "\"uptime\":\"" + Uptime + "\",";
  Content += "\"runtime\":\"" + Runtime + "\",";
  Content += "\"countdown\":\"" + Countdown + "\",";
  Content += "\"stove_temp_c\":\"" + String(stoveTempC,1) + "\",";
  Content += "\"stove_temp_f\":\"" + String(stoveTempF,1) + "\",";
  if (UseThermostat) {
    Content += "\"room_temp_c\":\"" + String(roomTempC,1) + "\",";
    Content += "\"room_temp_f\":\"" + String(roomTempF,1) + "\",";
  }
  if (HighBurn) {
    Content += "\"high_burn\":\"true\",";
  } else {
    Content += "\"high_burn\":\"false\",";
  }
  Content += "\"status\":\"" + Status + "\"}\n";
  return Content;
}
//------------------------------------------------------------------------------------------------
inline String resetController() {
  OpMode = 0;
  SetMemory();
  StartTime = 0;
  TargetTime = 0;
  HighBurn = false;
  gpioToggle = false;
  FEED_TIME = feedRateLow * 1000;
  timerAlarmDisable(timer);
  digitalWrite(TOP_AUGER,LOW);  
  digitalWrite(BOTTOM_AUGER,LOW);
  digitalWrite(COMBUSTION_BLOWER,LOW);
  digitalWrite(ROOM_BLOWER,LOW);
  digitalWrite(IGNITOR,LOW);
  Countdown = "00:00";
  Runtime = "00:00:00";
  Status = "Stove controller reset by API";
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setBottomAuger(byte Mode) {
  if (OpMode == 0) {
    if (Mode > 1) Mode = 1;
    digitalWrite(BOTTOM_AUGER,Mode);
    return jsonSuccess;
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String setBurnMode(byte Mode) {
  if ((OpMode == 1) || (OpMode == 2)) {
    if (Mode == 1) {
      HighBurn = true;
      gpioToggle = false;
      FEED_TIME = feedRateHigh * 1000;
      PopoverMessage("High burn mode activated");
    } else {
      HighBurn = false;
      FEED_TIME = feedRateLow * 1000;
      PopoverMessage("Idle burn mode activated");
    }
    return jsonSuccess;
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String setCombustionBlower(byte Mode) {
  if (OpMode == 0) {
    if (Mode > 1) Mode = 1;
    digitalWrite(COMBUSTION_BLOWER,Mode);
    return jsonSuccess;
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String setFeedHigh(float Time) {
  feedRateHigh = roundToOneDecimal(Time);
  if (HighBurn) FEED_TIME = feedRateHigh * 1000;
  SetMemory();
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setFeedLow(float Time) {
  feedRateLow = roundToOneDecimal(Time);
  if (! HighBurn) FEED_TIME = feedRateLow * 1000;
  SetMemory();
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setIgnitor(byte Mode) {
  if (OpMode == 0) {
    if (Mode > 1) Mode = 1;
    digitalWrite(IGNITOR,Mode);
    return jsonSuccess;
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String setMaxTemp(float Temp) {
  if (TemperatureMode == 0) {
    maxTempF = roundToOneDecimal(Temp);
    maxTempC = FtoC(Temp);
  } else {
    maxTempC = roundToOneDecimal(Temp);
    maxTempF = CtoF(Temp);
  }
  SetMemory();
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setMinTemp(float Temp) {
  if (TemperatureMode == 0) {
    minTempF = roundToOneDecimal(Temp);
    minTempC = FtoC(Temp);
  } else {
    minTempC = roundToOneDecimal(Temp);
    minTempF = CtoF(Temp);
  }
  SetMemory();
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setRoomBlower(byte Mode) {
  if (OpMode == 0) {
    if (Mode > 1) Mode = 1;
    digitalWrite(ROOM_BLOWER,Mode);
    return jsonSuccess;
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String setStartupTimer(int Time) {
  StartupTimer = Time;
  SetMemory();
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setTempMode(String Mode) {
  if (Mode == "c") {
    TemperatureMode = 1;
  } else {
    TemperatureMode = 0;
  }
  SetMemory();
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setThermTemp(float Temp) {
  if (TemperatureMode == 0) {
    targetTempF = roundToOneDecimal(Temp);
    targetTempC = FtoC(Temp);
  } else {
    targetTempC = roundToOneDecimal(Temp);
    targetTempF = CtoF(Temp);
  }
  SetMemory();
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setThermostat(byte Mode) {
  if (Mode == 0) {
    UseThermostat = 0;
  } else {
    UseThermostat = 1;
  }
  SetMemory();
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setTopAuger(byte Mode) {
  if (OpMode == 0) {
    if (Mode > 1) Mode = 1;
    digitalWrite(TOP_AUGER,Mode);
    return jsonSuccess;
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String stoveShutdown() {
  if ((OpMode == 1) || (OpMode == 2)) {
    ToggleRunState(false);
    return jsonSuccess;
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String stoveStartup() {
  if ((OpMode == 0) || (OpMode == 4)) {
    ToggleRunState(true);
    return jsonSuccess;
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String toggleRun() {
  if ((OpMode == 0) || (OpMode == 4)) {
    ToggleRunState(true);
    return jsonSuccess;
  } else if ((OpMode == 1) || (OpMode == 2)) {
    ToggleRunState(false);
    return jsonSuccess;
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String handleWebRequest(String Msg) { // The web API request handler
  String Result = jsonFailure;

  Msg.trim();
  if (Msg.length() == 0) return Result;

  while (Msg.endsWith("/")) {
    Msg = Msg.substring(0,Msg.length() - 1);
  }

  // Count "/" delimiters
  int delimiterCount = 0;
  for (int i = 0; i < Msg.length(); i ++) {
    if (Msg[i] == '/') delimiterCount++;
  }

  // Create an array for the parts
  String parts[delimiterCount + 1];
  int partCount = 0;
  int startIndex = 0;

  // Split the Msg string
  while (startIndex < Msg.length()) {
    int endIndex = Msg.indexOf('/',startIndex);
    if (endIndex == -1) {
      parts[partCount] = Msg.substring(startIndex);
      break;
    }
    parts[partCount] = Msg.substring(startIndex,endIndex);
    partCount ++;
    startIndex = endIndex + 1;
  }

  // Some home automation systems add two forward slashes at the beginning, why?
  if (parts[0].length() == 0) {
    for (int i = 0; i < partCount; i ++) {
      parts[i] = parts[i + 1];
    }
  }

  // parts[0] : The request type identifier
  // parts[1..(partCount-1)] : Any additional parameters for the request type
  if (parts[0] == "ajax-livedata") {
    if (partCount == 1) Result = LiveData(0);
  } else if (parts[0] == "ajax-settings") {
    if (partCount == 1) Result = SettingsData();
  } else if (parts[0] == "bottom-auger") {
    if (partCount == 2) Result = setBottomAuger(parts[1].toInt());
  } else if (parts[0] == "burn-mode") {
    if (partCount == 2) Result = setBurnMode(parts[1].toInt());
  } else if (parts[0] == "combustion") {
    if (partCount == 2) Result = setCombustionBlower(parts[1].toInt());
  } else if (parts[0] == "countdown") {
    if (partCount == 1) Result = Countdown;
  } else if (parts[0] == "cz-stats") {
    if (partCount == 1) Result = czStats();
  } else if (parts[0] == "feed-high") {
    if (partCount == 2) Result = setFeedHigh(parts[1].toFloat());
  } else if (parts[0] == "feed-low") {
    if (partCount == 2) Result = setFeedLow(parts[1].toFloat());
  } else if (parts[0] == "form") {
    if (partCount == 2) Result = getForm(parts[1].toInt());
  } else if (parts[0] == "ignitor") {
    if (partCount == 2) Result = setIgnitor(parts[1].toInt());
  } else if (parts[0] == "json-stats") {
    if (partCount == 1) Result = jsonStats();
  } else if (parts[0] == "max-temp") {
    if (partCount == 2) Result = setMaxTemp(parts[1].toFloat());
  } else if (parts[0] == "min-temp") {
    if (partCount == 2) Result = setMinTemp(parts[1].toFloat());
  } else if (parts[0] == "op-mode") {
    if (partCount == 1) Result = String(OpMode);
  } else if (parts[0] == "reboot") {
    if (partCount == 1) Result = "Rebooting...";
  } else if (parts[0] == "reset") {
    if (partCount == 1) Result = resetController();
  } else if (parts[0] == "room-blower") {
    if (partCount == 2) Result = setRoomBlower(parts[1].toInt());
  } else if (parts[0] == "room-temp") {
    if (partCount == 1) Result = getRoomTemp();
  } else if (parts[0] == "runtime") {
    if (partCount == 1) Result = Runtime;
  } else if (parts[0] == "shutdown") {
    if (partCount == 1) Result = stoveShutdown();
  } else if (parts[0] == "startup") {
    if (partCount == 1) Result = stoveStartup();
  } else if (parts[0] == "startup-timer") {
    if (partCount == 2) Result = setStartupTimer(parts[1].toInt());
  } else if (parts[0] == "stove-temp") {
    if (partCount == 1) Result = getStoveTemp();
  } else if (parts[0] == "temp-mode") {
    if (partCount == 2) Result = setTempMode(parts[1]);
  } else if (parts[0] == "therm-temp") {
    if (partCount == 2) Result = setThermTemp(parts[1].toFloat());
  } else if (parts[0] == "thermostat") {
    if (partCount == 2) Result = setThermostat(parts[1].toInt());
  } else if (parts[0] == "toggle-run") {
    if (partCount == 1) Result = toggleRun();
  } else if (parts[0] == "top-auger") {
    if (partCount == 2) Result = setTopAuger(parts[1].toInt());
  } else if (parts[0] == "uptime") {
    if (partCount == 1) Result = Uptime;
  } else if (parts[0] == "wifi-stats") {
    if (partCount == 1) Result = getWifiStats();
  } else {
    Result = HomePage(); // Web user request, not an API call, send the home page
  }

  return Result;
}
//------------------------------------------------------------------------------------------------
