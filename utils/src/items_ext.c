/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <proc.h>
#include <strg.h>
#include <items_ext.h>

void RuleQueue_enqueue(RuleQueue* q, const char* s);

// Rational

void ItemRational_destroy(Item* _this)
{
	ItemRational* i = (ItemRational*)_this;

	String_free(i->_numerator);
	String_free(i->_denominator);
}

void ItemRational_toString(int pl, Item* _this, String* s)
{
	ItemRational* i = (ItemRational*)_this;

	bool isZ = strcmp(i->_denominator->_str, "1") == 0;
	
	String_cpy(s, "");

	if (i->_sign == '-')
		String_cat(s, "(-");

	if (!isZ)
		String_cat(s, "(");

	String_cat(s, i->_numerator->_str);
	
	if(!isZ)
		String_cat(s, "/");
	
	if(!isZ)
		String_cat(s, i->_denominator->_str);

	if (!isZ)
		String_cat(s, ")");

	if (i->_sign == '-')
		String_cat(s, ")");
}

void ItemRational_getRule(int rl, Item* _this, RuleQueue* rq)
{
	ItemRational* i = (ItemRational*)_this;

	RuleQueue_enqueue(rq, "R");
}

ItemRational* ItemRational_init(int pl, const char sign, const char* s1, const char* s2)
{
	ItemRational* i = (ItemRational*)malloc(sizeof(ItemRational));
	assert(i != 0);

	i->_item._left = NULL;
	i->_item._right = NULL;

	i->_item._destroyFunc = ItemRational_destroy;
	i->_item._toStringFunc = ItemRational_toString;
	i->_item._getRuleFunc = ItemRational_getRule;

	i->_item._type = ITEM_Rational;
	i->_item._proc_level = pl;

	i->_isPrimary = false;

	i->_sign = sign;
	i->_numerator = String_init();
	String_cpy(i->_numerator, s1);
	i->_denominator = String_init();
	String_cpy(i->_denominator, s2);

	return i;
}

