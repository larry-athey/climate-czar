//------------------------------------------------------------------------------------------------
// Climate Czar Combo Hub v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//
// Example Climate Czar Server web API calls:
//
// curl -s -m 5 --http0.9 http://192.168.1.100/0/humidity (get humidity from the master hub)
// curl -s -m 5 --http0.9 http://192.168.1.100/2/humidity (get humidity from the first slave hub)
// curl -s -m 5 --http0.9 http://192.168.1.100/3/humidity (get humidity from the second slave hub)
//
//------------------------------------------------------------------------------------------------
inline String QuerySlave(int ID, String Msg) {
  String Result = jsonFailure;
  long timeout = millis() + 4000; // 4-second timeout
  slaveCount ++;

  // Clear the modem buffer of any junk before sending to the slave
  while (Serial2.available()) Serial2.read();
  
  // Format and send the message: AT+SEND=address,length,message
  String command = "AT+SEND=" + String(ID) + "," + String(Msg.length()) + "," + Msg;
  if ((Serial) && (ActiveMenu == 5)) Serial.println("S" + String(slaveCount) + "->: " + command);
  Serial2.print(command + "\r\n");
  delay(100);
  Serial2.readStringUntil('\n'); // Purge the +OK response
  
  // Wait for Result
  while (millis() < timeout) {
    if (Serial2.available()) {
      String incoming = Serial2.readStringUntil('\n');
      if ((Serial) && (ActiveMenu == 5)) Serial.println("Raw Msg: " + incoming);
      // Check if the message is a received LoRa message
      if (incoming.startsWith("+RCV")) {
        // Parse the Result: +RCV=SenderID,length,message,RSSI,SNR
        int firstComma = incoming.indexOf(',');
        if (firstComma > 4) { // Ensure valid +RCV format
          String senderIDStr = incoming.substring(5,firstComma); // Extract SenderID
          int senderID = senderIDStr.toInt();
          // Only process if the sender matches the target ID
          if (senderID == ID) {
            int secondComma = incoming.indexOf(',',firstComma + 1);
            int thirdComma = incoming.indexOf(',',secondComma + 1);
            if (thirdComma > secondComma) {
              Result = incoming.substring(secondComma + 1,thirdComma);
              if ((Serial) && (ActiveMenu == 5)) Serial.println("A" + String(apiCount) + "<-: " + Result);
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
inline String getDallasTemp(int ID, String Address, String Format) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/ds18b20/" + Address + "/" + Format);
  } else {
    return GetDS18B20(Address,Format);
  }
}
//------------------------------------------------------------------------------------------------
inline String getDeviceName(int ID) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/device-name");
  } else {
    return CZ_deviceName;
  }
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
    if (Format == "0") {
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
inline String getRelayState(int ID, String Relay) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/relay-state/" + Relay);
  } else {
    return GetRelay(Relay.toInt());
  }
}
//------------------------------------------------------------------------------------------------
inline String getSwitch(int ID, String Switch) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/switch/" + Switch);
  } else {
    return GetSwitch(Switch.toInt());
  }
}
//------------------------------------------------------------------------------------------------
inline String getTemperature(int ID, String Format) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/temperature/" + Format);
  } else {
    if (Format == "c") {
      return GetDHT22(1);
    } else {
      return GetDHT22(2);
    }
  }
}
//------------------------------------------------------------------------------------------------
inline String getUptime(int ID) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/uptime");
  } else {
    return Uptime;
  }
}
//------------------------------------------------------------------------------------------------
inline String getWifiStats(int ID) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/wifi-stats");
  } else {
    return "WiFi Channel: " + String(WiFi.channel()) + "\n" + "WiFi Signal: " + String(WiFi.RSSI());
  }
}
//------------------------------------------------------------------------------------------------
inline String rebootHub(int ID) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/reboot");
  } else {
    if (LoRa_Mode == 0) {
      return "Rebooting...";
    } else {
      return "Restarting...";
    }
  }
}
//------------------------------------------------------------------------------------------------
inline String setRelayState(int ID, String Relay, String State) {
  if (ID > 0) {
    return QuerySlave(ID,"/0/relay/" + Relay + "/" + State);
  } else {
    return SetRelay(Relay.toInt(),State.toInt());
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

  if (parts[0].toInt() == 1) return Result; // LoRa ID 1 is the master hub and must be addressed as unit zero

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
  } else if (parts[1] == "switch") {
    if (partCount == 3) Result = getSwitch(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "temperature") {
    if (partCount == 3) Result = getTemperature(parts[0].toInt(),parts[2]);
  } else if (parts[1] == "uptime") {
    if (partCount == 2) Result = getUptime(parts[0].toInt());
  } else if (parts[1] == "wifi-stats") {
    if (partCount == 2) Result = getWifiStats(parts[0].toInt());
  }

  apiCount ++;
  return Result;
}
//------------------------------------------------------------------------------------------------
