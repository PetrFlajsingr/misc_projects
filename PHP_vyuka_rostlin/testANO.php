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
  <title>TestANO</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
  <div class="top-panel">
    <ul>
      <li><a href="tests.php" >Zpet</a></li>
      <li class="hide"><a href="#" >nic</a></li>
      <li class="hide"><a href="#" >nic</a></li>
      <li class="hide"><a href="#" >nic</a></li>
    </ul>
  </div>
<div class="container">
  <div class="moznostiANO">
  <?php
  itu_dbOpen();
  if(itu_favCount($prihlasenyuzivatel) > 0){
    if(isset($_GET['cor'])){
        $cor = $_GET['cor'];
        if($cor){
          if(isset($_SESSION['correct'])){
            $_SESSION['correct']++;
          } else{
            $_SESSION['correct'] = 1;
          }
        }else{
          if(isset($_SESSION['wrong'])){
            $_SESSION['wrong']++;
          } else{
            $_SESSION['wrong'] = 1;
          }
        }
      }
    if(((isset($_SESSION['correct']) ? $_SESSION['correct'] : 0) + (isset($_SESSION['wrong']) ? $_SESSION['wrong'] : 0)) == itu_favCount($prihlasenyuzivatel)){
      echo "Spravne odpovedi: ". (isset($_SESSION['correct']) ? $_SESSION['correct'] : 0). "<br>";
      echo "Spatne odpovedi: ". (isset($_SESSION['wrong']) ? $_SESSION['wrong'] : 0);
      itu_changeStat($prihlasenyuzivatel, (isset($_SESSION['correct']) ? $_SESSION['correct'] : 0), (isset($_SESSION['wrong']) ? $_SESSION['wrong'] : 0));
      if(isset($_SESSION['correct'])){
        $_SESSION['correct'] = 0;
      }
      if(isset($_SESSION['wrong'])){
        $_SESSION['wrong'] = 0;
      }
    }else{
      $r = (isset($_SESSION['correct']) ? $_SESSION['correct'] : 0);
        $w = (isset($_SESSION['wrong']) ? $_SESSION['wrong'] : 0);
        $otazka = $r + $w + 1;
        echo "Otazka " . $otazka . "/" . itu_favCount($prihlasenyuzivatel) . "<br>";
      while(!itu_isFav($rnd = rand(0,9), $prihlasenyuzivatel)) ;
       while(!itu_isFav($rnd2 = rand(0,9), $prihlasenyuzivatel));
      echo "<img src=\"".itu_getLink($rnd)."\"/>";
      echo "<h1>".itu_getName($rnd2)."</h1>";
      if($rnd == $rnd2){
        $yes = true;
      }else{
        $yes = false;
      }
      $no = !$yes;
        echo "<ul>";
          echo '<li class="hide"><a href="#" >nic</a></li>';
          echo "<li id=\"ano\"><a href=\"testANO.php?cor=$yes\">ANO</a></li>";
          echo '<li class="hide"><a href="#" >nic</a></li>';
          echo "<li id=\"ne\"><a href=\"testANO.php?cor=$no\">NE</a></li>";
          echo '<li class="hide"><a href="#" >nic</a></li>';
        echo '</ul>';
      }
    }
    itu_dbClose();
    ?>
  </div>
</div>
</body>
</html>
