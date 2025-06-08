<?php
//---------------------------------------------------------------------------------------------------
require_once("subs.php");
//---------------------------------------------------------------------------------------------------
$DBcnx = mysqli_connect(DB_HOST,DB_USER,DB_PASS,DB_NAME);
if (! UserLoggedIn($DBcnx)) {
  mysqli_close($DBcnx);
  exit;
}
//---------------------------------------------------------------------------------------------------
if (isset($_POST["LOGGED_IN_KEY"])) {
  if ($_POST["LOGGED_IN_KEY"] != LOGGED_IN_KEY) {
    //echo("Invalid LOGGED_IN_KEY sent");
    //header("Location: " . $_SERVER["HTTP_REFERER"]);
    mysqli_close($DBcnx);
    exit;
  }
} elseif (isset($_GET["LOGGED_IN_KEY"])) {
  if ($_GET["LOGGED_IN_KEY"] != LOGGED_IN_KEY) {
    //echo("Invalid LOGGED_IN_KEY sent");
    //header("Location: " . $_SERVER["HTTP_REFERER"]);
    mysqli_close($DBcnx);
    exit;
  } else {
    $_POST["RETURN_PAGE"] = $_GET["RETURN_PAGE"];
  }
} else {
  //echo("No LOGGED_IN_KEY sent");
  //header("Location: " . $_SERVER["HTTP_REFERER"]);
  mysqli_close($DBcnx);
  exit;
}
//---------------------------------------------------------------------------------------------------
//echo("<pre>");
//print_r($_GET);
//echo("</pre>");
//exit;
//---------------------------------------------------------------------------------------------------
if (isset($_GET["cz_delete_device"])) {
  if ($_GET["DevType"] == 0) {
    $Result = mysqli_query($DBcnx,"DELETE FROM InputDevices WHERE ID=" . $_GET["cz_delete_device"]);
    $Result = mysqli_query($DBcnx,"DELETE FROM InputHistory WHERE DeviceID=" . $_GET["cz_delete_device"]);
  } else {
    $Result = mysqli_query($DBcnx,"DELETE FROM OutputSwitches WHERE ID=" . $_GET["cz_delete_device"]);
    $Result = mysqli_query($DBcnx,"DELETE FROM OutputHistory WHERE DeviceID=" . $_GET["cz_delete_device"]);
  }
/***************************************************************************************************/
} elseif (isset($_GET["cz_delete_group"])) {
  if ($_GET["cz_delete_group"] > 1) {
    $Result = mysqli_query($DBcnx,"DELETE FROM DeviceGroups WHERE ID=" . $_GET["cz_delete_group"]);
  }
  if ($_GET["cz_delete_group"] == $_COOKIE["CZ_GROUP"]) setcookie("CZ_GROUP",1,0,"/");
/***************************************************************************************************/
} elseif (isset($_GET["cz_delete_script"])) {
  unlink(APP_ROOT . "commands/" . $_GET["cz_delete_script"]);
/***************************************************************************************************/
} elseif (isset($_POST["cz_group_save"])) {
  $_POST["GroupName"] = mysqli_escape_string($DBcnx,$_POST["GroupName"]);
  if ($_POST["ID"] == 0) { // Create new group
    $Result = mysqli_query($DBcnx,"INSERT INTO DeviceGroups (Name) VALUES ('" . $_POST["GroupName"] . "')");
  } else { // Update existing group
    $Result = mysqli_query($DBcnx,"UPDATE DeviceGroups SET Name='" . $_POST["GroupName"] . "' WHERE ID=" . $_POST["ID"]);
  }
/***************************************************************************************************/
} elseif (isset($_POST["cz_input_sensor_save"])) {
  $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $_POST["ID"]);
  $Dev    = mysqli_fetch_assoc($Result);
