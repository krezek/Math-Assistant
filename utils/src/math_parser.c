/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <proc.h>
#include <math_parser.h>
#include <lexer.h>

#define MAX_ERROR_MSG 255
wchar_t g_math_parser_err_msg[MAX_ERROR_MSG];

const wchar_t* MParser_get_last_error()
{
	return g_math_parser_err_msg;
}

MParser* MParser_init()
{
	MParser* mp = (MParser*)malloc(sizeof(MParser));
	assert(mp != NULL);

	return mp;
}

void MParser_free(MParser* mp)
{
	free(mp);
}

///////////////////////// Stack //////////////////////////

typedef struct _StackNode
{
	void* _data;
	struct _StackNode* _next;
} StackNode;

static StackNode* stackNode_init(void* d)
{
	StackNode* stackNode = (StackNode*)malloc(sizeof(StackNode));
	stackNode->_data = d;
	stackNode->_next = NULL;
	return stackNode;
}

static bool stack_isEmpty(StackNode* root)
{
	return !root;
}

static void stack_push(StackNode** root, void* data)
{
	StackNode* stackNode = stackNode_init(data);
	stackNode->_next = *root;
	*root = stackNode;
}

static void* stack_pop(StackNode** root)
{
	if (stack_isEmpty(*root))
		return NULL;
	StackNode* temp = *root;
	*root = (*root)->_next;
	void* popped = temp->_data;
	free(temp);

	return popped;
}

///////////////////////// Stack end //////////////////////

//    list : equ {(",") equ} .
//    equ : expr {("=") expr} .
//    expr : term {("+"|"-") term} .
//    term : factor {("*"|"/"|"%") factor} .
//    factor: "-" factorial | "+" factorial | factorial .
//    factorial : power "!" | power
//    power : subscript { "^" subscript } .
//    subscript : primary { "_" primary } .
//    recursion : "(" expr ")" .
//    func :  func_b "(" expr ")" func_e .
//    primary : recursion | func | number | literal

static int list(MParser* pp, void** pItems, TokensQueue* tokens);
static int equ(MParser* pp, void** pItems, TokensQueue* tokens);
static int expr(MParser* pp, void** pItems, TokensQueue* tokens);
static int term(MParser* pp, void** pItems, TokensQueue* tokens);
static int factor(MParser* pp, void** pItems, TokensQueue* tokens);
static int factorial(MParser* pp, void** pItems, TokensQueue* tokens);
static int power(MParser* pp, void** pItems, TokensQueue* tokens);
static int subscript(MParser* pp, void** pItems, TokensQueue* tokens);
static int recursion(MParser* pp, void** pItems, TokensQueue* tokens);
static int func(MParser* pp, void** pItems, TokensQueue* tokens);
static int number(MParser* pp, void** pItems, TokensQueue* tokens);
static int literal(MParser* pp, void** pItems, TokensQueue* tokens);
static int primary(MParser* pp, void** pItems, TokensQueue* tokens);

static TreeFreeFunc _treeFreeFunc = NULL;

int MParser_do(MParser* pp, void** pItems, const char* s, TreeFreeFunc treeFreeFunc)
{
	_treeFreeFunc = treeFreeFunc;

	void* nodes = NULL;
	int rs = 0;
	TokensQueue* q = tokensQueue_init();

	if (lexer(s, q))
	{
		tokensQueue_free(q);
		return -1;
	}

#ifdef _DEBUG
	tokensQueue_print(q);
	printf("\n");
#endif

	wsprintf(g_math_parser_err_msg, L"");

	rs = list(pp, &nodes, q);

	if (rs < 0 || !tokensQueue_empty(q))
	{
#ifdef _DEBUG
		printf("Parser Error:\n");
#endif
		if(nodes)
			treeFreeFunc(&nodes);
		nodes = NULL;
		rs = -1;
	}

	*pItems = nodes;

	tokensQueue_free(q);

	return rs;
}

