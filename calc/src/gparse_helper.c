/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <items.h>
#include <gitems.h>
#include <math_parser.h>

#define REMOVE_EXTRA_PARENTHESE	1

void GList_append(GList* dist, GList* src)
{
	if (src)
	{
		if (src->_pFront)
		{
			GNode* gn = src->_pFront;
			while (gn)
			{
				GList_pushback(dist, gn->_pGItem);
				gn->_pGItem = NULL;

				gn = gn->_pNext;
			}
		}
	}
}

void GList_append_str(int pl, GList* dist, const char* s)
{
	for (size_t ix = 0, len = strlen(s); ix < len; ++ix)
		switch (s[ix])
		{
		case '*':
			GList_pushback(dist, (GItem*)GItemChar_init(pl, L'\u00D7'));
			break;
		default:
			GList_pushback(dist, (GItem*)GItemChar_init(pl, s[ix]));
		}
		
}

static void* list(int pl, void* v, void* v1, const char c)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	char str[] = { c, 0 };
	GList_append_str(pl, gl, str);

	GList_append(gl, gl1);
	GList_free(gl1);

	gl->_item_type = ITEM_List;

	return gl;
}

static void* equ(int pl, void* v, void* v1, const char* pstr)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(pl, gl, pstr);

	GList_append(gl, gl1);
	GList_free(gl1);

	gl->_item_type = ITEM_Equ;

	return gl;
}

static void* add(int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(pl, gl, "+");

	GList_append(gl, gl1);
	GList_free(gl1);

	gl->_item_type = ITEM_Add;

	return gl;
}

static void* sub (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(pl, gl, "-");

	GList_append(gl, gl1);
	GList_free(gl1);

	gl->_item_type = ITEM_Sub;

	return gl;
}

static void* frac (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList* r = GList_init(NULL);
	GList_pushback(r, (GItem*)GItemDivision_init(pl, gl, gl1));

	gl->_item_type = ITEM_Frac;

	return r;
}

static void* mult (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(pl, gl, "*");

	GList_append(gl, gl1);
	GList_free(gl1);

	gl->_item_type = ITEM_Mult;

	return gl;
}

static void* sign (int pl, void* v, const char c)
{
	GList* gl = (GList*)v;

	GList* r = GList_init(NULL);
	GList_pushback(r, (GItem*)GItemChar_init(pl, c));
	GList_append(r, gl);

	GList_free(gl);

	r->_item_type = ITEM_Sign;

	return r;
}

static void* factorial (int pl, void* v)
{
	GList* gl = (GList*)v;

	GList_append_str(pl, gl, "!");

	gl->_item_type = ITEM_Factorial;

	return gl;
}

static void* power (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	if(gl1)
		GList_pushback(gl, (GItem*)GItemPower_init(pl, gl1));

	gl->_item_type = ITEM_Pow;

	return gl;
}

static void* subscript (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	if (gl1)
		GList_pushback(gl, (GItem*)GItemSubscript_init(pl, gl1));

	gl->_item_type = ITEM_Subscript;

	return gl;
}

