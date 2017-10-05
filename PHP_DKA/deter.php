<?php

#DKA:xflajs00
/**
 * Predmet: IPP
 * Projekt: proj1
 * Varianta zadani: DKA (determinizace konecneho automatu)
 * @author: Petr Flajsingr, xflajs00
 **/

	/**
	 * Determinizace KA podle alogritmu z IFJ
	 * @param &$states
	 * stavy pro upravu
	 * @param &$alphabet
	 * abeceda pro upravu
	 * @param &$rules
	 * pravidla pro upravu
	 * @param &$defState
	 * startovni stav
	 * @param &$finStates
	 * konecne stavy
	 **/
	function determinize(&$states, &$alphabet, &$rules, &$defState, &$finStates){
		$detRules = array(); #Rd
		$detStates = array(); #Qd
		$detFinStates = array(); #Fd
		$newStates = array(array($defState)); #Qnew
		$i = 0;

		while(count($newStates)){ #do vycerpani vsech stavu
			$tempStates = $newStates[intval($i)]; #Q'=Qnew
			unset($newStates[intval($i)]); #Qnew = Qnew - {Q'}
			$i++;
			joinStates($detStates, $tempStates); #Qd = Qd u {Q'}
			foreach($alphabet as $symbol){ #for each a € epsilon
				# Q'' = {q: p€Q', pa->€R}
				$tempStates2 = array();
				foreach($tempStates as $state){
					foreach($rules as $rule){
						if(!strcmp($rule->startState, $state) && !strcmp($rule->symbol, $symbol)){
							addItem($tempStates2, $rule->endState);
						}
					}
				}
				#Q''!={} then Rd = Rd u {Q'a->Q''}
				if(count($tempStates2)){
					joinNames($tempStates, $startState);
					joinNames($tempStates2, $endState);
					$rule = new rule();
					$rule->startState = $startState;
					$rule->symbol = $symbol;
					$rule->endState = $endState;
					addRule($detRules, $rule);
				}
				#Q''!€Qd v {} then Qnew = Qnew u {Q''}
				if(!checkStates($detStates, $tempStates2)){
					joinStates($newStates, $tempStates2);
				}
			}
			addFinState($tempStates, $finStates, $detFinStates);
		}
		joinAll($detStates, $detFinStates);
		#ulozeni zmeneneho KA
		$states = $detStates;
		$rules = $detRules;
		$finStates = $detFinStates;
	}

	/**
	 * Spojeni stavu
	 * @param &$newStates
	 * kontrolovane stavy
	 * @param &$join
	 * ukladane stavy
	 **/
	function joinStates(&$newStates, &$join){
		$check = false;
		foreach($newStates as $temp){
			if(!array_diff($temp, $join) && !array_diff($join, $temp)){
				$check = true;
			}
		}
		if(!$check){
			array_push($newStates, $join);
		}
	}

	/**
	 * Spojeni nazvu stavu
	 * @param &$states
	 * spojovane stavy
	 * @param &$name
	 * vysledek spojeni
	 **/
	function joinNames(&$states, &$name){
		sort($states); #spravne serazeni
		$name = "";
		for($i = 0; $i < count($states) - 1; $i++){
			$name .= $states[intval($i)];
			$name .= "_";
		}
		$name .= $states[intval(count($states) - 1)];
	}

	/**
	 * Pridani konecneho stavu
	 * @param &$tempStates
	 * kontrolovane stavy
	 * @param &$finStates
	 * puvodni konecne stavy
	 * @param &$detFinStates
	 * upravene konecne stavy
	 **/
	function addFinState(&$tempStates, &$finStates, &$detFinStates){
		foreach($tempStates as $temp){
			if(checkItem($finStates, $temp)){
				joinStates($detFinStates, $tempStates);
				break;
			}
		}
	}
	/**
	 * Pridani konecneho stavu
	 * @param &$tempStates
	 * kontrolovane stavy
	 * @param &$finStates
	 * puvodni konecne stavy
	 * @param &$detFinStates
	 **/
	function checkStates(&$newStates, &$tempStates2){
		if(!count($tempStates2)){ #prazdna mnozina
			return true;
		}
		foreach($newStates as $temp){
			if(!array_diff($temp, $tempStates2) && !array_diff($tempStates2, $temp)){ #obsahuje
				return true;
			}
		}
		return false;
	}

	/**
	 * Konecne spojeni nazvu
	 * @param &$detStates
	 * zmenene stavy
	 * @param &$detFinStates
	 * zmenene konecne stavy
	 **/
	function joinAll(&$detStates, &$detFinStates){
		$nDetStates = array();
		$nFinStates = array();
		foreach($detStates as $temp){
			joinNames($temp, $temp2);
			array_push($nDetStates, $temp2);
		}
		foreach($detFinStates as $temp){
			joinNames($temp, $temp2);
			array_push($nFinStates, $temp2);
		}
		$detStates = $nDetStates;
		$detFinStates = $nFinStates;
	}

?>