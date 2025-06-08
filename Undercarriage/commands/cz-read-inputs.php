#!/usr/bin/php
<?php
//---------------------------------------------------------------------------------------------
require_once("../subs.php");
//---------------------------------------------------------------------------------------------
$DBcnx = mysqli_connect(DB_HOST,DB_USER,DB_PASS,DB_NAME);
//---------------------------------------------------------------------------------------------
$Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices ORDER BY DeviceType");
if (mysqli_num_rows($Result) > 0) {
  while ($Sensor = mysqli_fetch_assoc($Result)) {
    $LogThis = false;
    $Reading = "";
    $RawText = "";
    echo("Reading Device: " . $Sensor["DeviceName"] . "\n");
    if ($Sensor["DeviceType"] == 1) {
      // Variable Value Sensor
      $Reading = trim(" " . shell_exec($Sensor["ReadCommand"]));
      if ((strlen($Reading) > 0) && (is_numeric($Reading))) {
        $LogThis = true;
        $Reading = round($Reading,1);
        echo("Current Value: $Reading\n\n");
        $Update = mysqli_query($DBcnx,"UPDATE InputDevices SET Reading=$Reading,LastUpdate=now() WHERE ID=" . $Sensor["ID"]);
      } else {
        echo("Error: non-numeric reading returned ($Reading)\n\n");
      }
    } elseif ($Sensor["DeviceType"] == 2) {
      // Average From a Group of Variable Value Sensors
      $SCount = 0;
      $Total  = 0;
      $List   = explode("|",$Sensor["SensorList"]);
      echo("Sensor List: " . $Sensor["SensorList"] . " (" . count($List) . ")\n");
      if (count($List) > 0) {
        for ($x = 0; $x <= (count($List) - 1); $x ++) {
          $Result2 = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $List[$x]);
          if (mysqli_num_rows($Result2) > 0) {
            $Device = mysqli_fetch_assoc($Result2);
            $Total += $Device["Reading"];
            $SCount ++;
          }
        }
        if ($SCount > 0) {
          $LogThis = true;
          $Reading = round($Total / $SCount,1);
          echo("Current Value: $Reading\n\n");
          $Update = mysqli_query($DBcnx,"UPDATE InputDevices SET Reading=$Reading,LastUpdate=now() WHERE ID=" . $Sensor["ID"]);
        }
      }
    } elseif ($Sensor["DeviceType"] == 3) {
      // Remote Switch, binary value sensor
      $Reading = trim(" " . shell_exec($Sensor["ReadCommand"]));
      if ((strlen($Reading) > 0) && (is_numeric($Reading)) && (($Reading == 0) || ($Reading == 1))) {
        $LogThis = true;
        echo("Current Value: $Reading\n\n");
        $Update = mysqli_query($DBcnx,"UPDATE InputDevices SET Reading=$Reading,LastUpdate=now() WHERE ID=" . $Sensor["ID"]);
      } else {
        echo("Error: non-numeric reading or reading not a 0 or 1\n\n");
      }
    } elseif (($Sensor["DeviceType"] >= 4) && ($Sensor["DeviceType"] <= 6)) {
      // Logic Gates of a Group of Remote Switches
      $SCount = 0;
      $Total  = 0;
      $List   = explode("|",$Sensor["SwitchList"]);
      echo("Sensor List: " . $Sensor["SwitchList"] . " (" . count($List) . ")\n");
      if (count($List) > 0) {
        for ($x = 0; $x <= (count($List) - 1); $x ++) {
          $Result2 = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $List[$x]);
          if (mysqli_num_rows($Result2) > 0) {
            $Device = mysqli_fetch_assoc($Result2);
            $Total += $Device["Reading"];
            $SCount ++;
          }
        }
        if ($SCount > 0) {
          $LogThis = true;
          if ($Sensor["DeviceType"] == 4) {
            // AND Gate of a Group of Remote Switches
            if ($Total == $SCount) { // All switches in the list are on
              $Reading = 1;
            } else {
              $Reading = 0;
            }
          } elseif ($Sensor["DeviceType"] == 5) {
            // OR Gate of a Group of Remote Switches
            if ($Total > 0) { // At least one of the switches in the list are on
              $Reading = 1;
            } else {
              $Reading = 0;
            }
          } elseif ($Sensor["DeviceType"] == 6) {
            // NOT Gate of a Group of Remote Switches
            if ($Total == 0) { // All switches in the list are off
              $Reading = 1;
            } else {
              $Reading = 0;
            }
          }
          echo("Current Value: $Reading\n\n");
          $Update = mysqli_query($DBcnx,"UPDATE InputDevices SET Reading=$Reading,LastUpdate=now() WHERE ID=" . $Sensor["ID"]);
        }
      }
    } elseif ($Sensor["DeviceType"] == 7) {
      // Scheduler or Timer, whatever you like to call it
      $Reading = 0;
      $Now     = strtotime("now");
      $Sensor["ScheduleList"] = str_replace("\r","",trim(" " . $Sensor["ScheduleList"]));
      $List = explode("\n",$Sensor["ScheduleList"]);
      for ($x = 0; $x <= (count($List) - 1); $x ++) {
        $Time = explode("-",$List[$x]);
//echo(date("Y-m-d H:i:s",strtotime($Time[0])) . "\n");
//echo(date("Y-m-d H:i:s",strtotime($Time[1])) . "\n");
//echo(date("Y-m-d H:i:s",$Now) . "\n");
//if (($Now >= $Time[0]) && ($Now <= $Time[1])) echo("PASS!\n");
//echo("\n");
        if (($Now >= strtotime($Time[0])) && ($Now <= strtotime($Time[1]))) $Reading = 1;
      }
      if ($Reading != $Sensor["Reading"]) {
        $LogThis = true;
        if ($Reading == 0) {
          echo("Toggled Off\n\n");
        } else {
          echo("Toggled On\n\n");
        }
        $Update = mysqli_query($DBcnx,"UPDATE InputDevices SET Reading=$Reading,LastUpdate=now() WHERE ID=" . $Sensor["ID"]);
      } else {
        echo("No change in toggle state\n\n");
      }
    } elseif ($Sensor["DeviceType"] == 8) {
      // Web Console Switch, do nothing
      echo("\n");
    } elseif ($Sensor["DeviceType"] == 9) {
      // Raw Text Device
      $RawText = trim(" " . shell_exec($Sensor["ReadCommand"]));
      if (strlen($RawText) > 0) {
        echo("Received " . strlen($RawText) . " bytes of raw text\n\n");
        $LogThis = true;
        $RawText = mysqli_escape_string($DBcnx,$RawText);
        $Update  = mysqli_query($DBcnx,"UPDATE InputDevices SET RawText='$RawText',LastUpdate=now() WHERE ID=" . $Sensor["ID"]);
      } else {
        echo("Error: no text returned\n\n");
      }
    }
    $Sec = date("s",time());
    if (($LogThis) && ($Sec <= 15)) {
      $Insert = mysqli_query($DBcnx,"INSERT INTO InputHistory (DeviceID,TimeStamp,Reading,RawText) VALUES (" . $Sensor["ID"] . ",now(),'$Reading','$RawText')");
    }
  }
}
//---------------------------------------------------------------------------------------------
mysqli_close($DBcnx);
//---------------------------------------------------------------------------------------------
?>
