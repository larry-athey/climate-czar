<?php
//---------------------------------------------------------------------------------------------------
//
// This web server should not be available over the internet without source IP firewall rules or
// anybody can mess with your pellet stove without you knowing about it until it's too late!
//
$db = new SQLite3("stove.db");
//---------------------------------------------------------------------------------------------------
function GetReading($Table,$Field) {
  global $db;
  $Result = $db->query("SELECT $Field FROM $Table WHERE ID=1");
  $RS = $Result->fetchArray();
  return $RS[0] . "\n";
}
//---------------------------------------------------------------------------------------------------
function FormatOpMode($OpMode) {
  if ($OpMode == 0) {
    return "<span class=\"text-primary\" style=\"font-weight: bold;\">Shut Down</span>";
  } elseif ($OpMode == 1) {
    return "<span class=\"text-primary blink\" style=\"font-weight: bold;\">Starting Up</span>";
  } elseif ($OpMode == 2) {
    return "<span class=\"text-success\" style=\"font-weight: bold;\">Up To Temp</span>";
  } elseif ($OpMode == 3) {
    return "<span class=\"text-warning blink\" style=\"font-weight: bold;\">Temp Failure</span>";
  } elseif ($OpMode == 4) {
    return "<span class=\"text-danger-emphasis blink\" style=\"font-weight: bold;\">Manual Off</span>";
  }
}
//---------------------------------------------------------------------------------------------------
function FormatOpModeText($OpMode) {
  if ($OpMode == 0) {
    return "Shut Down";
  } elseif ($OpMode == 1) {
    return "Starting Up";
  } elseif ($OpMode == 2) {
    return "Up To Temp";
  } elseif ($OpMode == 3) {
    return "Temp Failure";
  } elseif ($OpMode == 4) {
    return "Manual Off";
  }
}
//---------------------------------------------------------------------------------------------------
function FormatBinary($Value) {
  if ($Value == 0) {
    $Color = "text-primary";
    $Text  = "OFF";
  } else {
    $Color = "text-success";
    $Text  = "ON";
  }
  return "<span class=\"$Color\" style=\"font-weight: bold;\">$Text</span>";
}
//---------------------------------------------------------------------------------------------------
function FormatBinaryText($Value) {
  if ($Value == 0) {
    return "OFF";
  } else {
    return "ON";
  }
}
//---------------------------------------------------------------------------------------------------
if ($_GET) {
  if (isset($_GET["Temperature"])) {
    echo(GetReading("Stats","Temperature"));
  } elseif (isset($_GET["Probe1"])) {
    echo(GetReading("Stats","Probe1"));
  } elseif (isset($_GET["Probe2"])) {
    echo(GetReading("Stats","Probe2"));
  } elseif (isset($_GET["Probe3"])) {
    echo(GetReading("Stats","Probe3"));
  } elseif (isset($_GET["Probe4"])) {
    echo(GetReading("Stats","Probe4"));
  } elseif (isset($_GET["Probe5"])) {
    echo(GetReading("Stats","Probe5"));
  } elseif (isset($_GET["Probe6"])) {
    echo(GetReading("Stats","Probe6"));
  } elseif (isset($_GET["Probe7"])) {
    echo(GetReading("Stats","Probe7"));
  } elseif (isset($_GET["Probe8"])) {
    echo(GetReading("Stats","Probe8"));
  } elseif (isset($_GET["Combustion"])) {
    echo(GetReading("Stats","Combustion"));
  } elseif (isset($_GET["RoomBlower"])) {
    echo(GetReading("Stats","RoomBlower"));
  } elseif (isset($_GET["Ignitor"])) {
    echo(GetReading("Stats","Ignitor"));
  } elseif (isset($_GET["OpMode"])) {
    echo(GetReading("Stats","OpMode"));
  } elseif (isset($_GET["StoveOn"])) {
    echo(GetReading("Stats","StoveOn"));
  } elseif (isset($_GET["HighBurn"])) {
    echo(GetReading("Stats","HighBurn"));
  } elseif (isset($_GET["CountDown"])) {
    echo(GetReading("Stats","CountDown"));
  } elseif (isset($_GET["LowTempFail"])) {
    echo(GetReading("Stats","LowTempFail"));
  } elseif (isset($_GET["ShutDownTime"])) {
    echo(GetReading("Stats","ShutDownTime"));
  } elseif (isset($_GET["Deadline"])) {
    echo(GetReading("Settings","Deadline"));
  } elseif (isset($_GET["OpTemp"])) {
    echo(GetReading("Settings","OpTemp"));
  } elseif (isset($_GET["MaxTemp"])) {
    echo(GetReading("Settings","MaxTemp"));
  } elseif (isset($_GET["HighFeed"])) {
    echo(GetReading("Settings","HighFeed"));
  } elseif (isset($_GET["LowFeed"])) {
    echo(GetReading("Settings","LowFeed"));
  } elseif (isset($_GET["CookTemp"])) {
    echo(GetReading("Settings","CookTemp"));
  } elseif (isset($_GET["html"])) {
    $Result = $db->query("SELECT * FROM Stats WHERE ID=1");
    $RS = $Result->fetchArray();
    if (trim($RS["LowTempFail"]) != "") {
      $ShutdownMsg = "<span class=\"iconify text-muted\" data-icon=\"jam:triangle-danger\"></span>&nbsp;<span class=\"text-danger-emphasis\" style=\"font-weight: bold;\">" . $RS["LowTempFail"] . "</span>";
    } elseif (trim($RS["ShutDownTime"]) != "") {
      $ShutdownMsg = "<span class=\"iconify text-muted\" data-icon=\"lucide:power-off\"></span>&nbsp;<span class=\"text-info-emphasis\" style=\"font-weight: bold;\">" . $RS["ShutDownTime"] . "</span>";
    } else {
      $ShutdownMsg = "Last Update: <span class=\"text-primary\" style=\"font-weight: bold;\">" . date("h:i:s a") . "</span>";
    }
    $Content  = "<div class=\"row\">";
    $Content .= "<div class=\"col\" style=\"font-size: 0.93em;\">Stove Status: " . FormatOpMode($RS["OpMode"]) . "</div>";
    $Content .= "<div class=\"col\" style=\"font-size: 0.93em;\">Temperature: <span class=\"text-primary\" style=\"font-weight: bold;\">" . $RS["Temperature"] . "&deg; F</span></div>";
    $Content .= "</div>";
    $Content .= "<div class=\"row\">";
    $Content .= "<div class=\"col\" style=\"font-size: 0.93em;\">Combustion Blower: " . FormatBinary($RS["Combustion"]) . "</div>";
    $Content .= "<div class=\"col\" style=\"font-size: 0.93em;\">High Burn Mode: " . FormatBinary($RS["HighBurn"]) . "</div>";
    $Content .= "</div>";
    $Content .= "<div class=\"row\">";
    $Content .= "<div class=\"col\" style=\"font-size: 0.93em;\">Ignition Element: " . FormatBinary($RS["Ignitor"]) . "</div>";
    $Content .= "<div class=\"col\" style=\"font-size: 0.93em;\">Countdown Timer: <span class=\"text-primary\" style=\"font-weight: bold;\">" . $RS["CountDown"] . "</span></div>";
    $Content .= "</div>";
    $Content .= "<div class=\"row\">";
    $Content .= "<div class=\"col\" style=\"font-size: 0.93em;\">Room Air Blower: " . FormatBinary($RS["RoomBlower"]) . "</div>";
    $Content .= "<div class=\"col\" style=\"font-size: 0.93em;\">$ShutdownMsg</div>";
    $Content .= "</div>";
    echo("$Content\n");
  } elseif (isset($_GET["tables"])) {
    echo("Stats Table: ");
    $Result = $db->query("SELECT * FROM Stats WHERE ID=1");
    $RS = $Result->fetchArray();
    print_r($RS);
    echo("\n");
    echo("Settings Table: ");
    $Result = $db->query("SELECT * FROM Settings WHERE ID=1");
    $RS = $Result->fetchArray();
    print_r($RS);
  }
} else {
  $Result = $db->query("SELECT * FROM Stats WHERE ID=1");
  $RS = $Result->fetchArray();
  if (trim($RS["LowTempFail"]) != "") {
    $ShutdownMsg = "Low Temp Fail: " . $RS["LowTempFail"];
  } elseif (trim($RS["ShutDownTime"]) != "") {
    $ShutdownMsg = "Manual Shutdown: " . $RS["ShutDownTime"];
  } else {
    $ShutdownMsg = "Last Update: " . date("h:i:s a");
  }
  echo(str_pad("Stove Status: " . FormatOpModeText($RS["OpMode"]),35) . "Temperature: " . $RS["Temperature"] . " F\n");
  echo(str_pad("Combustion Blower: " . FormatBinaryText($RS["Combustion"]),35) . "High Burn Mode: " . FormatBinaryText($RS["HighBurn"]) . "\n");
  echo(str_pad("Ignition Element: " . FormatBinaryText($RS["Ignitor"]),35) . "Countdown Timer: " . $RS["CountDown"] . "\n");
  echo(str_pad("Room Air Blower: " . FormatBinaryText($RS["RoomBlower"]),35) . $ShutdownMsg . "\n");
}
//---------------------------------------------------------------------------------------------------
?>
