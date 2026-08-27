/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <gmp.h>
#include <mpfr.h>

#include <items.h>
#include <calc.h>

#define MY_MPFR_RND MPFR_RNDD

void calculate(mpfr_t* t, Item** nodes, int* pCode);
void parse_items(Item** pItems, const char* s);

char* do_calc(const char* expr)
{
	Item* item = NULL;
	parse_items(&item, expr);

	if (item)
	{
#ifdef _DEBUG
		printf("calc origin: %s\n", expr);

		String* sx = String_init();
		item->_toStringFunc(0, item, sx);
		printf("calc after: %s\n", sx->_str);
		String_free(sx);
#endif

		int rc = 0;
		char str[255];

		mpfr_t t;
		mpfr_init2 (t, MPFR_PRECISION);

		calculate(&t, &item, &rc);
		if (rc == 0)
		{
			mpfr_sprintf(str, "%.25R*f", MPFR_RNDN, t);
		}
		else
		{
			sprintf(str, "InvalidValue");
		}

		ItemTree_free(&item);

		char* rt = (char*)malloc((strlen(str) + 1) * sizeof(char));
		assert(rt != NULL);
		sprintf(rt, "%s", str);

		mpfr_clear(t);
		mpfr_free_cache();

		return rt;
	}


	return NULL;
}

