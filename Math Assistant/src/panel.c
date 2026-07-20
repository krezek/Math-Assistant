#include "pch.h"
#include "platform.h"

#include <proc.h>
#include <gitems.h>
#include <panel.h>

#define PANEL_LIST_MARGIN_H 10
#define PANEL_LIST_MARGIN_V 10
#define PANEL_MARGIN_H 5
#define PANEL_MARGIN_V 5

extern HFONT g_math_font;

RECT Panel_GetRect(Panel* p)
{
	RECT rc;

	rc.left = p->_x0;
	rc.top = p->_y0;

	rc.right = rc.left + p->_width;
	rc.bottom = rc.top + p->_height;

	return rc;
}

PanelNode* PanelNode_init(Panel* p, PanelNode* nxt, PanelNode* prv)
{
	PanelNode* pn = (PanelNode*)malloc(sizeof(PanelNode));
	assert(pn);

	pn->_panel = p;
	pn->_next = nxt;
	pn->_prev = prv;

	return pn;
}

void PanelNode_free(PanelNode* pn)
{
	if (pn->_panel)
	{
		pn->_panel->_freeFunc(pn->_panel);
	}

	free(pn);
}

PanelList* PanelList_init()
{
	PanelList* pl = (PanelList*)malloc(sizeof(PanelList));
	assert(pl != NULL);

	pl->_front = NULL;
	pl->_rear = NULL;

	return pl;
}

void PanelList_free(PanelList* pl)
{
	if (pl)
	{
		if (pl->_front)
		{
			while (pl->_front)
			{
				PanelNode* pn = pl->_front;
				pl->_front = pl->_front->_next;

				PanelNode_free(pn);
			}

			pl->_front = NULL;
			pl->_rear = NULL;
		}

		free(pl);
	}
}

void PanelList_pushpack(PanelList* pl, Panel* p)
{
	if (pl->_rear == NULL)
	{
		assert(pl->_front == NULL);
		pl->_front = pl->_rear = PanelNode_init(p, NULL, NULL);
	}
	else
	{
		PanelNode* pn = PanelNode_init(p, NULL, pl->_rear);
		pl->_rear->_next = pn;
		pl->_rear = pn;
	}
}

void PanelList_AddNewGeneralPanel(PanelList* pl, const wchar_t* inStr, const wchar_t* outStr)
{
	Panel* p = (Panel*)GeneralPanel_init(inStr, outStr);
	PanelList_pushpack(pl, p);
}

void PanelList_AddNewDraw2DPanel(PanelList* pl, 
	const wchar_t* funcStr, 
	const wchar_t* intervalStr,
	const wchar_t* stepStr)
{
	Panel* p = (Panel*)Draw2DPanel_init(funcStr, intervalStr, stepStr);
	PanelList_pushpack(pl, p);
}

int PanelList_GetViewportWidth(PanelList* pl)
{
	int w = 0;

	if (pl)
	{
		if (pl->_front)
		{
			PanelNode* pn = pl->_front;
			while (pn)
			{
				w = max(w, pn->_panel->_width);
				pn = pn->_next;
			}
		}
	}

	w += PANEL_LIST_MARGIN_V * 2;

	return w;
}

int PanelList_GetViewportHeight(PanelList* pl)
{
	int y = PANEL_LIST_MARGIN_H;

	if (pl)
	{
		if (pl->_front)
		{
			PanelNode* pn = pl->_front;
			while (pn)
			{
				y += pn->_panel->_height + PANEL_LIST_MARGIN_H;
				pn = pn->_next;
			}
		}
		else
			y += PANEL_LIST_MARGIN_H;
	}

	return y;
}

void PanelList_Paint(PanelList* pl, HDC hdc, RECT* rcPaint, int x0, int y0)
{
	if (pl)
	{
		if (pl->_front)
		{
			PanelNode* pn = pl->_front;
			while (pn)
			{
				Panel* p = pn->_panel;
				RECT rc, pRect;

				pRect.left = p->_x0 - x0;
				pRect.top = p->_y0 - y0;
				pRect.right = pRect.left + p->_width;
				pRect.bottom = pRect.top + p->_height;

				if (IntersectRect(&rc, rcPaint, &pRect))
				{
					pn->_panel->_paintFunc(pn->_panel, hdc, x0, y0);

					ExcludeClipRect(hdc, rc.left, rc.top, rc.right + 1, rc.bottom + 1);
				}

				pn = pn->_next;
			}
		}
	}
}

