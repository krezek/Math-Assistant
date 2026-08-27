/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <items.h>
#include <math_parser.h>

static void* list(int pl, void* v, void* v1, const char c)
{
	return ItemList_init(v, v1, c);
}

static void* equ(int pl, void* v, void* v1, const char* opstr)
{
	return ItemEqu_init(v, v1, opstr);
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

static void* common (int pl, void* v, const char* s)
{
	if (_stricmp(s, "Ln") == 0 ||
		_stricmp(s, "Atan") == 0 ||
		_stricmp(s, "Acos") == 0 ||
		_stricmp(s, "Asin") == 0 ||
		_stricmp(s, "Exp") == 0 ||
		_stricmp(s, "Log") == 0 ||
		_stricmp(s, "Tan") == 0 ||
		_stricmp(s, "Cos") == 0 ||
		_stricmp(s, "Sin") == 0)
	{
		return ItemTriangle_init(pl, v, NULL, s);
	}
	else if (_stricmp(s, "Root") == 0)
	{
		Item* l = ((Item*)v)->_left;
		Item* r = ((Item*)v)->_right;

		((Item*)v)->_left = ((Item*)v)->_right = NULL;
		free(v);
		
		return ItemRoot_init(pl, l, r);
	}
	else if (_stricmp(s, "Integral") == 0)
	{
		Item* l = ((Item*)v)->_left;
		Item* r = ((Item*)v)->_right;
		
		((Item*)v)->_left = ((Item*)v)->_right = NULL;
		free(v);
		
		return ItemIntegral_init(pl, l, r);
	}
	else if (_stricmp(s, "Derivative") == 0)
	{
		Item* l = ((Item*)v)->_left;
		Item* r = ((Item*)v)->_right;

		((Item*)v)->_left = ((Item*)v)->_right = NULL;
		free(v);
		
		return ItemDerivative_init(pl, l, r);
	}
	else if (_stricmp(s, "Lim") == 0)
	{
		Item* l = ((Item*)v)->_left;
		Item* r = ((Item*)v)->_right;
		
		((Item*)v)->_left = ((Item*)v)->_right = NULL;
		free(v);
		
		return ItemLimit_init(pl, l, r);
	}
	else
	{
		Item* i = (Item*)v;
		if (i)
		{
			ItemTree_free(&i);
		}
		return NULL;
	}
}

static void* number (int pl, const char* s)
{
	return ItemNumber_init(pl, s);
}

static void* literal (int pl, const char* s)
{
	return ItemLiteral_init(pl, s);
}


int parse_items(Item** pItems, const char* s)
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
	mp->_commonFnFunc = common;
	mp->_numberFunc = number;
	mp->_literalFunc = literal;

	Item* nodes = NULL;

	int rs = MParser_do(mp, &nodes, s, (TreeFreeFunc)ItemTree_free);
	if (!rs)
	{
		*pItems = nodes;
	}

	MParser_free(mp);

	return rs;
}
