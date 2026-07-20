#include "pch.h"
#include "platform.h"

#include <gitems.h>
#include <math_parser.h>

void GList_append(GList* src, GList* dist)
{
	if (src)
	{
		if (src->_front)
		{
			GNode* gn = src->_front;
			while (gn)
			{
				GList_pushback(dist, gn->_pGItem);
				gn->_pGItem = NULL;

				gn = gn->_next;
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

static void* list(int pl, void* v, void* v1)
{
	return v;
}

static void* equ(int pl, void* v, void* v1, const char c, bool concat)
{
	return v;
}

static void* add(int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(pl, gl, "+");

	GList_append(gl1, gl);
	GList_free(gl1);

	return gl;
}

static void* sub (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(pl, gl, "-");

	GList_append(gl1, gl);
	GList_free(gl1);

	return gl;
}

static void* frac (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	if (gl->_front->_pGItem->_type == GITEM_CHAR)
	{
		GNode* n = gl->_front;
		GItemChar* i = (GItemChar*)n->_pGItem;
		if (i->_ch == L'(')
		{
			gl->_front = gl->_front->_next;
			gl->_front->_prev = NULL;
			GNode_free(n);
		}
	}

	if (gl->_rear->_pGItem->_type == GITEM_CHAR)
	{
		GNode* n = gl->_rear;
		GItemChar* i = (GItemChar*)n->_pGItem;
		if (i->_ch == L')')
		{
			gl->_rear = gl->_rear->_prev;
			gl->_rear->_next = NULL;
			GNode_free(n);
		}
	}

	if (gl1->_front->_pGItem->_type == GITEM_CHAR)
	{
		GNode* n = gl1->_front;
		GItemChar* i = (GItemChar*)n->_pGItem;
		if (i->_ch == L'(')
		{
			gl1->_front = gl1->_front->_next;
			gl1->_front->_prev = NULL;
			GNode_free(n);
		}
	}

	if (gl1->_rear->_pGItem->_type == GITEM_CHAR)
	{
		GNode* n = gl1->_rear;
		GItemChar* i = (GItemChar*)n->_pGItem;
		if (i->_ch == L')')
		{
			gl1->_rear = gl1->_rear->_prev;
			gl1->_rear->_next = NULL;
			GNode_free(n);
		}
	}

	GList* r = GList_init(NULL);
	GList_pushback(r, (GItem*)GItemFraction_init(pl, gl, gl1));

	return r;
}

static void* mult (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(pl, gl, "*");

	GList_append(gl1, gl);
	GList_free(gl1);

	return gl;
}

static void* sign (int pl, void* v, const char c)
{
	GList* gl = (GList*)v;

	GList* r = GList_init(NULL);
	GList_pushback(r, (GItem*)GItemChar_init(pl, c));
	GList_append(gl, r);

	GList_free(gl);

	return r;
}

static void* factorial (int pl, void* v)
{
	GList* gl = (GList*)v;

	GList_append_str(pl, gl, "!");

	return gl;
}

static void* power (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	if (gl1 && gl1->_front->_pGItem->_type == GITEM_CHAR)
	{
		GNode* n = gl1->_front;
		GItemChar* i = (GItemChar*)n->_pGItem;
		if (i->_ch == L'(')
		{
			gl1->_front = gl1->_front->_next;
			gl1->_front->_prev = NULL;
			GNode_free(n);
		}
	}

	if (gl1 && gl1->_rear->_pGItem->_type == GITEM_CHAR)
	{
		GNode* n = gl1->_rear;
		GItemChar* i = (GItemChar*)n->_pGItem;
		if (i->_ch == L')')
		{
			gl1->_rear = gl1->_rear->_prev;
			gl1->_rear->_next = NULL;
			GNode_free(n);
		}
	}

	if(gl1)
		GList_pushback(gl, (GItem*)GItemPower_init(pl, gl1));

	return gl;
}

static void* subscript (int pl, void* v, void* v1)
{
	return v;
}

static void* parentheses (int pl, void* v)
{
	GList* gl = (GList*)v;

	GList* r = GList_init(NULL);
	GList_append_str(pl, r, "(");
	GList_append(gl, r);
	GList_append_str(pl, r, ")");

	GList_free(gl);

	return r;
}

static void* common (int pl, void* v, void* v1, const char* s)
{
	GList* gl = (GList*)v;

	GList* r = GList_init(NULL);
	GList_append_str(pl, r, s);
	GList_append_str(pl, r, "(");
	GList_append(gl, r);
	GList_append_str(pl, r, ")");

	GList_free(gl);

	return r;
}

static void* root (int pl, void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList* r = GList_init(NULL);
	GList_pushback(r, (GItem*)GItemRoot_init(pl, gl, gl1));

	return r;
}

static void* number (int pl, const char* s)
{
	GList* gl = GList_init(NULL);
	GList_append_str(pl, gl, s);
	
	return gl;
}

static void* symbol (int pl, const char c)
{
	GList* gl = GList_init(NULL);
	GList_pushback(gl, (GItem*)GItemChar_init(pl, c));

	return gl;
}

static void* literal (int pl, const char* s)
{
	GList* gl = GList_init(NULL);

	if (_stricmp(s, "pi") == 0)
	{
		GList_pushback(gl, (GItem*)GItemChar_init(pl, L'\u03C0'));
	}
	else
		GList_append_str(pl, gl, s);

	return gl;
}


void parse_gitems(GList** ppGl, const char* s)
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

	GList* i = NULL;
	int rs = MParser_do(mp, &i, s);
	if (rs)
	{
		GList_free(i);
	}
	else
		*ppGl = i;

	MParser_free(mp);
}
