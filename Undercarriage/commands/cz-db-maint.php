#!/usr/bin/php
<?php
require_once("../subs.php");
//---------------------------------------------------------------------------------------------
$DBcnx = mysqli_connect(DB_HOST,DB_USER,DB_PASS,DB_NAME);
//---------------------------------------------------------------------------------------------
$Result = mysqli_query($DBcnx,"DELETE FROM InputHistory WHERE TimeStamp < (NOW() - INTERVAL 14 DAY)");
$Result = mysqli_query($DBcnx,"DELETE FROM OutputHistory WHERE TimeStamp < (NOW() - INTERVAL 14 DAY)");

$Result = mysqli_query($DBcnx,"OPTIMIZE TABLE DeviceGroups");
$Result = mysqli_query($DBcnx,"OPTIMIZE TABLE InputDevices");
$Result = mysqli_query($DBcnx,"OPTIMIZE TABLE InputHistory");
$Result = mysqli_query($DBcnx,"OPTIMIZE TABLE OutputSwitches");
$Result = mysqli_query($DBcnx,"OPTIMIZE TABLE OutputHistory");
$Result = mysqli_query($DBcnx,"OPTIMIZE TABLE Users");
//---------------------------------------------------------------------------------------------
mysqli_close($DBcnx);
//---------------------------------------------------------------------------------------------
?>