//    list : equ {(",") equ} .
static int list(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs = 0;
	void* node = NULL;

	rs = equ(pp, &node, tokens);

	if (!rs)
	{
		Token* tok = tokensQueue_front(tokens);
		while (tok && (accept_tok(tok, OPERATOR, ",") ||
			accept_tok(tok, OPERATOR, ";")))
		{
			char c;
			tok = tokensQueue_dequeue(tokens);
			c = tok->_str[0];
			token_free(tok);

			void* r_node = NULL;
			rs = equ(pp, &r_node, tokens);
			node = pp->_listFunc(PROC_LIST, node, r_node, c);
			if (rs)
			{
				swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"list: token expected ...");
				rs = -1;
				break;
			}
				
			tok = tokensQueue_front(tokens);
		}

		if (tok && !accept_tok(tok, PARENTHESE, ")"))
		{
			swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"list: wrong end ...");
			rs = -1;
		}
	}

	*pItems = node;

	return rs;
}

//    equ : expr {("=") expr} .
static int equ(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs = 0;
	void* node = NULL;

	rs = expr(pp, &node, tokens);

	if (!rs)
	{
		Token* tok = tokensQueue_front(tokens);
		Token* ntok = tokensQueue_next(tokens);

		if (tok && (accept_tok(tok, OPERATOR, "=") |
			accept_tok(tok, OPERATOR, "<") | 
			accept_tok(tok, OPERATOR, ">") |
			accept_tok(tok, OPERATOR, "!")))
		{
			if (accept_tok(tok, OPERATOR, "!"))
			{
				if (ntok &&
					accept_tok(ntok, OPERATOR, "="))
				{
					tok = tokensQueue_dequeue(tokens);
					ntok = tokensQueue_dequeue(tokens);

					void* r_node = NULL;
					rs = expr(pp, &r_node, tokens);
					node = pp->_equFunc(PROC_EQU, node, r_node, "!=");

					token_free(tok);
					token_free(ntok);
				}
				else
				{
					swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"equ: token expected ...");
					rs = -1;
				}
			}
			else if (ntok &&
				accept_tok(ntok, OPERATOR, "="))
			{
				tok = tokensQueue_dequeue(tokens);
				ntok = tokensQueue_dequeue(tokens);

				void* r_node = NULL;
				rs = expr(pp, &r_node, tokens);
				const char opstr[3] = {tok->_str[0], ntok->_str[0], 0};
				node = pp->_equFunc(PROC_EQU, node, r_node, opstr);
					
				token_free(tok);
				token_free(ntok);
			}
			else if (ntok &&
				accept_tok(ntok, OPERATOR, ">"))
			{
				tok = tokensQueue_dequeue(tokens);
				ntok = tokensQueue_dequeue(tokens);

				void* r_node = NULL;
				rs = expr(pp, &r_node, tokens);
				const char opstr[3] = { tok->_str[0], ntok->_str[0], 0 };
				node = pp->_equFunc(PROC_EQU, node, r_node, opstr);

				token_free(tok);
				token_free(ntok);
			}
			else
			{
				tok = tokensQueue_dequeue(tokens);

				void* r_node = NULL;
				rs = expr(pp, &r_node, tokens);
				const char opstr[2] = { tok->_str[0], 0 };
				node = pp->_equFunc(PROC_EQU, node, r_node, opstr);

				token_free(tok);
			}
		}

		tok = tokensQueue_front(tokens);
		if (tok && !(accept_tok(tok, PARENTHESE, ")") ||
			accept_tok(tok, OPERATOR, ",") ||
			accept_tok(tok, OPERATOR, ";")))
		{
			swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"equ: wrong end ...");
			rs = -1;
		}
	}

	*pItems = node;

	return rs;
}

//    expr : term {("+"|"-") term} .
static int expr(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs = 0;
	void* node = NULL;

	rs = term(pp, &node, tokens);

	if (!rs)
	{
		Token* tok = tokensQueue_front(tokens);
		while (tok && (accept_tok(tok, OPERATOR, "+") ||
			accept_tok(tok, OPERATOR, "-")))
		{
			tok = tokensQueue_dequeue(tokens);
			if (strcmp("+", tok->_str) == 0)
			{
				void* l_node = NULL;
				rs = term(pp, &l_node, tokens);
				node = pp->_addFunc(PROC_EXPR, node, l_node);
				if (rs)
				{
					swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"expr: token expected ...");
					token_free(tok);
					rs = -1;
					break;
				}
			}
			else if (strcmp("-", tok->_str) == 0)
			{
				void* l_node = NULL;
				rs = term(pp, &l_node, tokens);
				node = pp->_subFunc(PROC_EXPR, node, l_node);
				if (rs)
				{
					swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"expr: token expected ...");
					token_free(tok);
					rs = -1;
					break;
				}
			}

			token_free(tok);
			tok = tokensQueue_front(tokens);
		}
	}

	*pItems = node;

	return rs;
}

