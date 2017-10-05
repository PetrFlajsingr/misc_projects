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
  <title>TestIMG</title>
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
  <div class="moznostiIMG">
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
      $array = itu_plantArr($prihlasenyuzivatel, $rnd);
      $ansAr = array(0, 0, 0, 0, 0);
      for($i = 0; $i < 4; $i++){
        if($array[$i] == $rnd){
          $ansAr[$i] = 1;
          break;
        }
      }
      echo "<img src=\"".itu_getLink($rnd, $prihlasenyuzivatel)."\">";
      echo '<ul>';
      echo "<li><a href=\"testIMG.php?cor=".$ansAr[0]."\" >".itu_getName($array[0])."</a></li>";
      echo "<li><a href=\"testIMG.php?cor=".$ansAr[1]."\" >".itu_getName($array[1])."</a></li>";
      echo "<li><a href=\"testIMG.php?cor=".$ansAr[2]."\" >".itu_getName($array[2])."</a></li>";
      echo "<li><a href=\"testIMG.php?cor=".$ansAr[3]."\" >".itu_getName($array[3])."</a></li>";
      echo "<li><a href=\"testIMG.php?cor=".$ansAr[4]."\" >".itu_getName($array[4])."</a></li>";
    }
    itu_dbClose();
  ?>
    </ul>
  </div>
</div>
</body>
</html>
