#include "pch.h"
#include "platform.h"

#include <proc.h>
#include <editor.h>
#include "ids.h"

void Editor_delete_selection(Editor* ed);

Editor* Editor_init()
{
	Editor* ed = (Editor*)malloc(sizeof(Editor));
	assert(ed != NULL);

	memset(ed, 0, sizeof(Editor));

	return ed;
}

void Editor_free(Editor* ed)
{
	free(ed);
}

void Editor_OnInit(Editor* ed, GList* gll)
{
	ed->_in_gitems_list = gll;
	ed->_current_glist = gll;

	if (ed->_in_gitems_list->_front)
	{
		ed->_current_glist->_current = ed->_current_glist->_front;
	}
	else
	{
		ed->_current_glist->_current = NULL;
	}
}

void Editor_MouseClicked(Editor* ed, HWND hWnd, int x0, int y0, int mx0, int my0)
{
	GNode* node = GList_GetGNodeFromPoint(ed->_in_gitems_list, mx0, my0);
	
	if (node)
	{
		GList* ncgll = GList_UpdateCurrent(ed->_in_gitems_list, node);

		ed->_current_glist = ncgll;
		ed->_current_glist->_current = node;
	}
}

void Editor_UpdateCaret(Editor* ed, HWND hWnd, int x0, int y0)
{
	if (!ed->_current_glist->_current)
		return;

	ed->_x0 = x0;
	ed->_y0 = y0;

	CreateCaret(hWnd, (HBITMAP)NULL, 2, ed->_current_glist->_current->_pGItem->_height);
	SetCaretPos(ed->_current_glist->_current->_pGItem->_x0 - x0, ed->_current_glist->_current->_pGItem->_y0 - y0);
	ShowCaret(hWnd);
	
}

void Editor_OnSetFocus(Editor* ed, HWND hWnd)
{
	Editor_UpdateCaret(ed, hWnd, ed->_x0, ed->_y0);
}

void Editor_OnKillFocus(Editor* ed)
{
	DestroyCaret();
}

void Editor_OnKey_LeftArrow(Editor* ed, HWND hWnd, bool bShift, bool bCtrl)
{
	GNode* node = (GNode*)ed->_current_glist->_current;

	if (!node)
		return;

	if (!node->_prev)
	{
		GList* parent = ed->_current_glist->_parent;
		if (parent)
		{
			if (parent->_current->_pGItem->_type == GITEM_POWER)
			{
				ed->_current_glist = parent;
			}
			else if (parent->_current->_pGItem->_type == GITEM_FRACTION)
			{
				if (ed->_current_glist != ((GItemFraction*)parent->_current->_pGItem)->_numerator)
				{
					ed->_current_glist = ((GItemFraction*)parent->_current->_pGItem)->_numerator;
					ed->_current_glist->_current = ed->_current_glist->_rear;
				}
				else
				{
					ed->_current_glist = parent;
				}
			}
			else if (parent->_current->_pGItem->_type == GITEM_ROOT)
			{
				if (ed->_current_glist != ((GItemRoot*)parent->_current->_pGItem)->_root)
				{
					ed->_current_glist = ((GItemRoot*)parent->_current->_pGItem)->_root;
					ed->_current_glist->_current = ed->_current_glist->_rear;
				}
				else
				{
					ed->_current_glist = parent;
				}
			}
		}
	}
	else
	{
		if (node->_prev->_pGItem->_type == GITEM_CHAR)
		{
			ed->_current_glist->_current = node->_prev;
		}
		else if (node->_prev->_pGItem->_type == GITEM_POWER)
		{
			ed->_current_glist->_current = node->_prev;
			ed->_current_glist = ((GItemPower*)node->_prev->_pGItem)->_exponent;
			ed->_current_glist->_current = ed->_current_glist->_rear;
		}
		else if (node->_prev->_pGItem->_type == GITEM_FRACTION)
		{
			ed->_current_glist->_current = node->_prev;
			ed->_current_glist = ((GItemFraction*)node->_prev->_pGItem)->_denominator;
			ed->_current_glist->_current = ed->_current_glist->_rear;
		}
		else if (node->_prev->_pGItem->_type == GITEM_ROOT)
		{
			ed->_current_glist->_current = node->_prev;
			ed->_current_glist = ((GItemRoot*)node->_prev->_pGItem)->_expr;
			ed->_current_glist->_current = ed->_current_glist->_rear;
		}
	}
}

