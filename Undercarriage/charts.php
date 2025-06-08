<?php
//---------------------------------------------------------------------------------------------------
require_once("subs.php");
$Content = "";
//---------------------------------------------------------------------------------------------------
if ($_POST) {
  if ($_POST["ChartsDeviceSelector"] != "OV") {
    if ($_POST["ChartsDeviceSelector"][0] == "I") {
      $_POST["ChartsDeviceSelector"] = str_replace("I","",$_POST["ChartsDeviceSelector"]);
      setcookie("CZ_OV",1,0,"/");
      $ShowInputs = true;
    } else {
      $_POST["ChartsDeviceSelector"] = str_replace("O","",$_POST["ChartsDeviceSelector"]);
      setcookie("CZ_OV",0,0,"/");
      $ShowInputs = false;
    }
  }
  $CZ_BASIC  = $_POST["CZ_BASIC"];
  $CZ_GROUP  = $_POST["CZ_GROUP"];
  $StartTime = str_replace("T"," ",$_POST["StartTime"]) . ":00";
  $EndTime   = str_replace("T"," ",$_POST["EndTime"]) . ":00";
} else {
  if (! isset($_GET["ScreenCheck"])) {
    // Cheap parlor trick to get the user's browser/screen resolution from Javascript into PHP
    require_once("colors.php");
    $JS = "<html>\n" .
          "<head>\n" .
          "<title>Screen Resolution Check</title>\n" .
          "<link href=\"bootstrap/css/bootstrap.min.css\" rel=\"stylesheet\">" .
          "<link href=\"css/cz.css\" rel=\"stylesheet\">" .
          "</head>\n" .
          "<body style=\"background-color: $DevFormBG;\">\n" .
          "<p class=\"text-primary fw-bolder blink\" style=\"text-align: center; padding-top: 150px;\">Graph Data Loading...</p>" .
          "<script type=\"text/javascript\">\n" .
          "window.onload = function() {\n" .
          "window.location.href = '" . $_SERVER["PHP_SELF"] . "?ScreenCheck=done&ScreenWidth=' + screen.availWidth + '&ScreenHeight=' + screen.availHeight + '&CZ_BASIC=" . $_GET["CZ_BASIC"] . "&CZ_GROUP=" . $_GET["CZ_GROUP"] . "';\n" .
          "}\n" .
          "</script>\n" .
          "</body>\n" .
          "</html>\n";
    echo($JS);
    exit;
  } else {
    setcookie("CZ_WIDTH",$_GET["ScreenWidth"],0,"/");
    setcookie("CZ_HEIGHT",$_GET["ScreenHeight"],0,"/");
  }
  $CZ_BASIC  = $_GET["CZ_BASIC"];
  $CZ_GROUP  = $_GET["CZ_GROUP"];
  $Now       = time();
  $Now_6     = ($Now - 21600);
  $StartTime = date('Y-m-d H:i:s',$Now_6);
  $EndTime   = date('Y-m-d H:i:s',$Now);
  $_POST["ChartsDeviceSelector"] = "OV";
  setcookie("CZ_OV",1,0,"/");
}
//---------------------------------------------------------------------------------------------------
function LineChart($DBcnx,$Input,$ID,$Type,$Width,$Height,$Labels,$RGB) {
  global $StartTime,$EndTime,$CZ_BASIC;
  if ($Input) {
    $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=$ID");
    $Sensor = mysqli_fetch_assoc($Result);
    $cz_Label = $Sensor["DeviceName"];
    $Result = mysqli_query($DBcnx,"SELECT * FROM InputHistory WHERE TimeStamp >= '$StartTime' AND TimeStamp <= '$EndTime' AND DeviceID=$ID ORDER BY ID");
  } else {
    $Result = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE ID=$ID");
    $Switch = mysqli_fetch_assoc($Result);
    $cz_Label = $Switch["DeviceName"];
    $Result = mysqli_query($DBcnx,"SELECT * FROM OutputHistory WHERE TimeStamp >= '$StartTime' AND TimeStamp <= '$EndTime' AND DeviceID=$ID ORDER BY ID");
  }
  $cz_Columns = "";
  $cz_Data    = "";
  $cz_ID      = "cz_" . generateRandomString();
  if (mysqli_num_rows($Result) > 0) {
    while ($Log = mysqli_fetch_assoc($Result)) {
      if ($Labels) {
        $cz_Columns .= "'" . $Log["TimeStamp"] . "',";
      } else {
        $cz_Columns .= "'',";
      }
      if ($Input) {
        $cz_Data .= "'" . $Log["Reading"] . "',";
      } else {
        $cz_Data .= "'" . $Log["Status"] . "',";
      }
    }
  }
  $cz_Columns = trim($cz_Columns,",");
  $cz_Data    = trim($cz_Data,",");
  if ($Type == 1) {
    $Content = file_get_contents(APP_ROOT . "css/linear-single.html");
  } elseif ($Type == 2) {
    $Content = file_get_contents(APP_ROOT . "css/stepped-single.html");
  }
  if (! $Labels) $Content = str_replace("pointRadius: 4","pointRadius: 0",$Content);
  $Content = str_replace("{cz_Width}",$Width,$Content);
  $Content = str_replace("{cz_Height}",$Height,$Content);
  $Content = str_replace("{cz_ID}",$cz_ID,$Content);
  $Content = str_replace("{cz_Columns}",$cz_Columns,$Content);
  $Content = str_replace("{cz_Data}",$cz_Data,$Content);
  $Content = str_replace("{cz_Label}",$cz_Label,$Content);
  $Content = str_replace("{cz_RGB}",$RGB,$Content);
  return $Content;
}
//---------------------------------------------------------------------------------------------------
$DBcnx = mysqli_connect(DB_HOST,DB_USER,DB_PASS,DB_NAME);

