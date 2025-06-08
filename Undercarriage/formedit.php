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
if (! $_GET) {
  mysqli_close($DBcnx);
  exit;
} else {
  $IDtag = "";
  if (! isset($_GET["LOGGED_IN_KEY"])) {
    mysqli_close($DBcnx);
    exit;
  } else {
    if ($_GET["LOGGED_IN_KEY"] != LOGGED_IN_KEY) {
      mysqli_close($DBcnx);
      exit;
    }
  }
  if (isset($_GET["FormID"])) {
    $FormID = $_GET["FormID"];
  } else {
    $FormID = 0;
  }
}
//---------------------------------------------------------------------------------------------------
function RGBtoHEX($Value) {
  $RGB = explode(",",$Value);
  $RGB[0] = dechex($RGB[0]);
  if (strlen($RGB[0]) < 2) $RGB[0] = '0' . $RGB[0];
  $RGB[1] = dechex($RGB[1]);
  if (strlen($RGB[1]) < 2) $RGB[1] = '0' . $RGB[1];
  $RGB[2] = dechex($RGB[2]);
  if (strlen($RGB[2]) < 2) $RGB[2] = '0' . $RGB[2];
  return '#' . $RGB[0] . $RGB[1] . $RGB[2];
}
//---------------------------------------------------------------------------------------------------
if ($FormID == 1) { // Group Editor
  if (! isset($_GET["GroupID"])) {
    mysqli_close($DBcnx);
    echo("No GroupID Provided");
    exit;
  }
  if ($_GET["GroupID"] > 0) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM DeviceGroups WHERE ID=" . $_GET["GroupID"]);
    if (mysqli_num_rows($Result) > 0) {
      $Group = mysqli_fetch_assoc($Result);
    } else {
      mysqli_close($DBcnx);
      echo("GroupID Not Found");
      exit;
    }
  } else {
    $Group["ID"]   = 0;
    $Group["Name"] = "";
    $Group["SecLevel"] = "subscriber";
  }
} elseif ($FormID == 2) { // Input Sensor Editor
  if ($_GET["DeviceID"] > 0) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $_GET["DeviceID"]);
    if (mysqli_num_rows($Result) > 0) {
      $Dev = mysqli_fetch_assoc($Result);
      $Dev["DeviceName"]    = htmlspecialchars($Dev["DeviceName"],ENT_QUOTES);
      $Dev["ReadingSuffix"] = htmlspecialchars($Dev["ReadingSuffix"],ENT_QUOTES);
      $Dev["GraphColor"]    = RGBtoHEX($Dev["GraphColor"]);
      $Dev["ScheduleList"]  = htmlspecialchars($Dev["ScheduleList"],ENT_QUOTES);
      $Dev["Notes"]         = htmlspecialchars($Dev["Notes"],ENT_QUOTES);
      $IDtag = "[ID#: " . $Dev["ID"] . "]";
    } else {
      mysqli_close($DBcnx);
      echo("DeviceID Not Found");
      exit;
    }
  } else {
    $Dev["DeviceName"]    = "";
    $Dev["DeviceType"]    = "1";
    $Dev["Dashboard"]     = "1";
    $Dev["Position"]      = "1";
    $Dev["SwitchSec"]     = "subscriber";
    $Dev["GroupID"]       = $_COOKIE["CZ_GROUP"];
    $Dev["ReadingSuffix"] = "";
    $Dev["GraphColor"]    = "#9966FF";
    $Dev["BGlow"]         = "30";
    $Dev["BGmid"]         = "60";
    $Dev["BGhigh"]        = "90";
    $Dev["SensorList"]    = "";
    $Dev["SwitchList"]    = "";
    $Dev["ScheduleList"]  = "";
    $Dev["ReadCommand"]   = "/var/www/html/cz/commands/your-script";
    $Dev["Notes"]         = "";
  }
} elseif ($FormID == 3) { // Output Switch Editor
  if ($_GET["DeviceID"] > 0) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE ID=" . $_GET["DeviceID"]);
    if (mysqli_num_rows($Result) > 0) {
      $Dev = mysqli_fetch_assoc($Result);
      $Dev["DeviceName"] = htmlspecialchars($Dev["DeviceName"],ENT_QUOTES);
      $Dev["GraphColor"] = RGBtoHEX($Dev["GraphColor"]);
      $Dev["Notes"]      = htmlspecialchars($Dev["Notes"],ENT_QUOTES);
      $IDtag = "[ID#: " . $Dev["ID"] . "]";
    } else {
      mysqli_close($DBcnx);
      echo("DeviceID Not Found");
      exit;
    }
  } else {
    $Dev["DeviceName"]     = "";
    $Dev["Dashboard"]      = "1";
    $Dev["Position"]       = "1";
    $Dev["SwitchSec"]      = "subscriber";
    $Dev["GroupID"]        = $_COOKIE["CZ_GROUP"];
    $Dev["InputID"]        = "0";
    $Dev["OverrideID"]     = "0";
    $Dev["OverrideStatus"] = "1";
    $Dev["Polarity"]       = "0";
    $Dev["LowValue"]       = "50";
    $Dev["HighValue"]      = "100";
    $Dev["OneShot"]        = "1";
    $Dev["GraphColor"]     = "#26A644";
    $Dev["OnCommand"]      = "/var/www/html/cz/commands/your-script &";
    $Dev["OffCommand"]     = "/var/www/html/cz/commands/your-script &";
    $Dev["Notes"]          = "";
  }
  $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE GroupID=" . $_COOKIE["CZ_GROUP"] . " AND DeviceType < 9 ORDER BY DeviceName");
  if (mysqli_num_rows($Result) > 0) {
    $JSarray = "const Dev = [];\n       Dev[0] = 0;\n";
    while ($RS = mysqli_fetch_assoc($Result)) {
      $JSarray .= "       Dev[" . $RS["ID"] . "] = " . $RS["DeviceType"] . ";\n";
    }
  }
} elseif ($FormID == 4) { // User Editor
  if (! isset($_GET["UserID"])) {
    mysqli_close($DBcnx);
    echo("No UserID Provided");
    exit;
  }
  if ($_GET["UserID"] > 0) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM Users WHERE ID=" . $_GET["UserID"]);
    if (mysqli_num_rows($Result) > 0) {
      $User = mysqli_fetch_assoc($Result);
    } else {
      mysqli_close($DBcnx);
      echo("UserID Not Found");
      exit;
    }
  } else {
    $User["ID"]           = 0;
    $User["UN"]           = "";
    $User["PW"]           = "";
    $User["SecLevel"]     = "subscriber";
    $User["Phone"]        = "";
    $User["LockedOut"]    = 0;
    $User["LastLogin"]    = "Never";
    $User["LastActivity"] = "Never";
  }
}
//---------------------------------------------------------------------------------------------------
?>
<!DOCTYPE html>
<?php
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
  <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
  <link href="bootstrap/css/bootstrap.min.css" rel="stylesheet">
  <link href="css/cz.css" rel="stylesheet">
  <script src="bootstrap/js/bootstrap.bundle.min.js"></script>
  <script src="js/jquery.min.js"></script>
  <style>
  body {
    background-color: <?= $DevFormBG ?>;
  }
  </style>
