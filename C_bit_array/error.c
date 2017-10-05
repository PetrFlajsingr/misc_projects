// error.c
// Řešení IJC-DU1, příklad b), 18.3.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// modul obsahujici funkci pro vypis chyby na stderr
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "error.h"

//vypis varovani na stderr bez ukonceni programu
void Warning(const char *fmt, ...)	
{
	va_list pArgs;
	va_start (pArgs, fmt);
	fprintf (stderr, "CHYBA: ");
	vfprintf (stderr, fmt, pArgs);
	va_end (pArgs);
}

//vypis varovani na stderr a ukonceni programu
void FatalError(const char *fmt, ...)	
{
	va_list pArgs;
	va_start (pArgs, fmt);
	fprintf (stderr, "CHYBA: ");
	vfprintf (stderr, fmt, pArgs);
	va_end (pArgs);
	exit(1);
}
