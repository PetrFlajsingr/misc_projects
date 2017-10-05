<!DOCTYPE html>
<?php
include 'config.php';
$_SESSION['correct'] = 0;
$_SESSION['wrong'] = 0;
?>
<html>
<head>
  <title>Test</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
  <div class="top-panel">
    <ul>
      <?php
      if($login){
        echo '<li><a href="index.php" >Domu</a></li>
        <li class="hide"><a href="#" >nic</a></li>
        <li><a href="testANO.php" >Ano/Ne</a></li>
        <li><a href="testIMG.php">Podle obrazku</a></li>
        <li><a href="testNAME.php" >Podle jmena</a></li>';
    } else {
        echo '<li><a href="index.php" >Domu</a></li>';
    }
       ?>
    </ul>
  </div>
<div class="container">
  <!-- Popis co jaky test dela-->
  <?php
  if ($login){
    echo '
  <h1>Existuji tyto testy:</h1>
  <h2>Ano/ne</h2>
    <p>Objevi se obrazek kvetiny a jeji jmeno<br>
      Musite rozhodnout jestli se tato kvetina takto jmenuje nebo ne
    </p>
  <h2>Podle obrazku</h2>
    <p>Objevi se obrazek kvetiny a nekolik moznosti, jak se jmenu<br>
      Musite zvolit tu spravnou
    </p>
  <h2>Podle jmena</h2>
    <p>Objevi se jmeno kvetiny a budete muset zvolit jeji vlastnosti
    </p>';
  }else {
    echo '<p>Tato funkce je dostupna pouze pro prihlasene uzivatele</p>';
    echo '<p>Prihlaseni/Registrace <a href="logReg.php">zde</a></p>';
  }
    ?>
  </div>
</body>
</html>
