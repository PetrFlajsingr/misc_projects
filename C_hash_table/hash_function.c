#include "htable.h"

/**
 * Vraci hodnotu hash na zaklade parametru
 * @param *str String, podle ktereho je tvoren hash
 * @param htab_size Velikost tabulky
 */
unsigned int hash_function(const char *str, unsigned htab_size) {
	unsigned int h = 0;
	const unsigned char *p;
	for(p = (const unsigned char*)str; *p != '\0'; p++)
		h = 65599 * h + *p;
	return h % htab_size;
}
