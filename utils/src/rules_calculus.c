/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <gmp.h>
#include <mpfr.h>

#include <proc.h>
#include <items_ext.h>
#include <map.h>

#include <rules.h>

// List of rules
   
// Integral((R*(L^R));L)
bool rule_Integral_factor(Item** pItem)
{
	ItemIntegral* i = (ItemIntegral*)*pItem;

	ItemLiteral* l1 = (ItemLiteral*)i->_item._left->_right->_left;
	ItemLiteral* l2 = (ItemLiteral*)i->_item._right;

	if (strcmp(l1->_str->_str, l2->_str->_str) == 0)
	{
		ItemLiteral* l = ItemLiteral_init(ITEM_Literal, l1->_str->_str);

		ItemRational* r1 = (ItemRational*)i->_item._left->_left;
		ItemRational* r2 = (ItemRational*)i->_item._left->_right->_right;

		if (strcmp(r2->_denominator->_str, "1") == 0)
		{
			mpz_t z1, z2, z3, gcd;

			mpz_init(z1);
			mpz_init(z2);
			mpz_init(z3);
			mpz_init(gcd);

			mpz_set_str(z1, r1->_numerator->_str, 10);
			mpz_set_str(z2, r1->_denominator->_str, 10);
			mpz_set_str(z3, r2->_numerator->_str, 10);

			mpz_add_ui(z3, z3, 1);
			mpz_mul(z2, z2, z3);
			mpz_gcd(gcd, z1, z2);
			mpz_div(z1, z1, gcd);
			mpz_div(z2, z2, gcd);

			char* z1_str = mpz_get_str(NULL, 10, z1);
			char* z2_str = mpz_get_str(NULL, 10, z2);
			char* z3_str = mpz_get_str(NULL, 10, z3);

			ItemTree_free(pItem);

			if (mpz_cmp_ui(z1, 1) == 0 &&
				mpz_cmp_ui(z2, 1) == 0)
			{
				*pItem = (Item*)ItemPow_init(PROC_NUMBER, (Item*)l, 
					(Item*)ItemRational_init(PROC_NUMBER, 0, z3_str, "1"));
			}
			else
			{
				*pItem = (Item*)ItemMult_init(PROC_FACTOR, (Item*)ItemRational_init(PROC_NUMBER, 0, z1_str, z2_str),
					(Item*)ItemPow_init(PROC_NUMBER, (Item*)l, (Item*)ItemRational_init(PROC_NUMBER, 0, z3_str, "1")));
			}			

			free(z1_str);
			free(z2_str);
			free(z3_str);

			mpz_clear(gcd);
			mpz_clear(z3);
			mpz_clear(z2);
			mpz_clear(z1);

			return true;
		}
	}

	return false;
}

// Integral((L^R);L)
bool rule_Integral_factor2(Item** pItem)
{
	ItemIntegral* i = (ItemIntegral*)*pItem;

	ItemLiteral* l1 = (ItemLiteral*)i->_item._left->_left;
	ItemLiteral* l2 = (ItemLiteral*)i->_item._right;

	if (strcmp(l1->_str->_str, l2->_str->_str) == 0)
	{
		ItemLiteral* l = ItemLiteral_init(ITEM_Literal, l1->_str->_str);

		ItemRational* r2 = (ItemRational*)i->_item._left->_right;

		if (strcmp(r2->_denominator->_str, "1") == 0)
		{
			mpz_t z1, z2, z3, gcd;

			mpz_init(z1);
			mpz_init(z2);
			mpz_init(z3);
			mpz_init(gcd);

			mpz_set_str(z1, "1", 10);
			mpz_set_str(z2, "1", 10);
			mpz_set_str(z3, r2->_numerator->_str, 10);

			mpz_add_ui(z3, z3, 1);
			mpz_mul(z2, z2, z3);
			mpz_gcd(gcd, z1, z2);
			mpz_div(z1, z1, gcd);
			mpz_div(z2, z2, gcd);

			char* z1_str = mpz_get_str(NULL, 10, z1);
			char* z2_str = mpz_get_str(NULL, 10, z2);
			char* z3_str = mpz_get_str(NULL, 10, z3);

			ItemTree_free(pItem);

			if (mpz_cmp_ui(z1, 1) == 0 &&
				mpz_cmp_ui(z2, 1) == 0)
			{
				*pItem = (Item*)ItemPow_init(PROC_NUMBER, (Item*)l,
					(Item*)ItemRational_init(PROC_NUMBER, 0, z3_str, "1"));
			}
			else
			{
				*pItem = (Item*)ItemMult_init(PROC_FACTOR, (Item*)ItemRational_init(PROC_NUMBER, 0, z1_str, z2_str),
					(Item*)ItemPow_init(PROC_NUMBER, (Item*)l, (Item*)ItemRational_init(PROC_NUMBER, 0, z3_str, "1")));
			}

			free(z1_str);
			free(z2_str);
			free(z3_str);

			mpz_clear(gcd);
			mpz_clear(z3);
			mpz_clear(z2);
			mpz_clear(z1);

			return true;
		}
	}

	return false;
}