void Editor_OnKey_RightArrow(Editor* ed, HWND hWnd, bool bShift, bool bCtrl)
{
	GNode* node = (GNode*)ed->_current_glist->_current;

	if (!node)
		return;

	if (!node->_next)
	{
		GList* parent = ed->_current_glist->_parent;
		if (parent)
		{
			if (parent->_current->_pGItem->_type == GITEM_POWER)
			{
				ed->_current_glist = parent;
				ed->_current_glist->_current = ed->_current_glist->_current->_next;
			}
			else if (parent->_current->_pGItem->_type == GITEM_FRACTION)
			{
				if (ed->_current_glist != ((GItemFraction*)parent->_current->_pGItem)->_denominator)
				{
					ed->_current_glist = ((GItemFraction*)parent->_current->_pGItem)->_denominator;
					ed->_current_glist->_current = ed->_current_glist->_front;
				}
				else
				{
					ed->_current_glist = parent;
					ed->_current_glist->_current = ed->_current_glist->_current->_next;
				}
			}
			else if (parent->_current->_pGItem->_type == GITEM_ROOT)
			{
				if (ed->_current_glist != ((GItemRoot*)parent->_current->_pGItem)->_expr)
				{
					ed->_current_glist = ((GItemRoot*)parent->_current->_pGItem)->_expr;
					ed->_current_glist->_current = ed->_current_glist->_front;
				}
				else
				{
					ed->_current_glist = parent;
					ed->_current_glist->_current = ed->_current_glist->_current->_next;
				}
			}
		}
	}
	else
	{
		if (node->_pGItem->_type == GITEM_CHAR)
		{
			ed->_current_glist->_current = node->_next;
		}
		else if (node->_pGItem->_type == GITEM_POWER)
		{
			ed->_current_glist = ((GItemPower*)node->_pGItem)->_exponent;
			ed->_current_glist->_current = ed->_current_glist->_front;
		}
		else if (node->_pGItem->_type == GITEM_FRACTION)
		{
			ed->_current_glist = ((GItemFraction*)node->_pGItem)->_numerator;
			ed->_current_glist->_current = ed->_current_glist->_front;
		}
		else if (node->_pGItem->_type == GITEM_ROOT)
		{
			ed->_current_glist = ((GItemRoot*)node->_pGItem)->_root;
			ed->_current_glist->_current = ed->_current_glist->_front;
		}
	}
}

