//------------------------------------------------------------------------------------------------
// Climate Czar Combo Hub v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------
inline void LoRa_Init() { // Initialize the RYLR998 modem after configuration changes
  Serial2.println(F("AT+RESET"));
  delay(500);
  Serial2.println("AT+NETWORKID=" + LoRa_Network);
  delay(100);
  Serial2.println("AT+ADDRESS=" + LoRa_Address);
  delay(100);
}
//------------------------------------------------------------------------------------------------
inline String handleSlaveRequest() {
  String Result = jsonFailure;

  String incoming = Serial2.readStringUntil('\n');
  if ((Serial) && (ActiveMenu == 5)) Serial.println(incoming);
  // Check if the message is a received LoRa message
  if (incoming.startsWith("+RCV")) {
    // Parse the Result: +RCV=SenderID,length,message,RSSI,SNR
    int firstComma = incoming.indexOf(',');
    if (firstComma > 4) { // Ensure valid +RCV format
      String senderIDStr = incoming.substring(4,firstComma); // Extract SenderID
      int senderID = senderIDStr.toInt();
          
      // Only process if the sender is the master hub (ID 1)
      if (senderID == 1) {
        int secondComma = incoming.indexOf(',',firstComma + 1);
        int thirdComma = incoming.indexOf(',',secondComma + 1);
        if (thirdComma > secondComma) {
          String message = incoming.substring(secondComma + 1,thirdComma);
              
          // Debugging ACK message
          //String reply = "ACK:" + CZ_deviceName;
          //String command = "AT+SEND=1," + String(reply.length()) + "," + reply;
          //Serial2.println(command);
              
          Result = handleWebRequest(message);
          return Result; // Return the master hub's request
        }
      }
    }
  }

  return Result;
}
//------------------------------------------------------------------------------------------------
