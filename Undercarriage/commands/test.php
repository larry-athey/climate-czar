#!/usr/bin/php
<?php
  // This is just a test script I use and an example to show how you can
  // use PHP as a far more functional shell scripting language than bash.

  // My cheap parlor trick to convert a static web page to a PHP include
  $Code  = addslashes(trim(file_get_contents("/home/larryathey/raw.html")));
  $Code2 = "<?phpn  $Content  = "<!-- Now add an include("include.php") and echo the variable $Content -->";n";
  $Lines = explode("n",$Code);
  for ($x = 0; $x <= (count($Lines) - 1); $x ++) {
    if (trim($Lines[$x] != "")) $Code2 .= "  $Content .= "" . trim($Lines[$x]) . "";n";
  }
  $Code2 .= "?>n";
  file_put_contents("/home/pi/include.php",$Code2);

  echo($Code2);
?>
