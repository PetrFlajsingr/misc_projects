<?php

#DKA:xflajs00
/**
 * Predmet: IPP
 * Projekt: proj1
 * Varianta zadani: DKA (determinizace konecneho automatu)
 * @author: Petr Flajsingr, xflajs00
 **/

/**
 * Navratove hodnoty skriptu:
 * -1 - chyba skriptu
 * 0 - vse OK
 * 1 - chyba argumentu
 * 2 - chyba souboru 
 * 40 - syntakticka chyba
 * 41 - semanticka chyba
 */
	#modul pro zpracovani parametru
	include_once "params.php";
	#modul pro nacteni a vypis konecneho automatu
	include_once "fsm.php";
	#modul pro odstraneni epsilon prechodu
	include_once "removeeps.php";
	#modul pro determinizaci konecneho automatu
	include_once "deter.php";
	#nastaveni kodovani
	mb_internal_encoding("UTF-8");
	/**
	 * Vypis napovedy skriptu
	 */
	function printHelp(){ #TODO - bonus
		echo "Skript pro determinizaci konecneho automatu\n\n";
		echo "Skript zpracovava textovy zapis konecneho automatu a pripadne generuje ekvivalentni determinsticky konecny automat bez generovani nedostupnych stavu\n\n";
		echo "Pouziti:\n";
		echo "\t--help\t\t\t-\tzobrazeni napovedy\n";
		echo "\t--input=filename\t-\tlokace vstupniho souboru\n";
		echo "\t--output=filename\t-\tlokace vystupniho souboru\n";
		echo "\t-e, --no-epsilon-rules\t-\todstraneni epsilon pravidel vstupniho KA\n";
		echo "\t-d, --determinization\t-\tdeterminizace bez generovani nedostupnych stavu\n";
		echo "\t-i, --case-insensitive\t-\tnebere ohled na velikost znaku\n";
	}

	#zpracovani argumentu
	$args = loadParams($argv, $argc);
	$noEpsRules = $deter = $caseInsens = false;
	setParamVars($args, $input, $output, $noEpsRules, $deter, $caseInsens);


	$alphabet = array(); #abeceda
	$states = array(); #stavy
	$finStates = array(); #finalni stavy
	$rules = array(); #pravidla
	$defState = "";
	loadFSM($alphabet, $states, $rules, $finStates, $caseInsens, $input, $defState);
	if($noEpsRules){
		removeEps($states, $rules, $finStates);
	}elseif($deter){
		removeEps($states, $rules, $finStates);
		determinize($states, $alphabet, $rules, $defState, $finStates);
	}
	printFSM($states, $alphabet, $rules, $defState, $finStates, $output);
	#zavreni I/O souboru
?>