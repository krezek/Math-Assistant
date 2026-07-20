#include "pch.h"
#include "platform.h"

#include <proc.h>
#include <strg.h>
#include <items.h>

// Item

void Item_destroy(Item* i)
{

}

bool Item_isLeaf(Item* i)
{
	if ((i->_left == NULL) && (i->_right == NULL))
		return true;
	else
		return false;
}

void Item_toString(int pl, Item* __this, String* s)
{
	String_cpy(s, "");
}

void Item_getRule(int rl, Item* __this, String* s)
{
	String_cpy(s, "");
}

Item* Item_init(Item* l, Item* r)
{
	Item* i = (Item*)malloc(sizeof(Item));
	assert(i != 0);
	
	i->_left = l;
	i->_right = r;

	i->_destroyFunc = Item_destroy;
	i->_toStringFunc = Item_toString;
	i->_getRuleFunc = Item_getRule;

	i->_type = ITEM_Base;

	return i;
}

// Literal

void ItemLiteral_destroy(Item* _this)
{
	ItemLiteral* i = (ItemLiteral*)_this;

	String_free(i->_str);
}

void ItemLiteral_toString(int pl, Item* _this, String* s)
{
	ItemLiteral* i = (ItemLiteral*)_this;

	String_cpy(s, i->_str->_str);
}

void ItemLiteral_getRule(int rl, Item* _this, String* s)
{
	ItemLiteral* i = (ItemLiteral*)_this;

	String_cpy(s, "L");
}

ItemLiteral* ItemLiteral_init(int pl, const char* s)
{
	ItemLiteral* i = (ItemLiteral*)malloc(sizeof(ItemLiteral));
	assert(i != 0);

	i->_item._left = NULL;
	i->_item._right = NULL;

	i->_item._destroyFunc = ItemLiteral_destroy;
	i->_item._toStringFunc = ItemLiteral_toString;
	i->_item._getRuleFunc = ItemLiteral_getRule;

	i->_item._type = ITEM_Literal;
	i->_item._proc_level = pl;

	i->_str = String_init();
	
	String_cpy(i->_str, s);
	
	return i;
}

// Number

void ItemNumber_destroy(Item* _this)
{
	ItemNumber* i = (ItemNumber*)_this;

	String_free(i->_str);
}

void ItemNumber_toString(int pl, Item* _this, String* s)
{
	ItemNumber* i = (ItemNumber*)_this;

	String_cpy(s, i->_str->_str);
}

void ItemNumber_getRule(int rl, Item* _this, String* s)
{
	ItemNumber* i = (ItemNumber*)_this;

	if(strchr(i->_str->_str, '.'))
		String_cpy(s, "D");
	else
		String_cpy(s, "I");
}

ItemNumber* ItemNumber_init(int pl, const char* s)
{
	ItemNumber* i = (ItemNumber*)malloc(sizeof(ItemNumber));
	assert(i != 0);

	i->_item._left = NULL;
	i->_item._right = NULL;

	i->_item._destroyFunc = ItemNumber_destroy;
	i->_item._toStringFunc = ItemNumber_toString;
	i->_item._getRuleFunc = ItemNumber_getRule;

	i->_item._type = ITEM_Number;
	i->_item._proc_level = pl;

	i->_str = String_init();
	String_cpy(i->_str, s);

	return i;
}

// List

void ItemList_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "[");
	String_cat(s, s1->_str);
	String_cat(s, ",");
	String_cat(s, s2->_str);
	String_cat(s, "]");

	String_free(s1);
	String_free(s2);
}

void ItemList_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);
	_this->_right->_getRuleFunc(rl - 1, _this->_right, s2);

	String_cpy(s, "[");
	String_cat(s, s1->_str);
	String_cat(s, ",");
	String_cat(s, s2->_str);
	String_cat(s, "]");

	String_free(s1);
	String_free(s2);
}

