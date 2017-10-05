<!DOCTYPE html>
<?php
session_start();
include 'ituscript.php';
include 'config.php';
$login = $_SESSION['login'];
if(isset($_SESSION['prihlasenyuzivatel'])){
  $prihlasenyuzivatel = $_SESSION['prihlasenyuzivatel'];
}
 ?>
<html>
<head>
  <title>Atlas</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
    <div class="top-panel">
      <ul>
        <li><a href="index.php" >Domu</a></li>
      </ul>
    </div>
<div class="container">
  <!--vypsani obsahu -->
  <?php

  if($login){
    if(isset($_SESSION['prihlasenyuzivatel'])){
      itu_dbOpen();
      $array = itu_selectPlant('*', '*', '*', '*');
      echo ("<table>");
      echo("<tr><th>Foto</th><th>Nazev</th></tr>");
      foreach($array as $tmp){
        if(itu_isFav($tmp['PLANTID'], $prihlasenyuzivatel)){
          echo("<tr><td width=\"185px\"><a href=\"flower.php?plantid=".$tmp['PLANTID']."&back=2\"><img class=\"thumbnail\" src=\"".$tmp['LINK']."\"/></a></td><td>".$tmp['NAME']."</td></tr>");
        }
      }
      echo "</table>";
      itu_dbClose();
    }
  }
  else {
    echo '<p>Tato funkce je dostupna pouze pro prihlasene uzivatele</p>';
    echo '<p>Prihlaseni/Registrace <a href="logReg.php">zde</a></p>';
  }
   ?>

</div>
</body>
</html>