void calcWolker(mpfr_t* t, Item* item, int* pCode)
{
	if (item->_type == ITEM_Number)
	{
		ItemNumber* i = (ItemNumber*)item;

		mpfr_set_str(*t, i->_str->_str, 10, MY_MPFR_RND);
	}
	else if (item->_type == ITEM_Add)
	{
		mpfr_t a1, a2;
		mpfr_init2 (a1, MPFR_PRECISION);
		mpfr_init2 (a2, MPFR_PRECISION);

		calcWolker(&a1, item->_left, pCode);
		calcWolker(&a2, item->_right, pCode);

		mpfr_add (*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Sub)
	{
		mpfr_t a1, a2;
		mpfr_init2 (a1, MPFR_PRECISION);
		mpfr_init2 (a2, MPFR_PRECISION);

		calcWolker(&a1, item->_left, pCode);
		calcWolker(&a2, item->_right, pCode);

		mpfr_sub (*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Mult)
	{
		mpfr_t a1, a2;
		mpfr_init2 (a1, MPFR_PRECISION);
		mpfr_init2 (a2, MPFR_PRECISION);

		calcWolker(&a1, item->_left, pCode);
		calcWolker(&a2, item->_right, pCode);

		mpfr_mul (*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Frac)
	{
		mpfr_t a1, a2;
		mpfr_init2 (a1, MPFR_PRECISION);
		mpfr_init2 (a2, MPFR_PRECISION);

		calcWolker(&a1, item->_left, pCode);
		calcWolker(&a2, item->_right, pCode);

		mpfr_div (*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Sign)
	{
		ItemSign* i = (ItemSign*)item;

		mpfr_t a;
		mpfr_init2 (a, MPFR_PRECISION);

		if (i->_sgn == L'-')
		{
			calcWolker(&a, item->_left, pCode);
			mpfr_neg(*t, a, MY_MPFR_RND);
		}
		else
		{
			calcWolker(&a, item->_left, pCode);
			mpfr_set(*t, a, MY_MPFR_RND);
		}

		mpfr_clear(a);
	}
	else if (item->_type == ITEM_Pow)
	{
		mpfr_t a1, a2;
		mpfr_init2 (a1, MPFR_PRECISION);
		mpfr_init2 (a2, MPFR_PRECISION);

		calcWolker(&a1, item->_left, pCode);
		calcWolker(&a2, item->_right, pCode);

		mpfr_pow (*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Factorial)
	{
		mpfr_t a;
		mpfr_init2 (a, MPFR_PRECISION);

		calcWolker(&a, item->_left, pCode);
		mpfr_fac_ui(*t, mpfr_get_si(a, MY_MPFR_RND), MY_MPFR_RND);

		mpfr_clear(a);
	}
	else if (item->_type == ITEM_Root)
	{
		mpfr_t a1, a2, a3;
		mpfr_init2 (a1, MPFR_PRECISION);
		mpfr_init2 (a2, MPFR_PRECISION);
		mpfr_init2 (a3, MPFR_PRECISION);

		calcWolker(&a1, item->_left, pCode);
		calcWolker(&a2, item->_right, pCode);

		mpfr_set_si(a3, 1, MY_MPFR_RND);
		mpfr_div(a3, a3, a2, MY_MPFR_RND);

		mpfr_pow(*t, a1, a3, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
		mpfr_clear(a3);
	}
	else if (item->_type == ITEM_Triangle)
	{
		ItemTriangle* i = (ItemTriangle*)item;
		
		if (_stricmp("sin", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2 (a, MPFR_PRECISION);

			calcWolker(&a, item->_left, pCode);
			mpfr_sin(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("cos", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2 (a, MPFR_PRECISION);

			calcWolker(&a, item->_left, pCode);
			mpfr_cos(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("tan", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2 (a, MPFR_PRECISION);

			calcWolker(&a, item->_left, pCode);
			mpfr_tan(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("log", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2 (a, MPFR_PRECISION);

			calcWolker(&a, item->_left, pCode);
			mpfr_log10(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("exp", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2 (a, MPFR_PRECISION);

			calcWolker(&a, item->_left, pCode);
			mpfr_exp(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("asin", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2 (a, MPFR_PRECISION);

			calcWolker(&a, item->_left, pCode);
			mpfr_asin(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("acos", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2 (a, MPFR_PRECISION);

			calcWolker(&a, item->_left, pCode);
			mpfr_acos(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("atan", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2 (a, MPFR_PRECISION);

			calcWolker(&a, item->_left, pCode);
			mpfr_atan(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("ln", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2 (a, MPFR_PRECISION);

			calcWolker(&a, item->_left, pCode);
			mpfr_log(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
	}
	else if (item->_type == ITEM_Parentheses)
	{
		calcWolker(t, item->_left, pCode);
	}
	else if (item->_type == ITEM_Literal)
	{
		ItemLiteral* i = (ItemLiteral*)item;
		
		if (_stricmp("pi", i->_str->_str) == 0)
		{
			mpfr_const_pi(*t, MY_MPFR_RND);
		}
		else if (strcmp("e", i->_str->_str) == 0)
		{
			mpfr_set_str(*t, "2.718281828459045235360287471352", 10, MY_MPFR_RND);
		}
		else
		{
			*pCode = -1;
		}
	}
	else
	{
		*pCode = -1;
	}
}

void calculate(mpfr_t* t, Item** nodes, int* pCode)
{
	*pCode = 0;
	calcWolker(t, *nodes, pCode);
}

void calcWolker_v3(float x, float y, float z, float* t, Item* item, int* pCode)
{
	if (item->_type == ITEM_Number)
	{
		ItemNumber* i = (ItemNumber*)item;

		*t = (float)atof(i->_str->_str);
	}
	else if (item->_type == ITEM_Add)
	{
		float a1, a2;
		
		calcWolker_v3(x, y, z, &a1, item->_left, pCode);
		calcWolker_v3(x, y, z, &a2, item->_right, pCode);

		*t = a1 + a2;
	}
	else if (item->_type == ITEM_Sub)
	{
		float a1, a2;
		
		calcWolker_v3(x, y, z, &a1, item->_left, pCode);
		calcWolker_v3(x, y, z, &a2, item->_right, pCode);

		*t = a1 - a2;
	}
	else if (item->_type == ITEM_Mult)
	{
		float a1, a2;

		calcWolker_v3(x, y, z, &a1, item->_left, pCode);
		calcWolker_v3(x, y, z, &a2, item->_right, pCode);

		*t = a1 * a2;
	}
	else if (item->_type == ITEM_Frac)
	{
		float a1, a2;

		calcWolker_v3(x, y, z, &a1, item->_left, pCode);
		calcWolker_v3(x, y, z, &a2, item->_right, pCode);

		*t = a1 / a2;
	}
	else if (item->_type == ITEM_Sign)
	{
		ItemSign* i = (ItemSign*)item;

		float a;

		if (i->_sgn == L'-')
		{
			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = -a;
		}
		else
		{
			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = a;
		}
	}
	else if (item->_type == ITEM_Pow)
	{
		float a1, a2;

		calcWolker_v3(x, y, z, &a1, item->_left, pCode);
		calcWolker_v3(x, y, z, &a2, item->_right, pCode);

		*t = powf(a1, a2);
	}
	else if (item->_type == ITEM_Factorial)
	{
		float a;
		
		calcWolker_v3(x, y, z, &a, item->_left, pCode);
		
		*t = a;
	}
	else if (item->_type == ITEM_Root)
	{
		float a1, a2;

		calcWolker_v3(x, y, z, &a1, item->_left, pCode);
		calcWolker_v3(x, y, z, &a2, item->_right, pCode);

		*t = powf(a1, 1 / a2);
	}
	else if (item->_type == ITEM_Triangle)
	{
		ItemTriangle* i = (ItemTriangle*)item;

		if (_stricmp("sin", i->_str->_str) == 0)
		{
			float a;

			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = sinf(a);
		}
		else if (_stricmp("cos", i->_str->_str) == 0)
		{
			float a;

			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = cosf(a);
		}
		else if (_stricmp("tan", i->_str->_str) == 0)
		{
			float a;

			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = tanf(a);
		}
		else if (_stricmp("log", i->_str->_str) == 0)
		{
			float a;

			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = log10f(a);
		}
		else if (_stricmp("exp", i->_str->_str) == 0)
		{
			float a;

			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = expf(a);
		}
		else if (_stricmp("asin", i->_str->_str) == 0)
		{
			float a;

			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = asinf(a);
		}
		else if (_stricmp("acos", i->_str->_str) == 0)
		{
			float a;

			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = acosf(a);
		}
		else if (_stricmp("atan", i->_str->_str) == 0)
		{
			float a;

			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = atanf(a);
		}
		else if (_stricmp("ln", i->_str->_str) == 0)
		{
			float a;

			calcWolker_v3(x, y, z, &a, item->_left, pCode);
			*t = logf(a);
		}
	}
	else if (item->_type == ITEM_Parentheses)
	{
		calcWolker_v3(x, y, z, t, item->_left, pCode);
	}
	else if (item->_type == ITEM_Literal)
	{
		ItemLiteral* i = (ItemLiteral*)item;

		if (_stricmp("pi", i->_str->_str) == 0)
		{
			*t = (float)M_PI;
		}
		else if (strcmp("e", i->_str->_str) == 0)
		{
			*t = (float)2.718281828459045235360287471352;
		}
		else if (strcmp("x", i->_str->_str) == 0)
		{
			*t = x;
		}
		else if (strcmp("y", i->_str->_str) == 0)
		{
			*t = y;
		}
		else if (strcmp("z", i->_str->_str) == 0)
		{
			*t = z;
		}
		else
		{
			*pCode = -1;
		}
	}
	else
	{
		*pCode = -1;
	}
}

void calcWolker_v4(float x, float y, float z, mpfr_t* t, Item* item, int* pCode)
{
	if (item->_type == ITEM_Number)
	{
		ItemNumber* i = (ItemNumber*)item;

		mpfr_set_str(*t, i->_str->_str, 10, MY_MPFR_RND);
	}
	else if (item->_type == ITEM_Add)
	{
		mpfr_t a1, a2;
		mpfr_init2(a1, MPFR_PRECISION);
		mpfr_init2(a2, MPFR_PRECISION);

		calcWolker_v4(x, y, z,&a1, item->_left, pCode);
		calcWolker_v4(x, y, z,&a2, item->_right, pCode);

		mpfr_add(*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Sub)
	{
		mpfr_t a1, a2;
		mpfr_init2(a1, MPFR_PRECISION);
		mpfr_init2(a2, MPFR_PRECISION);

		calcWolker_v4(x, y, z,&a1, item->_left, pCode);
		calcWolker_v4(x, y, z,&a2, item->_right, pCode);

		mpfr_sub(*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Mult)
	{
		mpfr_t a1, a2;
		mpfr_init2(a1, MPFR_PRECISION);
		mpfr_init2(a2, MPFR_PRECISION);

		calcWolker_v4(x, y, z,&a1, item->_left, pCode);
		calcWolker_v4(x, y, z,&a2, item->_right, pCode);

		mpfr_mul(*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Frac)
	{
		mpfr_t a1, a2;
		mpfr_init2(a1, MPFR_PRECISION);
		mpfr_init2(a2, MPFR_PRECISION);

		calcWolker_v4(x, y, z,&a1, item->_left, pCode);
		calcWolker_v4(x, y, z,&a2, item->_right, pCode);

		mpfr_div(*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Sign)
	{
		ItemSign* i = (ItemSign*)item;

		mpfr_t a;
		mpfr_init2(a, MPFR_PRECISION);

		if (i->_sgn == L'-')
		{
			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_neg(*t, a, MY_MPFR_RND);
		}
		else
		{
			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_set(*t, a, MY_MPFR_RND);
		}

		mpfr_clear(a);
	}
	else if (item->_type == ITEM_Pow)
	{
		mpfr_t a1, a2;
		mpfr_init2(a1, MPFR_PRECISION);
		mpfr_init2(a2, MPFR_PRECISION);

		calcWolker_v4(x, y, z,&a1, item->_left, pCode);
		calcWolker_v4(x, y, z,&a2, item->_right, pCode);

		mpfr_pow(*t, a1, a2, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
	}
	else if (item->_type == ITEM_Factorial)
	{
		mpfr_t a;
		mpfr_init2(a, MPFR_PRECISION);

		calcWolker_v4(x, y, z,&a, item->_left, pCode);
		mpfr_fac_ui(*t, mpfr_get_si(a, MY_MPFR_RND), MY_MPFR_RND);

		mpfr_clear(a);
	}
	else if (item->_type == ITEM_Root)
	{
		mpfr_t a1, a2, a3;
		mpfr_init2(a1, MPFR_PRECISION);
		mpfr_init2(a2, MPFR_PRECISION);
		mpfr_init2(a3, MPFR_PRECISION);

		calcWolker_v4(x, y, z,&a1, item->_left, pCode);
		calcWolker_v4(x, y, z,&a2, item->_right, pCode);

		mpfr_set_si(a3, 1, MY_MPFR_RND);
		mpfr_div(a3, a3, a2, MY_MPFR_RND);

		mpfr_pow(*t, a1, a3, MY_MPFR_RND);

		mpfr_clear(a1);
		mpfr_clear(a2);
		mpfr_clear(a3);
	}
	else if (item->_type == ITEM_Triangle)
	{
		ItemTriangle* i = (ItemTriangle*)item;

		if (_stricmp("sin", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2(a, MPFR_PRECISION);

			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_sin(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("cos", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2(a, MPFR_PRECISION);

			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_cos(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("tan", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2(a, MPFR_PRECISION);

			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_tan(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("log", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2(a, MPFR_PRECISION);

			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_log10(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("exp", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2(a, MPFR_PRECISION);

			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_exp(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("asin", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2(a, MPFR_PRECISION);

			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_asin(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("acos", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2(a, MPFR_PRECISION);

			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_acos(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("atan", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2(a, MPFR_PRECISION);

			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_atan(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
		else if (_stricmp("ln", i->_str->_str) == 0)
		{
			mpfr_t a;
			mpfr_init2(a, MPFR_PRECISION);

			calcWolker_v4(x, y, z,&a, item->_left, pCode);
			mpfr_log(*t, a, MY_MPFR_RND);

			mpfr_clear(a);
		}
	}
	else if (item->_type == ITEM_Parentheses)
	{
		calcWolker_v4(x, y, z,t, item->_left, pCode);
	}
	else if (item->_type == ITEM_Literal)
	{
		ItemLiteral* i = (ItemLiteral*)item;

		if (_stricmp("pi", i->_str->_str) == 0)
		{
			mpfr_const_pi(*t, MY_MPFR_RND);
		}
		else if (strcmp("e", i->_str->_str) == 0)
		{
			mpfr_set_str(*t, "2.718281828459045235360287471352", 10, MY_MPFR_RND);
		}
		else if (strcmp("x", i->_str->_str) == 0)
		{
			mpfr_set_flt(*t, x, MY_MPFR_RND);
		}
		else if (strcmp("y", i->_str->_str) == 0)
		{
			mpfr_set_flt(*t, y, MY_MPFR_RND);
		}
		else if (strcmp("z", i->_str->_str) == 0)
		{
			mpfr_set_flt(*t, z, MY_MPFR_RND);
		}
		else
		{
			*pCode = -1;
		}
	}
	else
	{
		*pCode = -1;
	}
}

void calculate_v3(float x, float y, float z, float* t, Item** nodes, int* pCode)
{
	*pCode = 0;
	calcWolker_v3(x, y, z, t, *nodes, pCode);
}

void calculate_v4(float x, float y, float z, mpfr_t* t, Item** nodes, int* pCode)
{
	*pCode = 0;
	calcWolker_v4(x, y, z, t, *nodes, pCode);
}

