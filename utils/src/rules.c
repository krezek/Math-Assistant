/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <gmp.h>
#include <mpfr.h>

#include <proc.h>
#include <items.h>
#include <map.h>

#include <rules.h>

typedef bool (*RuleFunc) (Item** pItem);

Map* gs_rules_map;

void do_free_map(void* v)
{

}

void init_rules_library()
{
	gs_rules_map = Map_init();

	Map_insert(gs_rules_map, "D", rule_replace_decimal);
	Map_insert(gs_rules_map, "I", rule_replace_I);
	Map_insert(gs_rules_map, "(R-R)", rule_replace_sub);
	Map_insert(gs_rules_map, "(-R)", rule_replace_sign);
	Map_insert(gs_rules_map, "(+R)", rule_replace_sign);
	Map_insert(gs_rules_map, "(R/R)", rule_replace_frac);

	Map_insert(gs_rules_map, "(R+R)", rule_add_R);
	Map_insert(gs_rules_map, "(R*R)", rule_mult_R);
	Map_insert(gs_rules_map, "R!", rule_factorial_R);

	Map_insert(gs_rules_map, "(R^R)", rule_replace_power);

	Map_insert(gs_rules_map, "Root(R;R)", rule_root_R);

	//  --------------- Polynomial -----------------------------------------
	Map_insert(gs_rules_map, "((L^R)*R)", rule_poly_replace_order);
	Map_insert(gs_rules_map, "((R*(L^R))*R)", rule_poly_replace_co_duplicate);

	//  --------------- calculus -------------------------------------------
	Map_insert(gs_rules_map, "Integral((R*(L^R));L)", rule_Integral_factor);
	Map_insert(gs_rules_map, "Integral((L^R);L)", rule_Integral_factor2);
	Map_insert(gs_rules_map, "Integral(R;L)", rule_Integral_factor3);
	Map_insert(gs_rules_map, "Integral((expr+expr);L)", rule_Integral_add_expr);
	Map_insert(gs_rules_map, "Integral((R+expr);L)", rule_Integral_add_expr2);
	Map_insert(gs_rules_map, "Integral((expr+R);L)", rule_Integral_add_expr3);

}

void destroy_rules_library()
{
	Map_free(gs_rules_map, do_free_map);
}

static bool apply_rule(int level, Item** pItems, String* s)
{
	bool again = false;
#ifdef _DEBUG
	printf("rule (%d): %s", level,  s->_str);
#endif
	RuleFunc pFunc = (RuleFunc)Map_get(gs_rules_map, s->_str);
	if (pFunc)
	{
#ifdef _DEBUG
		printf(" apply");
#endif
		again = again || pFunc(pItems);
	}
#ifdef _DEBUG
	printf("\n");

	String* expr = String_init();
	(*pItems)->_toStringFunc(0, *pItems, expr);
	printf("%s\n", expr->_str);
	String_free(expr);

#endif
	return again;
}

bool traverse_rules(Item** pItems)
{
	bool again = false;

	RuleQueue* rq = RuleQueue_init();

	(*pItems)->_getRuleFunc(0, *pItems, rq);

	RNode* rn = rq->_front;
	while (rn)
	{
		if(rn->_val)
			again = again || apply_rule(0, pItems, rn->_val->_str);
		rn = rn->_next;
	}
	
	RuleQueue_free(rq);

	Item** pLeft = &(*pItems)->_left;
	Item** pRight = &(*pItems)->_right;

	if (*pLeft)
		again = again || traverse_rules(pLeft);

	if (*pRight)
		again = again || traverse_rules(pRight);

	return again;
}
