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
        <form action="" method="post">
          <label>Typ:</label>
            <select name="TYPE">
                <option value="*" selected> </option>
                <option value="Jednodelozna">Jednodelozna</option>
                <option value="Krytosemenna">Krytosemenna</option>
                <option value="Bylina">Bylina</option>
                <option value="Ker">Ker</option>
            </select>
          <label>Vyska:</label>
            <select name="HEIGHT">
                <option value="*" selected> </option>
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
                <option value="*" selected> </option>
                <option value="Ne">Ne</option>
                <option value="Jahody">Jahody</option>
                <option value="Cervene bobule">Cervene bobule</option>
                <option value="Sipky">Sipky</option>
                <option value="Cerne bobule">Cerne bobule</option>
            </select>
          <label>Kvet:</label>
            <select name="FLOWER">
                <option value="*" selected> </option>
                <option value="Ne">Ne</option>
                <option value="Ruzne barvy">Ruzne barvy</option>
                <option value="Fialovy">Fialovy</option>
                <option value="Zluty">Zluty</option>
                <option value="Ruzovy">Ruzovy</option>
                <option value="Bily">Bily</option>
                <option value="Hnedofialovy">Hnedofialovy</option>
            </select>
           <input type="submit" value="Hledej">
        </form>
      </ul>
    </div>
<div class="container">
<?php
  itu_dbOpen();
  if(isset($_GET['id']) and isset($prihlasenyuzivatel)){
    $id = $_GET['id'];
    itu_addFav($id, $prihlasenyuzivatel);
  }
  $type = isset($_POST['TYPE']) ? $_POST['TYPE'] : '*';
  $height = isset($_POST['HEIGHT']) ? $_POST['HEIGHT'] : '*';
  $fruit = isset($_POST['FRUIT']) ? $_POST['FRUIT'] : '*';
  $flower = isset($_POST['FLOWER']) ? $_POST['FLOWER'] : '*';
  $array = itu_selectPlant($type, $height, $fruit, $flower);
  if($array != FALSE){

    echo ("<table>");
    echo("<tr><th width=\"185px\">Detail</th><th>Jmeno</th><th>Typ</th><th>Rarita</th><th width=\"50px\" padding=\"0px\">Vyska</th><th>Kvet</th><th>Popis</th><th>Favorite</th></tr>");
    foreach($array as $tmp){
      $uga = "img/favFalse.png";
      if(isset($prihlasenyuzivatel) and itu_isFav($tmp['PLANTID'], $prihlasenyuzivatel) == TRUE){
        $uga = "img/favTrue.png";
      }else{
        $uga = "img/favFalse.png";
      }
      echo("<tr><td width=\"185px\"><a href=\"flower.php?plantid=".$tmp['PLANTID']."&back=1\"><img class=\"thumbnail\" src=\"".$tmp['LINK']."\"/></a></td><td>".$tmp['NAME']."</td><td>".$tmp['TYPE']."</td><td>".$tmp['RARITY']."</td><td width=\"50px\">".$tmp['HEIGHT']."</td><td>".$tmp['FLOWER']."</td><td>".$tmp['DESCR']."</td><td><a href=atlas.php?id=".$tmp['PLANTID']."><img src=\"".$uga."\"/></a></td></tr>");

    }
      echo ("</table>");
  }
  itu_dbClose();
?>
</div>
</body>
</html>
