
<!DOCTYPE html>
<?php
include 'config.php';
include 'ituscript.php';
$login = $_SESSION['login'];
if(isset($_SESSION['prihlasenyuzivatel'])){
  $prihlasenyuzivatel = $_SESSION['prihlasenyuzivatel'];
}
$_SESSION['correct'] = 0;
$_SESSION['wrong'] = 0;
 ?>
<html>
<head>
  <meta charset="UTF-8">
  <title>Index</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
  <div class="top-panel">
    <ul>
      <!--ktery moznosti mu povolim kdyz je/neni lognuty -->
      <?php
    $tmp = rand(0,9);
      echo "<li><a href=\"flower.php?plantid=".$tmp."&next=1\">Nove kvetiny</a></li>";
      echo "<li><a href=\"tests.php?plantid=\">Testy</a></li>";
      echo '<li><a href="myPlants.php" >Oblibene</a></li>';
      echo '<li><a href="atlas.php" >Atlas</a></li>';
    if ($login){
      echo '<li><a href="odhlasenie.php" >Odhlaseni</a></li>';}
    else {
      echo '<li><a href="logReg.php" >Prihlaseni/Registrace</a></li>';}?>
    </ul>
  </div>
  <h2 text >
<div class="container">
  <!--vypsani obsahu -->
    <?php
    itu_dbOpen();
          if ($login){
            echo '<h5>Statistiky</h5>';
            echo '<p>';
            echo "Celkovy pocet spravnych odpovedi: ".itu_getCor($prihlasenyuzivatel)."<br>";
            echo "Celkovy pocet spatnych odpovedi: ".itu_getWrong($prihlasenyuzivatel)."<br>";
            echo "Pocet oblibenych rostlin: ".itu_favCount($prihlasenyuzivatel)."<br>";
            echo '</p>';
            echo '<h5>Novinky</h5>
            <p>Spustili jsme nasi stranku!<br>
            Pokud byste meli nejake navrhy nebo pripominky prosim kontaktujte nas na emailech:<br>
            xvalka03@stud.fit.vutbr.cz<br>
            xulicn00@stud.fit.vutbr.cz<br>
            xflajs00@stud.fit.vutbr.cz<br>';
          }
          else {
            echo '<h1>Vitejte</h1>
            <h5>O strance</h5>
            <p>Nase stranka nabizi atlas kvetin ceske republiky.<br>
            Po prihlaseni se vam navic zpristupni jednotlive funkce pro vyuku</p>
            <h5>Novinky</h5>
            <p>Spustili jsme nasi stranku!<br>
            Pokud byste meli nejake navrhy nebo pripominky prosim kontaktujte nas na emailech:<br>
            xvalka03@stud.fit.vutbr.cz<br>
            xulicn00@stud.fit.vutbr.cz<br>
            xflajs00@stud.fit.vutbr.cz<br>
            </p>
            ';}
    itu_dbClose();
    ?>

</div>
</body>
</html>