</head>
<body>

<?php if ($FormID == 1) { ?>
  <div style="margin: 0.5em;">
    <form id="formedit" method="post" action="process.php" target="_top">
    <div class="mb-3">
      <label for="GroupName" class="form-label">Group Name</label>
      <input type="text" class="form-control" id="GroupName" name="GroupName" maxlength="50" value="<?= $Group["Name"] ?>" aria-describedby="GroupNameHelp">
      <div id="GroupNameHelp" class="form-text">Maximum group name length is limited to 50 characters</div>
    </div>
    <div class="mb-3" style="padding-bottom: 0.5em;">
<?php if ($Group["ID"] != 1) { ?>
      <label for="SwitchSec" class="form-label">Group Security Level</label>
      <?= SwitchSecSelector($Group["SecLevel"]) ?>
<?php } else { ?>
      <label for="SwitchSec" class="form-label">Group Security Level</label>
      <input type="text" class="form-control" id="SwitchSec" name="SwitchSec" maxlength="20" value="anonymous" disabled>
<?php } ?>
    </div>
    <button type="submit" class="btn btn-primary btn-sm" name="cz_group_save">Submit</button>
    <input type="hidden" name="ID" value="<?= $_GET["GroupID"] ?>">
    <input type="hidden" name="LOGGED_IN_KEY" value="<?= LOGGED_IN_KEY ?>">
    <input type="hidden" name="RETURN_PAGE" value="<?= $_GET["RETURN_PAGE"] ?>">
    </form>
  </div>
<?php } elseif ($FormID == 2) { ?>
  <script type="text/javascript">
  //---------------------------------------------------------------------------------------------------
  function ToggleInputFields(DeviceType) {
    var Value = DeviceType.options[DeviceType.selectedIndex].value;
    if (Value <= 2) {
      if (Value == 2) {
        ReadCommandDiv.style.display  = 'none';
        SensorListDiv.style.display   = 'inline';
      } else {
        ReadCommandDiv.style.display  = 'inline';
        SensorListDiv.style.display   = 'none';
      }
      ReadingSuffixDiv.style.display  = 'inline';
      SwitchSecDiv.style.display      = 'none';
      VariableMeterDiv.style.display  = 'inline';
      SwitchListDiv.style.display     = 'none';
      ScheduleListDiv.style.display   = 'none';
    } else {
      ReadingSuffixDiv.style.display  = 'none';
      SensorListDiv.style.display     = 'none';
      VariableMeterDiv.style.display  = 'none';
      if ((Value == 3) || (Value == 9)) {
        ReadCommandDiv.style.display  = 'inline';
      } else {
        ReadCommandDiv.style.display  = 'none';
      }
      if ((Value >= 4) && (Value <= 6)) {
        SwitchListDiv.style.display   = 'inline';
      } else {
        SwitchListDiv.style.display   = 'none';
      }
      if (Value == 7) {
        ScheduleListDiv.style.display = 'inline';
      } else {
        ScheduleListDiv.style.display = 'none';
      }
      if (Value == 8) {
        SwitchSecDiv.style.display    = 'inline';
      } else {
        SwitchSecDiv.style.display    = 'none';
      }
      if (Value == 9) {
        GraphColorDiv.style.display   = 'none';
      } else {
        GraphColorDiv.style.display   = 'inline';
      }
    }
  }
  //---------------------------------------------------------------------------------------------------
  window.onload = function() {
    ToggleInputFields(document.getElementById('DeviceType'));
  }
  //---------------------------------------------------------------------------------------------------
  </script>

  <div style="margin: 0.5em;">
    <form id="formedit" method="post" action="process.php" target="_top">
    <div class="mb-3">
      <label for="DeviceName" class="form-label">Device Name <?= $IDtag ?></label>
      <input type="text" class="form-control" id="DeviceName" name="DeviceName" maxlength="50" value="<?= $Dev["DeviceName"] ?>" aria-describedby="DeviceNameHelp">
      <div id="DeviceNameHelp" class="form-text">Maximum device name length is limited to 50 characters</div>
    </div>
    <div style="margin-bottom: 1em;">
      <label for="DeviceType" class="form-label">Device Type</label>
      <?= DeviceTypeSelector($Dev["DeviceType"]) ?>
    </div>
    <div class="mb-3" id="ReadCommandDiv">
      <label for="ReadCommand" class="form-label">Device Read Command</label>
      <input type="text" class="form-control" id="ReadCommand" name="ReadCommand" maxlength="255" value="<?= $Dev["ReadCommand"] ?>" aria-describedby="ReadCommandHelp">
      <div id="ReadCommandHelp" class="form-text">Full path and file name of the script or executable and optional parameters</div>
    </div>
    <div style="margin-top: 1em;">
      <label for="Dashboard" class="form-label">Show in Dashboard?</label>
      <?= YNSelector($Dev["Dashboard"],"Dashboard") ?>
    </div>
    <div class="mb-3">
      <label for="Position" class="form-label">Dashboard Position</label>
      <input type="number" min="1" max="99" class="form-control" id="Position" name="Position" value="<?= $Dev["Position"] ?>" aria-describedby="PositionHelp">
      <div id="PositionHelp" class="form-text">This device's position as input sensors are displayed from left to right in the dashboard</div>
    </div>
    <div>
      <label for="GroupID" class="form-label">Device Group Assignment</label>
      <?= GroupSelector($DBcnx,$Dev["GroupID"]) ?>
    </div>
    <div class="mb-3" style="margin-top: 1em;" id="GraphColorDiv">
      <label for="GraphColor" class="form-label">Graph Color</label>
      <input type="color" class="form-control" id="GraphColor" name="GraphColor" value="<?= $Dev["GraphColor"] ?>" aria-describedby="GraphColorHelp">
      <div id="GraphColorHelp" class="form-text">The color to render this device's historical graph with under Logs &amp; Graphs</div>
    </div>
    <div class="mb-3" id="ReadingSuffixDiv">
      <label for="ReadingSuffix" class="form-label">Reading Suffix</label>
      <input type="text" class="form-control" id="ReadingSuffix" name="ReadingSuffix" maxlength="50" value="<?= $Dev["ReadingSuffix"] ?>" aria-describedby="ReadingSuffixHelp">
      <div id="ReadingSuffixHelp" class="form-text" style="margin-bottom: 1em;">Suffix to display after the reading of Variable Value Sensors <i>(such as HTML character codes)</i></div>
    </div>
    <div id="SwitchSecDiv">
      <label for="SwitchSec" class="form-label">Web Console Switch Minimum Security Level</label>
      <?= SwitchSecSelector($Dev["SwitchSec"]) ?>
    </div>
    <div id="VariableMeterDiv">
      <div class="row" style="width:99%;">
        <label for="BGlow" class="form-label">Variable Value Sensor Dashboard Meter Crossover Points (1..100)</label>
        <div class="col"><input type="number" class="form-control" min="1" max="100" id="BGlow" name="BGlow" value="<?= $Dev["BGlow"]  ?>"></div>
        <div class="col"><input type="number" class="form-control" min="1" max="100" id="BGmid" name="BGmid" value="<?= $Dev["BGmid"]  ?>"></div>
        <div class="col"><input type="number" class="form-control" min="1" max="100" id="BGhigh" name="BGhigh" value="<?= $Dev["BGhigh"]  ?>"></div>
      </div>
      <div class="row" style="width:99%; margin-bottom: 0.5em;">
        <div class="col text-muted">Blue&rarr;Green</div>
        <div class="col text-muted">Green&rarr;Amber</div>
        <div class="col text-muted">Amber&rarr;Red</div>
      </div>
    </div>
    <div class="mb-3" id="SensorListDiv">
      <label for="SensorList" class="form-label">Average of multiple Variable Value Sensors</label>
      <?= ArraySelector($DBcnx,1,"SensorList",$Dev["SensorList"]) ?>
      <div id="SensorListHelp" class="form-text">Control/Command+Click all sensors to include in the array</div>
    </div>
    <div class="mb-3" id="SwitchListDiv">
      <label for="SwitchList" class="form-label">Logic Gate of multiple Remote Binary Switches</label>
      <?= ArraySelector($DBcnx,3,"SwitchList",$Dev["SwitchList"]) ?>
      <div id="SwitchListHelp" class="form-text">Control/Command+Click all switches to include in the array</div>
    </div>
    <div class="mb-3" id="ScheduleListDiv">
      <label for="ScheduleList" class="form-label">Daily Schedule List</label>
      <textarea class="form-control" id="ScheduleList" name="ScheduleList" style="height: 100px;" aria-describedby="ScheduleListHelp"><?= $Dev["ScheduleList"] ?></textarea>
      <div id="ScheduleListHelp" class="form-text">ON/OFF schedules in 24 hour format on separate lines (example: 06:00-21:00)</div>
    </div>
    <div style="margin-top: 1em; margin-bottom: 1em;">
      <label for="Notes" class="form-label">Device Notes</label>
      <textarea class="form-control" id="Notes" name="Notes" style="height: 100px;"><?= $Dev["Notes"] ?></textarea>
    </div>
    <button type="submit" class="btn btn-primary" name="cz_input_sensor_save">Submit</button>
    <input type="hidden" name="ID" value="<?= $_GET["DeviceID"] ?>">
    <input type="hidden" name="LOGGED_IN_KEY" value="<?= LOGGED_IN_KEY ?>">
    <input type="hidden" name="RETURN_PAGE" value="<?= $_GET["RETURN_PAGE"] . "&DevType=0" ?>">
    </form>
  </div>
<?php } elseif ($FormID == 3) { ?>
  <script type="text/javascript">
  //---------------------------------------------------------------------------------------------------
  function ToggleOutputFields(DeviceType) {
    var Value = DeviceType.options[DeviceType.selectedIndex].value;
    // Insert the PHP generated InputType array here
    <?= $JSarray ?>
    InputType = Dev[Value];
    if ((InputType == 1) || (InputType == 2)) {
      HighLowDiv.style.display  = 'inline';
      PolarityDiv.style.display = 'inline';
    } else {
      HighLowDiv.style.display  = 'none';
      PolarityDiv.style.display = 'none';
    }
  }
  //---------------------------------------------------------------------------------------------------
  window.onload = function() {
    ToggleOutputFields(document.getElementById('InputID'));
  }
  //---------------------------------------------------------------------------------------------------
  </script>

  <div style="margin: 0.5em;">
    <form id="formedit" method="post" action="process.php" target="_top">
    <div class="mb-3">
      <label for="DeviceName" class="form-label">Device Name <?= $IDtag ?></label>
      <input type="text" class="form-control" id="DeviceName" name="DeviceName" maxlength="50" value="<?= $Dev["DeviceName"] ?>" aria-describedby="DeviceNameHelp">
      <div id="DeviceNameHelp" class="form-text">Maximum device name length is limited to 50 characters</div>
    </div>
    <div style="margin-bottom: 1em;">
      <label for="InputID" class="form-label">Assigned Input Sensor</label>
      <?= InputSelector($DBcnx,$Dev["InputID"],"InputID") ?>
    </div>
    <div class="mb-3">
      <label for="OnCommand" class="form-label">Device ON Command</label>
      <input type="text" class="form-control" id="OnCommand" name="OnCommand" maxlength="255" value="<?= $Dev["OnCommand"] ?>" aria-describedby="OnCommandHelp">
      <div id="OnCommandHelp" class="form-text">Full path and file name of the script or executable and optional parameters</div>
    </div>
    <div class="mb-3">
      <label for="OffCommand" class="form-label">Device OFF Command</label>
      <input type="text" class="form-control" id="OffCommand" name="OffCommand" maxlength="255" value="<?= $Dev["OffCommand"] ?>" aria-describedby="OffCommandHelp">
      <div id="OffCommandHelp" class="form-text">Full path and file name of the script or executable and optional parameters</div>
    </div>
    <div>
      <label for="Dashboard" class="form-label">Show in Dashboard?</label>
      <?= YNSelector($Dev["Dashboard"],"Dashboard") ?>
    </div>
    <div class="mb-3">
      <label for="Position" class="form-label">Dashboard Position</label>
      <input type="number" min="1" max="99" class="form-control" id="Position" name="Position" value="<?= $Dev["Position"] ?>" aria-describedby="PositionHelp">
      <div id="PositionHelp" class="form-text">This device's position as output switches are displayed from left to right in the dashboard</div>
    </div>
    <div style="margin-bottom: 1em;">
      <label for="GroupID" class="form-label">Device Group Assignment</label>
      <?= GroupSelector($DBcnx,$Dev["GroupID"]) ?>
    </div>
    <div class="mb-3">
      <label for="GraphColor" class="form-label">Graph Color</label>
      <input type="color" class="form-control" id="GraphColor" name="GraphColor" value="<?= $Dev["GraphColor"] ?>" aria-describedby="GraphColorHelp">
      <div id="GraphColorHelp" class="form-text">The color to render this device's historical graph with under Logs &amp; Graphs</div>
    </div>
    <div>
      <label for="SwitchSec" class="form-label">AUTO/ON/OFF Switch Minimum Security Level</label>
      <?= SwitchSecSelector($Dev["SwitchSec"]) ?>
    </div>
    <div style="margin-top: 1em; margin-bottom: 1em;">
      <label for="OverrideID" class="form-label">Assigned Override Output Switch</label>
      <?= OutputSelector($DBcnx,$Dev["OverrideID"],$_GET["DeviceID"],"OverrideID") ?>
      <label for="OverrideStatus" class="form-label">Override Switch Status <span class="text-muted"><i>(To Prevent Activation)</i></span></label>
      <?= OnOffSelector($Dev["OverrideStatus"],"OverrideStatus") ?>
    </div>
    <div id="HighLowDiv">
      <div class="row" style="width:99%;">
        <label for="BGlow" class="form-label">Switching Range for Variable Value Sensors</label>
        <div class="col"><input type="number" class="form-control" id="LowValue" name="LowValue" step=".1" value="<?= $Dev["LowValue"]  ?>"></div>
        <div class="col"><input type="number" class="form-control" id="HighValue" name="HighValue" step=".1" value="<?= $Dev["HighValue"]  ?>"></div>
      </div>
      <div class="row" style="width:99%; margin-bottom: 0.5em;">
        <div class="col text-muted">Low Value</div>
        <div class="col text-muted">High Value</div>
      </div>
    </div>
    <div id="PolarityDiv">
      <label for="Polarity" class="form-label">Switching Polarity for Variable Value Sensors</label>
      <?= PolaritySelector($Dev["Polarity"]) ?>
    </div>
    <div class="mb-3" style="margin-top: 1em;">
      <label for="OneShot" class="form-label">One-Shot ON/OFF Command Mode</label>
      <?= YNSelector($Dev["OneShot"],"OneShot") ?>
      <div id="OneShotHelp" class="form-text">Yes = run commands once per status change, No = run commands every minute</div>
    </div>
    <div style="margin-top: 1em; margin-bottom: 1em;">
      <label for="Notes" class="form-label">Device Notes</label>
      <textarea class="form-control" id="Notes" name="Notes" style="height: 100px;"><?= $Dev["Notes"] ?></textarea>
    </div>
    <button type="submit" class="btn btn-primary" name="cz_output_switch_save">Submit</button>
    <input type="hidden" name="ID" value="<?= $_GET["DeviceID"] ?>">
    <input type="hidden" name="LOGGED_IN_KEY" value="<?= LOGGED_IN_KEY ?>">
    <input type="hidden" name="RETURN_PAGE" value="<?= $_GET["RETURN_PAGE"] . "&DevType=1" ?>">
    </form>
  </div>
<?php } elseif ($FormID == 4) { ?>
  <div style="margin: 0.5em;">
    <form id="formedit" method="post" action="process.php" target="_top">
    <div class="mb-3">
      <label for="UN" class="form-label">Username</label>
      <input type="text" class="form-control" id="GroupName" name="UN" maxlength="50" value="<?= $User["UN"] ?>" aria-describedby="UserNameHelp">
      <div id="UserNameHelp" class="form-text">Maximum username length is limited to 50 characters</div>
    </div>
    <div class="mb-3">
      <label for="PW" class="form-label">Password</label>
      <input type="text" class="form-control" id="GroupName" name="PW" maxlength="50" value="" aria-describedby="PasswordHelp" placeholder="Enter new password">
      <div id="PasswordHelp" class="form-text">Maximum password length is limited to 50 characters</div>
    </div>
    <div class="mb-3">
<?php if ($User["ID"] != 1) { ?>
      <label for="SwitchSec" class="form-label">User Security Level</label>
      <?= SwitchSecSelector($User["SecLevel"]) ?>
<?php } else { ?>
      <label for="SwitchSec" class="form-label">User Security Level</label>
      <input type="text" class="form-control" id="SwitchSec" name="SwitchSec" maxlength="20" value="administrator" disabled>
<?php } ?>
    </div>
    <div class="mb-3">
      <label for="Phone" class="form-label">Mobile Phone</label>
      <input type="text" class="form-control" id="Phone" name="Phone" maxlength="15" value="<?= $User["Phone"] ?>" aria-describedby="PhoneHelp">
      <div id="PhoneHelp" class="form-text">Maximum mobile phone number length is limited to 15 characters</div>
    </div>
    <div class="mb-3">
<?php if ($User["ID"] != 1) { ?>
      <label for="LockedOut" class="form-label">Locked Out</label>
      <?= YNSelector($User["LockedOut"],"LockedOut") ?>
<?php } else { ?>
      <label for="LockedOut" class="form-label">LockedOut</label>
      <input type="text" class="form-control" id="LockedOut" name="LockedOut" maxlength="2" value="No" disabled>
<?php } ?>
    </div>
    <div class="mb-3">
      <label for="LastLogin" class="form-label">Last Login</label>
      <input type="text" class="form-control" id="LastLogin" name="LastLogin" maxlength="20" value="<?= $User["LastLogin"] ?>" disabled>
    </div>
    <div class="mb-3" style="padding-bottom: 0.5em;">
      <label for="LastActivity" class="form-label">Last Activity</label>
      <input type="text" class="form-control" id="LastActivity" name="LastActivity" maxlength="20" value="<?= $User["LastActivity"] ?>" disabled>
    </div>
    <button type="submit" class="btn btn-primary" name="cz_user_save">Submit</button>
    <input type="hidden" name="ID" value="<?= $_GET["UserID"] ?>">
    <input type="hidden" name="LOGGED_IN_KEY" value="<?= LOGGED_IN_KEY ?>">
    <input type="hidden" name="RETURN_PAGE" value="<?= $_GET["RETURN_PAGE"] ?>">
    </form>
  </div>
<?php } mysqli_close($DBcnx); ?>

</body>
</html>
