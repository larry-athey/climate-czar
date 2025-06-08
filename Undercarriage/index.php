<?php
//---------------------------------------------------------------------------------------------------
require_once("html.php");
$DBcnx = mysqli_connect(DB_HOST,DB_USER,DB_PASS,DB_NAME);
//---------------------------------------------------------------------------------------------------
if (($_GET) && (! isset($_GET["page"]))){
  if (isset($_GET["CZ_BASIC"])) {
    if ($_GET["CZ_BASIC"] == 1) {
      setcookie("CZ_BASIC",1,0,"/");
    } else {
      setcookie("CZ_BASIC",0,0,"/");
    }
  }
  if ((isset($_GET["CZ_GROUP"])) && (UserLoggedIn($DBcnx))) {
    // Check user security level before setting the cookie
    $Result = mysqli_query($DBcnx,"SELECT * FROM DeviceGroups WHERE ID=" . $_GET["CZ_GROUP"]);
    if (mysqli_num_rows($Result) > 0) {
      $Group = mysqli_fetch_assoc($Result);
      if (UserSec($DBcnx) >= DeviceSec($Group["SecLevel"])) {
        setcookie("CZ_GROUP",$_GET["CZ_GROUP"],0,"/");
      }
    }
  }
  if (isset($_GET["CZ_THEME"])) {
    setcookie("CZ_THEME",$_GET["CZ_THEME"],0,"/");
  }
  mysqli_close($DBcnx);
  header("Location: index.php");
  exit;
}
//---------------------------------------------------------------------------------------------------
?>
<!DOCTYPE html>
<?
if (CZ_THEME == 0) {
  echo("<html lang=\"en\">\n");
} else {
  echo("<html lang=\"en\" data-bs-theme=\"dark\">\n");
}
require_once("colors.php");
?>
<head>
  <title>Climate Czar Server v<?= VERSION ?></title>
  <meta http-equiv="cache-control" content="max-age=0">
  <meta http-equiv="cache-control" content="no-cache">
  <meta http-equiv="expires" content="0">
  <meta http-equiv="expires" content="Tue, 01 Jan 1980 1:00:00 GMT">
  <meta http-equiv="pragma" content="no-cache">
  <meta http-equiv="refresh" content="3600">
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  <link rel="icon" href="favicon.ico?v=1.1">
  <link href="bootstrap/css/bootstrap.min.css" rel="stylesheet">
  <link href="css/cz.css" rel="stylesheet">
  <script src="bootstrap/js/bootstrap.bundle.min.js"></script>
  <script src="js/iconify.min.js"></script>
  <script src="js/chart.js"></script>
  <script src="js/jquery.min.js"></script>
  <script type="text/javascript">
  //-----------------------------------------------------------------------------------------------------
  function ConfirmScriptDelete(LoggedInKey,ReturnPage,Script) {
    if (confirm('Are you sure that you want to delete the script \"' + Script + '\"?')) {
      window.location.href = 'process.php?LOGGED_IN_KEY=' + LoggedInKey + '&RETURN_PAGE=' + ReturnPage + '&cz_delete_script=' + Script;
    }
  }
  //-----------------------------------------------------------------------------------------------------
  function ConfirmGroupDelete(LoggedInKey,ReturnPage,ID,Group) {
    if (confirm('Are you sure that you want to delete the group\n\"' + Group + '\"?')) {
      window.location.href = 'process.php?LOGGED_IN_KEY=' + LoggedInKey + '&RETURN_PAGE=' + ReturnPage + '&cz_delete_group=' + ID;
    }
  }
  //-----------------------------------------------------------------------------------------------------
  function ConfirmDeviceDelete(LoggedInKey,ReturnPage,DevType,ID,DevName) {
    if (confirm('Are you sure that you want to delete the device\n\"' + DevName + '\"?')) {
      window.location.href = 'process.php?LOGGED_IN_KEY=' + LoggedInKey + '&RETURN_PAGE=' + ReturnPage + '&DevType=' + DevType + '&cz_delete_device=' + ID;
    }
  }
  //-----------------------------------------------------------------------------------------------------
  function ConfirmUserDelete(LoggedInKey,ReturnPage,ID,User) {
    if (confirm('Are you sure that you want to delete the user\n\"' + User + '\"?')) {
      window.location.href = 'process.php?LOGGED_IN_KEY=' + LoggedInKey + '&RETURN_PAGE=' + ReturnPage + '&cz_delete_user=' + ID;
    }
  }
  //-----------------------------------------------------------------------------------------------------

  </script>
  <style>
  body {
    background-color: <?= $BodyBG ?>;
  }
  </style>
</head>
<body>
<?php
echo("  " . cz_menu_bar($DBcnx) . "\n");

$Content  = "  <div class=\"container-fluid\" style=\"align: left;\">";
$Content .=     "<div class=\"row\">";

if (! isset($_GET["page"])) {
  $Content .= cz_home_page($DBcnx,1);
} else {
  if ($_GET["page"] == "input-sensors") {
    $Content .= cz_home_page($DBcnx,2);
  } elseif ($_GET["page"] == "list-devices") {
    $Content .= cz_list_devices($DBcnx);
  } elseif ($_GET["page"] == "list-groups") {
    $Content .= cz_list_groups($DBcnx);
  } elseif ($_GET["page"] == "list-scripts") {
    $Content .= cz_list_scripts($DBcnx);
  } elseif ($_GET["page"] == "list-users") {
    $Content .= cz_list_users($DBcnx);
  } elseif ($_GET["page"] == "login") {
    $Content .= cz_login();
  } elseif ($_GET["page"] == "logs-graphs") {
    $Content .= cz_logs_graphs($DBcnx);
  } elseif ($_GET["page"] == "output-switches") {
    $Content .= cz_home_page($DBcnx,3);
  }
}

$Content .=     "</div>";
$Content .=   "</div>";

echo("$Content\n");
mysqli_close($DBcnx);
?>
</body>
</html>
