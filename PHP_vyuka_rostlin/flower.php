<!DOCTYPE html>
<?php
session_start();
include 'config.php';
include 'ituscript.php';
$plantid=$_GET['plantid'];
$login = $_SESSION['login'];
if(isset($_SESSION['prihlasenyuzivatel'])){
  $prihlasenyuzivatel = $_SESSION['prihlasenyuzivatel'];
}
itu_dbOpen();
if(isset($plantid)){
  $jmenoKytky = itu_getName($plantid);
}
if(isset($_GET['id']) and isset($prihlasenyuzivatel)){
  $id = $_GET['id'];
  itu_addFav($id, $prihlasenyuzivatel);
}
itu_dbClose();
?>
<html>
<head>
  <title>Flower</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
  <div class="top-panel">
    <ul>
      <li><a href="index.php" >Domu</a></li>
      <li class="hide"><a href="#" >nic</a></li>
<?php
if ($login){
  $link = "atlas.php";
  if(isset($_GET['back'])){
    $hideback = " ";
    if($_GET['back'] == 2){
      $link = "myPlants.php";
    }
  }else{
    $hideback = "class=\"hide\"";
  }
    echo "<li $hideback><a href=\"".$link."\">Zpet</a></li>";
  if(isset($_GET['next'])){
    $hidenext = " ";
  }else{
    $hidenext = "class=\"hide\"";
  }
    echo "<li $hidenext><a href=\"flower.php?plantid=".rand(0,9)."&next=1\" >Dalsi</a></li>";
  itu_dbOpen();
    if(isset($plantid) and isset($prihlasenyuzivatel) and itu_isFav($plantid, $prihlasenyuzivatel)){
      $hide = "class=\"hide\"";
    }else{
      $hide = " ";
    }
    echo "<li $hide><a href=flower.php?id=$plantid&plantid=$plantid&back=1>Add</a></li>";
    itu_dbClose();
}
?>
    </ul>
  </div>
<div class="container">
  <?php
  itu_dbOpen();
  $desc = itu_getDesc($plantid);
  $usage = itu_getUsage($plantid);
  $area = itu_getArea($plantid);
echo "<h1>"."$jmenoKytky"."</h1>";
echo '<a href="https://www.google.cz/search?q=',"$jmenoKytky",'&source=lnms&tbm=isch&sa=X&ved=0ahUKEwiaurjLjcnQAhXI3CwKHfzwCl0Q_AUICCgB&biw=1396&bih=668',
  'target="_blank"><img src="img/',strtolower($jmenoKytky),'.jpg"></a>';
echo "
<details open>
  <summary>Popis</summary>
    $desc
</details>
<details open>
  <summary>Vyskyt</summary>
    $area
</details>
<details open>
  <summary>Vyuziti</summary>
    $usage
</details>
";
itu_dbClose();
?>
</div>
</body>
</html>
