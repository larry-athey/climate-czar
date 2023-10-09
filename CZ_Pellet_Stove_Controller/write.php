<?php
//---------------------------------------------------------------------------------------------------
//
// This web server should not be available over the internet without source IP firewall rules or
// anybody can mess with your pellet stove without you knowing about it until it's too late!
//
$db = new SQLite3("stove.db");
//---------------------------------------------------------------------------------------------------
if ($_GET) {
  if (isset($_GET["HighBurn"])) {
    $Result = $db->query("UPDATE Stats SET HighBurn=" . $_GET["HighBurn"] . " WHERE ID=1");
  } elseif (isset($_GET["Restart"])) {
    $Result = $db->query("UPDATE Stats SET Restart=" . $_GET["Restart"] . " WHERE ID=1");
  } elseif (isset($_GET["StoveOn"])) {
    $Result = $db->query("UPDATE Stats SET StoveOn=" . $_GET["StoveOn"] . " WHERE ID=1");
  } elseif (isset($_GET["Deadline"])) {
    $Result = $db->query("UPDATE Settings SET Deadline=" . $_GET["Deadline"] . " WHERE ID=1");
  } elseif (isset($_GET["OpTemp"])) {
    $Result = $db->query("UPDATE Settings SET OpTemp=" . $_GET["OpTemp"] . " WHERE ID=1");
  } elseif (isset($_GET["MaxTemp"])) {
    $Result = $db->query("UPDATE Settings SET MaxTemp=" . $_GET["MaxTemp"] . " WHERE ID=1");
  } elseif (isset($_GET["HighFeed"])) {
    $Result = $db->query("UPDATE Settings SET HighFeed=" . $_GET["HighFeed"] . " WHERE ID=1");
  } elseif (isset($_GET["LowFeed"])) {
    $Result = $db->query("UPDATE Settings SET LowFeed=" . $_GET["LowFeed"] . " WHERE ID=1");
  } elseif (isset($_GET["CookTemp"])) {
    $Result = $db->query("UPDATE Settings SET CookTemp=" . $_GET["CookTemp"] . " WHERE ID=1");
  }
  $Changes = $db->changes();
  echo("$Changes row(s) affected\n");
}
//---------------------------------------------------------------------------------------------------
?>
