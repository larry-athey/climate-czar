<?php
//---------------------------------------------------------------------------------------------------
require_once("html.php");
//---------------------------------------------------------------------------------------------------
$DBcnx = mysqli_connect(DB_HOST,DB_USER,DB_PASS,DB_NAME);
if ((! UserLoggedIn($DBcnx)) || (UserSec($DBcnx) < editor)) {
  mysqli_close($DBcnx);
  exit;
}
mysqli_close($DBcnx);
//---------------------------------------------------------------------------------------------------
if (! $_GET) {
  exit;
} else {
  if (! isset($_GET["LOGGED_IN_KEY"])) {
    exit;
  } else {
    if ($_GET["LOGGED_IN_KEY"] != LOGGED_IN_KEY) exit;
  }
  if (isset($_GET["ScriptName"])) {
    $ScriptName = $_GET["ScriptName"];
    $OldName    = $ScriptName;
    if ($ScriptName == "new-script") {
      $Code = "#!/bin/bash\\n\\n";
    } else {
      $Code = file_get_contents("commands/$ScriptName");
      $Code = addslashes($Code);
      $Code = str_replace("\n","\\n",$Code);
    }
  } else {
    $ScriptName = "new-script";
    $OldName    = $ScriptName;
    $Code       = "#!/bin/bash\\n\\n";
  }
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
  <title>Climate Czar Script Editor</title>
  <meta http-equiv="cache-control" content="max-age=0">
  <meta http-equiv="cache-control" content="no-cache">
  <meta http-equiv="expires" content="0">
  <meta http-equiv="expires" content="Thu, 01 Jan 1970 1:00:00 GMT">
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
  <form id="scripteditor" method="post" action="process.php" target="_top" onSubmit="setupFormPost()">
  <table class="table table-borderless">
    <tr>
      <td colspan="3"><div id="vscode" style="width:100%;height:700px;border:1px solid grey;"></div></td>
    <tr>
    <tr>
      <td width="33%" align="left">
        <a class="btn btn-success" href="scriptedit.php?LOGGED_IN_KEY=<?= urlencode(LOGGED_IN_KEY) ?>&RETURN_PAGE=<?= $_GET["RETURN_PAGE"] . "&ScriptName=new-script" ?>" role="button">New Script</a>
      </td>
      <td width="33%" align="right">
        <input type="submit" class="btn btn-primary" name="cz_script_edit_save" value="Save Script">
      </td>
      <td  width="33%">
        <input class="form-control" name="ScriptName" type="text" maxlength="50" value="<?= $ScriptName ?>">
        <input type="hidden" id="Code" name="Code" value="">
        <input type="hidden" name="OldName" value="<?= $OldName ?>">
        <input type="hidden" name="LOGGED_IN_KEY" value="<?= LOGGED_IN_KEY ?>">
        <input type="hidden" name="RETURN_PAGE" value="<?= $_GET["RETURN_PAGE"] ?>">
      </td>
    </tr>
  </table>
  </form>

  <script src="monaco-editor/min/vs/loader.js"></script>
  <script type="text/javascript">
    require.config({paths: {'vs':'monaco-editor/min/vs'}});
    require(['vs/editor/editor.main'],function() {
      var editor = monaco.editor.create(document.getElementById('vscode'), {
        value: '<?= $Code ?>',
<?php if (InStr(".php",$ScriptName)) { ?>
        language: 'php',
<?php } elseif (InStr(".py",$ScriptName)) { ?>
        language: 'python',
<?php } else { ?>
        language: 'shell',
<?php } ?>
        theme: 'vs-dark'
      });
    });

    function setupFormPost() {
      var value = monaco.editor.getModels()[0].getValue();
      jQuery('#Code').val(value);
      return true;
    }
  </script>

</body>
</html>
