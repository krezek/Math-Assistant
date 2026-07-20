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
//    primary : func "(" expr ")" | "(" expr ")" | number | literal
//    func : "Root" | CommFunc

static int list(MParser* pp, void** pItems, TokensQueue* tokens);
static int equ(MParser* pp, void** pItems, TokensQueue* tokens);
static int expr(MParser* pp, void** pItems, TokensQueue* tokens);
static int term(MParser* pp, void** pItems, TokensQueue* tokens);
static int factor(MParser* pp, void** pItems, TokensQueue* tokens);
static int factorial(MParser* pp, void** pItems, TokensQueue* tokens);
static int power(MParser* pp, void** pItems, TokensQueue* tokens);
static int subscript(MParser* pp, void** pItems, TokensQueue* tokens);
static int primary(MParser* pp, void** pItems, TokensQueue* tokens);
static int func(MParser* pp, void** pItems, TokensQueue* tokens);
static int number(MParser* pp, void** pItems, TokensQueue* tokens);
static int literal(MParser* pp, void** pItems, TokensQueue* tokens);

int MParser_do(MParser* pp, void** pItems, const char* s)
{
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

	rs = list(pp, &nodes, q);

#ifdef _DEBUG
	if(rs)
		wprintf(L"%s\n", g_math_parser_err_msg);
#endif

	*pItems = nodes;

	tokensQueue_free(q);

	return rs;
}

//    list : equ {(",") equ} .
static int list(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs;
	void* node = NULL;

	rs = equ(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, ","))
			{
				tok = tokensQueue_dequeue(tokens);
				token_free(tok);

				void* r_node = NULL;
				rs = equ(pp, &r_node, tokens);
				node = pp->_listFunc(PROC_LIST, node, r_node);
				if (rs)
					break;
				
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
		}
	}

	*pItems = node;

	return rs;
}

//    equ : expr {("=") expr} .
static int equ(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs;
	void* node = NULL;

	rs = expr(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			Token* ntok = tokensQueue_next(tokens);

			if (accept_tok(tok, OPERATOR, "=") |
				accept_tok(tok, OPERATOR, "<") | 
				accept_tok(tok, OPERATOR, ">") |
				accept_tok(tok, OPERATOR, "!"))
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
						node = pp->_equFunc(PROC_EQU, node, r_node, tok->_str[0], true);

						token_free(tok);
						token_free(ntok);
					}
					else
					{
						swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"token expected ...");
						return -1;
					}
				}
				else if (ntok &&
					accept_tok(ntok, OPERATOR, "="))
				{
					tok = tokensQueue_dequeue(tokens);
					ntok = tokensQueue_dequeue(tokens);

					void* r_node = NULL;
					rs = expr(pp, &r_node, tokens);
					node = pp->_equFunc(PROC_EQU, node, r_node, tok->_str[0], true);
					
					token_free(tok);
					token_free(ntok);
				}
				else
				{
					tok = tokensQueue_dequeue(tokens);

					void* r_node = NULL;
					rs = expr(pp, &r_node, tokens);
					node = pp->_equFunc(PROC_EQU, node, r_node, tok->_str[0], false);

					token_free(tok);
				}
				
			}
		}
	}

	*pItems = node;

	return rs;
}

//    expr : term {("+"|"-") term} .
static int expr(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs;
	void* node = NULL;

	rs = term(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, "+") ||
				accept_tok(tok, OPERATOR, "-"))
			{
				tok = tokensQueue_dequeue(tokens);
				if (strcmp("+", tok->_str) == 0)
				{
					void* l_node = NULL;
					rs = term(pp, &l_node, tokens);
					node = pp->_addFunc(PROC_EXPR, node, l_node);
					if (rs)
					{
						token_free(tok);
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
						token_free(tok);
						break;
					}
				}

				token_free(tok);
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
		}
	}

	*pItems = node;

	return rs;
}

