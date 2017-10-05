// bit-array.h
// Řešení IJC-DU1, příklad a), 18.3.2015
// Autor: Petr Flajšingr, FIT
// Přeloženo: gcc 4.8.4
// hlavickovy soubor definujici makra, inline funkce a datovy typ pro praci s bitovym polem

#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H
#include "error.h"
#include <stdbool.h>

//bitove pole
typedef unsigned long BitArray_t[];

//makro pro zjisteni velikosti unsigned long v bitech
#define BA_ul_bit_size (unsigned int)(sizeof(unsigned long) * 8)

//makro pro vytvoreni bitoveho pole, na nultem indexu je ulozena velikost pole v bytech
#define BA_create(var, size) int x = size; x /= BA_ul_bit_size; if(size % BA_ul_bit_size) x++; \
								unsigned long var[(x + 1)]; \
								var[0] = x;\
								for(int y = 1; y <= x; y++) var[y] = 0;
//makro pro vrácení hodnoty bitu
#define DU1_GET_BIT_(p, i) (((p[i / BA_ul_bit_size + 1]) & ((unsigned long)1 << (i % BA_ul_bit_size))) != 0)
//makro pro nastavení hodnoty bitu
#define DU1_SET_BIT_(p, i, b) if(b) (p[i / BA_ul_bit_size + 1] |= ((unsigned long)1 << (i % BA_ul_bit_size))); \
									else (p[i / BA_ul_bit_size + 1] &= ~((unsigned long)1 << (i % BA_ul_bit_size)))
//parametr USE_INLINE
#ifdef USE_INLINE
//presmerovani maker na inline funkce
#define BA_get_bit(var, pos) inlineGetBit(var, pos)
#define BA_set_bit(var, pos, value) inlineSetBit(var, pos, value)
#define BA_size(var) inlineSize(var);

//deklarace inline funkci
	inline bool inlineGetBit(BitArray_t, unsigned int);
	inline void inlineSetBit(BitArray_t, unsigned int, bool);
	inline int inlineSize(BitArray_t);
#else
//makro pro ziskani hodnoty bitu s kontrolou přetečení
#define BA_get_bit(var, pos) (((unsigned int)pos >= (unsigned int)BA_size(var) ? \
							(FatalError("Index %ld mimo rozsah 0..%ld", (long)pos, (long)BA_size(var) - 1), 0) : \
							(DU1_GET_BIT_(var, pos))))
							
//makro pro zapsani hodnoty bitu s kontrolou přetečení
#define BA_set_bit(var, pos, value) if((unsigned int)pos >= (unsigned int)BA_size(var)) \
									{FatalError("Index %ld mimo rozsah 0..%ld", (long)pos, (long)BA_size(var) - 1);} \
									DU1_SET_BIT_(var, pos, value)
									
//makro pro zjisteni velikosti bitoveho pole v bitech
#define BA_size(var) (var[0] * BA_ul_bit_size)
#endif

//inline funkce nahrazujici makro BA_get_bit
inline bool inlineGetBit(BitArray_t var, unsigned int pos)
{
	if(pos >= (unsigned int)(var[0] * BA_ul_bit_size))
	{
		FatalError("Index %ld mimo rozsah 0..%ld", (long)pos, (long)((var[0] * BA_ul_bit_size) - 1));
	}
	return (DU1_GET_BIT_(var, pos));
}
//inline funkce nahrazujici makro BA_set_bit
inline void inlineSetBit(BitArray_t var, unsigned int pos, bool value)
{
	if(pos >= (unsigned int)(var[0] * BA_ul_bit_size))
	{
		FatalError("Index %ld mimo rozsah 0..%ld", (long)pos, (long)((var[0] * BA_ul_bit_size) - 1));
	}
	else
		DU1_SET_BIT_(var, pos, value);
}
//inline funkce nahrazující makro BA_size
inline int inlineSize(BitArray_t var)
{
	return var[0];
}
#endif
