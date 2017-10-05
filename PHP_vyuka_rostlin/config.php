<?php
session_start();
mb_internal_encoding("UTF-8");
$jmenoKytky="pampeliska";
$login = false;
if (isset($_SESSION['login']))
{
$login = $_SESSION['login'];
}

if (isset($_SESSION['prihlasenyuzivatel']))
{
$prihlasenyuzivatel = $_SESSION['prihlasenyuzivatel'];
}

 ?>