void PanelList_PropertyChangedEvent(PanelList* pl, BOOL all, Panel* effected, HWND hWnd, int x0, int y0)
{
	int x = PANEL_LIST_MARGIN_V;
	int y = PANEL_LIST_MARGIN_H;

	if (pl)
	{
		if (pl->_front)
		{
			PanelNode* pn = pl->_front;
			while (pn)
			{
				pn->_panel->_x0 = x;
				pn->_panel->_y0 = y;

				if(all || (effected && (effected == pn->_panel)))
					pn->_panel->_propertyChangedFunc(pn->_panel, hWnd);

				y += pn->_panel->_height + PANEL_LIST_MARGIN_H;

				pn = pn->_next;
			}
		}
	}
}

Panel* PanelList_GetPanelFromPoint(PanelList* pl, int px, int py)
{
	if (pl)
	{
		if (pl->_front)
		{
			PanelNode* pn = pl->_front;
			while (pn)
			{
				POINT pt;
				pt.x = px;
				pt.y = py;

				RECT rc = Panel_GetRect(pn->_panel);

				if (PtInRect(&rc, pt))
					return pn->_panel;

				pn = pn->_next;
			}
		}
	}

	return NULL;
}

void PanelList_DeletePanel(PanelList* pl, Panel* p)
{
	if (pl && pl->_front)
	{
		if (!((pl->_front == pl->_rear) && (pl->_front->_panel == p)))
		{
			if (pl->_front->_panel == p)
			{
				PanelNode* pn = pl->_front;
				pl->_front = pl->_front->_next;
				PanelNode_free(pn);
			}
			else
			{
				PanelNode* pn = pl->_front;

				while (pn)
				{
					PanelNode* pnn = pn->_next;
					if (pnn && (pnn->_panel == p))
					{
						if (pnn == pl->_rear)
							pl->_rear = pn;
						pn->_next = pnn->_next;
						PanelNode_free(pnn);
					}

					pn = pn->_next;
				}
			}
		}
	}
}

// General Panel

void GeneralPanel_free(Panel* gp)
{
	GeneralPanel* p = (GeneralPanel*)gp;
	Editor_free(p->_editor);

	GList_free(p->_in_gitems_list);
	GList_free(p->_out_gitems_list);

	free(p->_outStr);
	free(p->_inStr);

	free(p);
}

void GeneralPanel_Paint(Panel* gp, HDC hdc, int x0, int y0)
{
	GeneralPanel* p = (GeneralPanel*)gp;

	RECT rc;
	rc.left = p->_panel._x0 - x0;
	rc.top = p->_panel._y0 - y0;
	rc.right = rc.left + p->_panel._width + 1;
	rc.bottom = rc.top + p->_panel._height + 1;

	FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOWFRAME));
	HFONT hOldFont = SelectObject(hdc, g_math_font);
	SelectObject(hdc, GetStockObject(DC_PEN));

	{
		SetDCPenColor(hdc, RGB(255, 0, 0));
		MoveToEx(hdc, p->_panel._x0 - x0, p->_panel._y0 - y0, NULL);
		LineTo(hdc, p->_panel._x0 - x0, p->_panel._y0 - y0 + p->_panel._height);
		LineTo(hdc, p->_panel._x0 - x0 + 10, p->_panel._y0 - y0 + p->_panel._height);
		LineTo(hdc, p->_panel._x0 - x0, p->_panel._y0 - y0 + p->_panel._height + x0 - p->_panel._x0);
		MoveToEx(hdc, p->_panel._x0 - x0, p->_panel._y0 - y0, NULL);
		LineTo(hdc, p->_panel._x0 - x0 + 10, p->_panel._y0 - y0);
	}

	{
		SetDCPenColor(hdc, RGB(255, 0, 0));
		MoveToEx(hdc, p->_panel._x0 - x0 + p->_panel._width - 10, p->_panel._y0 - y0, NULL);
		LineTo(hdc, p->_panel._x0 - x0 + p->_panel._width, p->_panel._y0 - y0);
		LineTo(hdc, p->_panel._x0 - x0 + p->_panel._width, p->_panel._y0 - y0 + p->_panel._height);
		LineTo(hdc, p->_panel._x0 - x0 + p->_panel._width - 10, p->_panel._y0 - y0 + p->_panel._height);
	}

	{
		SetTextColor(hdc, RGB(255, 0, 0));
		TextOut(hdc,
			p->_panel._x0 - x0 + PANEL_MARGIN_V,
			p->_panel._y0 - y0 + PANEL_MARGIN_H +
			max(p->_inStrSize.cy, p->_in_gitems_list->_height) / 2 - p->_inStrSize.cy / 2,
			p->_inStr,
			(int)wcslen(p->_inStr));
		TextOut(hdc,
			p->_panel._x0 - x0 + PANEL_MARGIN_V,
			p->_panel._y0 - y0 + 2 * PANEL_MARGIN_H +
			max(p->_inStrSize.cy, p->_in_gitems_list->_height) +
			max(p->_outStrSize.cy, p->_out_gitems_list->_height) / 2 - p->_outStrSize.cy / 2,
			p->_outStr,
			(int)wcslen(p->_outStr));
	}

	SetTextColor(hdc, RGB(0, 0, 0));
	SetDCPenColor(hdc, RGB(0, 0, 0));

	if (p->_in_gitems_list)
	{
		GList_draw(p->_in_gitems_list, hdc, p->_panel._x0 - x0, p->_panel._y0 - y0);
	}

	if (p->_out_gitems_list)
	{
		GList_draw(p->_out_gitems_list, hdc, p->_panel._x0 - x0, p->_panel._y0 - y0);
	}

	SelectObject(hdc, hOldFont);
}

