<?php

#DKA:xflajs00
/**
 * Predmet: IPP
 * Projekt: proj1
 * Varianta zadani: DKA (determinizace konecneho automatu)
 * @author: Petr Flajsingr, xflajs00
 **/

	/**
	 * Vypis chyby na STDERR, ukonceni programu
	 * @param string
	 * String poslany na STDERR
	 * @param code
	 * Navratovy kod skriptu
	 */
	function errorExit($string, $code){
		fwrite(STDERR, "ERROR: $string\n");
		exit($code);
	}
	/**
	 * Funkce zpracovava parametry a kontroluje jejich spravnost
	 * @description 
	 * @param $argv
	 * Argumenty skriptu
	 * @param $argc
	 * Pocet argumentu skriptu
	 * @return $args
	 * vsechny parametry pro pozdejsi praci
	 */
	function loadParams($argv, $argc){ //TODO - bonus
		if($argc == 1){
			return ;
		}
		$shortArgs = "e"; //spravne kratke argumenty
		$shortArgs .= "d";
		$shortArgs .= "i";
		$longArgs = array("help", "input:", "output:", "no-epsilon-rules", "determinization", "case-insensitive"); //spravne dlouhe argumenty

		if(!($args = getopt($shortArgs, $longArgs))){ //kontrola spravnosti argumentu
			errorExit("Chybne zadani argumentu", 1);
		}
		if(count($args) != count($argv) - 1){
			errorExit("Chybne zadani argumentu", 1);
		}
		//argumentem je "--help"
		if(isset($args["help"])){
			if($argc > 2){
				errorExit("Chybne zadani argumentu", 1);
			}else{
				printHelp();
				exit(0);
			}
		}
		//kontrola spravneho nacteni "-e"
		if(isset($args["e"])){
			if(!(in_array("-e", $argv))){
				errorExit("Chybne zadani argumentu", 1);
			}
			if(isset($args["no-epsilon-rules"])){
				errorExit("Chybne zadani argumentu", 1);
			}
		}
		//kontrola spravneho nacteni "-d"
		if(isset($args["d"])){
			if(!(in_array("-d", $argv))){
				errorExit("Chybne zadani argumentu", 1);
			}
			if(isset($args["determinization"])){
				errorExit("Chybne zadani argumentu", 1);
			}
		}
		//kontrola spravneho nacteni "-i"
		if(isset($args["i"])){
			if(!(in_array("-i", $argv))){
				errorExit("Chybne zadani argumentu", 1);
			}
			if(isset($args["case-insensitive"])){
				errorExit("Chybne zadani argumentu", 1);
			}
		}
		/*if((in_array("-e", $argv) && in_array("-d", $argv)) || ((isset($args["determinization"]) && isset($args["no-epsilon-rules"])))){
			errorExit("Chybne zadani argumentu", 1);
		}*/
		return $args;
	}
	/**
	 * Nastaveni prepinacu podle parametru skriput
	 * @param $args
	 * Parametry skriptu
	 * @param $input
	 * vstupni soubor
	 * @param $output
	 * vystupni soubor
	 * @param $noEpsRules
	 * prepinac pro odstraneni epsilon pravidel
	 * @param $deter
	 * prepinac pro determinizaci
	 * @param $caseInses
	 * prepinac pro nebrani ohledu na velikost pismen
	 */
	function setParamVars($args, &$input, &$output, &$noEpsRules, &$deter, &$caseInsens){
		//vstupni soubor
		$input = "php://stdin";
		if(isset($args["input"])){
			$input = file_get_contents($args["input"], "r");
			if($input == false || !(is_readable($args["input"]))){
				errorExit("Soubor nelze otevrit nebo z nej nelze cist", 2);
			}
		}
		if(strcmp($input, "php://stdin") == 0){
			$input = file_get_contents("php://stdin", "r");
		}
		//vystupni soubor
		$output = "php://stdout";
		if(isset($args["output"])){
			$output = $args["output"];
		}
		if(isset($args["e"]) || isset($args["no-epsilon-rules"]))
			$noEpsRules = true;
		if(isset($args["d"]) || isset($args["determinization"]))
			$deter = true;
		if(isset($args["i"]) || isset($args["case-insensitive"]))
			$caseInsens = true;
		if($deter && $noEpsRules){
			errorExit("Chybne zadani argumentu", 1);
		}
	}
?>