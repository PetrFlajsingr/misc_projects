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
  <title>TestNAME</title>
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

    <?php
    itu_dbOpen();
    if(itu_favCount($prihlasenyuzivatel) > 0){
      if(((isset($_SESSION['correct']) ? $_SESSION['correct'] : 0) + (isset($_SESSION['wrong']) ? $_SESSION['wrong'] : 0)) >= itu_favCount($prihlasenyuzivatel)){
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
        echo "<h1>".itu_getName($rnd)."</h1>";

        echo '<div class="moznostiNAME">
        <ul>
    <form action="" method="post">
      <label>Typ:</label>
        <select name="TYPE">
            <option value="Jednodelozna">Jednodelozna</option>
            <option value="Krytosemenna">Krytosemenna</option>
            <option value="Bylina">Bylina</option>
            <option value="Ker">Ker</option>
        </select>
      <label>Vyska:</label>
        <select name="HEIGHT">
            <option value="7">7</option>
            <option value="10">10</option>
            <option value="50">50</option>
            <option value="60">60</option>
            <option value="100">100</option>
            <option value="160">160</option>
            <option value="200">200</option>
            <option value="300">300</option>
        </select>
      <label>Plod:</label>
        <select name="FRUIT">
            <option value="Ne">Ne</option>
            <option value="Jahody">Jahody</option>
            <option value="Cervene bobule">Cervene bobule</option>
            <option value="Sipky">Sipky</option>
            <option value="Cerne bobule">Cerne bobule</option>
        </select>
      <label>Kvet:</label>
        <select name="FLOWER">
            <option value="Ne">Ne</option>
            <option value="Ruzne barvy">Ruzne barvy</option>
            <option value="Fialovy">Fialovy</option>
            <option value="Zluty">Zluty</option>
            <option value="Ruzovy">Ruzovy</option>
            <option value="Bily">Bily</option>
            <option value="Hnedofialovy">Hnedofialovy</option>
        </select>
       <input type="submit" value="Potvrdit">
    </form>';
    $type = isset($_POST['TYPE']) ? $_POST['TYPE'] : '*';
    $height = isset($_POST['HEIGHT']) ? $_POST['HEIGHT'] : '*';
    $fruit = isset($_POST['FRUIT']) ? $_POST['FRUIT'] : '*';
    $flower = isset($_POST['FLOWER']) ? $_POST['FLOWER'] : '*';
    $array = itu_selectPlant($type, $height, $fruit, $flower);
    $sw = 0;
    if($array != FALSE){
      foreach($array as $tmp){
        if($tmp['PLANTID'] == $rnd){
           if(isset($_SESSION['correct'])){
             $_SESSION['correct']++;
           } else{
             $_SESSION['correct'] = 1;
           }
           $sw = 1;
           break;
        }
      }
    }
    if(!$sw){
      if(isset($_SESSION['wrong'])){
         $_SESSION['wrong']++;
       } else{
        $_SESSION['wrong'] = 1;
       }
     }
      }

    }
    itu_dbClose();
    ?>
  </div>
</div>
</body>
</html>
