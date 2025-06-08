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
if ($_GET["Type"] == 0) {
  $Result = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE ID=" . $_GET["ID"]);
  $Switch = mysqli_fetch_assoc($Result);
  if (UserSec($DBcnx) < DeviceSec($Switch["SwitchSec"])) {
    mysqli_close($DBcnx);
    exit;
  }

  $Query = "";
  if ($_GET["Mode"] == 1) $Query = ",Manual=1";
  if ($_GET["Mode"] == 2) $Query = ",Manual=2";
  $Result = mysqli_query($DBcnx,"UPDATE OutputSwitches SET OpMode=" . $_GET["Mode"] . "$Query WHERE ID=" . $_GET["ID"]);
} else {
  $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $_GET["ID"]);
  $Switch = mysqli_fetch_assoc($Result);
  if (UserSec($DBcnx) < DeviceSec($Switch["SwitchSec"])) {
    mysqli_close($DBcnx);
    exit;
  }

  $Result = mysqli_query($DBcnx,"UPDATE InputDevices SET Reading=" . $_GET["Mode"] . " WHERE ID=" . $_GET["ID"]);
  $Result = mysqli_query($DBcnx,"INSERT INTO InputHistory (DeviceID,TimeStamp,Reading,RawText) VALUES (" . $_GET["ID"] . ",now(),'" . $_GET["Mode"] . "','')");
}
if ($_GET["Basic"] == 1) {
//echo($_SERVER["HTTP_REFERER"]);
  header("Location: " . $_SERVER["HTTP_REFERER"]);
} else {
  echo("<p class=\"text-primary-emphasis\" style=\"font-style: italic;font-size: 0.9em;\">Processing...</p>\n");
}
mysqli_close($DBcnx);
//---------------------------------------------------------------------------------------------------
?>
