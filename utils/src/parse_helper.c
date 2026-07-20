#include "pch.h"
#include "platform.h"

#include <items.h>
#include <math_parser.h>

static void* list(int pl, void* v, void* v1)
{
	return ItemList_init(v, v1);
}

static void* equ(int pl, void* v, void* v1, const char c, bool concat)
{
	return v;
}

static void* add(int pl, void* v, void* v1)
{
	return ItemAdd_init(pl, v, v1);
}

static void* sub (int pl, void* v, void* v1)
{
	return ItemSub_init(pl, v, v1);
}

static void* frac (int pl, void* v, void* v1)
{
	return ItemFrac_init(pl, v, v1);
}

static void* mult (int pl, void* v, void* v1)
{
	return ItemMult_init(pl, v, v1);
}

static void* sign (int pl, void* v, const char c)
{
	return ItemSign_init(pl, v, c);
}

static void* factorial (int pl, void* v)
{
	return ItemFactorial_init(pl, v);
}

static void* power (int pl, void* v, void* v1)
{
	return ItemPow_init(pl, v, v1);
}

static void* subscript (int pl, void* v, void* v1)
{
	return ItemSubscript_init(pl, v, v1);
}

static void* parentheses (int pl, void* v)
{
	return v;
}

static void* common (int pl, void* v, void* v1, const char* s)
{
	return ItemCommFunc_init(pl, v, v1, s);
}

static void* root (int pl, void* v, void* v1)
{
	return ItemRoot_init(pl, v, v1);
}

static void* number (int pl, const char* s)
{
	return ItemNumber_init(pl, s);
}

static void* literal (int pl, const char* s)
{
	return ItemLiteral_init(pl, s);
}


void parse_items(Item** pItems, const char* s)
{
	MParser* mp = MParser_init();

	mp->_listFunc = list;
	mp->_equFunc = equ;
	mp->_addFunc = add;
	mp->_subFunc = sub;
	mp->_fracFunc = frac;
	mp->_multFunc = mult;
	mp->_signFunc = sign;
	mp->_factorialFunc = factorial;
	mp->_powerFunc = power;
	mp->_subscriptFunc = subscript;
	mp->_parenthesesFunc = parentheses;
	mp->_commonFunc = common;
	mp->_rootFunc = root;
	mp->_numberFunc = number;
	mp->_literalFunc = literal;

	Item* i = NULL;
	int rs = MParser_do(mp, (void**)&i, s);
	if (rs)
	{
		if(i)
			ItemTree_free(&i);
	}
	else
		*pItems = i;

	MParser_free(mp);
}
