//------------------------------------------------------------------------------------------------
// Climate Czar Combo Hub v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------
inline String jsonSuccess = "{\"status\": \"success\",\"message\": \"Operation completed successfully\"}";
inline String jsonFailure = "{\"status\": \"error\",\"message\": \"Operation failed\"}";
//------------------------------------------------------------------------------------------------
inline String QuerySlave(int ID, String Msg) {
  String Result = jsonFailure;
  long timeout = millis() + 4000; // 4-second timeout
  
  // Format and send the message: AT+SEND=address,length,message
  String command = "AT+SEND=" + String(ID) + "," + String(Msg.length()) + "," + Msg;
  Serial2.println(command);
  
  // Wait for Result
  while (millis() < timeout) {
    if (Serial2.available()) {
      String incoming = Serial2.readStringUntil('\n');
      // Check if the message is a received LoRa message
      if (incoming.startsWith("+RCV")) {
        // Parse the Result: +RCV=SenderID,length,message,RSSI,SNR
        int firstComma = incoming.indexOf(',');
        if (firstComma > 4) { // Ensure valid +RCV format
          String senderIDStr = incoming.substring(4,firstComma); // Extract SenderID
          int senderID = senderIDStr.toInt();
          // Only process if the sender matches the target ID
          if (senderID == ID) {
            int secondComma = incoming.indexOf(',',firstComma + 1);
            int thirdComma = incoming.indexOf(',',secondComma + 1);
            if (thirdComma > secondComma) {
              Result = incoming.substring(secondComma + 1,thirdComma);
              return Result;
            }
          }
        }
      }
    }
    yield(); // Prevent ESP32 watchdog timeout
  }
  
  return Result; // Return jsonFailure if no response
}
//------------------------------------------------------------------------------------------------
inline String getHumidity(int ID) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/humidity");
  } else {
    return GetDHT22(0);
  }
}
//------------------------------------------------------------------------------------------------
inline String getLightLevel(int ID, String Format) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/light/" + Format);
  } else {
    if (Format == "lux") {
      return GetBH1750(0);
    } else {
      return GetBH1750(1);
    }
  }
}
//------------------------------------------------------------------------------------------------
inline String getRemoteSensor(int ID, String Payload) {
  if (ID > 0) {
    return QuerySlave(ID,Payload);
  } else {
    return jsonFailure;
  }
}
//------------------------------------------------------------------------------------------------
inline String getTemperature(int ID, String Format) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/temperature/" + Format);
  } else {
    if (Format == "c") {
      return GetDHT22(2);
    } else {
      return GetDHT22(3);
    }
  }
}
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
  for (int i = 0; i < Msg.length(); i++) {
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
    partCount++;
    startIndex = endIndex + 1;
  }

  digitalWrite(LED,HIGH);
  // parts[0] : Device ID, 0=local, anything else is a LoRa slave number
  // parts[1] : The request type identifier
  // parts[2..(partCount-1)] : Any additional parameters for the request type 
  if (parts[1] == "devicename") {

  } else if (parts[1] == "ds18b20") {

  } else if (parts[1] == "humidity") {
    Result = getHumidity(parts[0].toInt());
  } else if (parts[1] == "light") {
    if (partCount == 3) Result = getLightLevel(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "lora-sensor") {
    if ((LoRa_Mode == 0) && (partCount == 3)) Result = getRemoteSensor(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "reboot") {
    Result = "Rebooting...";
  } else if (parts[1] == "relay") {

  } else if (parts[1] == "switch") {

  } else if (parts[1] == "temperature") {
    if (partCount == 3) Result = getTemperature(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "uptime") {

  } else if (parts[1] == "wifi-stats") {

  }
  digitalWrite(LED,LOW);

  return Result;
}
//------------------------------------------------------------------------------------------------
