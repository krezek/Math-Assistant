#ifndef _MATH_PARSER_H_
#define _MATH_PARSER_H_

typedef struct _Parser Parser;

typedef void* (*PListFunc) (int pl, void* v, void* v1);
typedef void* (*PEquFunc) (int pl, void* v, void* v1, const char c, bool concat);
typedef void* (*PAddFunc) (int pl, void* v, void* v1);
typedef void* (*PSubFunc) (int pl, void* v, void* v1);
typedef void* (*PFracFunc) (int pl, void* v, void* v1);
typedef void* (*PMultFunc) (int pl, void* v, void* v1);
typedef void* (*PSignFunc) (int pl, void* v, const char c);
typedef void* (*PFactorialFunc) (int pl, void* v);
typedef void* (*PPowerFunc) (int pl, void* v, void* v1);
typedef void* (*PSubscriptFunc) (int pl, void* v, void* v1);
typedef void* (*PParenthesesFunc) (int pl, void* v);
typedef void* (*PCommonFunc) (int pl, void* v, void* v1, const char* s);
typedef void* (*PRootFunc) (int pl, void* v, void* v1);
typedef void* (*PNumberFunc) (int pl, const char* s);
typedef void* (*PLiteralFunc) (int pl, const char* s);

typedef struct _MParser
{
	PListFunc _listFunc;
	PEquFunc _equFunc;
	PAddFunc _addFunc;
	PSubFunc _subFunc;
	PFracFunc _fracFunc;
	PMultFunc _multFunc;
	PSignFunc _signFunc;
	PFactorialFunc _factorialFunc;
	PPowerFunc _powerFunc;
	PSubscriptFunc _subscriptFunc;
	PParenthesesFunc _parenthesesFunc;
	PCommonFunc _commonFunc;
	PRootFunc _rootFunc;
	PNumberFunc _numberFunc;
	PLiteralFunc _literalFunc;
} MParser;

MParser* MParser_init();
void MParser_free(MParser* mp);

int MParser_do(MParser* pp, void** pgItems, const char* s);
const wchar_t* MParser_get_last_error();

#endif /* _MATH_PARSER_H_ */