ItemList* ItemList_init(Item* l, Item* r)
{
	ItemList* i = (ItemList*)malloc(sizeof(ItemList));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemList_toString;
	i->_item._getRuleFunc = ItemList_getRule;

	i->_item._type = ITEM_List;

	return i;
}

// Equ

void ItemEqu_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "[");
	String_cat(s, s1->_str);
	String_cat(s, "=");
	String_cat(s, s2->_str);
	String_cat(s, "]");

	String_free(s1);
	String_free(s2);
}

void ItemEqu_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);
	_this->_right->_getRuleFunc(rl - 1, _this->_right, s2);

	String_cpy(s, "[");
	String_cat(s, s1->_str);
	String_cat(s, "=");
	String_cat(s, s2->_str);
	String_cat(s, "]");

	String_free(s1);
	String_free(s2);
}

ItemEqu* ItemEqu_init(Item* l, Item* r, char sy)
{
	ItemEqu* i = (ItemEqu*)malloc(sizeof(ItemEqu));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemEqu_toString;
	i->_item._getRuleFunc = ItemEqu_getRule;

	i->_item._type = ITEM_Equ;

	i->_sy = sy;

	return i;
}

ItemEqu* ItemEqu_init_eq(Item* l, Item* r)
{
	return ItemEqu_init(l, r, '=');
}

/*ItemEqu* ItemEqu_init_nq(Item* l, Item* r)
{
	return ItemEqu_init(l, r, '\u2260');
}*/

ItemEqu* ItemEqu_init_g(Item* l, Item* r)
{
	return ItemEqu_init(l, r, '>');
}

ItemEqu* ItemEqu_init_l(Item* l, Item* r)
{
	return ItemEqu_init(l, r, '<');
}

/*ItemEqu* ItemEqu_init_eg(Item* l, Item* r)
{
	return ItemEqu_init(l, r, '\u2265');
}*/

/*ItemEqu* ItemEqu_init_el(Item* l, Item* r)
{
	return ItemEqu_init(l, r, '\u2264');
}*/

// Add

void ItemAdd_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, "+");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemAdd_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);
	_this->_right->_getRuleFunc(rl - 1, _this->_right, s2);
	
	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, "+");
	String_cat(s, s2->_str);
	String_cat(s, ")");
	
	String_free(s1);
	String_free(s2);
}

ItemAdd* ItemAdd_init(int pl, Item* l, Item* r)
{
	ItemAdd* i = (ItemAdd*)malloc(sizeof(ItemAdd));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemAdd_toString;
	i->_item._getRuleFunc = ItemAdd_getRule;

	i->_item._type = ITEM_Add;
	i->_item._proc_level = pl;

	return i;
}

// Sub

void ItemSub_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, "-");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemSub_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);
	_this->_right->_getRuleFunc(rl - 1, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, "-");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

ItemSub* ItemSub_init(int pl, Item* l, Item* r)
{
	ItemSub* i = (ItemSub*)malloc(sizeof(ItemSub));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemSub_toString;
	i->_item._getRuleFunc = ItemSub_getRule;

	i->_item._type = ITEM_Sub;
	i->_item._proc_level = pl;

	return i;
}

// Mult

void ItemMult_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, "*");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemMult_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);
	_this->_right->_getRuleFunc(rl - 1, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, "*");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

ItemMult* ItemMult_init(int pl, Item* l, Item* r)
{
	ItemMult* i = (ItemMult*)malloc(sizeof(ItemMult));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemMult_toString;
	i->_item._getRuleFunc = ItemMult_getRule;

	i->_item._type = ITEM_Mult;
	i->_item._proc_level = pl;

	return i;
}

// Frac

void ItemFrac_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, "/");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemFrac_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);
	_this->_right->_getRuleFunc(rl - 1, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, "/");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

ItemFrac* ItemFrac_init(int pl, Item* l, Item* r)
{
	ItemFrac* i = (ItemFrac*)malloc(sizeof(ItemFrac));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemFrac_toString;
	i->_item._getRuleFunc = ItemFrac_getRule;

	i->_item._type = ITEM_Frac;
	i->_item._proc_level = pl;

	i->_fHeight = 8;

	return i;
}