void Editor_OnChar_Default(Editor* ed, int ch, HWND hWnd)
{
	if (ch == ':')
	{
		GList* root = GList_init(ed->_current_glist);
		GList* expr = GList_init(ed->_current_glist);

		GList_pushback(root, (GItem*)GItemChar_init(PROC_PRIMARY_2, 0));
		GList_pushback(expr, (GItem*)GItemChar_init(PROC_PRIMARY_2, 0));

		GItemRoot* g = GItemRoot_init(0, expr, root);
		GNode* node = GNode_init((GItem*)g, ed->_current_glist->_current, ed->_current_glist->_current->_prev);
		if (ed->_current_glist->_current->_prev)
			ed->_current_glist->_current->_prev->_next = node;
		else
		{
			ed->_current_glist->_front = node;
		}
		ed->_current_glist->_current->_prev = node;
		ed->_current_glist->_current = node;

		// Move Cursor right
		Editor_OnKey_RightArrow(ed, hWnd, false, false);
	}
	else if (ch == '/')
	{
		GList* num = GList_init(ed->_current_glist);
		GList* den = GList_init(ed->_current_glist);
		GList_pushback(num, (GItem*)GItemChar_init(PROC_PRIMARY_2, 0));
		GList_pushback(den, (GItem*)GItemChar_init(PROC_PRIMARY_2, 0));

		GItemFraction* g = GItemFraction_init(PROC_FACTOR, num, den);
		GNode* node = GNode_init((GItem*)g, ed->_current_glist->_current, ed->_current_glist->_current->_prev);
		if (ed->_current_glist->_current->_prev)
			ed->_current_glist->_current->_prev->_next = node;
		else
		{
			ed->_current_glist->_front = node;
		}
		ed->_current_glist->_current->_prev = node;
		ed->_current_glist->_current = node;

		// Move Cursor right
		Editor_OnKey_RightArrow(ed, hWnd, false, false);
	}
	else if (ch == '^')
	{
		GList* exponent = GList_init(ed->_current_glist);
		GList_pushback(exponent, (GItem*)GItemChar_init(PROC_PRIMARY_2, 0));
		GItemPower* g = GItemPower_init(PROC_POWER, exponent);

		GNode* node = GNode_init((GItem*)g, ed->_current_glist->_current, ed->_current_glist->_current->_prev);
		if (ed->_current_glist->_current->_prev)
			ed->_current_glist->_current->_prev->_next = node;
		else
		{
			ed->_current_glist->_front = node;
		}
		ed->_current_glist->_current->_prev = node;
		ed->_current_glist->_current = node;

		// Move Cursor right
		Editor_OnKey_RightArrow(ed, hWnd, false, false);
	}
	else
	{
		if (ch == '*')
			ch = L'\u00D7';

		if (ch == 16)
			ch = L'\u03C0';

		GItemChar* g = GItemChar_init(PROC_PRIMARY_2, ch);
		GNode* node = GNode_init((GItem*)g, ed->_current_glist->_current, ed->_current_glist->_current->_prev);
		if (ed->_current_glist->_current->_prev)
			ed->_current_glist->_current->_prev->_next = node;
		else
		{
			ed->_current_glist->_front = node;
		}
		ed->_current_glist->_current->_prev = node;
		ed->_current_glist->_current = node->_next;
	}
}

void Editor_OnChar_Backspace(Editor* ed)
{
	GNode* node = ed->_current_glist->_current->_prev;

	if (ed->_current_glist->_current->_prev)
	{
		if (ed->_current_glist->_current->_prev->_prev)
		{
			ed->_current_glist->_current->_prev = ed->_current_glist->_current->_prev->_prev;
			ed->_current_glist->_current->_prev->_next = ed->_current_glist->_current;
		}
		else
		{
			ed->_current_glist->_front = ed->_current_glist->_current;
			ed->_current_glist->_front->_prev = NULL;
		}

		GNode_free(node);
	}
}

void Editor_OnKey_Delete(Editor* ed)
{
	GNode* current_node = ed->_current_glist->_current;
	GNode* next_node = current_node->_next;
	GNode* prev_node = current_node->_prev;

	if (current_node)
	{
		if (!((current_node->_pGItem->_type == GITEM_CHAR) &&
			(((GItemChar*)current_node->_pGItem)->_ch == 0)))
		{
			if (prev_node)
			{
				next_node->_prev = prev_node;
				prev_node->_next = next_node;
				ed->_current_glist->_current = next_node;
			}
			else
			{
				next_node->_prev = prev_node;
				ed->_current_glist->_front = next_node;
				ed->_current_glist->_current = next_node;
			}

			GNode_free(current_node);
		}
	}
}

void Editor_OnChar_Return(Editor* ed)
{
}

