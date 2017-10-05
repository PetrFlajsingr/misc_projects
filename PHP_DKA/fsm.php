<?php

#DKA:xflajs00
/**
 * Predmet: IPP
 * Projekt: proj1
 * Varianta zadani: DKA (determinizace konecneho automatu)
 * @author: Petr Flajsingr, xflajs00
 **/
	/**
	 * Kontrola podminky a ukonceni programu se syntaktickou chybou
	 * @param $check
	 * kontrolovana podminka
	 **/
	function checkSyn($check){
		if($check){
			errorExit("Syntakticka chyba\n", 40);
		}
	}

	/**
	 * Kontrola existence polozky v poli
	 * @param $array
	 * kontrolovane pole
	 * @param $name
	 * kontrolovana polozka
	 **/
	function checkItem($array, $name){
		foreach($array as $temp){
			if(!strcmp($temp, $name)){
				return true;
			}
		}
		return false;
	}
	/**
	 * Nacteni znaku v UTF
	 * @param &$input
	 * vstupni data
	 * @param $caseInses
	 * prepinac case sensitivity
	 **/
	function readUTFChar(&$input, $caseInsens){
		$symbol = mb_substr($input, 0, 1);
		$input = mb_substr($input, 1);
		if(!$caseInsens){
			return $symbol;
		}
		return mb_strtolower($symbol);
	}

	/**
	 * Preskoceni bilych znaku a komentaru
	 * @param $symbol
	 * posledni nacteny znak
	 * @param &$input
	 * vstupni data
	 * @param $caseInses
	 * prepinac case sensitivity
	 **/
	function skipWhite($symbol, &$input, $caseInsens){
		while(ctype_space($symbol) || (!strcmp($symbol, "#"))){
			if(!strcmp($symbol, "#")){
				while(strcmp($symbol, "\n") && mb_strlen($input)){
					$symbol = readUTFChar($input, $caseInsens);
				}
			}
			$symbol = readUTFChar($input, $caseInsens);
		}
		return $symbol;
	}

	/**
	 * Pridani polozky do pole, pokud tam jeste neni
	 * @param &$array
	 * reference na pole pro ulozeni polozky
	 * @param $name
	 * Nazev ukladane polozky
	 **/
	function addItem(&$array, $name){
		$check = false;
		foreach($array as $tmp){
			if(!strcmp($tmp, $name)){
				$check = true;
			}
		}
		if(!$check){
			array_push($array, $name);
		}
	}

	/**
	 * Pridani pravidla
	 * @param &$rules
	 * reference na vsechna pravidla
	 * @param $rule
	 * ukladane pravidlo
	 **/
	function addRule(&$rules, $rule){
		$check = false;
		foreach($rules as $temp){
			if((!strcmp($temp->startState, $rule->startState)) && (!strcmp($temp->symbol, $rule->symbol)) && (!strcmp($temp->endState, $rule->endState))){
				$check = true;
			}
		}
		if(!$check){
			array_push($rules, $rule);
		}
	}

	/**
	 * Trida pro ukladani pravidel
	 **/
	class rule{
		public $startState;
		public $symbol;
		public $endState;
	}

	/**
	 * Nacteni konecneho automatu
	 * @param &$alphabet
	 * abeceda KA
	 * @param &$states
	 * stavy KA
	 * @param &$rules
	 * pravidla KA
	 * @param &$finStates
	 * konecne stavy
	 * @param $caseInsens
	 * prepinac case sensitivity
	 * @param &$input
	 * vstupni data
	 * @param &$defState
	 * startovni stav KA
	 **/
	function loadFSM(&$alphabet, &$states, &$rules, &$finStates, $caseInsens, &$input, &$defState){
		$presentState = 0;
		$a = false;
		while(mb_strlen($input)){
			$symbol = readUTFChar($input, $caseInsens);
			switch ($presentState) {
				#zacatek KA - '('
				case 0:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					checkSyn(strcmp($symbol, "("));
					$presentState = 1;
					break;
				/***************************************MNOZINA STAVU***********************************************/
				#zacatek mnoziny stavu
				case 1:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					checkSyn(strcmp($symbol, "{"));
					$presentState = 2;
					break;
				#ukonceni mnoziny stavu/zacatek nacteni dalsiho stavu
				case 2:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					if(!strcmp($symbol, "}")){ #prazdna mnozina stavu
						$symbol = readUTFChar($input, $caseInsens);
						$symbol = skipWhite($symbol, $input, $caseInsens);
						checkSyn(!strcmp($symbol, ","));
					}else{
						checkSyn(!ctype_alpha($symbol));
						$temp = $symbol;
						$presentState = 3;
					}
					break;
				#nacteni viceznakoveho stavu/ulozeni
				case 3:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					if(!strcmp($symbol, ",")){
						addItem($states, $temp);
						$presentState = 2;
					}elseif(!strcmp($symbol, "}")){
						checkSyn(strcmp(mb_substr($temp, -1), "_") == 0);
						addItem($states, $temp);
						$symbol = readUTFChar($input, $caseInsens);
						$symbol = skipWhite($symbol, $input, $caseInsens);
						checkSyn(strcmp($symbol, ","));
						$presentState = 4;
					}elseif(ctype_alnum($symbol) || !strcmp($symbol, "_")){
						$temp .= $symbol;
					}else{
						errorExit("Syntakticka chyba", 40);
					}
					break;
				/***************************************\MNOZINA STAVU***********************************************/
				/***************************************ABECEDA******************************************************/
				#zacatek abecedy
				case 4:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					checkSyn(strcmp($symbol, "{"));
					$presentState = 5;
					break;
				#nacitani abecedy
				case 5:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					if(empty($alphabet) && (!strcmp($symbol, "}"))){ #prazdna abeceda
						errorExit("Semanticka chyba", 41);
					}else{
						$symbol = skipWhite($symbol, $input, $caseInsens);
						checkSyn(strcmp($symbol, "'")); 
						$symbol = readUTFChar($input, $caseInsens);
						if(!strcmp($symbol, "'")){ #dalsi "'"
							$presentState = 6;
						}else{
							addItem($alphabet, $symbol);
							$symbol = readUTFChar($input, $caseInsens);
							$symbol = skipWhite($symbol, $input, $caseInsens);
							checkSyn(strcmp($symbol, "'")); #konec polozky
							$symbol = readUTFChar($input, $caseInsens);
							$symbol = skipWhite($symbol, $input, $caseInsens);
							if(!strcmp($symbol, "}")){ #konec abecedy
								$presentState = 7;
							}else{
								checkSyn(strcmp($symbol, ",")); #dalsi polozka
							}
						}
					}
					break;
				#prazdny retezec
				case 6:
					checkSyn(strcmp($symbol, "'"));
					$symbol = readUTFChar($input, $caseInsens);;
					checkSyn(strcmp($symbol, "'"));
					$symbol .= "'";
					addItem($alphabet, $symbol);
					$symbol = readUTFChar($input, $caseInsens);
					$symbol = skipWhite($symbol, $input, $caseInsens);
					if(!strcmp($symbol, ",")){
						$presentState = 5;
					}elseif(!strcmp($symbol, "}")){
						$presentState = 7;
					}
					break;
				/***************************************\ABECEDA****************************************************/
				/***************************************PRAVIDLA****************************************************/
				#zacatek pravidel
				case 7:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					checksyn(strcmp($symbol, ",")); #oddelovac
					$symbol = readUTFChar($input, $caseInsens);
					$symbol = skipWhite($symbol, $input, $caseInsens);
					checksyn(strcmp($symbol, "{"));
					$presentState = 8;
					break;
				#nacitani vstupniho stavu
				case 8:
					$rule = new rule();
					$symbol = skipWhite($symbol, $input, $caseInsens);
					if(empty($rules) && !strcmp($symbol, "}")){
						$presentState = 15;
					}elseif(ctype_alpha($symbol)){
						$temp = $symbol;
						$presentState = 9;
					}else{
						errorExit("Syntakticka chyba", 40);
					}
					break;
				#viceznakovy stav
				case 9:
					$symbol = skipWhite($symbol, $input, $caseInsens); 
					if(!strcmp($symbol, "'")){#vstupni znak
						$rule->startState = $temp;
						if(!checkItem($states, $temp)){
							errorExit("Semanticka chyba", 41);
						}
						checkSyn(!strcmp(mb_substr($temp, -1), "_"));
						$presentState = 10;
					}elseif(ctype_alnum($symbol) || !strcmp($symbol, "_")){
						$temp .= $symbol;
					}else{
						errorExit("Syntakticka chyba", 40);
					}
					break;
				#vstupni znak
				case 10:
					if(!strcmp($symbol, "'")){
						$presentState = 11;
					}else{
						$rule->symbol = $symbol;
						if(!checkItem($alphabet, $rule->symbol)){
							errorExit("Semanticka chyba", 41);
						}
						$symbol = readUTFChar($input, $caseInsens);
						checkSyn(strcmp($symbol, "'"));
						$presentState = 12;
						$a = false;
					}
					break;
				#prazdny retezec
				case 11:
					$a = false;
					if(!strcmp($symbol, "'")){
						$symbol = readUTFChar($input, $caseInsens);
						checkSyn(strcmp($symbol, "'"));
						$rule->symbol = "''";
						$presentState = 12;
					}else{
						$a = true;
						$symbol = skipWhite($symbol, $input, $caseInsens); 
						$temp = $symbol;
						$rule->symbol = "eps";
						$presentState = 12;

					}
					break;
				#kontrola "->"
				case 12:

					if(!$a){
						$symbol = skipWhite($symbol, $input, $caseInsens); 
						checkSyn(strcmp($symbol, "-"));
						$symbol = readUTFChar($input, $caseInsens);
						$a = false;
					}else{
						checkSyn(strcmp($temp, "-"));
					}
					checkSyn(strcmp($symbol, ">"));
					$presentState = 13;
					break;
				#vystupni stav
				case 13:
					$symbol = skipWhite($symbol, $input, $caseInsens); 
					if(ctype_alpha($symbol)){
						$temp = $symbol;
						$presentState = 14;
					}else{
						errorExit("Syntakticka chyba", 40);
					}
					break;
				#viceznakovy vystupni stav
				case 14:
					$symbol = skipWhite($symbol, $input, $caseInsens); 
					if(!strcmp($symbol, ",") || !strcmp($symbol, "}")){#dalsi pravidlo
						$rule->endState = $temp;
						if(!checkItem($states, $temp)){
							errorExit("Semanticka chyba", 41);
						}
						checkSyn(!strcmp(mb_substr($temp, -1), "_"));
						addRule($rules, $rule);
						$presentState = 8;
						if(!strcmp($symbol, "}")){
							$presentState = 15;
						}
					}elseif(ctype_alnum($symbol) || !strcmp($symbol, "_")){
						$temp .= $symbol;
					}else{
						errorExit("Syntakticka chyba", 40);
					}
					break;
				/***************************************\PRAVIDLA****************************************************/
				/***************************************POCATECNI STAV****************************************************/
				#pocatecni stav
				case 15:
					$symbol = skipWhite($symbol, $input, $caseInsens); 
					checkSyn(strcmp($symbol, ","));
					$symbol = readUTFChar($input, $caseInsens);
					$symbol = skipWhite($symbol, $input, $caseInsens); 
					if(ctype_alpha($symbol)){
						$temp = $symbol;
						$presentState = 16;
					}else{
						errorExit("Syntakticka chyba", 40);
					}
					break;
				#viceznakovy stav
				case 16:
					$symbol = skipWhite($symbol, $input, $caseInsens); 
					if(!strcmp($symbol, ",")){#vstupni znak
						if(!checkItem($states, $temp)){
							errorExit("Semanticka chyba", 41);
						}
						checkSyn(!strcmp(mb_substr($temp, -1), "_"));
						$defState = $temp;
						$presentState = 17;
					}elseif(ctype_alnum($symbol) || !strcmp($symbol, "_")){
						$temp .= $symbol;
					}else{
						errorExit("Syntakticka chyba", 40);
					}
					break;
				/***************************************\POCATECNI STAV****************************************************/
				/***************************************KONECNE STAVY****************************************************/
				#konecne stavy - zacatek
				case 17:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					checkSyn(strcmp($symbol, "{"));
					$presentState = 18;
					break;
				#nacteni
				case 18:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					if(empty($rules) && !strcmp($symbol, "}")){
						$presentState = 20;
					}elseif(ctype_alpha($symbol)){
						$temp = $symbol;
						$presentState = 19;
					}else{
						errorExit("Syntakticka chyba", 40);
					}
					break;
				#vice znaku
				case 19:
					$symbol = skipWhite($symbol, $input, $caseInsens); 
					if(!strcmp($symbol, ",") || !strcmp($symbol, "}")){#konec stavu
						if(!checkItem($states, $temp)){
							errorExit("Semanticka chyba", 41);
						}
						checkSyn(!strcmp(mb_substr($temp, -1), "_"));
						addItem($finStates, $temp);
						$presentState = 18;
						if(!strcmp($symbol, "}")){
							$presentState = 20;
						}
					}elseif(ctype_alnum($symbol) || !strcmp($symbol, "_")){
						$temp .= $symbol;
					}else{
						errorExit("Syntakticka chyba", 40);
					}
					break;
				/***************************************\KONECNE STAVY****************************************************/
				#konec souboru
				case 20:
					$symbol = skipWhite($symbol, $input, $caseInsens);
					checkSyn(strcmp($symbol, ")"));
					$symbol = readUTFChar($input, $caseInsens);
					$symbol = skipWhite($symbol, $input, $caseInsens); 
					checkSyn(mb_strlen($input)); //radne ukonceni souboru
					break;
				default:
					errorExit("Chyba programu", -1);
					break;
			}
		}
	}

	/**
	 * Vypis konecneho automatu
	 * @param &$alphabet
	 * abeceda KA
	 * @param &$states
	 * stavy KA
	 * @param &$rules
	 * pravidla KA
	 * @param &$finStates
	 * konecne stavy
	 * @param $output
	 * vystup
	 * @param &$defState
	 * startovni stav
	 **/
	function printFSM(&$states, &$alphabet, &$rules, &$defState, &$finStates, $output){
		sort($states);
		sort($alphabet);
		sort($finStates);
		sort($rules);
		foreach($rules as $rule){
			if(!strcmp($rule->symbol, "eps")){
				$rule->symbol = "";
			}
		}
		$out = "(\n{";
		if(count($states)){
			for($i = 0; $i < count($states) - 1; $i++){
				$out .= $states[intval($i)];
				$out .= ", ";
			}
			$out .= $states[intval(count($states) - 1)];
		}
		$out .= "},\n{";

		for($i = 0; $i < count($alphabet) - 1; $i++){
			$out .= "'";
			$out .= $alphabet[intval($i)];
			$out .= "'";
			$out .= ", ";
		}
		$out .= "'";
		$out .= $alphabet[intval(count($alphabet) - 1)];
		$out .= "'},\n{\n";

		for($i = 0; $i < count($rules) - 1; $i++){
			$out .= $rules[intval($i)]->startState;
			$out .= " '";
			$out .= $rules[intval($i)]->symbol;
			$out .= "' -> ";
			$out .= $rules[intval($i)]->endState;
			$out .= ",\n";
		}
		if(count($rules)){
			$out .= $rules[intval(count($rules) - 1)]->startState;
			$out .= " '";
			$out .= $rules[intval(count($rules) - 1)]->symbol;
			$out .= "' -> ";
			$out .= $rules[intval(count($rules) - 1)]->endState;
			$out .= "\n";
		}
		$out .= "},\n$defState,\n{";

		if(count($finStates)){
			for($i = 0; $i < count($finStates) - 1; $i++){
				$out .= $finStates[intval($i)];
				$out .= ", ";
			}
			$out .= $finStates[intval(count($finStates) - 1)];
		}
		$out .= "}\n)";
		if(!file_put_contents($output, $out)){
			errorExit("Chyba vystupniho souboru", 3);
		}
	}
?>