// Sign

void ItemSign_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();

	ItemSign* i = (ItemSign*)_this; 

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);

	String_cpy(s, "(");
	if(i->_sgn == '-')
		String_cat(s, "-");
	String_cat(s, s1->_str);
	String_cat(s, ")");

	String_free(s1);
}

void ItemSign_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();

	ItemSign* i = (ItemSign*)_this;

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);

	String_cpy(s, "(");
	if (i->_sgn == '-')
		String_cat(s, "-");
	if (i->_sgn == '+')
		String_cat(s, "+");
	String_cat(s, s1->_str);
	String_cat(s, ")");

	String_free(s1);
}

ItemSign* ItemSign_init(int pl, Item* l, const char sgn)
{
	ItemSign* i = (ItemSign*)malloc(sizeof(ItemSign));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = NULL;

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemSign_toString;
	i->_item._getRuleFunc = ItemSign_getRule;

	i->_item._type = ITEM_Sign;
	i->_item._proc_level = pl;

	i->_sgn = sgn;

	return i;
}

ItemSign* ItemSignMinus_init(Item* l)
{
	return ItemSign_init(l->_proc_level, l, L'-');
}

ItemSign* ItemSignPlus_init(Item* l)
{
	return ItemSign_init(l->_proc_level, l, L'+');
}

// Pow

void ItemPow_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, ")");
	String_cat(s, "^");
	String_cat(s, "(");
	String_cat(s, s2->_str);
	String_cat(s, ")");
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemPow_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);
	_this->_right->_getRuleFunc(rl - 1, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, "^");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

ItemPow* ItemPow_init(int pl, Item* l, Item* r)
{
	ItemPow* i = (ItemPow*)malloc(sizeof(ItemPow));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemPow_toString;
	i->_item._getRuleFunc = ItemPow_getRule;

	i->_item._type = ITEM_Pow;
	i->_item._proc_level = pl;

	return i;
}

// Subscript

void ItemSubscript_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, ")");
	String_cat(s, "_");
	String_cat(s, "("); 
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemSubscript_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);
	_this->_right->_getRuleFunc(rl - 1, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, ")");
	String_cat(s, "_");
	String_cat(s, "(");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

ItemSubscript* ItemSubscript_init(int pl, Item* l, Item* r)
{
	ItemSubscript* i = (ItemSubscript*)malloc(sizeof(ItemSubscript));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemSubscript_toString;
	i->_item._getRuleFunc = ItemSubscript_getRule;

	i->_item._type = ITEM_Subscript;
	i->_item._proc_level = pl;

	return i;
}

// Factorial

void ItemFactorial_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);

	String_cpy(s, s1->_str);
	String_cat(s, "!");

	String_free(s1);
}

void ItemFactorial_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);

	String_cpy(s, s1->_str);
	String_cat(s, "!");
	
	String_free(s1);
}

ItemFactorial* ItemFactorial_init(int pl, Item* l)
{
	ItemFactorial* i = (ItemFactorial*)malloc(sizeof(ItemFactorial));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = NULL;

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemFactorial_toString;
	i->_item._getRuleFunc = ItemFactorial_getRule;

	i->_item._type = ITEM_Factorial;
	i->_item._proc_level = pl;

	return i;
}

// CommFunc

void ItemCommFunc_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	ItemCommFunc* i = (ItemCommFunc*)_this;

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);

	String_cpy(s, i->_str);
	String_cat(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, ")");

	String_free(s1);
}

void ItemCommFunc_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	ItemCommFunc* i = (ItemCommFunc*)_this;

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);

	String_cpy(s, i->_str);
	String_cat(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, ")");

	String_free(s1);
}

void ItemCommFunc_destroy(Item* _this)
{
	ItemCommFunc* i = (ItemCommFunc*)_this;
	free(i->_str);
}

