/**
 * @file ial.c
 * @project IFJ16
 * @author Petr Flajsingr (xflajs00@stud.fit.vutbr.cz)
 * @date 4 November 2016
 * @brief Hlavickovy soubor obahujici prototypy funkci a struktur pro vyhledavaci algoritmus KMP
 * a radici algoritmus heap sort
 **/

#ifndef IAL_H
#define IAL_H

//***************SORT******************

//struktura pro heapsort, resena jako strom
typedef struct S_heap{
	char value; /*< hodnota prvku */
	struct S_heap* parent; /*< nadrazeny uzel */
	struct S_heap* left_child; /*< podrazeny uzel */
	struct S_heap* right_child; /*< podrazeny uzel */
}T_heap;

typedef T_heap* T_stack; /*< zasobnik pro heap */

char* sort(char*); /*< heapsort */
T_heap* buildHeap(char*, int); /*< tvorba heap */
void minHeap(T_heap*, int); /*< presun min hodnoty do root */
T_heap* newHeapRoot(T_heap*, int); /*< odstraneni root a vytvoreni noveho */
T_heap* getNodePtr(T_heap*, int); /*< ziskani pointeru na uzel */
void swap(char*, char*); /*< prehozeni dvou char hodnot */
void freeHeap(T_heap*); /*< uvolneni alokovane pameti */

//**************/SORT******************
//***************FIND******************
int find(char*, char*); /*< vyhledavaci algoritmus KMP */
void matchTable(char*, int*); /*< vytvoreni tabulky pro KMP */
//**************/FIND******************
#endif //IAL_H