void GeneralPanel_PropertyChangedEvent(Panel* gp, HWND hWnd)
{
	GeneralPanel* p = (GeneralPanel*)gp;

	HDC hdc = GetDC(hWnd);
	SelectObject(hdc, g_math_font);
	SelectObject(hdc, GetStockObject(DC_PEN));

	GetTextExtentPoint(hdc, p->_inStr, (int)wcslen(p->_inStr), &p->_inStrSize);
	GetTextExtentPoint(hdc, p->_outStr, (int)wcslen(p->_outStr), &p->_outStrSize);
	GetTextExtentPoint(hdc, L"W", 1, &p->_paddingSize);

	GList_calcFontId(p->_in_gitems_list, 0);
	GList_calcWidth(p->_in_gitems_list, hdc);
	GList_calcHeight(p->_in_gitems_list, hdc);
	GList_calcBaseLineY(p->_in_gitems_list, hdc);
	GList_calcXY(p->_in_gitems_list, hdc,
		PANEL_MARGIN_V + p->_inStrSize.cx + p->_paddingSize.cx,
		PANEL_MARGIN_H + p->_in_gitems_list->_baseLineY);

	GList_calcFontId(p->_out_gitems_list, 0);
	GList_calcWidth(p->_out_gitems_list, hdc);
	GList_calcHeight(p->_out_gitems_list, hdc);
	GList_calcBaseLineY(p->_out_gitems_list, hdc);
	GList_calcXY(p->_out_gitems_list, hdc,
		PANEL_MARGIN_V + p->_inStrSize.cx + p->_paddingSize.cx,
		PANEL_MARGIN_H * 2 + p->_out_gitems_list->_baseLineY + p->_in_gitems_list->_height);

	ReleaseDC(hWnd, hdc);

	p->_panel._width = max(p->_inStrSize.cx, p->_outStrSize.cx) +
		p->_paddingSize.cx + 2 * PANEL_LIST_MARGIN_V +
		max(p->_in_gitems_list->_width, p->_out_gitems_list->_width);
	p->_panel._height = max(p->_inStrSize.cy, p->_in_gitems_list->_height) +
		max(p->_outStrSize.cy, p->_out_gitems_list->_height) +
		3 * PANEL_MARGIN_H;
}

Editor* GeneralPanel_GetEditor(Panel* gp)
{
	GeneralPanel* p = (GeneralPanel*)gp;
	return p->_editor;
}

void GeneralPanel_NextEditor(Panel* gp)
{

}

void GeneralPanel_SelectEditorFromPoint(Panel* gp, int mx0, int my0)
{

}