ItemCommFunc* ItemCommFunc_init(int pl, Item* l, Item* r, const char* s)
{
	ItemCommFunc* i = (ItemCommFunc*)malloc(sizeof(ItemCommFunc));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = r;

	i->_item._destroyFunc = ItemCommFunc_destroy;
	i->_item._toStringFunc = ItemCommFunc_toString;
	i->_item._getRuleFunc = ItemCommFunc_getRule;

	i->_item._type = ITEM_CommFunc;
	i->_item._proc_level = pl;

	i->_len = strlen(s);
	i->_str = (char*)malloc(sizeof(char) * (i->_len + 1));
	assert(i->_str != 0);

	memset(i->_str, 0, i->_len + 1);
	strcpy(i->_str, s);

	return i;
}

ItemCommFunc* ItemSinFunc_init(Item* l)
{
	return ItemCommFunc_init(l->_proc_level, l, NULL, "Sin");
}

ItemCommFunc* ItemCosFunc_init(Item* l)
{
	return ItemCommFunc_init(l->_proc_level, l, NULL, "Cos");
}

ItemCommFunc* ItemTanFunc_init(Item* l)
{
	return ItemCommFunc_init(l->_proc_level, l, NULL, "Tan");
}

ItemCommFunc* ItemLogFunc_init(Item* l)
{
	return ItemCommFunc_init(l->_proc_level, l, NULL, "Log");
}

ItemCommFunc* ItemExpFunc_init(Item* l)
{
	return ItemCommFunc_init(l->_proc_level, l, NULL, "Exp");
}

ItemCommFunc* ItemAsinFunc_init(Item* l)
{
	return ItemCommFunc_init(l->_proc_level, l, NULL, "Asin");
}

ItemCommFunc* ItemAcosFunc_init(Item* l)
{
	return ItemCommFunc_init(l->_proc_level, l, NULL, "Acos");
}

ItemCommFunc* ItemAtanFunc_init(Item* l)
{
	return ItemCommFunc_init(l->_proc_level, l, NULL, "Atan");
}

ItemCommFunc* ItemLnFunc_init(Item* l)
{
	return ItemCommFunc_init(l->_proc_level, l, NULL, "Ln");
}

// Root

void ItemRoot_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "Root(");
	String_cat(s, s1->_str);
	String_cat(s, ";");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemRoot_getRule(int rl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_getRuleFunc(rl - 1, _this->_left, s1);
	_this->_right->_getRuleFunc(rl - 1, _this->_right, s2);

	String_cpy(s, "Root(");
	String_cat(s, s1->_str);
	String_cat(s, ";");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

ItemRoot* ItemRoot_init(int pl, Item* l, Item* r)
{
	ItemRoot* i = (ItemRoot*)malloc(sizeof(ItemRoot));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemRoot_toString;
	i->_item._getRuleFunc = ItemRoot_getRule;

	i->_item._type = ITEM_Root;
	i->_item._proc_level = pl;

	i->_sHeight = 0;
	i->_sPadding = 20;

	return i;
}

static void ItemTreeChild_free(Item* item)
{
	Item* left = item->_left;
	Item* right = item->_right;

	if (left)
	{
		ItemTreeChild_free(left);
		left->_destroyFunc(left);
		free(left);
	}

	if (right)
	{
		ItemTreeChild_free(right);
		right->_destroyFunc(right); 
		free(right);
	}
}

void ItemTree_free(Item** Item)
{
	ItemTreeChild_free(*Item);
	(*Item)->_destroyFunc(*Item);
	free(*Item);
}

void get_level(int* pl, Item* pItems)
{
	if (pItems->_left || pItems->_right)
	{
		int pl1 = 0, pl2 = 0;

		if (pItems->_left)
		{
			pl1 += 1;
			get_level(&pl1, pItems->_left);
		}

		if (pItems->_right)
		{
			pl2 += 1;
			get_level(&pl2, pItems->_right);
		}

		*pl += max(pl1, pl2);
	}

	
}
