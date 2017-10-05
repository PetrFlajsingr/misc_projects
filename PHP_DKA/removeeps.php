<?php

#DKA:xflajs00
/**
 * Predmet: IPP
 * Projekt: proj1
 * Varianta zadani: DKA (determinizace konecneho automatu)
 * @author: Petr Flajsingr, xflajs00
 **/
	/**
	 * odstraneni epsilon prechodu
	 * @param &$states
	 * stavy KA
	 * @param &$rules
	 * pravidla KA
	 * @param &$finStates
	 * konecne stavy KA
	 **/
	function removeEps(&$states, &$rules, &$finStates){
		$nRules = array(); #pravidla bez epsilon prechodu
		$nFinStates = array(); #koncove stavy bez epsilon prechodu
		foreach($states as $state){ #pruchod vsech stavu
			$tempAr1 = array();
			addItem($tempAr1, $state);
			$tempAr2 = array();
			while(array_diff($tempAr1, $tempAr2)){ #kontrola vsech nasledujicich stavu
				$tempAr2 = $tempAr1;
				foreach($tempAr2 as $tmpState){
					foreach($rules as $rule){ #pruchod vsech pravidel
						if (!strcmp($tmpState, $rule->startState) && !strcmp($rule->symbol, "eps")){ #pokud se jedna o epsilon prechod
							addItem($tempAr1, $rule->endState); 
						}
					}
				}
			}

			foreach($tempAr1 as $temp){ #vsechny nalezene epsilon prechody
				foreach($rules as $rule){ #vsechna pravidla
					if(!strcmp($temp, $rule->startState) && strcmp($rule->symbol, "eps")){ #nahrazeni epsilon pravidla novym pravidlem a uozeni platnych pravidel
						$nRule = new rule();
						$nRule->startState = $state;
						$nRule->symbol = $rule->symbol;
						$nRule->endState = $rule->endState;
						addRule($nRules, $nRule); #ulozeni noveho pravidla
					}
				}
			}

			foreach($tempAr1 as $temp){ #uprava koncovych stavu
				foreach($finStates as $fin){
					if(!strcmp($temp, $fin)){
						addItem($nFinStates, $state);
					}
				}
			}
		}
		$rules = $nRules; #ulozeni zmenenych pravidel
		$finStates = $nFinStates; #ulozeni zmenenych koncovych stavu
	}
?>