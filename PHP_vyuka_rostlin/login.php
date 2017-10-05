

<!DOCTYPE html>
<?php
include 'ituscript.php';
include 'config.php';

itu_dbOpen();
$tmp = itu_getPass($_POST["username"]);
$tmps = itu_getPass('ugandix');
itu_dbClose();

$login=false;
 ?>
<html>
<head>
  <title>Login</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
  <div class="top-panel">
    <ul>
      <li><a href="logReg.php" >Zpet</a></li>
      <li class="hide"><a href="#" >nic</a></li>
    <ul>
  </div>
<div class="container">
  <?php
  if ($tmp == $_POST["password"])
  	{ 
  	//echo "Prihlaseni probehlo uspesne";
  	$login = true;
  	$_SESSION['login'] = $login;
  	$_SESSION['prihlasenyuzivatel'] = $_POST["username"];
    header('Location: index.php');
  	}
  else
  	{
  	echo "Nespravne prihlasovaci jmeno nebo heslo";
  	$login = false;
  	$_SESSION['login'] = $login;
  	$_SESSION['prihlasenyuzivatel'] = false;
  	}
  ?>
  <a href="index.php" >Pokracuj</a>
</div>
</body>
</html>