static void* parentheses (int pl, void* v)
{
	GList* gl = (GList*)v;

#ifdef REMOVE_EXTRA_PARENTHESE
	bool surround = true;

	switch (gl->_item_type)
	{
	case ITEM_Number:
		surround = false;
		break;
	case ITEM_Literal:
		surround = false;
		break;
	case ITEM_Frac:
		surround = false;
		break;
	case ITEM_Pow:
		surround = false;
		break;
	case ITEM_Subscript:
		surround = false;
		break;

	case ITEM_Parentheses:
	{
		if (gl && gl->_pFront)
		{
			if (gl->_pFront->_pGItem->_type == GITEM_CHAR)
			{
				if (((GItemChar*)gl->_pFront->_pGItem)->_ch == L'(')
				{
					if (gl && gl->_pRear)
					{
						if (gl->_pRear->_pGItem->_type == GITEM_CHAR)
						{
							if (((GItemChar*)gl->_pRear->_pGItem)->_ch == L')')
							{
								surround = false;
							}
						}
					}
				}
			}
		}
		
		break;
	}

	default:
		surround = true;
	}

	GList* r = GList_init(NULL);
	if(surround)
		GList_append_str(pl, r, "(");
	GList_append(r, gl);
	if(surround)
		GList_append_str(pl, r, ")");

	GList_free(gl);
#else
	GList* r = GList_init(NULL);
	GList_append_str(pl, r, "(");
	GList_append(r, gl);
	GList_append_str(pl, r, ")");

	GList_free(gl);
#endif

	r->_item_type = ITEM_Parentheses;

	return r;
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
		GList* gl = (GList*)v;

		GList* r = GList_init(NULL);
		GList_append_str(pl, r, s);
		GList_append_str(pl, r, "(");
		GList_append(r, gl);
		GList_append_str(pl, r, ")");

		GList_free(gl);

		r->_item_type = ITEM_Triangle;

		return r;
	}
	else if (_stricmp(s, "Root") == 0)
	{
		GList* gl = (GList*)v;

		GList* r = GList_init(NULL);
		GList_append_str(pl, r, s);
		GList_append_str(pl, r, "(");
		GList_append(r, gl);
		GList_append_str(pl, r, ")");

		GList_free(gl);

		r->_item_type = ITEM_Root;

		return r;
	}
	else if (_stricmp(s, "Integral") == 0)
	{
		GList* gl = (GList*)v;

		GList* r = GList_init(NULL);
		GList_append_str(pl, r, s);
		GList_append_str(pl, r, "(");
		GList_append(r, gl);
		GList_append_str(pl, r, ")");

		GList_free(gl);

		r->_item_type = ITEM_Integral;

		return r;
	}
	else if (_stricmp(s, "Derivative") == 0)
	{
		GList* gl = (GList*)v;

		GList* r = GList_init(NULL);
		GList_append_str(pl, r, s);
		GList_append_str(pl, r, "(");
		GList_append(r, gl);
		GList_append_str(pl, r, ")");

		GList_free(gl);

		r->_item_type = ITEM_Derivative;

		return r;
	}
	else if (_stricmp(s, "Lim") == 0)
	{
		GList* gl = (GList*)v;

		GList* r = GList_init(NULL);
		GList_append_str(pl, r, s);
		GList_append_str(pl, r, "(");
		GList_append(r, gl);
		GList_append_str(pl, r, ")");

		GList_free(gl);

		r->_item_type = ITEM_Limit;

		return r;
	}
	else
	{
		GList* gi = (GList*)v;
		if (gi)
		{
			GListTree_free(&gi);
		}
		return NULL;
	}
}

static void* number (int pl, const char* s)
{
	GList* gl = GList_init(NULL);
	GList_append_str(pl, gl, s);

	gl->_item_type = ITEM_Number;
	
	return gl;
}

static void* symbol (int pl, const char c)
{
	GList* gl = GList_init(NULL);
	GList_pushback(gl, (GItem*)GItemChar_init(pl, c));

	gl->_item_type = ITEM_Literal;

	return gl;
}

static void* literal (int pl, const char* s)
{
	GList* gl = GList_init(NULL);

	if (_stricmp(s, "pi") == 0)
	{
		GList_pushback(gl, (GItem*)GItemChar_init(pl, L'\u03C0'));
	}
	else if (_stricmp(s, "theta") == 0)
	{
		GList_pushback(gl, (GItem*)GItemChar_init(pl, L'\u03B8'));
	}
	else if (_stricmp(s, "phi") == 0)
	{
		GList_pushback(gl, (GItem*)GItemChar_init(pl, L'\u03C6'));
	}
	else
		GList_append_str(pl, gl, s);

	gl->_item_type = ITEM_Literal;

	return gl;
}


int parse_gitems(GList** ppGl, const char* s)
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

	GList* gitems = NULL;
	int rs = MParser_do(mp, &gitems, s, (TreeFreeFunc)GListTree_free);
	if (!rs)
	{
		*ppGl = gitems;
	}

	MParser_free(mp);
	return rs;
}
