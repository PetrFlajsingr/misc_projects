// error.h
// Řešení IJC-DU1, příklad b), 18.3.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// hlavickovy soubor obsahujici rozhrani modulu error.c
#ifndef ERROR_H
#define ERROR_H
void Warning(const char *fmt, ...);
void FatalError(const char *fmt, ...);
#endif
