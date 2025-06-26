//------------------------------------------------------------------------------------------------
// Climate Czar Combo Hub v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------
inline void LoRa_Init() { // Initialize the RYLR998 modem after configuration changes
  if (Serial) Serial.println(F("Initializing the RYLR998 modem..."));
  Serial2.print(F("AT+FACTORY\r\n"));
  delay(1000);
  echoRYLR998();
  Serial2.print(F("AT+RESET\r\n"));
  delay(200);
  echoRYLR998();
  Serial2.print("AT+ADDRESS=" + String(LoRa_Address) + "\r\n");
  delay(200);
  echoRYLR998();
  Serial2.print("AT+NETWORKID=" + String(LoRa_Network) + "\r\n");
  delay(200);
  echoRYLR998();
  Serial2.print(F("AT+BAND=915000000\r\n"));
  delay(200);
  echoRYLR998();
  Serial2.print(F("AT+IPR=115200\r\n"));
  delay(200);
  echoRYLR998();
  Serial2.print(F("AT+PARAMETER=9,7,1,12\r\n"));
  delay(200);
  echoRYLR998();
}
//------------------------------------------------------------------------------------------------
inline String handleSlaveRequest() {
  String Result = "";

  String incoming = Serial2.readStringUntil('\n');
  if ((Serial) && (ActiveMenu == 5)) Serial.println("Raw Msg: " + incoming);
  // Check if the message is a received LoRa message
  if (incoming.startsWith("+RCV")) {
    // Parse the Result: +RCV=SenderID,length,message,RSSI,SNR
    int firstComma = incoming.indexOf(',');
    if (firstComma > 4) { // Ensure valid +RCV format
      String senderIDStr = incoming.substring(5,firstComma); // Extract SenderID
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

          slaveCount ++;
          if ((Serial) && (ActiveMenu == 5)) Serial.println("S" + String(slaveCount) + "<-: " + message);
          Result = handleWebRequest(message);
          return Result; // Return the master hub's request
        }
      }
    }
  }

  return Result;
}
//------------------------------------------------------------------------------------------------