//    term : factor {("*"|"/"|"%") factor} .
static int term(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs;
	void* node = NULL;

	rs = factor(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, "*") ||
				accept_tok(tok, OPERATOR, "/"))
			{
				tok = tokensQueue_dequeue(tokens);
				if ((strcmp("*", tok->_str) == 0))
				{
					void* l_node = NULL;
					rs = factor(pp, &l_node, tokens);
					node = pp->_multFunc(PROC_TERM, node, l_node);
					if (rs)
					{
						token_free(tok);
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
						token_free(tok);
						break;
					}
				}

				token_free(tok);
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
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
	int rs;

	if (!tokensQueue_empty(tokens))
	{
		tok = tokensQueue_front(tokens);
		if (accept_tok(tok, OPERATOR, "+") ||
			accept_tok(tok, OPERATOR, "-"))
		{
			signToken = tokensQueue_dequeue(tokens);
		}
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
	int rs;

	rs = power(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			Token* ntok = tokensQueue_next(tokens);

			if (accept_tok(tok, OPERATOR, "!"))
			{
				if (!ntok || (ntok && !accept_tok(ntok, OPERATOR, "=")))
				{
					tok = tokensQueue_dequeue(tokens);
					node = pp->_factorialFunc(PROC_FACTORIAL, node);
					token_free(tok);
				}
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
	int rs;
	StackNode* root = NULL;

	rs = subscript(pp, &node, tokens);

	if (!rs)
	{
		stack_push(&root, node);

		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, "^"))
			{
				tok = tokensQueue_dequeue(tokens);

				void* l_node = NULL;
				rs = subscript(pp, &l_node, tokens);
				stack_push(&root, l_node);
				if (rs)
				{
					token_free(tok);
					break;
				}

				token_free(tok);
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
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
	int rs;
	StackNode* root = NULL;

	rs = primary(pp, &node, tokens);

	if (!rs)
	{
		stack_push(&root, node);

		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, "_"))
			{
				tok = tokensQueue_dequeue(tokens);

				void* l_node = NULL;
				rs = primary(pp, &l_node, tokens);
				stack_push(&root, l_node);
				if (rs)
				{
					token_free(tok);
					break;
				}

				token_free(tok);
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
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

//    primary : func "(" expr ")" | "(" expr ")" | number | literal
static int primary(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = -1;

	if (!tokensQueue_empty(tokens))
	{
		Token* tok = tokensQueue_front(tokens);
		Token* ntok = tokensQueue_next(tokens);

		if (tok->_typ == LITERAL && ntok && 
			(accept_tok(ntok, PARENTHESE, "(") || accept_tok(ntok, PARENTHESE, "[") || accept_tok(ntok, PARENTHESE, "{")))
		{
			rs = func(pp, &node, tokens);
			*pItems = node;
			return rs;
		}
		else if (accept_tok(tok, PARENTHESE, "(")) // (expr)
		{
			tok = tokensQueue_dequeue(tokens);
			token_free(tok);

			rs = expr(pp, &node, tokens);
			*pItems = node;

			if (!rs)
			{
				if (tokensQueue_empty(tokens)) // expect ")"
				{
					swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"parenthese expected ...");
					return -1;
				}

				tok = tokensQueue_front(tokens);
				if (!accept_tok(tok, PARENTHESE, ")"))
				{
					swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"parenthese expected ...");
					return -1;
				}

				*pItems = pp->_parenthesesFunc(PROC_PRIMARY_1, *pItems);

				tok = tokensQueue_dequeue(tokens);
				token_free(tok);
			}

			return rs;
		}
		else if (tok->_typ == NUMBER) // Number
		{
			rs = number(pp, &node, tokens);
			*pItems = node;
			return rs;
		}
		else if (tok->_typ == LITERAL) // Literal
		{
			rs = literal(pp, &node, tokens);
			*pItems = node;
			return rs;
		}
		else
		{
			swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"token expected ...");
			rs = -1;
		}
	}
	else
	{
		swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"token expected ...");
		rs = -1;
	}

	*pItems = node;

	return rs;
}

//    func : "Root" | CommFunc
static int func(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = -1;

	Token* tok_func_name = tokensQueue_dequeue(tokens); // Function name

	Token* tok_par = tokensQueue_dequeue(tokens); // Parenthes
	token_free(tok_par);

	if (accept_tok(tok_func_name, LITERAL, "Root"))
	{
		rs = expr(pp, &node, tokens);
		
		if (!rs)
		{
			void* bNode = NULL;

			if (!tokensQueue_empty(tokens))
			{
				Token* bTok = tokensQueue_front(tokens);
				if (accept_tok(bTok, OPERATOR, ";"))
				{
					bTok = tokensQueue_dequeue(tokens);
					token_free(bTok);

					rs = list(pp, &bNode, tokens);
				}

				node = pp->_rootFunc(PROC_PRIMARY_2, node, bNode);
			}
		}

		*pItems = node;
	}
	else
	{
		void* rNode = NULL;

		rs = expr(pp, &node, tokens);
		if (!rs)
		{
			if (!tokensQueue_empty(tokens))
			{
				Token* rTok = tokensQueue_front(tokens);
				if (accept_tok(rTok, OPERATOR, ";"))
				{
					rTok = tokensQueue_dequeue(tokens);
					token_free(rTok);

					rs = expr(pp, &rNode, tokens);
					if (!rs)
					{
						swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"expr expected ...");
						return -1;
					}
				}
			}
		}

		node = pp->_commonFunc(PROC_PRIMARY_3, node, rNode, tok_func_name->_str);
		*pItems = node;
	}

	if (tokensQueue_empty(tokens)) // expect "Parenthes"
	{
		swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"parenthese expected ...");
		token_free(tok_func_name);
		return -1;
	}

	tok_par = tokensQueue_front(tokens);
	if (!accept_tok(tok_par, PARENTHESE, ")") &&
		!accept_tok(tok_par, PARENTHESE, "]") &&
		!accept_tok(tok_par, PARENTHESE, "}"))
	{
		swprintf(g_math_parser_err_msg, MAX_ERROR_MSG, L"parenthese expected ...");
		token_free(tok_func_name);
		return -1;
	}

	tok_par = tokensQueue_dequeue(tokens); // Parenthes
	token_free(tok_par);

	token_free(tok_func_name);

	return rs;
}

//    number .
static int number(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = -1;

	Token* tok = tokensQueue_dequeue(tokens);
	node = pp->_numberFunc(PROC_PRIMARY_4, tok->_str);
	token_free(tok);

	*pItems = node;
	return (rs = 0);
}

//    literal .
static int literal(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = -1;

	Token* tok = tokensQueue_dequeue(tokens);
	node = pp->_literalFunc(PROC_PRIMARY_5, tok->_str);
	token_free(tok);

	*pItems = node;
	return (rs = 0);
}
