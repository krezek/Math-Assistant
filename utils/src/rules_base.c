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

// R+R
bool rule_add_R(Item** pItem)
{
	ItemRational* r1 = (ItemRational*)(*pItem)->_left;
	ItemRational* r2 = (ItemRational*)(*pItem)->_right;

	mpz_t z, r1_num, r1_den, r2_num, r2_den, gcd;
	mpz_init(z);
	mpz_init(r1_num);
	mpz_init(r1_den);
	mpz_init(r2_num);
	mpz_init(r2_den);
	mpz_init(gcd);
	char sign;

	mpz_set_str(r1_num, r1->_numerator->_str, 10);
	mpz_set_str(r1_den, r1->_denominator->_str, 10);
	mpz_set_str(r2_num, r2->_numerator->_str, 10);
	mpz_set_str(r2_den, r2->_denominator->_str, 10);

	if (r1->_sign == r2->_sign)
	{
		mpz_mul(z, r1_den, r2_den);
		mpz_mul(r1_num, r1_num, r2_den);
		mpz_mul(r2_num, r1_den, r2_num);
		mpz_add(r1_num, r1_num, r2_num);

		mpz_gcd(gcd, r1_num, z);
		mpz_div(r1_num, r1_num, gcd);
		mpz_div(z, z, gcd);

		sign = r1->_sign;
	}
	else
	{
		mpz_mul(z, r1_den, r2_den);
		mpz_mul(r1_num, r1_num, r2_den);
		mpz_mul(r2_num, r1_den, r2_num);
		if (mpz_cmp(r1_num, r2_num) >= 0)
		{
			sign = r1->_sign;
		}
		else
		{
			sign = r2->_sign;
		}

		mpz_sub(r1_num, r1_num, r2_num);
		mpz_abs(r1_num, r1_num);

		mpz_gcd(gcd, r1_num, z);
		mpz_div(r1_num, r1_num, gcd);
		mpz_div(z, z, gcd);
	}

	char* r1_num_str = mpz_get_str(NULL, 10, r1_num);
	char* z_str = mpz_get_str(NULL, 10, z);

	ItemRational* r = ItemRational_init(r1->_item._proc_level, sign,
		r1_num_str, z_str);
	ItemTree_free(pItem);
	*pItem = (Item*)r;

	free(r1_num_str);
	free(z_str);

	mpz_clear(z);
	mpz_clear(r1_num);
	mpz_clear(r1_den);
	mpz_clear(r2_num);
	mpz_clear(r2_den);
	mpz_clear(gcd);

	return true;
}

// R*R
bool rule_mult_R(Item** pItem)
{
	ItemRational* r1 = (ItemRational*)(*pItem)->_left;
	ItemRational* r2 = (ItemRational*)(*pItem)->_right;

	mpz_t r1_num, r1_den, r2_num, r2_den, gcd;
	mpz_init(r1_num);
	mpz_init(r1_den);
	mpz_init(r2_num);
	mpz_init(r2_den);
	mpz_init(gcd);
	char sign;

	mpz_set_str(r1_num, r1->_numerator->_str, 10);
	mpz_set_str(r1_den, r1->_denominator->_str, 10);
	mpz_set_str(r2_num, r2->_numerator->_str, 10);
	mpz_set_str(r2_den, r2->_denominator->_str, 10);

	mpz_mul(r1_num, r1_num, r2_num);
	mpz_mul(r1_den, r1_den, r2_den);
	mpz_gcd(gcd, r1_num, r1_den);
	mpz_div(r1_num, r1_num, gcd);
	mpz_div(r1_den, r1_den, gcd);

	if ((r1->_sign == '-' || r2->_sign == '-') &&
		r1->_sign != r2->_sign)
	{
		sign = '-';
	}
	else
	{
		sign = 0;
	}

	char* r1_num_str = mpz_get_str(NULL, 10, r1_num);
	char* r1_den_str = mpz_get_str(NULL, 10, r1_den);

	ItemRational* r = ItemRational_init(r1->_item._proc_level, sign,
		r1_num_str, r1_den_str);
	ItemTree_free(pItem);
	*pItem = (Item*)r;

	free(r1_num_str);
	free(r1_den_str);

	mpz_clear(r1_num);
	mpz_clear(r1_den);
	mpz_clear(r2_num);
	mpz_clear(r2_den);
	mpz_clear(gcd);

	return true;
}

