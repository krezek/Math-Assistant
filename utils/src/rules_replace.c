#include "pch.h"
#include "platform.h"

#include <gmp.h>
#include <mpfr.h>

#include <proc.h>
#include <items_ext.h>
#include <map.h>

#include <rules.h>

// List of rules

// D
bool rule_replace_decimal(Item** pItem)
{
	ItemNumber* i = (ItemNumber*)(*pItem);

	size_t l1 = strchr(i->_str->_str, '.') - i->_str->_str;
	size_t l2 = strlen(i->_str->_str) - l1 - 1;

	for (size_t ix = 0; ix <= l2; ++ix)
		i->_str->_str[l1 + ix] = i->_str->_str[l1 + ix + 1];

	String* str2 = String_init();
	String_cpy(str2, "1");

	for (size_t ix = 0; ix < l2; ++ix)
		String_cat(str2, "0");

	mpz_t z1, z2;

	mpz_init(z1);
	mpz_init(z2);

	mpz_set_str(z1, i->_str->_str, 10);
	mpz_set_str(z2, str2->_str, 10);

	ItemTree_free(pItem);

	char* z1_str = mpz_get_str(NULL, 10, z1);
	char* z2_str = mpz_get_str(NULL, 10, z2);

	*pItem = (Item*)ItemFrac_init(PROC_TERM, (Item*)ItemNumber_init(PROC_PRIMARY_4, z1_str),
		(Item*)ItemNumber_init(PROC_PRIMARY_4, z2_str));

	free(z1_str);
	free(z2_str);

	mpz_clear(z2);
	mpz_clear(z1);

	String_free(str2);

	return true;
}

// I
bool rule_replace_I(Item** pItem)
{
	ItemNumber* i = (ItemNumber*)(*pItem);

	ItemRational* r = ItemRational_init(i->_item._proc_level, 0, i->_str->_str, "1");
	ItemTree_free(pItem);
	*pItem = (Item*)r;

	return true;
}

// R-R
bool rule_replace_sub(Item** pItem)
{
	ItemRational* r1 = (ItemRational*)(*pItem)->_left;
	ItemRational* r2 = (ItemRational*)(*pItem)->_right;

	ItemRational* nr1 = ItemRational_init(r1->_item._proc_level, r1->_sign,
		r1->_numerator->_str, r1->_denominator->_str);
	ItemRational* nr2 = ItemRational_init(r2->_item._proc_level, r2->_sign,
		r2->_numerator->_str, r2->_denominator->_str);

	ItemTree_free(pItem);
	*pItem = (Item*)ItemAdd_init(PROC_EXPR, (Item*)nr1,
		(Item*)ItemSign_init(PROC_FACTOR, (Item*)nr2, '-'));
	return true;
}

// (+R) or (-R)
bool rule_replace_sign(Item** pItem)
{
	ItemRational* r = (ItemRational*)(*pItem)->_left;
	ItemRational* nr = ItemRational_init(r->_item._proc_level, r->_sign,
		r->_numerator->_str, r->_denominator->_str);

	if (((ItemSign*)(*pItem))->_sgn == '-')
	{
		if (nr->_sign == '-')
			nr->_sign = 0;
		else
			nr->_sign = '-';
	}

	ItemTree_free(pItem);
	*pItem = (Item*)nr;

	return true;
}

// R/R
bool rule_replace_frac(Item** pItem)
{
	ItemRational* r1 = (ItemRational*)(*pItem)->_left;
	ItemRational* r2 = (ItemRational*)(*pItem)->_right;

	ItemRational* nr1 = ItemRational_init(r1->_item._proc_level, r1->_sign,
		r1->_numerator->_str, r1->_denominator->_str);
	ItemRational* nr2 = ItemRational_init(r2->_item._proc_level, r2->_sign,
		r2->_denominator->_str, r2->_numerator->_str);
	ItemTree_free(pItem);
	*pItem = (Item*)ItemMult_init(PROC_TERM, (Item*)nr1, (Item*)nr2);

	return true;
}

// R^R
bool rule_replace_power(Item** pItem)
{
	ItemRational* r = (ItemRational*)(*pItem)->_left;
	ItemRational* p = (ItemRational*)(*pItem)->_right;

	if (strcmp(p->_denominator->_str, "1") == 0)
	{
		bool psign = p->_sign != '-';

		mpz_t rz1, rz2, pz1, iz1, iz2;
		mpz_init(rz1);
		mpz_init(rz2);
		mpz_init(pz1);
		mpz_init(iz1);
		mpz_init(iz2);

		mpz_set_str(rz1, r->_numerator->_str, 10);
		mpz_set_str(rz2, r->_denominator->_str, 10);
		mpz_set_str(pz1, p->_numerator->_str, 10);

		char* s;
		if (r->_sign == '-')
			s = "-1";
		else
			s = "1";

		mpz_set_str(iz1, s, 10);
		mpz_set_str(iz2, "1", 10);

		mpz_pow_ui(rz1, rz1, mpz_get_ui(pz1));
		mpz_pow_ui(rz2, rz2, mpz_get_ui(pz1));
		mpz_pow_ui(iz1, iz1, mpz_get_ui(pz1));

		ItemTree_free(pItem);
		char* z1_str = mpz_get_str(NULL, 10, rz1);
		char* z2_str = mpz_get_str(NULL, 10, rz2);
		char csign;
		if (mpz_cmp(iz1, iz2) == 0)
			csign = 0;
		else
			csign = '-';

		if (psign)
			*pItem = (Item*)ItemRational_init(PROC_FACTOR, csign,
				z1_str, z2_str);
		else
			*pItem = (Item*)ItemRational_init(PROC_FACTOR, csign,
				z2_str, z1_str);

		free(z1_str);
		free(z2_str);

		mpz_clear(rz1);
		mpz_clear(rz2);
		mpz_clear(pz1);
		mpz_clear(iz1);
		mpz_clear(iz2);

		return true;
	}
	else
	{
		ItemRational* nr = ItemRational_init(r->_item._proc_level, r->_sign,
			r->_numerator->_str, r->_denominator->_str);

		Item* i = (Item*)ItemPow_init(PROC_POWER,
			(Item*)ItemRoot_init(PROC_POWER, (Item*)nr, (Item*)ItemRational_init(PROC_FACTOR,
				0, p->_denominator->_str, "1")),
			(Item*)ItemRational_init(PROC_FACTOR, p->_sign, p->_numerator->_str, "1"));
		ItemTree_free(pItem);
		*pItem = i;

		return true;
	}

	return false;
}
