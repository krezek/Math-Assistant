/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <proc.h>
#include <strg.h>
#include <items.h>

//  --------------------  Rule Queue  --------------------------------

Rule* Rule_init(const char* s)
{
	Rule* rl = (Rule*)malloc(sizeof(Rule));
	assert(rl != 0);

	rl->_str = String_init();
	String_cpy(rl->_str, s);

	return rl;
}


void Rule_free(Rule* rl)
{
	String_free(rl->_str);
	free(rl);
}

RNode* RNode_init(Rule* rl)
{
	RNode* n = (RNode*)malloc(sizeof(RNode));
	assert(n != 0);

	n->_val = rl;
	n->_next = NULL;

	return n;
}

void RNode_free(RNode* n)
{
	free(n);
}

RuleQueue* RuleQueue_init()
{
	RuleQueue* q = (RuleQueue*)malloc(sizeof(RuleQueue));
	assert(q != 0);

	q->_front = q->_rear = NULL;
	return q;
}

void RuleQueue_enqueue(RuleQueue* q, const char* s)
{
	Rule* v = Rule_init(s);
	RNode* tmp = RNode_init(v);

	if (q->_rear == NULL) {
		q->_front = q->_rear = tmp;
		return;
	}

	q->_rear->_next = tmp;
	q->_rear = tmp;
}

Rule* RuleQueue_dequeue(RuleQueue* q)
{
	if (q->_front == NULL)
		return NULL;

	RNode* tmp = q->_front;

	q->_front = q->_front->_next;

	if (q->_front == NULL)
		q->_rear = NULL;

	Rule* v = tmp->_val;

	RNode_free(tmp);

	return v;
}

Rule* RuleQueue_front(RuleQueue* q)
{
	if (q->_front)
		return q->_front->_val;

	return NULL;
}

Rule* RuleQueue_next(RuleQueue* q)
{
	if (q->_front && q->_front->_next)
	{
		return q->_front->_next->_val;
	}

	return NULL;
}

bool RuleQueue_empty(RuleQueue* q)
{
	return (q->_front) ? false : true;
}

void RuleQueue_print(RuleQueue* q)
{
	RNode* t = q->_front;
	while (t)
	{
		if(t->_val)
			printf(t->_val->_str->_str);
		t = t->_next;
	}
}

void RuleQueue_free(RuleQueue* q)
{
	RNode* rn = q->_front;
	while (rn)
	{
		if (rn->_val)
		{
			Rule_free(rn->_val);
		}
		RNode* tmp = rn;
		rn = rn->_next;
		RNode_free(tmp);
	}

	free(q);
}


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

void Item_getRule(int pl, Item* __this, RuleQueue* rq)
{
	RuleQueue_enqueue(rq, "");
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

void ItemLiteral_getRule(int pl, Item* _this, RuleQueue* rq)
{
	ItemLiteral* i = (ItemLiteral*)_this;

	RuleQueue_enqueue(rq, "L");
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

void ItemNumber_getRule(int pl, Item* _this, RuleQueue* rq)
{
	ItemNumber* i = (ItemNumber*)_this;

	if (strchr(i->_str->_str, '.'))
		RuleQueue_enqueue(rq, "D");
	else
		RuleQueue_enqueue(rq, "I");
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
	ItemList* i = (ItemList*)_this;

	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	const char colon[] = {i->_colon, 0};

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, colon);
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemList_getRule(int pl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, ",");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "list");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
}

ItemList* ItemList_init(Item* l, Item* r, const char c)
{
	ItemList* i = (ItemList*)malloc(sizeof(ItemList));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemList_toString;
	i->_item._getRuleFunc = ItemList_getRule;

	i->_item._type = ITEM_List;

	i->_colon = c;

	return i;
}

// Equ

void ItemEqu_destroy(Item* _this)
{
	ItemEqu* i = (ItemEqu*)_this;
	String_free(i->_operator);
}

void ItemEqu_toString(int pl, Item* _this, String* s)
{
	ItemEqu* i = (ItemEqu*)_this;

	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, i->_operator->_str);
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemEqu_getRule(int rl, Item* _this, RuleQueue* rq)
{
	ItemEqu* i = (ItemEqu*)_this;

	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, i->_operator->_str);
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "equation");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
}

ItemEqu* ItemEqu_init(Item* l, Item* r, const char* op)
{
	ItemEqu* i = (ItemEqu*)malloc(sizeof(ItemEqu));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = ItemEqu_destroy;
	i->_item._toStringFunc = ItemEqu_toString;
	i->_item._getRuleFunc = ItemEqu_getRule;

	i->_item._type = ITEM_Equ;

	i->_operator = String_init();
	String_cpy(i->_operator, op);

	return i;
}

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

void ItemAdd_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, "+");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "expr");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
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

void ItemSub_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, "-");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "expr");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
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

void ItemMult_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, "*");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "expr");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
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

void ItemFrac_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, "/");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "expr");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
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

void ItemSign_getRule(int rl, Item* _this, RuleQueue* rq)
{
	ItemSign* i = (ItemSign*)_this;

	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		if (rn1->_val)
		{
			String* s = String_init();
			String_cpy(s, "(");
			if (i->_sgn == '-')
				String_cat(s, "-");
			String_cat(s, rn1->_val->_str->_str);
			String_cat(s, ")");
			RuleQueue_enqueue(rq, s->_str);
			String_free(s);
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "expr");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
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

void ItemPow_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, "^");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "expr");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
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

