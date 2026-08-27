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

// ((L^R)*R)
bool rule_poly_replace_order(Item** pItem)
{
	Item* tmp = (*pItem)->_left;
	(*pItem)->_left = (*pItem)->_right;
	(*pItem)->_right = tmp;

	return true;
}

// ((R*(L^R))*R)
bool rule_poly_replace_co_duplicate(Item** pItem)
{
	Item* tmp = (*pItem)->_left->_right;
	(*pItem)->_left->_right = (*pItem)->_right;
	(*pItem)->_right = tmp;

	return true;
}