GeneralPanel* GeneralPanel_init(const wchar_t* inStr, const wchar_t* outStr)
{
	GeneralPanel* p = (GeneralPanel*)malloc(sizeof(GeneralPanel));
	assert(p != NULL);

	p->_panel._type = PT_GENERAL;

	p->_panel._freeFunc = GeneralPanel_free;
	p->_panel._paintFunc = GeneralPanel_Paint;
	p->_panel._propertyChangedFunc = GeneralPanel_PropertyChangedEvent;
	p->_panel._getEditorFunc = GeneralPanel_GetEditor;
	p->_panel._nextEditorFunc = GeneralPanel_NextEditor;
	p->_panel._selectEditorFromPointFunc = GeneralPanel_SelectEditorFromPoint;

	p->_inStr = (wchar_t*)malloc((wcslen(inStr) + 1) * sizeof(wchar_t));
	assert(p->_inStr != NULL);

	p->_outStr = (wchar_t*)malloc((wcslen(outStr) + 1) * sizeof(wchar_t));
	assert(p->_outStr != NULL);

	wcscpy(p->_inStr, inStr);
	wcscpy(p->_outStr, outStr);

	p->_in_gitems_list = GList_init(NULL);
	p->_out_gitems_list = GList_init(NULL);

	p->_editor = Editor_init();

	GList_pushback(p->_in_gitems_list, (GItem*)GItemChar_init(PROC_PRIMARY_2, 0));

	Editor_OnInit(p->_editor, p->_in_gitems_list);

	return p;
}

// Draw2D Panel

void Draw2DPanel_free(Panel* gp)
{
	Draw2DPanel* p = (Draw2DPanel*)gp;

	Editor_free(p->_func_editor);
	Editor_free(p->_interval_editor);
	Editor_free(p->_step_editor);

	GList_free(p->_func_gitems_list);
	GList_free(p->_interval_gitems_list);
	GList_free(p->_step_gitems_list);

	free(p->_funcStr);
	free(p->_intervalStr);
	free(p->_stepStr);

	free(p);
}

void Draw2DPanel_Paint(Panel* gp, HDC hdc, int x0, int y0)
{
	Draw2DPanel* p = (Draw2DPanel*)gp;

	RECT rc;
	rc.left = p->_panel._x0 - x0;
	rc.top = p->_panel._y0 - y0;
	rc.right = rc.left + p->_panel._width + 1;
	rc.bottom = rc.top + p->_panel._height + 1;

	FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOWFRAME));
	HFONT hOldFont = SelectObject(hdc, g_math_font);
	SelectObject(hdc, GetStockObject(DC_PEN));

	{
		SetDCPenColor(hdc, RGB(255, 0, 0));
		MoveToEx(hdc, p->_panel._x0 - x0, p->_panel._y0 - y0, NULL);
		LineTo(hdc, p->_panel._x0 - x0, p->_panel._y0 - y0 + p->_panel._height);
		LineTo(hdc, p->_panel._x0 - x0 + 10, p->_panel._y0 - y0 + p->_panel._height);
		LineTo(hdc, p->_panel._x0 - x0, p->_panel._y0 - y0 + p->_panel._height + x0 - p->_panel._x0);
		MoveToEx(hdc, p->_panel._x0 - x0, p->_panel._y0 - y0, NULL);
		LineTo(hdc, p->_panel._x0 - x0 + 10, p->_panel._y0 - y0);
	}

	{
		SetDCPenColor(hdc, RGB(255, 0, 0));
		MoveToEx(hdc, p->_panel._x0 - x0 + p->_panel._width - 10, p->_panel._y0 - y0, NULL);
		LineTo(hdc, p->_panel._x0 - x0 + p->_panel._width, p->_panel._y0 - y0);
		LineTo(hdc, p->_panel._x0 - x0 + p->_panel._width, p->_panel._y0 - y0 + p->_panel._height);
		LineTo(hdc, p->_panel._x0 - x0 + p->_panel._width - 10, p->_panel._y0 - y0 + p->_panel._height);
	}

	{
		SetTextColor(hdc, RGB(255, 0, 0));
		TextOut(hdc,
			p->_panel._x0 - x0 + PANEL_MARGIN_V,
			p->_panel._y0 - y0 + PANEL_MARGIN_H +
			max(p->_funcStrSize.cy, p->_func_gitems_list->_height) / 2 - p->_funcStrSize.cy / 2,
			p->_funcStr,
			(int)wcslen(p->_funcStr));
		TextOut(hdc,
			p->_panel._x0 - x0 + PANEL_MARGIN_V,
			p->_panel._y0 - y0 + 2 * PANEL_MARGIN_H +
			max(p->_funcStrSize.cy, p->_func_gitems_list->_height) +
			max(p->_intervalStrSize.cy, p->_interval_gitems_list->_height) / 2 - p->_intervalStrSize.cy / 2,
			p->_intervalStr,
			(int)wcslen(p->_intervalStr));
		TextOut(hdc,
			p->_panel._x0 - x0 + PANEL_MARGIN_V,
			p->_panel._y0 - y0 + 3 * PANEL_MARGIN_H +
			max(p->_funcStrSize.cy, p->_func_gitems_list->_height) +
			max(p->_intervalStrSize.cy, p->_interval_gitems_list->_height) +
			max(p->_stepStrSize.cy, p->_step_gitems_list->_height) / 2 - p->_stepStrSize.cy / 2,
			p->_stepStr,
			(int)wcslen(p->_stepStr));
	}

	SetTextColor(hdc, RGB(0, 0, 0));
	SetDCPenColor(hdc, RGB(0, 0, 0));

	if (p->_func_gitems_list)
	{
		GList_draw(p->_func_gitems_list, hdc, p->_panel._x0 - x0, p->_panel._y0 - y0);
	}

	if (p->_interval_gitems_list)
	{
		GList_draw(p->_interval_gitems_list, hdc, p->_panel._x0 - x0, p->_panel._y0 - y0);
	}

	if (p->_step_gitems_list)
	{
		GList_draw(p->_step_gitems_list, hdc, p->_panel._x0 - x0, p->_panel._y0 - y0);
	}

	SelectObject(hdc, hOldFont);
}

