#ifndef _ITEMS_H_
#define _ITEMS_H_

#include <strg.h>

typedef enum {ITEM_Base = 0, ITEM_Literal, ITEM_Number, ITEM_Rational, 
				ITEM_List, ITEM_Equ, ITEM_Add, ITEM_Sub, ITEM_Mult, ITEM_Frac,
				ITEM_Sign, ITEM_Pow, ITEM_Subscript, ITEM_Factorial, ITEM_Parentheses, 
				ITEM_Triangle, ITEM_Root, ITEM_Integral, ITEM_Derivative, ITEM_Limit
} ItemType;

typedef struct _Item Item;
typedef struct _RuleQueue RuleQueue;

typedef void (*destroyFunc) (Item* _this);
typedef void (*toStringItemFunc) (int pl, Item* _this, String* s);
typedef void (*getRuleFunc) (int pl, Item* _this, RuleQueue* rq);

typedef struct
{
	String* _str;
} Rule;

typedef struct _RNode
{
	Rule* _val;
	struct _RNode* _next;
} RNode;

typedef struct _RuleQueue
{
	RNode* _front, * _rear;
} RuleQueue;

typedef struct _Item
{
	struct _Item* _left;
	struct _Item* _right;

	destroyFunc _destroyFunc;
	toStringItemFunc _toStringFunc;
	getRuleFunc _getRuleFunc;

	ItemType _type;
	int _proc_level;

} Item;

typedef struct
{
	Item _item;

	String* _str;
} ItemLiteral;

typedef struct
{
	Item _item;

	String* _str;
} ItemNumber;

typedef struct
{
	Item _item;

	char _colon;
} ItemList;

typedef struct
{
	Item _item;
	String* _operator;
} ItemEqu;

typedef struct
{
	Item _item;
} ItemAdd;

typedef struct
{
	Item _item;
} ItemSub;

typedef struct
{
	Item _item;
} ItemMult;

typedef struct
{
	Item _item;

	int _fHeight;
} ItemFrac;

typedef struct
{
	Item _item;

	char _sgn;
} ItemSign;

typedef struct
{
	Item _item;
} ItemPow;

typedef struct
{
	Item _item;
} ItemSubscript;

typedef struct
{
	Item _item;
} ItemFactorial;

typedef struct
{
	Item _item;

	String* _str;
} ItemTriangle;

typedef struct
{
	Item _item;

	int _sHeight;
	int _sPadding;
} ItemRoot;

typedef struct
{
	Item _item;

} ItemIntegral;

typedef struct
{
	Item _item;

} ItemDerivative;

typedef struct
{
	Item _item;

} ItemLimit;

ItemList* ItemList_init(Item* l, Item* r, const char c);
ItemEqu* ItemEqu_init(Item* l, Item* r, const char* op);

ItemLiteral* ItemLiteral_init(int pl, const char* s);
ItemNumber* ItemNumber_init(int pl, const char* s);

ItemAdd* ItemAdd_init(int pl, Item* l, Item* r);
ItemSub* ItemSub_init(int pl, Item* l, Item* r);

ItemMult* ItemMult_init(int pl, Item* l, Item* r);
ItemFrac* ItemFrac_init(int pl, Item* l, Item* r);

ItemSign* ItemSign_init(int pl, Item* l, const char sgn);
ItemFactorial* ItemFactorial_init(int pl, Item* l);

ItemPow* ItemPow_init(int pl, Item* l, Item* r);
ItemSubscript* ItemSubscript_init(int pl, Item* l, Item* r);

ItemTriangle* ItemTriangle_init(int pl, Item* l, Item* r, const char* s);
ItemRoot* ItemRoot_init(int pl, Item* l, Item* r);
ItemIntegral* ItemIntegral_init(int pl, Item* l, Item* r);
ItemDerivative* ItemDerivative_init(int pl, Item* l, Item* r);
ItemLimit* ItemLimit_init(int pl, Item* l, Item* r);

bool Item_isLeaf(Item* i);
void ItemTree_free(Item** Item);

RuleQueue* RuleQueue_init();
void RuleQueue_free(RuleQueue* q);

#endif /* _ITEMS_H_ */

