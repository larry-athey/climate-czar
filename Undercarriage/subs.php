<?php
//---------------------------------------------------------------------------------------------------
require_once("CZ_TZ.php");
date_default_timezone_set($TZ);
//---------------------------------------------------------------------------------------------------
//ini_set("display_errors",1);
//ini_set("error_reporting",E_ALL);
ini_set("display_errors",1);
ini_set("display_startup_errors",1);
error_reporting(E_ALL);
//---------------------------------------------------------------------------------------------------
// Security level hold-overs from the original Wordpress plugin
define("anonymous",0);       // Public access
define("subscriber",20);     // Change groups
define("contributor",40);    // Control switching [default value]
define("author",60);         // Manage devices
define("editor",80);         // Manage scripts
define("administrator",100); // Manage users
//---------------------------------------------------------------------------------------------------
define("VERSION","1.3.0");
define("APP_ROOT",realpath(__DIR__) . DIRECTORY_SEPARATOR);

define("DB_HOST","localhost");
define("DB_NAME","ClimateCzar");
define("DB_USER","czdbuser");
define("DB_PASS","Cl!m@t3cZ4r");
//---------------------------------------------------------------------------------------------------
if (! defined("CZ_BASIC")) { // Toggle simplified mobile view
  if (isset($_COOKIE["CZ_BASIC"])) {
    if ($_COOKIE["CZ_BASIC"] == 1) {
      define("CZ_BASIC",true);
    } else {
      define("CZ_BASIC",false);
    }
  } else {
    setcookie("CZ_BASIC",0,0,"/");
    define("CZ_BASIC",false);
  }
}
/***************************************************************************************************/
if (! defined("CZ_GROUP")) { // Set the selected device group
  if (isset($_COOKIE["CZ_GROUP"])) {
    define("CZ_GROUP",$_COOKIE["CZ_GROUP"]);
  } else {
    setcookie("CZ_GROUP",1,0,"/");
    define("CZ_GROUP",1);
  }
}
/***************************************************************************************************/
if (! defined("CZ_THEME")) { // Set light or dark theme
  if (isset($_COOKIE["CZ_THEME"])) {
    define("CZ_THEME",$_COOKIE["CZ_THEME"]);
  } else {
    setcookie("CZ_THEME",0,0,"/");
    define("CZ_THEME",0);
  }
}
//---------------------------------------------------------------------------------------------------
function DeviceSec($SecLevel) {
  if ($SecLevel == "anonymous") { return anonymous; }
  elseif ($SecLevel == "subscriber") { return subscriber; }
  elseif ($SecLevel == "contributor") { return contributor; }
  elseif ($SecLevel == "author") { return author; }
  elseif ($SecLevel == "editor") { return editor; }
  elseif ($SecLevel == "administrator") { return administrator; }
}
//---------------------------------------------------------------------------------------------------
function UserSec($DBcnx) {
  if ((isset($_COOKIE["CZ_USER"])) && (isset($_COOKIE["CZ_TOKEN"]))) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM Users WHERE ID=" . $_COOKIE["CZ_USER"]);
    $User = mysqli_fetch_assoc($Result);

    if ($User["SecLevel"] == "subscriber") { return subscriber; }
    elseif ($User["SecLevel"] == "contributor") { return contributor; }
    elseif ($User["SecLevel"] == "author") { return author; }
    elseif ($User["SecLevel"] == "editor") { return editor; }
    elseif ($User["SecLevel"] == "administrator") { return administrator; }
  } else {
    return anonymous;
  }
}
//---------------------------------------------------------------------------------------------------
function UserLoggedIn($DBcnx) {
  if ((isset($_COOKIE["CZ_USER"])) && (isset($_COOKIE["CZ_TOKEN"]))) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM Users WHERE ID=" . $_COOKIE["CZ_USER"]);
    $User = mysqli_fetch_assoc($Result);
    if (mysqli_num_rows($Result) > 0) {
      if ($User["LockedOut"] == 1) {
        setcookie("CZ_USER","",time() - 3600,"/");
        setcookie("CZ_TOKEN","",time() - 3600,"/");
        setcookie("CZ_GROUP","",time() - 3600,"/");
        define("LOGGED_IN_KEY","NOT_LOGGED_IN");
        return false;
      } else {
        $Key = md5($User["UN"] . ":" . $User["PW"]);
        if ($Key == $_COOKIE["CZ_TOKEN"]) {
          $Result = mysqli_query($DBcnx,"UPDATE Users SET LastActivity=now() WHERE ID=" . $_COOKIE["CZ_USER"]);
          define("LOGGED_IN_KEY",$Key);
          return true;
        } else {
          setcookie("CZ_USER","",time() - 3600,"/");
          setcookie("CZ_TOKEN","",time() - 3600,"/");
          setcookie("CZ_GROUP","",time() - 3600,"/");
          define("LOGGED_IN_KEY","NOT_LOGGED_IN");
          return false;
        }
      }
    } else {
      define("LOGGED_IN_KEY","NOT_LOGGED_IN");
      return false;
    }
  } else {
    define("LOGGED_IN_KEY","NOT_LOGGED_IN");
    return false;
  }
}
//---------------------------------------------------------------------------------------------------
function FormatSensor($DBcnx,$InputID) {
  $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=$InputID");
  if (mysqli_num_rows($Result) > 0) {
    $Sensor = mysqli_fetch_assoc($Result);
    return "<span class=\"text-primary\">" .$Sensor["DeviceName"] . "</span>";
  } else {
    return "<span class=\"text-danger\">None Found</span>";
  }
}
//---------------------------------------------------------------------------------------------------
function FormatBinarySensor($DBcnx,$InputID) {
  $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=$InputID");
  if (mysqli_num_rows($Result) > 0) {
    $Sensor = mysqli_fetch_assoc($Result);
    if ($Sensor["Reading"] == 1) {
      return "<span class=\"text-success\">HIGH</span>";
    } else {
      return "<span class=\"text-danger\">LOW</span>";
    }
  }
}
//---------------------------------------------------------------------------------------------------
function FormatRange($Value) {
  $Result = "<span class=\"text-primary\">$Value</span>";
  return $Result;
}
//---------------------------------------------------------------------------------------------------
function FormatPolarity($Value) {
  if ($Value == 0) {
    $Result = "<span class=\"text-primary\">High=On / Low=Off</span>";
  } else {
    $Result = "<span class=\"text-primary\">Low=On / High=Off</span>";
  }
  return $Result;
}
//---------------------------------------------------------------------------------------------------
function FormatMode($Value) {
  if ($Value == 0) {
    $Result = "<span class=\"text-primary\">Automatic</span>";
  } elseif ($Value == 1) {
    $Result = "<span class=\"text-success blink\">Manual On</span>";
  } else {
    $Result = "<span class=\"text-danger blink\">Disabled</span>";
  }
  return $Result;
}
//---------------------------------------------------------------------------------------------------
function FormatStatus($Value) {
  if ($Value == 0) {
    $Result = "<span class=\"text-primary\" style=\"font-weight: bold;\">OFF</span>";
  } else {
    $Result = "<span class=\"text-success text-opacity-75 blink\" style=\"font-weight: bold;\">ON</span>";
  }
  return $Result;
}
//---------------------------------------------------------------------------------------------------
function ChartsDeviceSelector($DBcnx,$CZ_GROUP) {
  $Selector  = "<select size=\"1\" class=\"form-control form-select\" id=\"ChartsDeviceSelector\" name=\"ChartsDeviceSelector\">";
  $Selector .= "<option selected value=\"OV\">System Overview</option>";

  $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE GroupID=$CZ_GROUP ORDER BY DeviceName");
  if (mysqli_num_rows($Result) > 0) {
    while ($RS = mysqli_fetch_assoc($Result)) {
      $Selector .= "<option value=\"I" . $RS["ID"] . "\">Input: " . $RS["DeviceName"] . "</option>";
    }
  }
  $Result = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE GroupID=$CZ_GROUP ORDER BY DeviceName");
  if (mysqli_num_rows($Result) > 0) {
    while ($RS = mysqli_fetch_assoc($Result)) {
      $Selector .= "<option value=\"O" . $RS["ID"] . "\">Output: " . $RS["DeviceName"] . "</option>";
    }
  }
  return $Selector . "</select>";
}
//---------------------------------------------------------------------------------------------------
function DeviceListSelector($DBcnx,$CZ_GROUP,$Selected) {
  $Result = mysqli_query($DBcnx,"SELECT * FROM DeviceGroups WHERE ID=$CZ_GROUP");
  $RS     = mysqli_fetch_assoc($Result);
  $Group  = $RS["Name"];
  if ($Selected == 0) {
    $S0 = "selected";
    $S1 = "";
  } else {
    $S0 = "";
    $S1 = "selected";
  }
  $Content  = "<form class=\"form-inline\" method=\"post\" action=\"index.php?page=list-devices\">";
  $Content .= "<select style=\"width: 100%;margin-bottom: 0.5em;\" size=\"1\" class=\"form-control form-select\" id=\"DevType\" name=\"DevType\" onChange=\"this.form.submit()\">";
  $Content .= "<option $S0 value=\"0\">Input Sensors : $Group</option>";
  $Content .= "<option $S1 value=\"1\">Output Switches : $Group</option>";
  $Content .= "</select>";
  $Content .= "</form>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function YNSelector($Selected,$ID) {
  if ($Selected == 0) {
    $S0 = "selected";
    $S1 = "";
  } else {
    $S0 = "";
    $S1 = "selected";
  }
  $Content  = "<select style=\"width: 100%;margin-bottom: 0.5em;\" size=\"1\" class=\"form-control form-select\" id=\"$ID\" name=\"$ID\" aria-describedby=\"$ID" . "Help\">";
  $Content .= "<option $S1 value=\"1\">Yes</option>";
  $Content .= "<option $S0 value=\"0\">No</option>";
  $Content .= "</select>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function OnOffSelector($Selected,$ID) {
  if ($Selected == 0) {
    $S0 = "selected";
    $S1 = "";
  } else {
    $S0 = "";
    $S1 = "selected";
  }
  $Content  = "<select style=\"width: 100%;margin-bottom: 0.5em;\" size=\"1\" class=\"form-control form-select\" id=\"$ID\" name=\"$ID\">";
  $Content .= "<option $S1 value=\"1\">On</option>";
  $Content .= "<option $S0 value=\"0\">Off</option>";
  $Content .= "</select>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function PolaritySelector($Selected) {
  if ($Selected == 0) {
    $S0 = "selected";
    $S1 = "";
  } else {
    $S0 = "";
    $S1 = "selected";
  }
  $Content  = "<select style=\"width: 100%;margin-bottom: 0.5em;\" size=\"1\" class=\"form-control form-select\" id=\"Polarity\" name=\"Polarity\">";
  $Content .= "<option $S1 value=\"1\">Low=On / High=Off</option>";
  $Content .= "<option $S0 value=\"0\">High=On / Low=Off</option>";
  $Content .= "</select>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function DeviceTypeSelector($Selected) {
  $DType[] = "";
  $DType[] = "Variable Value Sensor";
  $DType[] = "Average of Variable Value Sensors";
  $DType[] = "Remote Binary (ON/OFF) Switch";
  $DType[] = "AND Gate of Remote Binary Switches";
  $DType[] = "OR Gate of Remote Binary Switches";
  $DType[] = "NOT Gate of Remote Binary Switches";
  $DType[] = "Daily Scheduler";
  $DType[] = "Web Console ON/OFF Switch";
  $DType[] = "Raw Text Output Device";
  $Content = "<select style=\"width: 100%;margin-bottom: 0.5em;\" size=\"1\" class=\"form-control form-select\" id=\"DeviceType\" name=\"DeviceType\" onChange=\"ToggleInputFields(this)\">";
  for ($x = 1; $x <= 9; $x ++) {
    if ($x == $Selected) {
      $Content .= "<option selected value=\"$x\">$DType[$x]</option>";
    } else {
      $Content .= "<option value=\"$x\">$DType[$x]</option>";
    }
  }
  $Content .= "</select>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function SwitchSecSelector($Selected) {
  $Level[] = "anonymous";
  $Level[] = "subscriber";
  $Level[] = "contributor";
  $Level[] = "author";
  $Level[] = "editor";
  $Level[] = "administrator";
  $Content = "<select style=\"width: 100%;margin-bottom: 0.5em;\" size=\"1\" class=\"form-control form-select\" id=\"SwitchSec\" name=\"SwitchSec\">";
  for ($x = 1; $x <= 5; $x ++) {
    if ($Level[$x] == $Selected) {
      $Content .= "<option selected value=\"$Level[$x]\">" . ucwords($Level[$x]) . "</option>";
    } else {
      $Content .= "<option value=\"$Level[$x]\">" . ucwords($Level[$x]) . "</option>";
    }
  }
  $Content .= "</select>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function ArraySelector($DBcnx,$Type,$ID,$Value) {
  if (trim($Value) == "") $Value = "0";
  if (InStr("|",$Value)) {
    $Sensor = explode("|",$Value);
  } else {
    $Sensor[0] = $Value;
  }
  if ($Type == 1) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE DeviceType=1 AND GroupID=" . $_COOKIE["CZ_GROUP"] . " ORDER BY DeviceName");
  } else {
    $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE DeviceType=3 AND GroupID=" . $_COOKIE["CZ_GROUP"] . " ORDER BY DeviceName");
  }
  $Content = "<select multiple style=\"width: 100%;margin-bottom: 0.5em;\" size=\"4\" class=\"form-control form-select\" id=\"$ID\" name=\"$ID" . "[]\" aria-describedby=\"$ID" ."Help\">";
  if (mysqli_num_rows($Result) > 0) {
    while ($Dev = mysqli_fetch_assoc($Result)) {
      $Content .= "<option ";
      for ($x = 0; $x <= (count($Sensor) - 1); $x ++) {
        if ($Sensor[$x] == $Dev["ID"]) $Content .= "selected ";
      }
      $Content .= "value=\"" . $Dev["ID"] . "\">" . $Dev["DeviceName"] . "</option>";
    }
  }
  $Content .= "</select>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function GroupSelector($DBcnx,$Selected) {
  $Result  = mysqli_query($DBcnx,"SELECT * FROM DeviceGroups ORDER BY Name");
  $Content = "<select style=\"width: 100%;margin-bottom: 0.5em;\" size=\"1\" class=\"form-control form-select\" id=\"GroupID\" name=\"GroupID\">";
  while ($Group = mysqli_fetch_assoc($Result)) {
    if ($Group["ID"] == $Selected) {
      $Content .= "<option selected value=\"" . $Group["ID"] . "\">" . $Group["Name"] . "</option>";
    } else {
      $Content .= "<option value=\"" . $Group["ID"] . "\">" . $Group["Name"] . "</option>";
    }
  }
  $Content .= "</select>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function InputSelector($DBcnx,$Selected,$ID) {
  $Result  = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE GroupID=" . $_COOKIE["CZ_GROUP"] . " ORDER BY DeviceName");
  $Content = "<select style=\"width: 100%;margin-bottom: 0.5em;\" size=\"1\" class=\"form-control form-select\" id=\"$ID\" name=\"$ID\" onChange=\"ToggleOutputFields(this)\">";
  if ($Selected == 0) {
    $Content .= "<option selected value=\"0\">None Selected</option>";
  } else {
    $Content .= "<option value=\"0\">None Selected</option>";
  }
  while ($Sensor = mysqli_fetch_assoc($Result)) {
    if ($Sensor["ID"] == $Selected) {
      $Content .= "<option selected value=\"" . $Sensor["ID"] . "\">" . $Sensor["DeviceName"] . "</option>";
    } else {
      $Content .= "<option value=\"" . $Sensor["ID"] . "\">" . $Sensor["DeviceName"] . "</option>";
    }
  }
  $Content .= "</select>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function OutputSelector($DBcnx,$Selected,$MyID,$ID) {
  $Result  = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE GroupID=" . $_COOKIE["CZ_GROUP"] . " ORDER BY DeviceName");
  $Content = "<select style=\"width: 100%;margin-bottom: 0.5em;\" size=\"1\" class=\"form-control form-select\" id=\"$ID\" name=\"$ID\">";
  if ($Selected == 0) {
    $Content .= "<option selected value=\"0\">None Selected</option>";
  } else {
    $Content .= "<option value=\"0\">None Selected</option>";
  }
  while ($Switch = mysqli_fetch_assoc($Result)) {
    if ($Switch["ID"] != $MyID) {
      if ($Switch["ID"] == $Selected) {
        $Content .= "<option selected value=\"" . $Switch["ID"] . "\">" . $Switch["DeviceName"] . "</option>";
      } else {
        $Content .= "<option value=\"" . $Switch["ID"] . "\">" . $Switch["DeviceName"] . "</option>";
      }
    }
  }
  $Content .= "</select>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function RenderBinary($Value,$Basic) {
  if ($Value == 0) {
    $Color = "text-danger";
    $Text  = "OFF";
  } else {
    $Color = "text-success";
    $Text  = "ON";
  }
  if ($Basic) {
    $Content = "<span class=\"$Color\" style=\"font-weight: bold;\">$Text</span>";
  } else {
    $Content = "<p class=\"card-text $Color\" style=\"font-size:2.75em;text-align:center;\">$Text</p>";
  }
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function RenderValueBasic($Score,$Lower,$Mid,$Upper,$Suffix) {
  $ScoreSaved = $Score;
  while ($Score > 100) $Score *= .99;
  if ($Score <= $Lower) {
    $Color = "text-primary-emphasis";
  } elseif (($Score >= $Mid) && ($Score < $Upper)) {
    $Color = "text-warning-emphasis";
  } elseif ($Score >= $Upper) {
    $Color = "text-danger";
  } else {
    $Color = "text-success";
  }
  $Content = "<span class=\"$Color\" style=\"font-weight: bold;\">$ScoreSaved$Suffix</span>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function RenderValue($Score,$Lower,$Mid,$Upper,$Suffix) {
  $ScoreSaved = $Score;
  while ($Score > 100) $Score *= .99;
  if ($Score <= $Lower) {
    $Color = "text-primary";
  } elseif (($Score >= $Mid) && ($Score < $Upper)) {
    $Color = "text-warning";
  } elseif ($Score >= $Upper) {
    $Color = "text-danger";
  } else {
    $Color = "text-success";
  }
  $Content = "<p class=\"card-text $Color\" style=\"font-size:3em;text-align:center;\">$ScoreSaved$Suffix</p>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function RenderProgressBar($Score,$Lower,$Mid,$Upper) {
  while ($Score > 100) $Score *= .99;
  if ($Score <= $Lower) {
    $Color = "bg-primary";
  } elseif (($Score >= $Mid) && ($Score < $Upper)) {
    $Color = "bg-warning";
  } elseif ($Score >= $Upper) {
    $Color = "bg-danger";
  } else {
    $Color = "bg-success";
  }
  $Content  = "<div class=\"progress\" role=\"progressbar\" aria-label=\"\" aria-valuenow=\"$Score\" aria-valuemin=\"0\" aria-valuemax=\"100\">";
  $Content .= " <div class=\"progress-bar $Color\" style=\"width: $Score%\"></div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function round_up($number,$precision = 2) {
  $fig = (int) str_pad('1', $precision, '0');
  return (ceil($number * $fig) / $fig);
}
//---------------------------------------------------------------------------------------------------
function round_down($number,$precision = 2) {
  $fig = (int) str_pad('1', $precision, '0');
  return (floor($number * $fig) / $fig);
}
//---------------------------------------------------------------------------------------------------
function InStr($Needle,$Haystack) {
  $Pos = strpos($Haystack,$Needle);
  if ($Pos === false) {
    return false;
  } else {
    return true;
  }
}
//---------------------------------------------------------------------------------------------------
function BinToStr($Value) {
  if ($Value == 1) {
    return "ON";
  } else {
    return "OFF";
  }
}
//---------------------------------------------------------------------------------------------------
function generateRandomString($length = 10) {
  $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
  $charactersLength = strlen($characters);
  $randomString = '';
  for ($i = 0; $i < $length; $i++) {
    $randomString .= $characters[rand(0, $charactersLength - 1)];
  }
  return $randomString;
}
//---------------------------------------------------------------------------------------------------
function normalizeString($str = '') {
  $str = strip_tags($str);
  $str = preg_replace('/[\r\n\t ]+/', ' ', $str);
  $str = preg_replace('/[\"\*\/\:\<\>\?\'\|]+/', ' ', $str);
  $str = strtolower($str);
  $str = html_entity_decode( $str, ENT_QUOTES, "utf-8" );
  $str = htmlentities($str, ENT_QUOTES, "utf-8");
  $str = preg_replace("/(&)([a-z])([a-z]+;)/i", '$2', $str);
  $str = str_replace(' ', '-', $str);
  $str = rawurlencode($str);
  $str = str_replace('%', '-', $str);
  return $str;
}
//---------------------------------------------------------------------------------------------------
function AjaxRefreshJS($ID,$RandID,$DeviceType) {
  if ($DeviceType == "input") {
    $File = "showinput";
  } else {
    $File = "showoutput";
  }
  $Content  = "\n<script type=\"text/javascript\">\n";
  //$Content .= "  // Random 4.5 to 5.5 second refresh time per device so things\n";
  //$Content .= "  // don't have such a robotic look by updating simultaneously.\n";
  //$Content .= "  // The sensor and switch loops run on 15 second delay cycles.\n";
  $Content .= "  jQuery(document).ready(function() {\n";
  $Content .= "    RandomDelay = 4500 + Math.floor(Math.random() * 1000) + 1;\n";
  $Content .= "    function refresh() {\n";
  $Content .= "      jQuery('#$RandID').load('./$File.php?ID=$ID');\n";
  $Content .= "    }\n";
  $Content .= "    setInterval(function() {\n";
  $Content .= "      refresh()\n";
  $Content .= "    },RandomDelay);\n";
  $Content .= "  });\n";
  $Content .= "</script>\n";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function InputSwitchControls($Type,$ID,$Status,$Caption) {
  $RandID = generateRandomString();
  $Blink0 = "";
  $Blink1 = "";
  $Float  = "right";
  if ($Type == 1) {
    $Float = "left";
    if ($Status == 0) {
      $Blink0 = " class=\"blink\"";
    } else {
      $Blink1 = " class=\"blink\"";
    }
  }
  $Content  = "<script type=\"text/javascript\">";
  $Content .= "function Toggle_$RandID(Switch) {";
  //$Content .= "alert('Value=' + Switch.value);";
  $Content .= "jQuery('#Switch_$RandID').load('./toggle.php?Type=1&ID=$ID&Basic=0&Mode=' + Switch.value);";
  $Content .= "}";
  $Content .= "</script>";
  $Content .= "<div class=\"row\">";
  if ($Type == 0) $Content .= "<div class=\"col\" id=\"Switch_$RandID\"></div><div class=\"col\">&nbsp;</div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-success\" name=\"sw_on_$RandID\" value=\"1\" onClick=\"Toggle_$RandID(this)\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink1>On</span></button>";
  $Content .= "</div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-danger\" name=\"sw_off_$RandID\" value=\"0\" onClick=\"Toggle_$RandID(this)\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink0>Off</span></button>";
  $Content .= "</div>";
  if ($Type == 1) $Content .= "<div class=\"col\">&nbsp;</div><div class=\"col\" id=\"Switch_$RandID\"><span style=\"float: right;\">$Caption</span></div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function DisabledInputSwitchControls($Type,$ID,$Status,$Caption) {
  $RandID = generateRandomString();
  $Blink0 = "";
  $Blink1 = "";
  $Float  = "right";
  if ($Type == 1) {
    $Float = "left";
    if ($Status == 0) {
      $Blink0 = " class=\"blink\"";
    } else {
      $Blink1 = " class=\"blink\"";
    }
  }
  $Content  = "<div class=\"row\">";
  if ($Type == 0) $Content .= "<div class=\"col\" id=\"Switch_$RandID\"></div><div class=\"col\">&nbsp;</div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-success\" disabled name=\"sw_on_$RandID\" value=\"1\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink1>On</span></button>";
  $Content .= "</div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-danger\" disabled name=\"sw_off_$RandID\" value=\"0\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink0>Off</span></button>";
  $Content .= "</div>";
  if ($Type == 1) $Content .= "<div class=\"col\">&nbsp;</div><div class=\"col\" id=\"Switch_$RandID\"><span style=\"float: right;\">$Caption</span></div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function OutputSwitchControls($Type,$ID,$Selected,$Caption) {
  $RandID = generateRandomString();
  $Blink0 = "";
  $Blink1 = "";
  $Blink2 = "";
  $Float  = "right";
  if ($Type == 1) {
    $Float = "left";
    if ($Selected == 0) { $Blink0 = " class=\"text-warning\""; }
    elseif ($Selected == 1) { $Blink1 = " class=\"blink\""; }
    elseif ($Selected == 2) { $Blink2 = " class=\"blink\""; }
  }
  $Content  = "<script type=\"text/javascript\">";
  $Content .= "function Toggle_$RandID(Switch) {";
  //$Content .= "alert('Value=' + Switch.value);";
  $Content .= "jQuery('#Switch_$RandID').load('./toggle.php?Type=0&ID=$ID&Basic=0&Mode=' + Switch.value);";
  $Content .= "}";
  $Content .= "</script>";
  $Content .= "<div class=\"row\">";
  if ($Type == 0) $Content .= "<div class=\"col\" id=\"Switch_$RandID\"></div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-primary\" name=\"sw_auto_$RandID\" value=\"0\" onClick=\"Toggle_$RandID(this)\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink0>Auto</span></button>";
  $Content .= "</div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-success\" name=\"sw_on_$RandID\" value=\"1\" onClick=\"Toggle_$RandID(this)\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink1>On</span></button>";
  $Content .= "</div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-danger\" name=\"sw_off_$RandID\" value=\"2\" onClick=\"Toggle_$RandID(this)\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink2>Off</span></button>";
  $Content .= "</div>";
  if ($Type == 1) $Content .= "<div class=\"col\" id=\"Switch_$RandID\"><span style=\"float: right;\">$Caption</span></div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function DisabledOutputSwitchControls($Type,$ID,$Selected,$Caption) {
  $RandID = generateRandomString();
  $Blink0 = "";
  $Blink1 = "";
  $Blink2 = "";
  $Float  = "right";
  if ($Type == 1) {
    $Float = "left";
    if ($Selected == 0) { $Blink0 = " class=\"text-warning\""; }
    elseif ($Selected == 1) { $Blink1 = " class=\"blink\""; }
    elseif ($Selected == 2) { $Blink2 = " class=\"blink\""; }
  }
  $Content  = "<div class=\"row\">";
  if ($Type == 0) $Content .= "<div class=\"col\" id=\"Switch_$RandID\"></div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-primary\" disabled name=\"sw_auto_$RandID\" value=\"0\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink0>Auto</span></button>";
  $Content .= "</div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-success\" disabled name=\"sw_on_$RandID\" value=\"1\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink1>On</span></button>";
  $Content .= "</div>";
  $Content .= "<div class=\"col\">";
  $Content .= "<button type=\"button\" class=\"btn btn-danger\" disabled name=\"sw_off_$RandID\" value=\"2\" style=\"float: $Float; --bs-btn-padding-y: .10rem; --bs-btn-padding-x: .75rem; --bs-btn-font-size: .75rem;\"><span$Blink2>Off</span></button>";
  $Content .= "</div>";
  if ($Type == 1) $Content .= "<div class=\"col\" id=\"Switch_$RandID\"><span style=\"float: right;\">$Caption</span></div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
?>
