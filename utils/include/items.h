#ifndef _ITEMS_H_
#define _ITEMS_H_

#include <strg.h>

typedef enum {ITEM_Base = 0, ITEM_Literal, ITEM_Number, ITEM_Rational, 
				ITEM_List, ITEM_Equ, ITEM_Add, ITEM_Sub, ITEM_Mult, ITEM_Frac,
				ITEM_Sign, ITEM_Pow, ITEM_Subscript, ITEM_Factorial, ITEM_Parentheses, ITEM_CommFunc, ITEM_Root
} ItemType;

typedef struct _Item Item;

typedef void (*destroyFunc) (Item* _this);
typedef void (*toStringItemFunc) (int pl, Item* _this, String* s);
typedef void (*getRuleFunc) (int pl, Item* _this, String* s);

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
} ItemList;

typedef struct
{
	Item _item;
	char _sy;
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

	char* _str;
	size_t _len;
} ItemCommFunc;

typedef struct
{
	Item _item;

	int _sHeight;
	int _sPadding;
} ItemRoot;

ItemList* ItemList_init(Item* l, Item* r);

ItemLiteral* ItemLiteral_init(int pl, const char* s);
ItemNumber* ItemNumber_init(int pl, const char* s);

ItemAdd* ItemAdd_init(int pl, Item* l, Item* r);
ItemSub* ItemSub_init(int pl, Item* l, Item* r);

ItemMult* ItemMult_init(int pl, Item* l, Item* r);
ItemFrac* ItemFrac_init(int pl, Item* l, Item* r);

ItemSign* ItemSign_init(int pl, Item* l, const char sgn);

ItemRoot* ItemRoot_init(int pl, Item* l, Item* r);
ItemPow* ItemPow_init(int pl, Item* l, Item* r);
ItemSubscript* ItemSubscript_init(int pl, Item* l, Item* r);
ItemCommFunc* ItemCommFunc_init(int pl, Item* l, Item* r, const char* s);

ItemFactorial* ItemFactorial_init(int pl, Item* l);

bool Item_isLeaf(Item* i);
void ItemTree_free(Item** Item);
void get_level(int* pl, Item* pItems);

#endif /* _ITEMS_H_ */