void Draw2DPanel_PropertyChangedEvent(Panel* gp, HWND hWnd)
{
	Draw2DPanel* p = (Draw2DPanel*)gp;

	HDC hdc = GetDC(hWnd);
	SelectObject(hdc, g_math_font);
	SelectObject(hdc, GetStockObject(DC_PEN));

	GetTextExtentPoint(hdc, p->_funcStr, (int)wcslen(p->_funcStr), &p->_funcStrSize);
	GetTextExtentPoint(hdc, p->_intervalStr, (int)wcslen(p->_intervalStr), &p->_intervalStrSize);
	GetTextExtentPoint(hdc, p->_stepStr, (int)wcslen(p->_stepStr), &p->_stepStrSize);
	GetTextExtentPoint(hdc, L"W", 1, &p->_paddingSize);

	GList_calcFontId(p->_func_gitems_list, 0);
	GList_calcWidth(p->_func_gitems_list, hdc);
	GList_calcHeight(p->_func_gitems_list, hdc);
	GList_calcBaseLineY(p->_func_gitems_list, hdc);
	GList_calcXY(p->_func_gitems_list, hdc,
		PANEL_MARGIN_V + p->_funcStrSize.cx + p->_paddingSize.cx,
		PANEL_MARGIN_H + p->_func_gitems_list->_baseLineY);

	GList_calcFontId(p->_interval_gitems_list, 0);
	GList_calcWidth(p->_interval_gitems_list, hdc);
	GList_calcHeight(p->_interval_gitems_list, hdc);
	GList_calcBaseLineY(p->_interval_gitems_list, hdc);
	GList_calcXY(p->_interval_gitems_list, hdc,
		PANEL_MARGIN_V + p->_intervalStrSize.cx + p->_paddingSize.cx,
		PANEL_MARGIN_H * 2 + p->_interval_gitems_list->_baseLineY + p->_func_gitems_list->_height);

	GList_calcFontId(p->_step_gitems_list, 0);
	GList_calcWidth(p->_step_gitems_list, hdc);
	GList_calcHeight(p->_step_gitems_list, hdc);
	GList_calcBaseLineY(p->_step_gitems_list, hdc);
	GList_calcXY(p->_step_gitems_list, hdc,
		PANEL_MARGIN_V + p->_stepStrSize.cx + p->_paddingSize.cx,
		PANEL_MARGIN_H * 3 + p->_step_gitems_list->_baseLineY + 
		p->_func_gitems_list->_height +
		p->_interval_gitems_list->_height);

	ReleaseDC(hWnd, hdc);

	p->_panel._width = max(max(p->_funcStrSize.cx, p->_intervalStrSize.cx), p->_stepStrSize.cx) +
		p->_paddingSize.cx + 2 * PANEL_LIST_MARGIN_V +
		max(max(p->_func_gitems_list->_width, p->_interval_gitems_list->_width), p->_step_gitems_list->_width);
	p->_panel._height = max(p->_funcStrSize.cy, p->_func_gitems_list->_height) +
		max(p->_intervalStrSize.cy, p->_interval_gitems_list->_height) +
		max(p->_stepStrSize.cy, p->_step_gitems_list->_height) +
		4 * PANEL_MARGIN_H;
}