// R!
bool rule_factorial_R(Item** pItem)
{
	ItemRational* r = (ItemRational*)(*pItem)->_left;

	if (strcmp(r->_denominator->_str, "1") == 0)
	{
		mpz_t z;
		mpz_init(z);

		mpz_set_str(z, r->_numerator->_str, 10);
		mpz_fac_ui(z, mpz_get_ui(z));

		char* z_str = mpz_get_str(NULL, 10, z);
		ItemRational* nr = ItemRational_init(r->_item._proc_level, r->_sign,
			z_str, r->_denominator->_str);
		free(z_str);

		ItemTree_free(pItem);
		*pItem = (Item*)nr;

		mpz_clear(z);

		return true;
	}

	return false;
}

// Root(R;R)
bool rule_root_R(Item** pItem)
{
	ItemRational* r = (ItemRational*)(*pItem)->_left;
	ItemRational* p = (ItemRational*)(*pItem)->_right;

	if ((strcmp(r->_denominator->_str, "1") == 0) &&
		(strcmp(p->_denominator->_str, "1") == 0) &&
		(strcmp(r->_numerator->_str, "0") == 0))
	{
		ItemRational* r_1 = ItemRational_init(r->_item._proc_level,
			0, "0", "1");

		ItemTree_free(pItem);
		*pItem = (Item*)r_1;

		return true;
	}
	else if ((strcmp(r->_denominator->_str, "1") == 0) &&
		(strcmp(p->_denominator->_str, "1") == 0) &&
		(strcmp(r->_numerator->_str, "1") == 0))
	{
		if (r->_sign != '-')
		{
			ItemRational* r_1 = ItemRational_init(r->_item._proc_level,
				r->_sign, "1", "1");

			ItemTree_free(pItem);
			*pItem = (Item*)r_1;

			return true;
		}
		else
		{
			mpz_t mod;
			mpz_init(mod);
			mpz_set_str(mod, p->_numerator->_str, 10);
			mpz_mod_ui(mod, mod, 2);

			if (mpz_cmp_ui(mod, 0))
			{
				ItemRational* r_1 = ItemRational_init(r->_item._proc_level,
					'-', "1", "1");

				ItemTree_free(pItem);
				*pItem = (Item*)r_1;

				mpz_clear(mod);

				return true;
			}

			return false;

		}

	}
	else if ((strcmp(r->_denominator->_str, "1") == 0) &&
		(strcmp(p->_denominator->_str, "1") == 0) &&
		!r->_isPrimary)
	{
		mpz_t n, power, po, primary, mod;
		mpz_init(n);
		mpz_init(power);
		mpz_init(po);
		mpz_init(primary);
		mpz_init(mod);

		mpz_set_str(n, r->_numerator->_str, 10);
		mpz_set_str(power, p->_numerator->_str, 10);
		mpz_set_ui(primary, 2);

		Item* i = NULL;

		mpz_mod_ui(mod, power, 2);
		if (!(r->_sign == '-' &&
			mpz_cmp_ui(mod, 0) == 0))
		{
			while (mpz_cmp(n, primary) >= 0)
			{
				mpz_set_ui(po, 0);

				do
				{
					mpz_mod(mod, n, primary);
					if (mpz_cmp_ui(mod, 0) == 0)
					{
						mpz_add_ui(po, po, 1);
						mpz_div(n, n, primary);
					}
				} while (mpz_cmp_ui(mod, 0) == 0);

				if ((mpz_cmp_ui(po, 0) != 0) &&
					(mpz_cmp(po, power) >= 0))
				{
					mpz_mod(mod, po, power);
					char* primary_str = mpz_get_str(NULL, 10, primary);
					char* mod_str = mpz_get_str(NULL, 10, mod);

					if (mpz_cmp_ui(mod, 0) == 0)
					{
						mpz_div(po, po, power);
						char* po_str = mpz_get_str(NULL, 10, po);

						if (!i)
						{
							ItemRational* r1 = ItemRational_init(r->_item._proc_level, 0, primary_str, "1");
							r1->_isPrimary = true;

							i = (Item*)ItemPow_init(PROC_POWER,
								(Item*)r1,
								(Item*)ItemRational_init(r->_item._proc_level, 0, po_str, "1"));
						}
						else
						{
							ItemRational* r1 = ItemRational_init(r->_item._proc_level, 0, primary_str, "1");
							r1->_isPrimary = true;

							i = (Item*)ItemMult_init(PROC_TERM, i,
								(Item*)ItemPow_init(PROC_POWER,
									(Item*)r1,
									(Item*)ItemRational_init(r->_item._proc_level, 0, po_str, "1")));
						}

						free(po_str);
					}
					else
					{
						mpz_div(po, po, power);
						char* po_str = mpz_get_str(NULL, 10, po);

						if (!i)
						{
							ItemRational* r1 = ItemRational_init(r->_item._proc_level, 0, primary_str, "1");
							ItemRational* r2 = ItemRational_init(r->_item._proc_level, 0, primary_str, "1");
							r1->_isPrimary = true;
							r2->_isPrimary = true;

							i = (Item*)ItemMult_init(PROC_POWER,
								(Item*)ItemPow_init(PROC_POWER, (Item*)r1,
									(Item*)ItemRational_init(r->_item._proc_level, 0, po_str, "1")),
								(Item*)ItemRoot_init(PROC_POWER,
									(Item*)r2,
									(Item*)ItemRational_init(r->_item._proc_level, 0, p->_numerator->_str, "1")));
						}
						else
						{
							ItemRational* r1 = ItemRational_init(r->_item._proc_level, 0, primary_str, "1");
							ItemRational* r2 = ItemRational_init(r->_item._proc_level, 0, primary_str, "1");
							r1->_isPrimary = true;
							r2->_isPrimary = true;

							i = (Item*)ItemMult_init(PROC_TERM, i,
								(Item*)ItemMult_init(PROC_POWER,
									(Item*)ItemPow_init(PROC_POWER, (Item*)r1,
										(Item*)ItemRational_init(r->_item._proc_level, 0, po_str, "1")),
									(Item*)ItemRoot_init(PROC_POWER,
										(Item*)r2,
										(Item*)ItemRational_init(r->_item._proc_level, 0, p->_numerator->_str, "1"))));
						}

						free(po_str);
					}

					free(primary_str);
					free(mod_str);

				}

				mpz_nextprime(primary, primary);
			}
		}

		if (i)
		{
			mpz_mod_ui(power, power, 2);
			if (mpz_cmp_ui(power, 0) != 0 &&
				r->_sign == '-')
			{
				ItemTree_free(pItem);
				*pItem = (Item*)ItemSign_init(PROC_FACTOR, i, '-');
			}
			else
			{
				ItemTree_free(pItem);
				*pItem = i;
			}
		}

		mpz_clear(n);
		mpz_clear(power);
		mpz_clear(po);
		mpz_clear(primary);
		mpz_clear(mod);

		if (i)
			return true;
	}
	else if ((strcmp(r->_denominator->_str, "1") != 0) &&
		(strcmp(p->_denominator->_str, "1") == 0))
	{
		ItemRational* r_num = ItemRational_init(r->_item._proc_level,
			r->_sign, r->_numerator->_str, "1");
		ItemRational* r_den = ItemRational_init(r->_item._proc_level,
			0, r->_denominator->_str, "1");
		ItemRational* p_num1 = ItemRational_init(p->_item._proc_level,
			0, p->_numerator->_str, "1");
		ItemRational* p_num2 = ItemRational_init(p->_item._proc_level,
			0, p->_numerator->_str, "1");
		Item* i = (Item*)ItemFrac_init(PROC_TERM,
			(Item*)ItemRoot_init(PROC_POWER, (Item*)r_num, (Item*)p_num1),
			(Item*)ItemRoot_init(PROC_POWER, (Item*)r_den, (Item*)p_num2));

		ItemTree_free(pItem);
		*pItem = i;

		return true;
	}

	return false;
}
