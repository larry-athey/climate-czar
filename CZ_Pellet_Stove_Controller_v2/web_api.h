//------------------------------------------------------------------------------------------------
// CZ Pellet Stove Controller v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
inline String handleWebRequest(String Msg) {
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

  if (parts[0].length() == 0) {
    for (int i = 0; i < partCount; i ++) {
      parts[i] = parts[i + 1];
    }
  }

/*
  // parts[0] : Device ID, 0=local, anything else is a LoRa slave number
  // parts[1] : The request type identifier
  // parts[2..(partCount-1)] : Any additional parameters for the request type 
  if (parts[1] == "device-name") {
    if (partCount == 2) Result = getDeviceName(parts[0].toInt());
  } else if (parts[1] == "ds18b20") {
    if (partCount == 4) Result = getDallasTemp(parts[0].toInt(),parts[2],parts[3]);
  } else if (parts[1] == "humidity") {
    if (partCount == 2) Result = getHumidity(parts[0].toInt());
  } else if (parts[1] == "light") {
    if (partCount == 3) Result = getLightLevel(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "lora-sensor") {
    if ((LoRa_Mode == 0) && (partCount == 3)) Result = getRemoteSensor(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "reboot") {
    if (partCount == 2) Result = rebootHub(parts[0].toInt());
  } else if (parts[1] == "relay") {
    if (partCount == 4) Result = setRelayState(parts[0].toInt(),parts[2],parts[3]);
  } else if (parts[1] == "relay-state") {
    if (partCount == 3) Result = getRelayState(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "sensor-address") {
    if (partCount == 3) Result = getSensorAddress(parts[0].toInt(),parts[2].toInt());
  } else if (parts[1] == "sensor-count") {
    if (partCount == 2) Result = getSensorCount(parts[0].toInt());
  } else if (parts[1] == "switch") {
    if (partCount == 3) Result = getSwitch(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "temperature") {
    if (partCount == 3) Result = getTemperature(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "uptime") {
    if (partCount == 2) Result = getUptime(parts[0].toInt());
  } else if (parts[1] == "wifi-stats") {
    if (partCount == 2) Result = getWifiStats(parts[0].toInt());
  }
*/
  return Result;
}
//------------------------------------------------------------------------------------------------