if ($_POST["ChartsDeviceSelector"] != "OV") {
  if (isset($_POST["cz_show_logs"])) {
    if ($ShowInputs) {
      $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $_POST["ChartsDeviceSelector"]);
      $Sensor = mysqli_fetch_assoc($Result);
      $Result = mysqli_query($DBcnx,"SELECT * FROM InputHistory WHERE TimeStamp >= '$StartTime' AND TimeStamp <= '$EndTime' AND DeviceID=" . $_POST["ChartsDeviceSelector"] . " ORDER BY ID desc");
    } else {
      $Result = mysqli_query($DBcnx,"SELECT * FROM OutputHistory WHERE TimeStamp >= '$StartTime' AND TimeStamp <= '$EndTime' AND DeviceID=" . $_POST["ChartsDeviceSelector"] . " ORDER BY ID desc");
    }
    if (mysqli_num_rows($Result) > 0) {
      $Content  = "<table align=\"center\" class=\"table table-striped table-hover\" style=\"width: 50%;\">";
      $Content .= "<tr><td><b>Time Stamp</b></td><td><b>Reading / Status</b></td></tr>";
      while ($Log = mysqli_fetch_assoc($Result)) {
        $Content .= "<tr><td>" . $Log["TimeStamp"] . "</td><td>";
        if ($ShowInputs) {
          if ($Sensor["DeviceType"] <= 2) {
            $Content .= $Log["Reading"] . $Sensor["ReadingSuffix"];
          } else {
            if ($Sensor["DeviceType"] == 9) {
              $Content .= $Log["RawText"];
            } else {
              $Content .= BinToStr($Log["Reading"]);
            }
          }
        } else {
          $Content .= BinToStr($Log["Status"]);
        }
        $Content .= "</td></tr>";
      }
      $Content .= "</table>";
    } else {
      $Content = "<br><br><br><br><center>No data found</center>";
    }
  } else {
    if ($ShowInputs) {
      $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE ID=" . $_POST["ChartsDeviceSelector"]);
      $Sensor = mysqli_fetch_assoc($Result);
      if ($Sensor["DeviceType"] <= 2) {
        $Content = LineChart($DBcnx,true,$_POST["ChartsDeviceSelector"],1,100,600,true,$Sensor["GraphColor"]);
      } elseif ($Sensor["DeviceType"] <= 8) {
        $Content = LineChart($DBcnx,true,$_POST["ChartsDeviceSelector"],2,100,600,true,$Sensor["GraphColor"]);
      }
    } else {
      $Result  = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE ID=" . $_POST["ChartsDeviceSelector"]);
      $Switch  = mysqli_fetch_assoc($Result);
      $Content = LineChart($DBcnx,false,$_POST["ChartsDeviceSelector"],2,100,600,true,$Switch["GraphColor"]);
    }
  }
} else {
  if (isset($_POST["cz_show_logs"])) {
    $Content = "<br><br><br><br><center>First select a device in order to view an event log</center>";
  } else {
    $Result = mysqli_query($DBcnx,"SELECT * FROM InputDevices WHERE GroupID=$CZ_GROUP AND DeviceType < 9 ORDER BY Position");
    if (mysqli_num_rows($Result) > 0) {
      $Content = "<div class=\"container text-center\">\n";
      $Column  = 0;
      while ($Sensor = mysqli_fetch_assoc($Result)) {
        $Column ++;
        if ($Column == 1) {
          $Content .= "<div class=\"row\">";
        }
        if ($Sensor["DeviceType"] <= 2) {
          $Content .= "<div class=\"col-4\">" . LineChart($DBcnx,true,$Sensor["ID"],1,85,186,false,$Sensor["GraphColor"]) . "</div>"; // #9966FF = "153,102,255"
        } else {
          $Content .= "<div class=\"col-4\">" . LineChart($DBcnx,true,$Sensor["ID"],2,85,186,false,$Sensor["GraphColor"]) . "</div>"; // #36A2EB = "54,162,235"
        }
        if ($Column == 3) {
          $Content .= "</div>\n";
          $Column   = 0;
        }
      }
      if ($Column == 1) { $Content .= "<div class=\"col-4\">&nbsp;</div><div class=\"col-4\">&nbsp;</div>\n"; }
      elseif ($Column == 2) { $Content .= "<div class=\"col-4\">&nbsp;</div>\n"; }
      $Content .= "</div>\n";
    }
    $Result = mysqli_query($DBcnx,"SELECT * FROM OutputSwitches WHERE GroupID=$CZ_GROUP ORDER BY Position");
    if (mysqli_num_rows($Result) > 0) {
      $Content .= "<div class=\"container text-center\">\n";
      $Column   = 0;
      while ($Switch = mysqli_fetch_assoc($Result)) {
        $Column ++;
        if ($Column == 1) {
          $Content .= "<div class=\"row\">";
        }
        $Content .= "<div class=\"col-4\">" . LineChart($DBcnx,false,$Switch["ID"],2,85,186,false,$Switch["GraphColor"]) . "</div>"; // #26A644 = "38,166,68"
        if ($Column == 3) {
          $Content .= "</div>\n";
          $Column   = 0;
        }
      }
      if ($Column == 1) { $Content .= "<div class=\"col-4\">&nbsp;</div><div class=\"col-4\">&nbsp;</div>\n"; }
      elseif ($Column == 2) { $Content .= "<div class=\"col-4\">&nbsp;</div>\n"; }
      $Content .= "</div>\n";
    }
  }
}

mysqli_close($DBcnx);
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
  <link href="bootstrap/css/bootstrap.min.css" rel="stylesheet">
  <link href="css/cz.css" rel="stylesheet">
  <script src="bootstrap/js/bootstrap.bundle.min.js"></script>
  <script src="js/chart.js"></script>
  <? if (CZ_THEME == 1) { ?>
  <style>
  body {
    background-color: <?= $BodyBG ?>;
  }
  </style>
  <? } ?>
</head>
<body>

<?= $Content ?>

</body>
</html>