Editor* Draw2DPanel_GetEditor(Panel* gp)
{
	Draw2DPanel* p = (Draw2DPanel*)gp;
	return p->_current_editor;
}

void Draw2DPanel_NextEditor(Panel* gp)
{
	Draw2DPanel* p = (Draw2DPanel*)gp;
	if (p->_current_editor == p->_func_editor)
		p->_current_editor = p->_interval_editor;
	else if (p->_current_editor == p->_interval_editor)
		p->_current_editor = p->_step_editor;
	else if (p->_current_editor == p->_step_editor)
		p->_current_editor = p->_func_editor;
}

void Draw2DPanel_SelectEditorFromPoint(Panel* gp, int mx0, int my0)
{
	GNode* node = NULL;
	Draw2DPanel* p = (Draw2DPanel*)gp;

	node = GList_GetGNodeFromPoint(p->_func_gitems_list, mx0, my0);

	if (node)
	{
		p->_current_editor = p->_func_editor;
		return;
	}

	node = GList_GetGNodeFromPoint(p->_interval_gitems_list, mx0, my0);

	if (node)
	{
		p->_current_editor = p->_interval_editor;
		return;
	}

	node = GList_GetGNodeFromPoint(p->_step_gitems_list, mx0, my0);

	if (node)
	{
		p->_current_editor = p->_step_editor;
		return;
	}
}

Draw2DPanel* Draw2DPanel_init(const wchar_t* funcStr, const wchar_t* intervalStr, const wchar_t* stepStr)
{
	Draw2DPanel* p = (Draw2DPanel*)malloc(sizeof(Draw2DPanel));
	assert(p != NULL);

	p->_panel._type = PT_DRAW2D;

	p->_panel._freeFunc = Draw2DPanel_free;
	p->_panel._paintFunc = Draw2DPanel_Paint;
	p->_panel._propertyChangedFunc = Draw2DPanel_PropertyChangedEvent;
	p->_panel._getEditorFunc = Draw2DPanel_GetEditor;
	p->_panel._nextEditorFunc = Draw2DPanel_NextEditor;
	p->_panel._selectEditorFromPointFunc = Draw2DPanel_SelectEditorFromPoint;

	p->_funcStr = (wchar_t*)malloc((wcslen(funcStr) + 1) * sizeof(wchar_t));
	assert(p->_funcStr != NULL);

	p->_intervalStr = (wchar_t*)malloc((wcslen(intervalStr) + 1) * sizeof(wchar_t));
	assert(p->_intervalStr != NULL);

	p->_stepStr = (wchar_t*)malloc((wcslen(stepStr) + 1) * sizeof(wchar_t));
	assert(p->_stepStr != NULL);
	
	wcscpy(p->_funcStr, funcStr);
	wcscpy(p->_intervalStr, intervalStr);
	wcscpy(p->_stepStr, stepStr);

	p->_func_gitems_list = GList_init(NULL);
	p->_interval_gitems_list = GList_init(NULL);
	p->_step_gitems_list = GList_init(NULL);

	p->_func_editor = Editor_init();
	p->_interval_editor = Editor_init();
	p->_step_editor = Editor_init();

	GList_pushback(p->_func_gitems_list, (GItem*)GItemChar_init(PROC_PRIMARY_2, 0));
	GList_pushback(p->_interval_gitems_list, (GItem*)GItemChar_init(PROC_PRIMARY_2, 0));
	GList_pushback(p->_step_gitems_list, (GItem*)GItemChar_init(PROC_PRIMARY_2, 0));

	Editor_OnInit(p->_func_editor, p->_func_gitems_list);
	Editor_OnInit(p->_interval_editor, p->_interval_gitems_list);
	Editor_OnInit(p->_step_editor, p->_step_gitems_list);

	p->_current_editor = p->_func_editor;

	return p;
}