void Editor_OnCmd(Editor* ed, int cmd, HWND hWnd)
{
	if (cmd == cmdButtonFraction)
	{
		Editor_OnChar_Default(ed, '/', hWnd);
	}
	else if (cmd == cmdButtonPower)
	{
		Editor_OnChar_Default(ed, '^', hWnd);
	}
	else if (cmd == cmdButtonRoot)
	{
		Editor_OnChar_Default(ed, ':', hWnd);
	}
	else if (cmd == cmdButtonSin)
	{
		Editor_OnChar_Default(ed, 'S', hWnd);
		Editor_OnChar_Default(ed, 'i', hWnd);
		Editor_OnChar_Default(ed, 'n', hWnd);
		Editor_OnChar_Default(ed, '(', hWnd);
		Editor_OnChar_Default(ed, ')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd, false, false);
	}
	else if (cmd == cmdButtonCos)
	{
		Editor_OnChar_Default(ed, 'C', hWnd);
		Editor_OnChar_Default(ed, 'o', hWnd);
		Editor_OnChar_Default(ed, 's', hWnd);
		Editor_OnChar_Default(ed, '(', hWnd);
		Editor_OnChar_Default(ed, ')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd, false, false);
	}
	else if (cmd == cmdButtonAsin)
	{
		Editor_OnChar_Default(ed, 'A', hWnd);
		Editor_OnChar_Default(ed, 's', hWnd);
		Editor_OnChar_Default(ed, 'i', hWnd);
		Editor_OnChar_Default(ed, 'n', hWnd);
		Editor_OnChar_Default(ed, '(', hWnd);
		Editor_OnChar_Default(ed, ')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd, false, false);
	}
	else if (cmd == cmdButtonAcos)
	{
		Editor_OnChar_Default(ed, 'A', hWnd);
		Editor_OnChar_Default(ed, 'c', hWnd);
		Editor_OnChar_Default(ed, 'o', hWnd);
		Editor_OnChar_Default(ed, 's', hWnd);
		Editor_OnChar_Default(ed, '(', hWnd);
		Editor_OnChar_Default(ed, ')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd, false, false);
	}
	else if (cmd == cmdButtonTan)
	{
		Editor_OnChar_Default(ed, 'T', hWnd);
		Editor_OnChar_Default(ed, 'a', hWnd);
		Editor_OnChar_Default(ed, 'n', hWnd);
		Editor_OnChar_Default(ed, '(', hWnd);
		Editor_OnChar_Default(ed, ')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd, false, false);
	}
	else if (cmd == cmdButtonAtan)
	{
		Editor_OnChar_Default(ed, 'A', hWnd);
		Editor_OnChar_Default(ed, 'T', hWnd);
		Editor_OnChar_Default(ed, 'a', hWnd);
		Editor_OnChar_Default(ed, 'n', hWnd);
		Editor_OnChar_Default(ed, '(', hWnd);
		Editor_OnChar_Default(ed, ')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd, false, false);
	}
	else if (cmd == cmdButtonLog)
	{
		Editor_OnChar_Default(ed, 'L', hWnd);
		Editor_OnChar_Default(ed, 'o', hWnd);
		Editor_OnChar_Default(ed, 'g', hWnd);
		Editor_OnChar_Default(ed, '(', hWnd);
		Editor_OnChar_Default(ed, ')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd, false, false);
	}
	else if (cmd == cmdButtonLn)
	{
		Editor_OnChar_Default(ed, 'L', hWnd);
		Editor_OnChar_Default(ed, 'n', hWnd);
		Editor_OnChar_Default(ed, '(', hWnd);
		Editor_OnChar_Default(ed, ')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd, false, false);
	}
	else if (cmd == cmdButtonExp)
	{
		Editor_OnChar_Default(ed, 'E', hWnd);
		Editor_OnChar_Default(ed, 'x', hWnd);
		Editor_OnChar_Default(ed, 'p', hWnd);
		Editor_OnChar_Default(ed, '(', hWnd);
		Editor_OnChar_Default(ed, ')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd, false, false);
	}
	else if (cmd == cmdButtonPi)
	{
		Editor_OnChar_Default(ed, L'\u03C0', hWnd);
	}
	else if (cmd == cmdButtonEuler)
	{
		Editor_OnChar_Default(ed, 'e', hWnd);
	}
	else if (cmd == cmdButtonTheta)
	{
		Editor_OnChar_Default(ed, L'\u03B8', hWnd);
	}
	else if (cmd == cmdButtonPhi)
	{
		Editor_OnChar_Default(ed, L'\u03C6', hWnd);
	}
}
