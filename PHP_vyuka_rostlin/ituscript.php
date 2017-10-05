<?php
$con; /*< promenna pro pripojeni k DB */

/**
 * Otevreni pripojeni k DB
 **/
function itu_dbOpen(){
	global $con;
	// Create connection
	$con = mysql_connect("localhost:/var/run/mysql/mysql.sock", "xflajs00", "5evevuca");
	// Check connection
	if(!$con){
		die("Connection failed: " .mysql_error());
	}
	if(!mysql_select_db('xflajs00', $con)){
		die('Database unavailable: '.mysql_error());
	}
}

/**
 * Uzavreni pripojeni k DB
 **/
function itu_dbClose(){
	global $con;
	mysql_close($con);
}

/**
 * Pridani uzivatele do DB
 * @return FALSE pri chybe vlozeni(existujici uzivatel/chyba pripojeni k DB)
 * @return TRUE vse ok
 **/
function itu_addUser($name, $password){
	global $con;
	//pridani uzivatele do tabulky USER
	$sql = "INSERT INTO USER (login,password) VALUES ('$name', '$password')";
	if (mysql_query($sql, $con) === TRUE) {
		//pridani zaznamu do RECORD
		$sql = "INSERT INTO RECORD (CORRANS, WRNGANS, USER) VALUES (0, 0, '$name')";
		if(mysql_query($sql, $con) === FALSE) {
			return FALSE;
		}
	}else{
	   	return FALSE;
	}
	return TRUE;
}

/**
 * Pocet spravnych odpovedi uzivatele
 **/
function itu_getCor($name){
	global $con;
	$sql = "SELECT CORRANS FROM RECORD WHERE USER = '$name'";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

/**
 * Pocet spatnych odpovedi uzivatele
 **/
function itu_getWrong($name){
	global $con;
	$sql = "SELECT WRNGANS FROM RECORD WHERE USER = '$name'";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

/**
 * Uprava statistik v DB
 * @return FALSE pri chybe UPDATE
 **/
function itu_changeStat($name, $cor, $wrong){
	global $con;
	$wrong += itu_getWrong($name);
	$cor += itu_getCor($name);
	$sql = "UPDATE RECORD SET WRNGANS = $wrong, CORRANS = $cor WHERE USER = '$name'";
	return mysql_query($sql, $con);
}

/**
 * Heslo uzivatele pro kontrolu prihlaseni
 **/
function itu_getPass($name){
	global $con;
	$sql = "SELECT PASSWORD FROM USER WHERE LOGIN = '$name'";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}
/*********************INFO O ROSTLINACH******************/
/**
 * Vraci pole vysledku select dotazu
 **/
function itu_selectPlant($type, $height, $fruit, $flower){
	global $con;
	$switch = false;
	$sql = "SELECT * FROM PLANT";
	if($type != "*" or $height != "*" or $fruit != "*" or $flower != "*"){
		$sql = $sql . " WHERE";
		if($type != "*"){
			$sql = $sql. " TYPE = '$type'";
			$switch = true;
		}
		if($height != "*"){
			if($switch){
				$sql = $sql . " AND";
			}
			$sql = $sql. " HEIGHT = '$height'";
			$switch = true;
		}
		if($fruit != "*"){
			if($switch){
				$sql = $sql . " AND";
			}
			$sql = $sql. " FRUIT = '$fruit'";
			$switch = true;
		}
		if($flower != "*"){
			if($switch){
				$sql = $sql . " AND";
			}
			$sql = $sql. " FLOWER = '$flower'";
		}
	}
	$res = mysql_query($sql, $con);

	if($res){
		while($row = mysql_fetch_assoc($res)){
	    	$json[] = $row;
	 	}
	}else{
		return FALSE;
	}
	return $json;
}

function itu_getName($id){
	global $con;
	$sql = "SELECT NAME FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

function itu_getType($id){
	global $con;
	$sql = "SELECT TYPE FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

function itu_getRarity($id){
	global $con;
	$sql = "SELECT RARITY FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

function itu_getHeight($id){
	global $con;
	$sql = "SELECT HEIGHT FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

function itu_getFruit($id){
	global $con;
	$sql = "SELECT FRUIT FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

function itu_getFlower($id){
	global $con;
	$sql = "SELECT FLOWER FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

function itu_getDesc($id){
	global $con;
	$sql = "SELECT DESCR FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

function itu_getLink($id){
	global $con;
	$sql = "SELECT LINK FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

function itu_getUsage($id){
	global $con;
	$sql = "SELECT VYUZITI FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

function itu_getArea($id){
	global $con;
	$sql = "SELECT VYSKYT FROM PLANT WHERE PLANTID = $id";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}
/*******************\ INFO O ROSTLINACH***********/
/**
 * Pridani zaznamu o favorite rostliny do DB
 * @return FALSE pri chybe vlozeni
 **/
function itu_addFav($plantid, $name){
	global $con;
	$sql = "SELECT RECORDID FROM RECORD WHERE USER = '$name'";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	$recordid = $rows[0];
	$sql = "INSERT INTO PLECORD (PLANT, RECORD) VALUES ($plantid, $recordid)";
	return mysql_query($sql, $con);
}

/**
 * Kontrola fav rostliny
 * @return FALSE pokud ji nezna
 **/
function itu_isFav($plantid, $name){
	if(!$name){
		return FALSE;
	}
	global $con;
	$sql = "SELECT RECORDID FROM RECORD WHERE USER = '$name'";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	$recordid = $rows[0];
	$sql = "SELECT COUNT(*) FROM PLECORD WHERE RECORD = $recordid AND PLANT = $plantid";
	$tmp = mysql_query($sql, $con);
	$uga = mysql_fetch_row($tmp);
	return $uga[0];
}

/**
 * Pocet favorite rostlin uzivatele
 **/
function itu_favCount($name){
	global $con;
	$sql = "SELECT RECORDID FROM RECORD WHERE USER = '$name'";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	$recordid = $rows[0];
	$sql = "SELECT COUNT(RECORD) FROM PLECORD WHERE RECORD = $recordid";
	$res = mysql_query($sql, $con);
	$rows = mysql_fetch_row($res);
	return $rows[0];
}

/**
 * Nahodny vyber ID rostliny z tech, ktere uzivatel zna
 **/
function itu_randPlant($name){
	$i = rand(0,9);
	while(!itu_isFav($i, $name)){
		$i = rand(0,9);
	}
	return $i;
}

/**
 * Tvorba array pro testy, netvori duplikaty
 * @return array nahodne serazene pole plantID
 **/
function itu_plantArr($name, $id){
	$array[] = $id;
	for($i = 0; $i < 4; $i++){
		$value = rand(0,9);
		if(array_search($value, $array) == FALSE){
			if(itu_isFav($id, $name)){
				array_push($array, $value);
			}else{
				$i--;
			}
		}else{
			$i--;
		}
	}
	shuffle($array);
	return $array;
}
?>
