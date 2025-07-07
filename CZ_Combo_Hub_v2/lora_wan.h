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
  Serial2.print("AT+CPIN=" + LoRa_PW + "\r\n");
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
inline String handleSlaveRequest() { // Handle API requests from the master hub sent via LoRa WAN
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
inline void pingCheck() { // Check for LoRa pings from slave hubs and sends a response back
  long timeout = millis() + 5000; // 5-second timeout

  while (millis() < timeout) {
    if (Serial2.available()) {
      String incoming = Serial2.readStringUntil('\n');
      if ((incoming.startsWith("+RCV")) && (incoming.indexOf('PING') > 0)) {
        if ((Serial) && (ActiveMenu == 5)) Serial.println("[Ping received]");

        int firstComma = incoming.indexOf(',');
        String senderIDStr = incoming.substring(5,firstComma);

        if ((Serial) && (ActiveMenu == 5)) Serial.println("[Response sent to slave ID " + senderIDStr + "]");
        Serial2.print("AT+SEND=" + senderIDStr + ",4,PONG\r\n");
        delay(100);
        Serial2.readStringUntil('\n'); // Purge the +OK response

        return;
      }
    }
  }
}
//------------------------------------------------------------------------------------------------
inline bool pingMaster() { // Pings the master hub via LoRa WAN messsage
  long timeout = millis() + 5000; // 5-second timeout

  // Clear the modem buffer of any junk pinging the master hub
  while (Serial2.available()) Serial2.read();

  if ((Serial) && (ActiveMenu == 5)) Serial.println("[Pinging master hub]");
  Serial2.print("AT+SEND=1,4,PING\r\n");
  delay(100);
  Serial2.readStringUntil('\n'); // Purge the +OK response

  while (millis() < timeout) {
    if (Serial2.available()) {
      String incoming = Serial2.readStringUntil('\n');
      if ((incoming.startsWith("+RCV")) && (incoming.indexOf('PONG') > 0)) {
        if ((Serial) && (ActiveMenu == 5)) Serial.println("[Response received]");
        return true;
      }
    }
  }

  if ((Serial) && (ActiveMenu == 5)) Serial.println("[No response]");
  return false;
}
//------------------------------------------------------------------------------------------------