//echo("<pre>");
//print_r($_POST);
  $DeviceName = mysqli_escape_string($DBcnx,trim($_POST["DeviceName"]));
  $DeviceType = $_POST["DeviceType"];
  $Dashboard = $_POST["Dashboard"];
  $Position = trim($_POST["Position"]);
  if ($Position == "") $Position = "0";
  $SwitchSec = $_POST["SwitchSec"];
  $GroupID = $_POST["GroupID"];
  if ($GroupID != $Dev["GroupID"]) {
    $Result = mysqli_query($DBcnx,"UPDATE OutputSwitches SET InputID=0 WHERE InputID=" . $_POST["ID"]);
  }
  $ReadingSuffix = mysqli_escape_string($DBcnx,trim($_POST["ReadingSuffix"]));
  list($R,$G,$B) = sscanf($_POST["GraphColor"],"#%02x%02x%02x");
  $GraphColor = "$R,$G,$B";
  $BGlow = trim($_POST["BGlow"]);
  $BGmid = trim($_POST["BGmid"]);
  $BGhigh = trim($_POST["BGhigh"]);
  if (($BGlow == "") || ($BGmid == "") || ($BGhigh == "")) {
    $BGlow  = "30";
    $BGmid  = "60";
    $BGhigh = "90";
  }
  if (isset($_POST["SensorList"])) $SensorList = implode("|",$_POST["SensorList"]);
  if (isset($_POST["SwitchList"])) $SwitchList = implode("|",$_POST["SwitchList"]);
  if (! isset($SensorList)) $SensorList = "";
  if (! isset($SwitchList)) $SwitchList = "";
  $ScheduleList = str_replace("\r","",trim($_POST["ScheduleList"]));
  $ReadCommand = trim($_POST["ReadCommand"]);
  $Notes = str_replace("\r","",trim($_POST["Notes"]));
//echo("</pre>");
//exit;
  if ($_POST["ID"] == 0) { // Create new device
    $Query = "INSERT INTO InputDevices (DeviceName,DeviceType,Dashboard,Position,SwitchSec,GroupID,ReadingSuffix,GraphColor,BGlow,BGmid,BGhigh,SensorList,SwitchList,ScheduleList,ReadCommand,Notes) " .
             "VALUES ('$DeviceName','$DeviceType','$Dashboard','$Position','$SwitchSec','$GroupID','$ReadingSuffix','$GraphColor','$BGlow','$BGmid','$BGhigh','$SensorList','$SwitchList','$ScheduleList','$ReadCommand','$Notes')";
    $Result = mysqli_query($DBcnx,$Query);
  } else { // Update existing device
    $Query = "UPDATE InputDevices SET DeviceName='$DeviceName',DeviceType='$DeviceType',Dashboard='$Dashboard',Position='$Position',SwitchSec='$SwitchSec',GroupID='$GroupID',ReadingSuffix='$ReadingSuffix',GraphColor='$GraphColor',BGlow='$BGlow',BGmid='$BGmid',BGhigh='$BGhigh',SensorList='$SensorList',SwitchList='$SwitchList',ScheduleList='$ScheduleList',ReadCommand='$ReadCommand',Notes='$Notes' WHERE ID=" . $_POST["ID"];
    $Result = mysqli_query($DBcnx,$Query);
  }
/***************************************************************************************************/
} elseif (isset($_POST["cz_output_switch_save"])) {
  $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $_POST["InputID"]);
  $Dev    = mysqli_fetch_assoc($Result);
