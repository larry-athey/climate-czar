//------------------------------------------------------------------------------------------------
// CZ Pellet Stove Controller v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------
inline String getForm(byte ID) { // Used by the web UI to load configuration settings forms
  String Content = "";
  Content = get_Form(ID);

}
//------------------------------------------------------------------------------------------------
inline String resetController() { // Emergency stove controller reset
  OpMode = 0;
  StartTime = 0;
  TargetTime = 0;
  HighBurn = false;
  FEED_TIME = feedRateLow * 1000;
  timerAlarmDisable(timer);
  digitalWrite(TOP_AUGER,LOW);  
  digitalWrite(BOTTOM_AUGER,LOW);
  digitalWrite(COMBUSTION_BLOWER,LOW);
  digitalWrite(ROOM_BLOWER,LOW);
  digitalWrite(IGNITOR,LOW);
  SetMemory();
  Countdown = "00:00";
  Runtime = "00:00:00";
  Status = "Stove controller reset by API";
  return jsonSuccess;
}
//------------------------------------------------------------------------------------------------
inline String setTempMode(String Mode) {
  if (Mode == "c") {
    TemperatureMode = 1; 
  } else {
    TemperatureMode = 0; 
  }
  return jsonSuccess;
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
  if (parts[0] == "bottom-auger") {
    if (partCount == 2) Result = "/bottom-auger/0-or-1";
  } else if (parts[0] == "burn-mode") {
    if (partCount == 2) Result = "/burn-mode/0-or-1";
  } else if (parts[0] == "combustion") {
    if (partCount == 2) Result = "/combustion/0-or-1";
  } else if (parts[0] == "countdown") {
    if (partCount == 1) Result = Countdown;
  } else if (parts[0] == "cz-stats") {
    if (partCount == 1) Result = "/cz-stats";
  } else if (parts[0] == "feed-high") {
    if (partCount == 2) Result = "/feed-high/seconds";
  } else if (parts[0] == "feed-low") {
    if (partCount == 2) Result = "/feed-low/seconds";
  } else if (parts[0] == "form") {
    if (partCount == 2) Result = getForm(parts[1].toInt());
  } else if (parts[0] == "ignitor") {
    if (partCount == 2) Result = "/ignitor/0-or-1";
  } else if (parts[0] == "max-temp") {
    if (partCount == 2) Result = "/max-temp/temp-c-or-f";
  } else if (parts[0] == "min-temp") {
    if (partCount == 2) Result = "/min-temp/temp-c-or-f";
  } else if (parts[0] == "op-mode") {
    if (partCount == 1) Result = "/op-mode";
  } else if (parts[0] == "reboot") {
    if (partCount == 1) Result = "Rebooting...";
  } else if (parts[0] == "reset") {
    if (partCount == 1) Result = resetController();
  } else if (parts[0] == "room-blower") {
    if (partCount == 2) Result = "/room-blower/0-or-1";
  } else if (parts[0] == "room-temp") {
    if (partCount == 1) Result = "/room-temp";
  } else if (parts[0] == "runtime") {
    if (partCount == 1) Result = Runtime;
  } else if (parts[0] == "shutdown") {
    if (partCount == 1) Result = stoveShutdown();
  } else if (parts[0] == "startup") {
    if (partCount == 1) Result = stoveStartup();
  } else if (parts[0] == "startup-timer") {
    if (partCount == 2) Result = "/startup-timer/seconds";
  } else if (parts[0] == "stove-temp") {
    if (partCount == 1) Result = "/stove-temp";
  } else if (parts[0] == "temp-mode") {
    if (partCount == 2) Result = setTempMode(parts[1]);
  } else if (parts[0] == "therm-temp") {
    if (partCount == 2) Result = "/therm-temp/temp-c-or-f"
  } else if (parts[0] == "thermostat") {
    if (partCount == 2) Result = "/thermostat/0-or-1"
  } else if (parts[0] == "top-auger") {
    if (partCount == 2) Result = "/top-auger/0-or-1";
  } else if (parts[0] == "uptime") {
    if (partCount == 1) Result = Uptime;
  } else if (parts[0] == "wifi-stats") {
    if (partCount == 1) Result = "/wifi-stats";
  } else {
    Result = HomePage(); // Web user request, not an API call, send the home page
  }

  return Result;
}
//------------------------------------------------------------------------------------------------
