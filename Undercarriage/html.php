<?php
//---------------------------------------------------------------------------------------------------
require_once("subs.php");
//---------------------------------------------------------------------------------------------------
function cz_home_page($DBcnx,$Mode) {
  $Active1 = "";
  $Active2 = "";
  $Active3 = "";
  if ($Mode == 1) {
    $Active1 = "active";
  } elseif ($Mode == 2) {
    $Active2 = "active";
  } else {
    $Active3 = "active";
  }
  if (CZ_THEME == 0) {
    $ButtonBG = "btn-light";
  } else {
    $ButtonBG = "btn-dark";
  }
  $Content  = "<div style=\"margin-top: .5em;\">";
  $Content .=   "<div class=\"btn-group\">";
  $Content .=     "<a class=\"btn $ButtonBG $Active1\" href=\"index.php\">All Devices</a>";
  $Content .=     "<a class=\"btn $ButtonBG $Active2\" href=\"index.php?page=input-sensors\">Input Sensors</a>";
  $Content .=     "<a class=\"btn $ButtonBG $Active3\" href=\"index.php?page=output-switches\">Output Switches</a>";
  $Content .=   "</div>";
  $Content .= "</div>";
  $Content .= "<div class=\"card\" style=\"width: 98.75%; padding-left: .25em; padding-right: .25em; margin-left: .75em; margin-bottom: 1em;\">";
  $Content .=   "<div class=\"card-body\">";
  if ($Mode == 1) {
    $Content .= cz_show_all_sensors($DBcnx);
    $Content .= cz_show_all_switches($DBcnx);
  } elseif ($Mode == 2) {
    $Content .= cz_show_all_sensors($DBcnx);
  } else {
    $Content .= cz_show_all_switches($DBcnx);
  }
  $Content .=   "</div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_list_devices($DBcnx) {
  $LOGGED_IN_KEY = urlencode(LOGGED_IN_KEY);
  $RETURN_PAGE   = urlencode("index.php?page=list-devices");
  $DevType = 0;
  $FormID  = 2;
  $BtnLbl  = "Input Sensor";
  if (isset($_POST["DevType"])) $DevType = $_POST["DevType"];
  if (isset($_GET["DevType"])) $DevType = $_GET["DevType"];
  if ($DevType == 1) {
    $FormID = 3;
    $BtnLbl  = "Output Switch";
  }

  $Content  = "<div class=\"card\" style=\"width: 98.75%; padding-left: .25em; padding-right: .25em; margin-left: .75em; margin-bottom: 1em;\">";
  $Content .=   "<div class=\"card-body\">";
  $Content .=     "<table class=\"table table-borderless\">";
  $Content .=       "<tr>";
  $Content .=         "<td width=\"30%\">";
  $Content .=           DeviceListSelector($DBcnx,CZ_GROUP,$DevType);
  $Content .=           "<a class=\"btn btn-outline-secondary\" href=\"formedit.php?FormID=$FormID&DeviceID=0&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\" target=\"formedit\" role=\"button\" style=\"width: 100%;margin-bottom: 0.5em;\">Add New $BtnLbl</a>";
  $Content .=           "<div style=\"height:73vh;width:100%;float:left;overflow-y:scroll;\">";
  $Content .=             "<table class=\"table table-striped table-hover\">";
  if ($DevType == 0) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE GroupID=" . CZ_GROUP ." ORDER BY DeviceName");
  } else {
    $Result = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE GroupID=" . CZ_GROUP ." ORDER BY DeviceName");
  }
  if (mysqli_num_rows($Result) > 0) {
    while ($Dev = mysqli_fetch_assoc($Result)) {
      $Content .= "<tr><td class=\"align-middle\">" . $Dev["DeviceName"] . "</td>";
      $Content .= "<td nowrap align=\"right\"><a class=\"btn btn-success btn-sm\" href=\"formedit.php?FormID=$FormID&DeviceID=" . $Dev["ID"] . "&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\" target=\"formedit\" role=\"button\">Edit</a>" .
                  "&nbsp;&nbsp;<a onClick=\"ConfirmDeviceDelete('$LOGGED_IN_KEY','$RETURN_PAGE','$DevType','" . $Dev["ID"] . "','" . $Dev["DeviceName"] . "')\" class=\"btn btn-danger btn-sm\" role=\"button\">Delete</a>" .
                  "</td></tr>";
    }
  }
  $Content .=             "</table>";
  $Content .=           "</div>";
  $Content .=         "</td>";
  $Content .=         "<td width=\"70%\">";
  $Content .=           "<iframe type=\"text/html\" name=\"formedit\" style=\"height:82vh;width:50%;float:left;overflow-y:auto;border:none;\" src=\"formedit.php?FormID=0&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\"\">";
  $Content .=         "</td>";
  $Content .=       "</tr>";
  $Content .=     "</table>";
  $Content .=   "</div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_list_groups($DBcnx) {
  $LOGGED_IN_KEY = urlencode(LOGGED_IN_KEY);
  $RETURN_PAGE   = urlencode("index.php?page=list-groups");
  $Content  = "<div class=\"card\" style=\"width: 98.75%; padding-left: .25em; padding-right: .25em; margin-left: .75em; margin-bottom: 1em;\">";
  $Content .=   "<div class=\"card-body\">";
  $Content .=     "<table class=\"table table-borderless\">";
  $Content .=       "<tr>";
  $Content .=         "<td width=\"30%\">";
  $Content .=           "<a class=\"btn btn-outline-secondary\" href=\"formedit.php?FormID=1&GroupID=0&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\" target=\"formedit\" role=\"button\" style=\"width: 100%;margin-bottom: 0.5em;\">Create New Group</a>";
  $Content .=           "<div style=\"height:77.5vh;width:100%;float:left;overflow-y:scroll;\">";
  $Content .=             "<table class=\"table table-striped table-hover\">";
  $Result = mysqli_query($DBcnx,"SELECT * FROM DeviceGroups ORDER BY Name");
  while ($Group = mysqli_fetch_assoc($Result)) {
    $Content .=           "<tr><td class=\"align-middle\">" . $Group["Name"] . "</td>" .
                          "<td align=\"right\"><a class=\"btn btn-success btn-sm\" href=\"formedit.php?FormID=1&GroupID=" . $Group["ID"] . "&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\" target=\"formedit\" role=\"button\">Edit</a>";
    if ($Group["ID"] > 1) {
      $Content .=         "&nbsp;&nbsp;<a onClick=\"ConfirmGroupDelete('$LOGGED_IN_KEY','$RETURN_PAGE','" . $Group["ID"] . "','" . $Group["Name"] . "')\" class=\"btn btn-danger btn-sm\" role=\"button\">Delete</a>";
    }
    $Content .=           "</td></tr>";
  }
  $Content .=             "</table>";
  $Content .=           "</div>";
  $Content .=         "</td>";
  $Content .=         "<td width=\"70%\">";
  $Content .=           "<iframe type=\"text/html\" name=\"formedit\" style=\"height:82vh;width:50%;float:left;overflow-y:auto;border:none;\" src=\"formedit.php?FormID=0&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\">";
  $Content .=         "</td>";
  $Content .=       "</tr>";
  $Content .=     "</table>";
  $Content .=   "</div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_list_scripts() {
  $LOGGED_IN_KEY = urlencode(LOGGED_IN_KEY);
  $RETURN_PAGE   = urlencode("index.php?page=list-scripts");
  $Files = array_diff(scandir(APP_ROOT . "commands/"),array('.','..'));
  $Files = array_values($Files);
  $Query = "?LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE";
  if (isset($_GET["ScriptName"])) $Query .= "&ScriptName=" . $_GET["ScriptName"];
  $Content  = "<div class=\"card\" style=\"width: 98.75%; padding-left: .25em; padding-right: .25em; margin-left: .75em; margin-bottom: 1em;\">";
  $Content .=   "<div class=\"card-body\">";
  $Content .=     "<table class=\"table table-borderless\">";
  $Content .=       "<tr>";
  $Content .=         "<td width=\"30%\">";
  $Content .=           "<div style=\"height:80vh;width:100%;float:left;overflow-y:scroll;\">";
  $Content .=             "<table class=\"table table-striped table-hover\">";
  for ($x = 0; $x <= (count($Files) - 1); $x ++) {
    $Pos = strpos($Files[$x],"cz-",0);
    if ($Pos !== false) { /* File Name starts with "cz-" */ } else {
      $Content .=           "<tr><td class=\"align-middle\">$Files[$x]</td>" .
                            "<td align=\"right\"><a class=\"btn btn-success btn-sm\" href=\"scriptedit.php?ScriptName=$Files[$x]&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\" target=\"vscode\" role=\"button\">Edit</a>" .
                            "&nbsp;&nbsp;<a onClick=\"ConfirmScriptDelete('$LOGGED_IN_KEY','$RETURN_PAGE','$Files[$x]')\" class=\"btn btn-danger btn-sm\" role=\"button\">Delete</a></td></tr>";
    }
  }
  $Content .=             "</table>";
  $Content .=           "</div>";
  $Content .=         "</td>";
  $Content .=         "<td width=\"70%\">";
  $Content .=           "<iframe type=\"text/html\" name=\"vscode\" style=\"height:82vh;width:100%;float:left;overflow-y:auto;border:none;\" src=\"scriptedit.php$Query\">";
  $Content .=         "</td>";
  $Content .=       "</tr>";
  $Content .=     "</table>";
  $Content .=   "</div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_list_users($DBcnx) {
  $LOGGED_IN_KEY = urlencode(LOGGED_IN_KEY);
  $RETURN_PAGE   = urlencode("index.php?page=list-users");
  $Content  = "<div class=\"card\" style=\"width: 98.75%; padding-left: .25em; padding-right: .25em; margin-left: .75em; margin-bottom: 1em;\">";
  $Content .=   "<div class=\"card-body\">";

  $Content .=     "<table class=\"table table-borderless\">";
  $Content .=       "<tr>";
  $Content .=         "<td width=\"30%\">";
  $Content .=           "<a class=\"btn btn-outline-secondary\" href=\"formedit.php?FormID=4&UserID=0&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\" target=\"formedit\" role=\"button\" style=\"width: 100%;margin-bottom: 0.5em;\">Create New User</a>";
  $Content .=           "<div style=\"height:77.5vh;width:100%;float:left;overflow-y:scroll;\">";
  $Content .=             "<table class=\"table table-striped table-hover\">";
  $Result = mysqli_query($DBcnx,"SELECT * FROM Users ORDER BY UN");
  while ($User = mysqli_fetch_assoc($Result)) {
    $Content .=           "<tr><td class=\"align-middle\">" . $User["UN"] . "</td>" .
                          "<td align=\"right\"><a class=\"btn btn-success btn-sm\" href=\"formedit.php?FormID=4&UserID=" . $User["ID"] . "&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\" target=\"formedit\" role=\"button\">Edit</a>";
    if ($User["ID"] > 1) {
      $Content .=         "&nbsp;&nbsp;<a onClick=\"ConfirmUserDelete('$LOGGED_IN_KEY','$RETURN_PAGE','" . $User["ID"] . "','" . $User["UN"] . "')\" class=\"btn btn-danger btn-sm\" role=\"button\">Delete</a>";
    }
    $Content .=           "</td></tr>";
  }
  $Content .=             "</table>";
  $Content .=           "</div>";
  $Content .=         "</td>";
  $Content .=         "<td width=\"70%\">";
  $Content .=           "<iframe type=\"text/html\" name=\"formedit\" style=\"height:82vh;width:50%;float:left;overflow-y:auto;border:none;\" src=\"formedit.php?FormID=0&LOGGED_IN_KEY=$LOGGED_IN_KEY&RETURN_PAGE=$RETURN_PAGE\">";
  $Content .=         "</td>";
  $Content .=       "</tr>";
  $Content .=     "</table>";

  $Content .=   "</div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_login() {
  if (isset($_GET["result"])) {
    $Failure = true;
  } else {
    $Failure = false;
  }
  if (CZ_THEME == 0) {
    $ButtonBG = "btn-success";
  } else {
    $ButtonBG = "btn-secondary";
  }

  $Content  = "<div class=\"d-flex justify-content-center align-items-center min-vh-100\">";
  $Content .=   "<div class=\"card\" style=\"width: 28rem; margin-bottom: 1em; margin-left: -1em; margin-right: 0.5em;\">";
  $Content .=     "<div class=\"card-body\">";
  $Content .=       "<h5 class=\"card-title\">Climate Czar Login</h5>";
  $Content .=       "<form method=\"post\" action=\"noyb.php\">";
  if ($Failure) {
    $Content .=     "<p class=\"text-danger fw-bolder\">Username and/or password incorrect or not found</p>";
  }
  $Content .=       "<div class=\"mb-3\">";
  $Content .=         "<label for=\"UN\" class=\"form-label\">Username</label>";
  $Content .=         "<input type=\"text\" maxlength=\"50\" class=\"form-control\" id=\"UN\" name=\"UN\" placeholder=\"Enter username\">";
  $Content .=       "</div>";
  $Content .=       "<div class=\"mb-3\">";
  $Content .=         "<label for=\"PW\" class=\"form-label\">Password</label>";
  $Content .=         "<input type=\"password\" maxlength=\"50\" class=\"form-control\" id=\"PW\" name=\"PW\" placeholder=\"Enter password\">";
  $Content .=       "</div>";
  $Content .=       "<button type=\"submit\" class=\"btn $ButtonBG w-100\">Login</button>";
  $Content .=       "</form>";
  $Content .=     "</div>";
  $Content .=   "</div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_logs_graphs($DBcnx) {
  $Now      = time();
  $Now_6    = ($Now - 21600);
  $TStart   = str_replace(" ","T",date('Y-m-d H:i',$Now_6));
  $TEnd     = str_replace(" ","T",date('Y-m-d H:i',$Now));
  if (CZ_BASIC) {
    $CZ_BASIC = 1;
  } else {
    $CZ_BASIC = 0;
  }
  if (CZ_THEME == 0) {
    $ButtonBG = "btn-success";
  } else {
    $ButtonBG = "btn-secondary";
  }

  $Content  = "<div class=\"card\" style=\"width: 98.75%; padding-left: .25em; padding-right: .25em; margin-left: .75em; margin-bottom: 1em;\">";
  $Content .=   "<div class=\"card-body\">";
  $Content .=     "<form method=\"post\" action=\"charts.php\" target=\"charts\">";
  $Content .=     "<input type=\"hidden\" name=\"CZ_BASIC\" value=\"" . CZ_BASIC . "\">";
  $Content .=     "<input type=\"hidden\" name=\"CZ_GROUP\" value=\"" . CZ_GROUP . "\">";
  $Content .=     "<div class=\"container text-center\" style=\"margin-bottom: 1em;\">";
  $Content .=       "<div class=\"row\">";
  $Content .=         "<div class=\"col-2\"><input class=\"form-control\" type=\"datetime-local\" id=\"StartTime\" name=\"StartTime\" value=\"$TStart\"></div>";
  $Content .=         "<div class=\"col-2\"><input class=\"form-control\" type=\"datetime-local\" id=\"EndTime\" name=\"EndTime\" value=\"$TEnd\"></div>";
  $Content .=         "<div class=\"col-3\">" . ChartsDeviceSelector($DBcnx,CZ_GROUP) . "</div>";
  $Content .=         "<div class=\"col-3\" style=\"text-align: right;\"><button type=\"submit\" class=\"btn $ButtonBG\" name=\"cz_show_graphs\">Show Charts &amp; Graphs</button></div>";
  $Content .=         "<div class=\"col-2\" style=\"text-align: left;\"><button type=\"submit\" class=\"btn $ButtonBG\" name=\"cz_show_logs\">Show Device Log</button></div>";
  $Content .=       "</div>";
  $Content .=     "</div>";
  $Content .=     "</form>";
  $Content .=     "<iframe type=\"text/html\" name=\"charts\" style=\"height:78vh;width:100%;float:left;overflow-y:auto;border:none;\" src=\"charts.php?CZ_BASIC=$CZ_BASIC&CZ_GROUP=" . CZ_GROUP . "\">";
  $Content .=   "</div>";
  $Content .= "</div>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_menu_bar($DBcnx) {
  global $BodyBG,$MenuBG,$DisabledFG;

  if (UserLoggedIn($DBcnx)) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM Users WHERE ID=" . $_COOKIE["CZ_USER"]);
    $User = mysqli_fetch_assoc($Result);
  } else {
    $User["SecLevel"] = "anonymous";
  }

  $Content  = "<nav class=\"navbar navbar-expand-lg\" style=\"background-color: $BodyBG;\">";
  $Content .=   "<div class=\"container-fluid\">";
  $Content .=     "<a class=\"navbar-brand\" href=\"index.php\"><span class=\"iconify text-light\" style=\"font-size: 1.5em;\" data-icon=\"game-icons:greenhouse\"></span>&nbsp;<span class=\"text-light\" style=\"font-weight: bold;\">Climate Czar Server v" . VERSION . "</span></a>";
  $Content .=     "<button class=\"navbar-toggler\" type=\"button\" data-bs-toggle=\"collapse\" data-bs-target=\"#navbarSupportedContent\" aria-controls=\"navbarSupportedContent\" aria-expanded=\"false\" aria-label=\"Toggle navigation\">";
  $Content .=       "<span class=\"navbar-toggler-icon\"></span>";
  $Content .=     "</button>";
  $Content .=     "<div class=\"collapse navbar-collapse\" id=\"navbarSupportedContent\">";
  $Content .=       "<ul class=\"navbar-nav me-auto mb-2 mb-lg-0\">";
  $Content .=         "<li class=\"nav-item\">";
  $Content .=           "<a class=\"nav-link\" aria-current=\"page\" href=\"index.php\">Home</a>";
  $Content .=         "</li>";
  $Content .=         "<li class=\"nav-item\">";
  $Content .=           "<a class=\"nav-link\" href=\"index.php?page=logs-graphs\">Logs &amp; Graphs</a>";
  $Content .=         "</li>";

  if (UserSec($DBcnx) > anonymous) {
    $Content .=       "<li class=\"nav-item dropdown\">";
    $Content .=         "<a class=\"nav-link dropdown-toggle\" href=\"#\" role=\"button\" data-bs-toggle=\"dropdown\" aria-expanded=\"false\">Change Group</a>";
    $Content .=         "<ul class=\"dropdown-menu\" style=\"background-color: $MenuBG;\">";
    $Result = mysqli_query($DBcnx,"SELECT * FROM DeviceGroups ORDER BY Name");
    while ($Group = mysqli_fetch_assoc($Result)) {
      if ($Group["ID"] == CZ_GROUP) $GroupName = $Group["Name"];
      $Group["Name"] = str_replace(" ","&nbsp;",$Group["Name"]);
      $Content .=         "<li><a class=\"dropdown-item\" href=\"index.php?CZ_GROUP=" . $Group["ID"] . "\">" . $Group["Name"] . "</a></li>";
    }
    $Content .=         "</ul>";
    $Content .=       "</li>";
  } else {
    $GroupName = "Default Group";
  }

  $Content .=         "<li class=\"nav-item dropdown\">";
  $Content .=           "<a class=\"nav-link dropdown-toggle\" href=\"#\" role=\"button\" data-bs-toggle=\"dropdown\" aria-expanded=\"false\">Management</a>";
  $Content .=           "<ul class=\"dropdown-menu\" style=\"background-color: $MenuBG;\">";
  if (UserSec($DBcnx) >= author) {
    $Content .=           "<li><a class=\"dropdown-item\" href=\"index.php?page=list-devices\">Manage&nbsp;Devices</a></li>";
    $Content .=           "<li><a class=\"dropdown-item\" href=\"index.php?page=list-groups\">Manage&nbsp;Groups</a></li>";
  }
  if (UserSec($DBcnx) > editor) $Content .= "<li><a class=\"dropdown-item\" href=\"index.php?page=list-scripts\">Manage&nbsp;Scripts</a></li>";
  if (UserSec($DBcnx) == administrator) $Content .= "<li><a class=\"dropdown-item\" href=\"index.php?page=list-users\">Manage&nbsp;Users</a></li>";
  if (UserSec($DBcnx) > subscriber) $Content .= "<li><hr class=\"dropdown-divider\"></li>";
  $Content .=             "<li><a class=\"dropdown-item\" href=\"index.php?CZ_BASIC=0\">Set&nbsp;Desktop&nbsp;View</a></li>";
  $Content .=             "<li><a class=\"dropdown-item\" href=\"index.php?CZ_BASIC=1\">Set&nbsp;Basic&nbsp;Mobile&nbsp;View</a></li>";
  $Content .=             "<li><a class=\"dropdown-item\" href=\"index.php?CZ_THEME=0\">Set&nbsp;Light&nbsp;Theme</a></li>";
  $Content .=             "<li><a class=\"dropdown-item\" href=\"index.php?CZ_THEME=1\">Set&nbsp;Dark&nbsp;Theme</a></li>";
  $Content .=           "</ul>";
  $Content .=         "</li>";

  $Content .=         "<li class=\"nav-item\">";
  if ($User["SecLevel"] == "anonymous") {
    $Content .=         "<a class=\"nav-link\" href=\"index.php?page=login\">Log In</a>";
  } else {
    $Content .=         "<a class=\"nav-link\" href=\"noyb.php\">Log Out</a>";
  }
  $Content .=         "</li>";

  $Content .=         "<li class=\"nav-item\">";
  $Content .=           "<a class=\"nav-link disabled\">Current Group: $GroupName</a>";
  $Content .=         "</li>";
  $Content .=       "</ul>";
  $Content .=     "</div>";
  $Content .=   "</div>";
  $Content .= "</nav>";
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_show_all_sensors($DBcnx) {
  $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE Dashboard=1 AND GroupID=" . CZ_GROUP . " ORDER BY Position");
  if (CZ_BASIC) {
    $Content = "";
  } else {
    $Content  = "<div class=\"container-fluid\">";
    $Content .=   "<div class=\"row\">";
  }
  if (mysqli_num_rows($Result) > 0) {
    while ($Sensor = mysqli_fetch_assoc($Result)) {
      $Data["id"] = $Sensor["ID"];
      if (CZ_BASIC) {
        $Content .= cz_show_sensor($DBcnx,$Data);
      } else {
        $Content .= "<div class=\"col\">";
        $Content .= cz_show_sensor($DBcnx,$Data);
        $Content .= "</div>";
      }
    }
  }
  if (CZ_BASIC) {

  } else {
    $Remainder = mysqli_num_rows($Result) % 3;
    if ($Remainder > 0) {
      $Cards = 3 - $Remainder;
      for ($x = 1; $x <= $Cards; $x ++) {
        $Content .= "<div class=\"col\">";
        $Content .=   "<div class=\"card\" style=\"width: 28rem; height: 175px; margin-bottom: 1em; margin-left: -1em; margin-right: 0.5em;\">";
        $Content .=     "<div class=\"card-body\" style=\"padding-bottom: 0.1rem;\">";
        $Content .=       "<p class=\"text-muted\" style=\"text-align: center; margin-top: 50px;\">Unassigned</p>";
        $Content .=     "</div>";
        $Content .=   "</div>";
        $Content .= "</div>";
      }
    }
    $Content .=   "</div>";
    $Content .= "</div>";
  }
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_show_all_switches($DBcnx) {
  $Result = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE Dashboard=1 AND GroupID=" . CZ_GROUP . " ORDER BY Position");
  if (CZ_BASIC) {
    $Content = "";
  } else {
    $Content  = "<div class=\"container-fluid\">";
    $Content .=   "<div class=\"row\">";
  }
  if (mysqli_num_rows($Result) > 0) {
    while ($Switch = mysqli_fetch_assoc($Result)) {
      $Data["id"] = $Switch["ID"];
      if (CZ_BASIC) {
        $Content .= cz_show_switch($DBcnx,$Data);
      } else {
        $Content .= "<div class=\"col\">";
        $Content .= cz_show_switch($DBcnx,$Data);
        $Content .= "</div>";
      }
    }
  }
  if (CZ_BASIC) {

  } else {
    $Remainder = mysqli_num_rows($Result) % 3;
    if ($Remainder > 0) {
      $Cards = 3 - $Remainder;
      for ($x = 1; $x <= $Cards; $x ++) {
        $Content .= "<div class=\"col\">";
        $Content .=   "<div class=\"card\" style=\"width: 28rem; height: 215px; margin-bottom: 1em; margin-left: -1em; margin-right: 0.5em;\">";
        $Content .=     "<div class=\"card-body\" style=\"padding-bottom: 0.1rem;\">";
        $Content .=       "<p class=\"text-muted\" style=\"text-align: center; margin-top: 75px;\">Unassigned</p>";
        $Content .=     "</div>";
        $Content .=   "</div>";
        $Content .= "</div>";
      }
    }
    $Content .=   "</div>";
    $Content .= "</div>";
  }
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_show_sensor($DBcnx,$Data) {
  if (isset($Data["id"])) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $Data["id"]);
    if (mysqli_num_rows($Result) > 0) {
      $Sensor = mysqli_fetch_assoc($Result);
      if (! isset($Data["noajax"])) {
        $RandID   = "input_" . generateRandomString();
        $Content  = AjaxRefreshJS($Sensor["ID"],$RandID,"input");
        $Content .= "<div id=\"$RandID\">";
      } else {
        $Content = "";
      }
      if (CZ_BASIC) {
        if (CZ_THEME == 0) {
          $TableBG = "table-success";
        } else {
          $TableBG = "table-dark";
        }
        $Content .= "<table class=\"table $TableBG table-striped\" style=\"margin-bottom: .5em;\">";
        $Content .= "<tr><td><span style=\"font-weight: bold;\">" . $Sensor["DeviceName"] . "</span></td></tr>";
        if ($Sensor["DeviceType"] <= 2) {
          $Content .= "<tr><td align=\"right\">Current Reading: " . RenderValueBasic($Sensor["Reading"],$Sensor["BGlow"],$Sensor["BGmid"],$Sensor["BGhigh"],$Sensor["ReadingSuffix"]) . "</td></tr>";
        } elseif ($Sensor["DeviceType"] <= 7) {
          $Content .= "<tr><td align=\"right\">Current Status: " . RenderBinary($Sensor["Reading"],true) . "</td></tr>";
        } elseif ($Sensor["DeviceType"] == 8) {
          if (UserSec($DBcnx) >= DeviceSec($Sensor["SwitchSec"])) {
            $Content .= "<tr><td>" . InputSwitchControls(1,$Sensor["ID"],$Sensor["Reading"],"Current&nbsp;Status:&nbsp;" . RenderBinary($Sensor["Reading"],true)) . "</td></tr>";
          } else {
            $Content .= "<tr><td>" . DisabledInputSwitchControls(1,$Sensor["ID"],$Sensor["Reading"],"Current&nbsp;Status:&nbsp;" . RenderBinary($Sensor["Reading"],true)) . "</td></tr>";
          }
        } elseif ($Sensor["DeviceType"] == 9) {
          $Content .=   "<tr><td><div style=\"height:10.5vh;width:100%;float:left;overflow-x:scroll;overflow-y:scroll;\">" . $Sensor["RawText"] . "</div></td></tr>";
        }
        $Content .= "</table>";
      } else {
        $Content .=   "<div class=\"card\" style=\"width: 28rem; margin-bottom: 1em; margin-left: -1em; margin-right: 0.5em;\">";
        $Content .=     "<div class=\"card-body\">";
        $Content .=       "<p style=\"font-weight: bold;\">" . $Sensor["DeviceName"] . "</p>";
        if ($Sensor["DeviceType"] <= 2) {
          $Content .=     RenderValue($Sensor["Reading"],$Sensor["BGlow"],$Sensor["BGmid"],$Sensor["BGhigh"],$Sensor["ReadingSuffix"]);
          $Content .=     RenderProgressBar($Sensor["Reading"],$Sensor["BGlow"],$Sensor["BGmid"],$Sensor["BGhigh"]);
        } elseif ($Sensor["DeviceType"] <= 7) {
          if ($Sensor["DeviceType"] == 3) {
            $Label = "Remote Binary Switch";
          } elseif ($Sensor["DeviceType"] == 4) {
            $Label = "Logical AND Gate";
          } elseif ($Sensor["DeviceType"] == 5) {
            $Label = "Logical OR Gate";
          } elseif ($Sensor["DeviceType"] == 6) {
            $Label = "Logical NOT Gate";
          } elseif ($Sensor["DeviceType"] == 7) {
            $Label = "Daily Scheduler";
          }
          $Content .=     RenderBinary($Sensor["Reading"],false);
          $Content .=     "<p class=\"text-muted\" style=\"font-weight: bold; float: right; margin-bottom: 0;\">Sensor Type: <span class=\"text-success\">$Label</span></p>";
        } elseif ($Sensor["DeviceType"] == 8) {
          $Content .=     RenderBinary($Sensor["Reading"],false);
          if (UserSec($DBcnx) >= DeviceSec($Sensor["SwitchSec"])) {
            $Content .=   InputSwitchControls(0,$Sensor["ID"],$Sensor["Reading"],"");
          } else {
            $Content .=   DisabledInputSwitchControls(0,$Sensor["ID"],$Sensor["Reading"],"");
          }
        } elseif ($Sensor["DeviceType"] == 9) {
          $Content .=     "<div style=\"height:11.5vh;width:100%;float:left;overflow-x:scroll;overflow-y:scroll;\">";
          $Content .=     $Sensor["RawText"];
          $Content .=     "</div>";
        }
        $Content .=     "</div>";
        $Content .=   "</div>";
      }
      if (! isset($Data["noajax"])) {
        $Content .= "</div>";
      }
    }
  }
  return $Content;
}
//---------------------------------------------------------------------------------------------------
function cz_show_switch($DBcnx,$Data) {
  $LOGGED_IN_KEY = urlencode(LOGGED_IN_KEY);
  if (isset($Data["id"])) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE ID=" . $Data["id"]);
    if (mysqli_num_rows($Result) > 0) {
      $Switch = mysqli_fetch_assoc($Result);
      if (! isset($Data["noajax"])) {
        $RandID   = "output_" . generateRandomString();
        $Content  = AjaxRefreshJS($Switch["ID"],$RandID,"output");
        $Content .= "<div id=\"$RandID\">";
      } else {
        $Content = "";
      }
      if (CZ_BASIC) {
        if (CZ_THEME == 0) {
          $TableBG = "table-success";
        } else {
          $TableBG = "table-dark";
        }
        $Content .= "<table class=\"table $TableBG table-striped\" style=\"margin-bottom: .5em;\">";
        $Content .= "<tr><td><span style=\"font-weight: bold;\">" . $Switch["DeviceName"] . "</span></td></tr>";
        if (UserSec($DBcnx) >= DeviceSec($Switch["SwitchSec"])) {
          $Content .= "<tr><td>" . OutputSwitchControls(1,$Switch["ID"],$Switch["OpMode"],"Current&nbsp;Status:&nbsp;" . FormatStatus($Switch["Status"])) . "</td></tr>";
        } else {
          $Content .= "<tr><td>" . DisabledOutputSwitchControls(1,$Switch["ID"],$Switch["OpMode"],"Current&nbsp;Status:&nbsp;" . FormatStatus($Switch["Status"])) . "</td></tr>";
        }
        $Content .= "</table>";
      } else {
        if ($Switch["InputType"] <= 2) {
          $LeftLabel1 = "Switch Polarity";
          $LeftValue1 = FormatPolarity($Switch["Polarity"]);
          $LeftLabel2 = "High Value";
          $LeftValue2 = FormatRange($Switch["HighValue"]);
          $LeftLabel3 = "Low Value";
          $LeftValue3 = FormatRange($Switch["LowValue"]);
        } else {
          $LeftLabel1 = "Sensor Type";
          $LeftValue1 = FormatRange("Binary Value");
          $LeftLabel2 = "Input Type";
          $LeftLabel3 = "Current State";
          $LeftValue3 = FormatBinarySensor($DBcnx,$Switch["InputID"]);
          if ($Switch["InputType"] == 3) {
            $LeftValue2 = FormatRange("Toggle");
          } elseif ($Switch["InputType"] == 4) {
            $LeftValue2 = FormatRange("AND Gate");
          } elseif ($Switch["InputType"] == 5) {
            $LeftValue2 = FormatRange("OR Gate");
          } elseif ($Switch["InputType"] == 6) {
            $LeftValue2 = FormatRange("NOT Gate");
          } elseif ($Switch["InputType"] == 7) {
            $LeftValue2 = FormatRange("Scheduler");
          } elseif ($Switch["InputType"] == 8) {
            $LeftValue2 = FormatRange("Web Console");
          }
        }
        $Content .=   "<div class=\"card\" style=\"width: 28rem; margin-bottom: 1em; margin-left: -1em; margin-right: 0.5em;\">";
        $Content .=     "<div class=\"card-body\" style=\"padding-bottom: 0.1rem;\">";
        $Content .=       "<p style=\"font-weight: bold; margin-bottom: 0.25em;\">" . $Switch["DeviceName"] . "</p>";
        $Content .=       "<table class=\"table table-borderless table-sm\">";
        $Content .=          "<tr><td width=\"50%\"><span class=\"text-muted\">$LeftLabel1: </span>$LeftValue1</td><td width=\"50%\"><span class=\"text-muted\">Input Sensor: </span>" . FormatSensor($DBcnx,$Switch["InputID"]) . "</td></tr>";
        $Content .=          "<tr><td><span class=\"text-muted\">$LeftLabel2: </span>$LeftValue2</td><td><span class=\"text-muted\">Switch Mode: </span>" . FormatMode($Switch["OpMode"]) . "</td></tr>";
        $Content .=          "<tr><td><span class=\"text-muted\">$LeftLabel3: </span>$LeftValue3</td><td><span class=\"text-muted\">Switch State: </span>" . FormatStatus($Switch["Status"]) . "</td></tr>";
        if (UserSec($DBcnx) >= DeviceSec($Switch["SwitchSec"])) {
          $Content .=        "<tr><td colspan=\"2\">" . OutputSwitchControls(0,$Switch["ID"],$Switch["OpMode"],"") . "</td></tr>";
        } else {
          $Content .=        "<tr><td colspan=\"2\">" . DisabledOutputSwitchControls(0,$Switch["ID"],$Switch["OpMode"],"") . "</td></tr>";
        }
        $Content .=       "</table>";
        $Content .=     "</div>";
        $Content .=   "</div>";
      }
      if (! isset($Data["noajax"])) {
        $Content .= "</div>";
      }
    }
  }
  return $Content;
}
 //---------------------------------------------------------------------------------------------------
?>