//    term : factor {("*"|"/"|"%") factor} .
static int term(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs = 0;
	void* node = NULL;

	rs = factor(pp, &node, tokens);

	if (!rs)
	{
		Token* tok = tokensQueue_front(tokens);
		while (tok && (accept_tok(tok, OPERATOR, "*") ||
			accept_tok(tok, OPERATOR, "/")))
		{
			tok = tokensQueue_dequeue(tokens);
			if ((strcmp("*", tok->_str) == 0))
			{
				void* l_node = NULL;
				rs = factor(pp, &l_node, tokens);
				node = pp->_multFunc(PROC_TERM, node, l_node);
				if (rs)
				{
					swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"term: token expected ...");
					token_free(tok);
					rs = -1;
					break;
				}
			}
			else if (strcmp("/", tok->_str) == 0)
			{
				void* l_node = NULL;
				rs = factor(pp, &l_node, tokens);
				node = pp->_fracFunc(PROC_TERM, node, l_node);
				if (rs)
				{
					swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"term: token expected ...");
					token_free(tok);
					rs = -1;
					break;
				}
			}

			token_free(tok);
			tok = tokensQueue_front(tokens);
		}
	}

	*pItems = node;

	return rs;
}

//    factor: "-" factorial | "+" factorial | factorial .
static int factor(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	Token* signToken = NULL;
	Token* tok = NULL;
	int rs = 0;

	tok = tokensQueue_front(tokens);
	if (tok && (accept_tok(tok, OPERATOR, "+") ||
		accept_tok(tok, OPERATOR, "-")))
	{
		signToken = tokensQueue_dequeue(tokens);
	}

	rs = factorial(pp, &node, tokens);

	if (signToken)
	{
		node = pp->_signFunc(PROC_FACTOR, node, signToken->_str[0]);
		token_free(signToken);
	}
	
	*pItems = node;

	return rs;
}

//    factorial : power "!" | power
static int factorial(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = 0;

	rs = power(pp, &node, tokens);

	if (!rs)
	{
		Token* tok = tokensQueue_front(tokens);
		Token* ntok = tokensQueue_next(tokens);

		if (tok && accept_tok(tok, OPERATOR, "!"))
		{
			if (!ntok || (ntok && !accept_tok(ntok, OPERATOR, "=")))
			{
				tok = tokensQueue_dequeue(tokens);
				node = pp->_factorialFunc(PROC_FACTORIAL, node);
				token_free(tok);
			}
		}
	}

	*pItems = node;

	return rs;
}

//    power : subscript { "^" subscript } .
static int power(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = 0;
	StackNode* root = NULL;

	rs = subscript(pp, &node, tokens);

	if (!rs)
	{
		stack_push(&root, node);

		Token* tok = tokensQueue_front(tokens);
		while (tok && accept_tok(tok, OPERATOR, "^"))
		{
			tok = tokensQueue_dequeue(tokens);

			void* l_node = NULL;
			rs = subscript(pp, &l_node, tokens);
			stack_push(&root, l_node);
			if (rs)
			{
				swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"power: token expected ...");
				token_free(tok);
				rs = -1;
				break;
			}

			token_free(tok);
			tok = tokensQueue_front(tokens);
		}

		node = stack_pop(&root);

		while (!stack_isEmpty(root))
		{
			void* tmp = stack_pop(&root);
			node = pp->_powerFunc(PROC_POWER, tmp, node);
		}
	}

	*pItems = node;

	return rs;
}

