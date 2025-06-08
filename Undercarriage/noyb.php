<?php
//---------------------------------------------------------------------------------------------------
require_once("subs.php");
//---------------------------------------------------------------------------------------------------
if ((isset($_POST["UN"])) && (isset($_POST["PW"]))) {
  $UN = trim($_POST["UN"]);
  $PW = md5(trim($_POST["PW"]));
  $DBcnx  = mysqli_connect(DB_HOST,DB_USER,DB_PASS,DB_NAME);
  $Result = mysqli_query($DBcnx,"SELECT * FROM Users WHERE UN = BINARY '$UN' AND PW = BINARY '$PW' AND LockedOut=0");
  if (mysqli_num_rows($Result) > 0) {
    $Found = true;
    $User = mysqli_fetch_assoc($Result);
    setcookie("CZ_USER",$User["ID"],0,"/");
    setcookie("CZ_TOKEN",md5($User["UN"] . ":" . $User["PW"]),0,"/");
    $Result = mysqli_query($DBcnx,"UPDATE Users SET LastLogin=now(),LastActivity=now() WHERE ID=" . $User["ID"]);
  } else {
    $Found = false;
  }
  mysqli_close($DBcnx);
  if ($Found) {
    header("Location: index.php");
  } else {
    header("Location: index.php?page=login&result=0");
  }
} else {
  setcookie("CZ_USER","",time() - 3600,"/");
  setcookie("CZ_TOKEN","",time() - 3600,"/");
  setcookie("CZ_GROUP","",time() - 3600,"/");
  header("Location: index.php");
}
//---------------------------------------------------------------------------------------------------
?>
