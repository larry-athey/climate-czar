#!/usr/bin/php
<?php
//---------------------------------------------------------------------------------------------
require_once("../subs.php");
//---------------------------------------------------------------------------------------------
$DBcnx = mysqli_connect(DB_HOST,DB_USER,DB_PASS,DB_NAME);
//---------------------------------------------------------------------------------------------
$Result = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches ORDER BY InputType");
if (mysqli_num_rows($Result) > 0) {
  while ($Switch = mysqli_fetch_assoc($Result)) {
    $Override = false;
    $LogThis  = false;
    $Status   = $Switch["Status"];
    unset($Sensor);
    echo("Reading Device: " . $Switch["DeviceName"] . "\n");
    $Result2 = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $Switch["InputID"]);
    if (mysqli_num_rows($Result2) > 0) $Sensor = mysqli_fetch_assoc($Result2);
    if (($Switch["OpMode"] == 0) && (isset($Sensor["ID"]))) {
      // If OpMode equals zero, an input sensor is controlling the output switch
      if ($Switch["OverrideID"] > 0) {
        // Check for an active output switch override
        $Result2 = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE ID=" . $Switch["OverrideID"]);
        if (mysqli_num_rows($Result2) > 0) {
          $Switch2 = mysqli_fetch_assoc($Result2);
          if ($Switch["OverrideStatus"] == $Switch2["Status"]) {
            $Override = true;
            if ($Switch["Status"] == 1) {
              $LogThis = true;
              $Update  = mysqli_query($DBcnx,"UPDATE OutputSwitches SET Status=0,LastUpdate=now() WHERE ID=" . $Switch["ID"]);
              if (trim($Switch["OffCommand"]) != "") shell_exec($Switch["OffCommand"]);
            }
          }
        }
      }
      if (! $Override) {
        echo("Sensor Reading: " . $Sensor["Reading"] . "\n");
        if ($Switch["InputType"] <= 2) {
          // Variable Value Sensor or Average From a Group of Variable Value Sensors
          if ($Switch["Polarity"] == 0) {
            // 0=High-On/Low-Off
            if ($Sensor["Reading"] >= $Switch["HighValue"]) {
              $Status = 1;
            } elseif ($Sensor["Reading"] <= $Switch["LowValue"]) {
              $Status = 0;
            }
          } else {
            // 1=Low-On/High-Off
            if ($Sensor["Reading"] <= $Switch["LowValue"]) {
              $Status = 1;
            } elseif ($Sensor["Reading"] >= $Switch["HighValue"]) {
              $Status = 0;
            }
          }
        } else {
          // Everything else is a Binary Value Sensor
          $Status = $Sensor["Reading"];
        }
        $Sec = date("s",time());
        if ($Sec > 15) $Switch["OneShot"] = 1;
        if (($Switch["Status"] != $Status) || ($Switch["OneShot"] == 0)) {
          if (($Switch["Status"] == $Status) && ($Switch["OneShot"] == 0)) echo("OneShot disabled, repeating current toggle state\n");
          if ($Status == 0) {
            echo("Executing toggle-off command\n");
            if (trim($Switch["OffCommand"]) != "") shell_exec($Switch["OffCommand"]);
          } else {
            echo("Executing toggle-on command\n");
            if (trim($Switch["OnCommand"]) != "") shell_exec($Switch["OnCommand"]);
          }
          if ($Switch["Status"] != $Status) {
            $LogThis = true;
            $Update  = mysqli_query($DBcnx,"UPDATE OutputSwitches SET Status=$Status,LastUpdate=now() WHERE ID=" . $Switch["ID"]);
          }
        } else {
          echo("No change in toggle status\n");
        }
      } else {
        echo("Overriden By: " . $Switch2["DeviceName"] . "\n");
      }
    } else {
      if (isset($Sensor["ID"])) {
        if ($Switch["OpMode"] == 1) {
          if (($Switch["Manual"] == 1) && (! $Override)) {
            echo("Manual Override: Always On\n");
            if (trim($Switch["OnCommand"]) != "") shell_exec($Switch["OnCommand"]);
            $Update  = mysqli_query($DBcnx,"UPDATE OutputSwitches SET Status=1,Manual=0 WHERE ID=" . $Switch["ID"]);
            $Status  = 1;
            $LogThis = true;
          }
        } else {
          if ($Switch["Manual"] == 2) {
            echo("Manual Override: Off/Disabled\n");
            if (trim($Switch["OffCommand"]) != "") shell_exec($Switch["OffCommand"]);
            $Update  = mysqli_query($DBcnx,"UPDATE OutputSwitches SET Status=0,Manual=0 WHERE ID=" . $Switch["ID"]);
            $Status  = 0;
            $LogThis = true;
          }
        }
      } else {
        echo("Input sensor not found\n");
      }
    }
    if ($LogThis) {
      $Insert = mysqli_query($DBcnx,"INSERT INTO OutputHistory (DeviceID,TimeStamp,Status) VALUES (" . $Switch["ID"] . ",now(),$Status)");
    }
    echo("\n");
  }
}
//---------------------------------------------------------------------------------------------
mysqli_close($DBcnx);
//---------------------------------------------------------------------------------------------
?>
