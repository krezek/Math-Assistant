#ifndef _ITEMS_EXT_H_
#define _ITEMS_EXT_H_

#include <strg.h>
#include <items.h>

typedef struct
{
	Item _item;

	char _sign;
	String* _numerator;
	String* _denominator;

	bool _isPrimary;
} ItemRational;

ItemRational* ItemRational_init(int pl, const char sign, const char* s1, const char* s2);


#endif /* _ITEMS_EXT_H_ */