//echo("<pre>");
//print_r($_POST);
  $DeviceName = mysqli_escape_string($DBcnx,trim($_POST["DeviceName"]));
  $InputType = $Dev["DeviceType"];
  $Dashboard = $_POST["Dashboard"];
  $Position = trim($_POST["Position"]);
  if ($Position == "") $Position = "0";
  $SwitchSec = $_POST["SwitchSec"];
  $GroupID = $_POST["GroupID"];
  if ($GroupID != $Dev["GroupID"]) {
    $InputID = "0";
  } else {
    $InputID = $_POST["InputID"];
  }
  $OverrideID = $_POST["OverrideID"];
  $OverrideStatus = $_POST["OverrideStatus"];
  $Polarity = $_POST["Polarity"];
  $HighValue = trim($_POST["HighValue"]);
  $LowValue = trim($_POST["LowValue"]);
  if (($HighValue == "") || ($LowValue == "")) {
    $HighValue = "100";
    $LowValue  = "50";
  }
  $OneShot = $_POST["OneShot"];
  list($R,$G,$B) = sscanf($_POST["GraphColor"],"#%02x%02x%02x");
  $GraphColor = "$R,$G,$B";
  $OnCommand = trim($_POST["OnCommand"]);
  $OffCommand = trim($_POST["OffCommand"]);
  $Notes = str_replace("\r","",trim($_POST["Notes"]));
//print_r($_POST);
//echo("</pre>");
//exit;
  if ($_POST["ID"] == 0) { // Create new device
    $Query = "INSERT INTO OutputSwitches (DeviceName,InputType,Dashboard,Position,SwitchSec,GroupID,InputID,OverrideID,OverrideStatus,Polarity,HighValue,LowValue,OneShot,GraphColor,OnCommand,OffCommand,Notes) " .
             "VALUES ('$DeviceName','$InputType','$Dashboard','$Position','$SwitchSec','$GroupID','$InputID','$OverrideID','$OverrideStatus','$Polarity','$HighValue','$LowValue','$OneShot','$GraphColor','$OnCommand','$OffCommand','$Notes')";
    $Result = mysqli_query($DBcnx,$Query);
  } else { // Update existing device
    $Query = "UPDATE OutputSwitches SET DeviceName='$DeviceName',InputType='$InputType',Dashboard='$Dashboard',Position='$Position',SwitchSec='$SwitchSec',GroupID='$GroupID',InputID='$InputID',OverrideID='$OverrideID',OverrideStatus='$OverrideStatus',Polarity='$Polarity',HighValue='$HighValue',LowValue='$LowValue',OneShot='$OneShot',GraphColor='$GraphColor',OnCommand='$OnCommand',OffCommand='$OffCommand',Notes='$Notes' WHERE ID=" . $_POST["ID"];
//echo("<pre>");
//echo("\n\n$Query");
//echo("</pre>");
//exit;
    $Result = mysqli_query($DBcnx,$Query);
  }
/***************************************************************************************************/
} elseif (isset($_POST["cz_script_edit_save"])) {
//echo("<pre>");
//print_r($_POST);
  $ScriptName = normalizeString(trim($_POST["ScriptName"]));
  $OldName    = trim($_POST["OldName"]);
  $Code       = trim($_POST["Code"]);
  $Code       = stripslashes($Code);
  $Code       = str_replace("\r\n","\n",$Code);
  $LastChar   = strlen($Code) - 1;
  if ($Code[$LastChar] != "\n") $Code .= "\n";
//echo("\n$Code\n---");
//echo("</pre>");
//exit;
  if (file_exists(APP_ROOT . "commands/$OldName")) unlink(APP_ROOT . "commands/$OldName");
  if ($ScriptName == "new-script") {
    $Count = 1;
    while (file_exists(APP_ROOT . "commands/new-script-" . $Count)) $Count ++;
    $ScriptName = "new-script-" . $Count;
  }
  file_put_contents(APP_ROOT . "commands/$ScriptName",$Code);
  shell_exec("chmod +x " . APP_ROOT . "commands/$ScriptName");
  $_POST["RETURN_PAGE"] = "index.php?page=list-scripts&ScriptName=$ScriptName";
/***************************************************************************************************/
}
//---------------------------------------------------------------------------------------------------
mysqli_close($DBcnx);
//echo("<pre>");
//echo($_POST["RETURN_PAGE"]);
//echo("</pre>");
header("Location: " . $_POST["RETURN_PAGE"]);
//---------------------------------------------------------------------------------------------------
?>