void ItemSubscript_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, "_");
				String_cat(s, rn2->_val->_str->_str);
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "expr");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
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

void ItemFactorial_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		if (rn1->_val)
		{
			String* s = String_init();
			String_cpy(s, rn1->_val->_str->_str);
			String_cat(s, "!");
			RuleQueue_enqueue(rq, s->_str);
			String_free(s);
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "expr");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
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

// Triangle

void ItemTriangle_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	ItemTriangle* i = (ItemTriangle*)_this;

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);

	String_cpy(s, i->_str->_str);
	String_cat(s, "(");
	String_cat(s, s1->_str);
	String_cat(s, ")");

	String_free(s1);
}

void ItemTriangle_getRule(int rl, Item* _this, RuleQueue* rq)
{
	ItemTriangle* i = (ItemTriangle*)_this;

	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		if (rn1->_val)
		{
			String* s = String_init();
			String_cpy(s, i->_str->_str);
			String_cat(s, "(");
			String_cat(s, rn1->_val->_str->_str);
			String_cat(s, ")");
			RuleQueue_enqueue(rq, s->_str);
			String_free(s);
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "Triangle");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
}

void ItemTriangle_destroy(Item* _this)
{
	ItemTriangle* i = (ItemTriangle*)_this;
	String_free(i->_str);
}

ItemTriangle* ItemTriangle_init(int pl, Item* l, Item* r, const char* s)
{
	ItemTriangle* i = (ItemTriangle*)malloc(sizeof(ItemTriangle));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = r;

	i->_item._destroyFunc = ItemTriangle_destroy;
	i->_item._toStringFunc = ItemTriangle_toString;
	i->_item._getRuleFunc = ItemTriangle_getRule;

	i->_item._type = ITEM_Triangle;
	i->_item._proc_level = pl;

	i->_str = String_init();
	String_cpy(i->_str, s);

	return i;
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

void ItemRoot_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "Root(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, ";");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "RootFunc");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
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

// Integral

void ItemIntegral_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "Integral(");
	String_cat(s, s1->_str);
	String_cat(s, ";");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemIntegral_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "Integral(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, ";");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "IntegralFunc");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
}

ItemIntegral* ItemIntegral_init(int pl, Item* l, Item* r)
{
	ItemIntegral* i = (ItemIntegral*)malloc(sizeof(ItemIntegral));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemIntegral_toString;
	i->_item._getRuleFunc = ItemIntegral_getRule;

	i->_item._type = ITEM_Integral;
	i->_item._proc_level = pl;

	return i;
}


// Derivative

void ItemDerivative_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "Derivative(");
	String_cat(s, s1->_str);
	String_cat(s, ";");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemDerivative_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "Derivative(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, ";");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "DerivativeFunc");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
}

ItemDerivative* ItemDerivative_init(int pl, Item* l, Item* r)
{
	ItemDerivative* i = (ItemDerivative*)malloc(sizeof(ItemDerivative));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemDerivative_toString;
	i->_item._getRuleFunc = ItemDerivative_getRule;

	i->_item._type = ITEM_Derivative;
	i->_item._proc_level = pl;

	return i;
}


// Limit

void ItemLimit_toString(int pl, Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_proc_level, _this->_left, s1);
	_this->_right->_toStringFunc(_this->_proc_level, _this->_right, s2);

	String_cpy(s, "Lim(");
	String_cat(s, s1->_str);
	String_cat(s, ";");
	String_cat(s, s2->_str);
	String_cat(s, ")");

	String_free(s1);
	String_free(s2);
}

void ItemLimit_getRule(int rl, Item* _this, RuleQueue* rq)
{
	RuleQueue* rq1 = RuleQueue_init();
	RuleQueue* rq2 = RuleQueue_init();

	_this->_left->_getRuleFunc(_this->_proc_level, _this->_left, rq1);
	_this->_right->_getRuleFunc(_this->_proc_level, _this->_right, rq2);

	RNode* rn1 = rq1->_front;
	while (rn1)
	{
		RNode* rn2 = rq2->_front;
		while (rn2)
		{
			if (rn1->_val && rn2->_val)
			{
				String* s = String_init();
				String_cpy(s, "Lim(");
				String_cat(s, rn1->_val->_str->_str);
				String_cat(s, ";");
				String_cat(s, rn2->_val->_str->_str);
				String_cat(s, ")");
				RuleQueue_enqueue(rq, s->_str);
				String_free(s);
			}

			rn2 = rn2->_next;
		}

		rn1 = rn1->_next;
	}

	RuleQueue_enqueue(rq, "LimitFunc");

	RuleQueue_free(rq1);
	RuleQueue_free(rq2);
}

ItemLimit* ItemLimit_init(int pl, Item* l, Item* r)
{
	ItemLimit* i = (ItemLimit*)malloc(sizeof(ItemLimit));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemLimit_toString;
	i->_item._getRuleFunc = ItemLimit_getRule;

	i->_item._type = ITEM_Limit;
	i->_item._proc_level = pl;

	return i;
}

// Utils

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