// Integral(R;L)
bool rule_Integral_factor3(Item** pItem)
{
	ItemIntegral* i = (ItemIntegral*)*pItem;

	ItemLiteral* l2 = (ItemLiteral*)i->_item._right;
	ItemLiteral* l = ItemLiteral_init(ITEM_Literal, l2->_str->_str);

	ItemRational* r1 = (ItemRational*)i->_item._left;

	if (strcmp(r1->_numerator->_str, "0") != 0)
	{
		mpz_t z1, z2, z3, gcd;

		mpz_init(z1);
		mpz_init(z2);
		mpz_init(z3);
		mpz_init(gcd);

		mpz_set_str(z1, r1->_numerator->_str, 10);
		mpz_set_str(z2, r1->_denominator->_str, 10);
		mpz_set_str(z3, "1", 10);

		mpz_gcd(gcd, z1, z2);
		mpz_div(z1, z1, gcd);
		mpz_div(z2, z2, gcd);

		char* z1_str = mpz_get_str(NULL, 10, z1);
		char* z2_str = mpz_get_str(NULL, 10, z2);
		char* z3_str = mpz_get_str(NULL, 10, z3);

		ItemTree_free(pItem);

		if (mpz_cmp_ui(z1, 1) == 0 &&
			mpz_cmp_ui(z2, 1) == 0)
		{
			*pItem = (Item*)ItemPow_init(PROC_NUMBER, (Item*)l,
				(Item*)ItemRational_init(PROC_NUMBER, 0, z3_str, "1"));
		}
		else
		{
			*pItem = (Item*)ItemMult_init(PROC_FACTOR, (Item*)ItemRational_init(PROC_NUMBER, 0, z1_str, z2_str),
				(Item*)ItemPow_init(PROC_NUMBER, (Item*)l, (Item*)ItemRational_init(PROC_NUMBER, 0, z3_str, "1")));
		}

		free(z1_str);
		free(z2_str);
		free(z3_str);

		mpz_clear(gcd);
		mpz_clear(z3);
		mpz_clear(z2);
		mpz_clear(z1);

		return true;
	}

	return false;
}

// Integral((expr+expr);L)
bool rule_Integral_add_expr(Item** pItem)
{
	ItemLiteral* l = (ItemLiteral*)(*pItem)->_right;
	ItemLiteral* l1 = ItemLiteral_init(PROC_LITERAL, l->_str->_str);
	ItemLiteral* l2 = ItemLiteral_init(PROC_LITERAL, l->_str->_str);

	Item* expr1 = (*pItem)->_left->_left;
	Item* expr2 = (*pItem)->_left->_right;

	(*pItem)->_left->_left = NULL;
	(*pItem)->_left->_right = NULL;
	ItemTree_free(pItem);

	*pItem = (Item*)ItemAdd_init(PROC_TERM,
		(Item*)ItemIntegral_init(PROC_FUNC, expr1, (Item*)l1),
		(Item*)ItemIntegral_init(PROC_FUNC, expr2, (Item*)l2));
	return true;
}

// Integral((R+expr);L)
bool rule_Integral_add_expr2(Item** pItem)
{
	ItemLiteral* l = (ItemLiteral*)(*pItem)->_right;
	ItemLiteral* l1 = ItemLiteral_init(PROC_LITERAL, l->_str->_str);
	ItemLiteral* l2 = ItemLiteral_init(PROC_LITERAL, l->_str->_str);

	Item* r = (*pItem)->_left->_left;
	Item* expr2 = (*pItem)->_left->_right;

	(*pItem)->_left->_left = NULL;
	(*pItem)->_left->_right = NULL;
	ItemTree_free(pItem);

	*pItem = (Item*)ItemAdd_init(PROC_TERM,
		(Item*)ItemIntegral_init(PROC_FUNC, r, (Item*)l1),
		(Item*)ItemIntegral_init(PROC_FUNC, expr2, (Item*)l2));
	return true;
}

// Integral((expr+R);L)
bool rule_Integral_add_expr3(Item** pItem)
{
	ItemLiteral* l = (ItemLiteral*)(*pItem)->_right;
	ItemLiteral* l1 = ItemLiteral_init(PROC_LITERAL, l->_str->_str);
	ItemLiteral* l2 = ItemLiteral_init(PROC_LITERAL, l->_str->_str);

	Item* expr1 = (*pItem)->_left->_left;
	Item* r = (*pItem)->_left->_right;

	(*pItem)->_left->_left = NULL;
	(*pItem)->_left->_right = NULL;
	ItemTree_free(pItem);

	*pItem = (Item*)ItemAdd_init(PROC_TERM,
		(Item*)ItemIntegral_init(PROC_FUNC, expr1, (Item*)l1),
		(Item*)ItemIntegral_init(PROC_FUNC, r, (Item*)l2));
	return true;
}
