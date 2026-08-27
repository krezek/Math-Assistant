#ifndef _LEXER_H_
#define _LEXER_H_

#include <stdbool.h>

typedef enum { UNDEFINED = 0, NUMBER, LITERAL, OPERATOR, PARENTHESE } TokenTyp;

typedef struct
{
	int _index;
	TokenTyp _typ;
	char* _str;
} Token;

typedef struct _TNode
{
	Token* _val;
	struct _TNode* _next;
} TNode;

typedef struct
{
	TNode* _front, * _rear;
} TokensQueue;

void token_free(Token* tok);
void token_print(const Token* tok);

TokensQueue* tokensQueue_init();
Token* tokensQueue_dequeue(TokensQueue* q);
Token* tokensQueue_front(TokensQueue* q);
Token* tokensQueue_next(TokensQueue* q);
bool tokensQueue_empty(TokensQueue* q);
void tokensQueue_print(TokensQueue* q);
void tokensQueue_free(TokensQueue* q);

int lexer(const char* expr, TokensQueue* queue);

bool accept_tok(Token* tok, TokenTyp typ, const char* str);

#endif /* _LEXER_H_ */

