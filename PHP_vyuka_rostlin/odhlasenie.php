<!DOCTYPE html>
<?php
session_start();
$login = false;
$_SESSION['login'] = $login;
$_SESSION['prihlasenyuzivatel'] = "";
header("Location: index.php");
die();

?>
