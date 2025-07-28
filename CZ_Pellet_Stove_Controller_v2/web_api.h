//------------------------------------------------------------------------------------------------
// CZ Pellet Stove Controller v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------

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

  // parts[0] : The request type identifier
  // parts[1..(partCount-1)] : Any additional parameters for the request type 
  if (parts[0] == "countdown") {
    if (partCount == 1) Result = Countdown;
  } else if (parts[0] == "reboot") {
    if (partCount == 1) Result = "Rebooting...";
  } else if (parts[0] == "runtime") {
    if (partCount == 1) Result = Runtime;
  } else if (parts[0] == "uptime") {
    if (partCount == 1) Result = Uptime;
  } else {
    // Return the web UI home page
    Result = HomePage();
  }

  return Result;
}
//------------------------------------------------------------------------------------------------