//    subscript : primary { "_" primary } .
static int subscript(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = 0;
	StackNode* root = NULL;

	rs = primary(pp, &node, tokens);

	if (!rs)
	{
		stack_push(&root, node);

		Token* tok = tokensQueue_front(tokens);
		while (tok && accept_tok(tok, OPERATOR, "_"))
		{
			tok = tokensQueue_dequeue(tokens);

			void* l_node = NULL;
			rs = primary(pp, &l_node, tokens);
			stack_push(&root, l_node);
			if (rs)
			{
				swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"subscript: token expected ...");
				token_free(tok);
				rs = -1;
				break;
			}

			token_free(tok);
			tok = tokensQueue_front(tokens);
		}

		node = stack_pop(&root);

		while (!stack_isEmpty(root))
		{
			void* tmp = stack_pop(&root);
			node = pp->_subscriptFunc(PROC_SUBSCRIPT, tmp, node);
		}
	}
	
	*pItems = node;

	return rs;
}

static int recursion(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = 0;

	Token* tok = tokensQueue_front(tokens);
	if (tok && accept_tok(tok, PARENTHESE, "(")) // (list)
	{
		tok = tokensQueue_dequeue(tokens);
		token_free(tok);

		rs = list(pp, &node, tokens);
		
		if (!rs)
		{
			tok = tokensQueue_front(tokens);
			if (!tok || !accept_tok(tok, PARENTHESE, ")"))
			{
				swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"recursion: parenthese expected ...");
				rs = -1;
			}
			else
			{
				node = pp->_parenthesesFunc(PROC_RECURSION, node);

				tok = tokensQueue_dequeue(tokens);
				token_free(tok);

				rs = 1;
			}
		}
	}

	*pItems = node;

	return rs;
}

static int func(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = 0;

	Token* tok = tokensQueue_front(tokens);
	Token* ntok = tokensQueue_next(tokens);

	if (tok && tok->_typ == LITERAL && ntok &&
		(accept_tok(ntok, PARENTHESE, "(")))
	{
		tok = tokensQueue_dequeue(tokens);
		ntok = tokensQueue_dequeue(tokens);

		rs = list(pp, &node, tokens);
		if (!rs)
		{
			Token* ptok = tokensQueue_front(tokens);
			if (!ptok || !accept_tok(ptok, PARENTHESE, ")"))
			{
				swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"func: parenthese expected ...");
				rs = -1;
			}
			else
			{
				ptok = tokensQueue_dequeue(tokens);
				token_free(ptok);

				node = pp->_commonFnFunc(PROC_FUNC, node, tok->_str);
				rs = 1;
			}
		}

		token_free(ntok);
		token_free(tok);
	}

	*pItems = node;

	return rs;
}

//    number .
static int number(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = 0;

	Token* tok = tokensQueue_front(tokens);
	if (tok && (tok->_typ == NUMBER)) // Number
	{
		tok = tokensQueue_dequeue(tokens);
		node = pp->_numberFunc(PROC_NUMBER, tok->_str);
		token_free(tok);

		*pItems = node;
		rs = 1;
	}

	return rs;
}

//    literal .
static int literal(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = 0;

	Token* tok = tokensQueue_front(tokens);
	if (tok && (tok->_typ == LITERAL)) // Literal
	{
		tok = tokensQueue_dequeue(tokens);
		node = pp->_literalFunc(PROC_LITERAL, tok->_str);
		token_free(tok);

		*pItems = node;
		rs = 1;
	}

	return rs;
}


//    primary : recursion | func | number | literal
static int primary(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = 0;

	rs = recursion(pp, &node, tokens);
	if (rs > 0)
	{
		*pItems = node;
		return rs >= 0 ? 0 : rs;
	}
	else
	{
		if (node)
			_treeFreeFunc(&node);
		node = NULL;
	}

	rs = func(pp, &node, tokens);
	if (rs > 0)
	{
		*pItems = node;
		return rs >= 0 ? 0 : rs;
	}
	else
	{
		if (node)
			_treeFreeFunc(&node);
		node = NULL;
	}

	rs = literal(pp, &node, tokens);
	if (rs > 0)
	{
		*pItems = node;
		return rs >= 0 ? 0 : rs;
	}
	else
	{
		if (node)
			_treeFreeFunc(&node);
		node = NULL;
	}


	rs = number(pp, &node, tokens);
	if (rs > 0)
	{
		*pItems = node;
		return rs >= 0 ? 0 : rs;
	}
	else
	{
		if (node)
			_treeFreeFunc(&node);
		node = NULL;
	}

	*pItems = node;
	return -1;
}
