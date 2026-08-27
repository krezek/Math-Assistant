/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <gitems.h>

extern HFONT g_fontList[5];

GNode* GNode_init(GItem* pGItem, GNode* n, GNode* p)
{
	GNode* gn = (GNode*)malloc(sizeof(GNode));
	assert(gn);

	gn->_pGItem = pGItem;
	gn->_pNext = n;
	gn->_pPrev = p;

	return gn;
}

void GNode_free(GNode* pn)
{
	if (pn->_pGItem)
	{
		// call free function for each GItem object
		pn->_pGItem->_fnFree(pn->_pGItem);

		// deallocate GItem object
		free(pn->_pGItem);
		pn->_pGItem = NULL;
	}
	
	free(pn);
}

GList* GList_init(GList* parent)
{
	GList* gll = (GList*)malloc(sizeof(GList));
	assert(gll != NULL);

	gll->_pFront = NULL;
	gll->_pRear = NULL;

	gll->_pParent = parent;

	gll->_item_type = 0; // ITEM_Base ItemType

	return gll;
}

void GList_free(GList* gll)
{
	if (gll)
	{
		if (gll->_pFront)
		{
			while (gll->_pFront)
			{
				GNode* gn = gll->_pFront;
				gll->_pFront = gll->_pFront->_pNext;

				GNode_free(gn);
			}

			gll->_pFront = NULL;
			gll->_pRear = NULL;
		}

		free(gll);
	}
}

void GListTree_free(GList** ppGl)
{
	if(*ppGl)
		GList_free(*ppGl);
	*ppGl = NULL;
}

void GList_pushback(GList* gll, GItem* pGItem)
{
	if (gll->_pRear == NULL)
	{
		assert(gll->_pFront == NULL);
		gll->_pFront = gll->_pRear = GNode_init(pGItem, NULL, NULL);
	}
	else
	{
		GNode* i = GNode_init(pGItem, NULL, gll->_pRear);
		gll->_pRear->_pNext = i;
		gll->_pRear = i;
	}
}

void GList_toString(GList* gll, String* str)
{
	if (gll)
	{
		if (gll->_pFront)
		{
			GNode* gn = gll->_pFront;
			while (gn)
			{
				gn->_pGItem->_fnToString(gn->_pGItem, str);

				gn = gn->_pNext;
			}
		}
	}
}

void GList_calcFontId(GList* gll, int id)
{
	if (gll)
	{
		if (gll->_pFront)
		{
			GNode* gn = gll->_pFront;
			while (gn)
			{
				gn->_pGItem->_fnFontId(gn->_pGItem, id);

				gn = gn->_pNext;
			}
		}
	}
}

void GList_calcWidth(GList* gll, HDC hdc)
{
	int width = 0;

	if (gll)
	{
		if (gll->_pFront)
		{
			GNode* gn = gll->_pFront;
			while (gn)
			{
				width += gn->_pGItem->_fnWidth(gn->_pGItem, hdc);

				gn = gn->_pNext;
			}
		}
		gll->_width = width;
	}
}

void GList_calcHeight(GList* gll, HDC hdc)
{
	int topHeight = INT_MIN;
	int bottomHeight = INT_MIN;

	if (gll)
	{
		if (gll->_pFront)
		{
			GNode* gn = gll->_pFront;
			while (gn)
			{
				topHeight = max(gn->_pGItem->_fnY1(gn->_pGItem, hdc), topHeight);
				bottomHeight = max(gn->_pGItem->_fnY2(gn->_pGItem, hdc), bottomHeight);

				gn = gn->_pNext;
			}
		}
		gll->_height = topHeight + bottomHeight;
	}
}

void GList_calcBaseLineY(GList* gll, HDC hdc)
{
	int baseLineY = 0;

	if (gll)
	{
		if (gll->_pFront)
		{
			GNode* gn = gll->_pFront;
			while (gn)
			{
				baseLineY = max(gn->_pGItem->_fnBaseLineY(gn->_pGItem, hdc), baseLineY);

				gn = gn->_pNext;
			}
		}
		gll->_baseLineY = baseLineY;
	}
}

void GList_calcXY(GList* gll, HDC hdc, int x0, int y0)
{
	gll->_x0 = x0;
	gll->y0 = y0;

	int x = x0;

	if (gll)
	{
		if (gll->_pFront)
		{
			GNode* gn = gll->_pFront;
			while (gn)
			{
				gn->_pGItem->_fnCalcXY(gn->_pGItem, hdc, x, y0);
				x += gn->_pGItem->_width;

				gn = gn->_pNext;
			}
		}
	}
}

void GList_draw(GList* gll, HDC hdc, int x0, int y0)
{
	if (gll)
	{
		if (gll->_pFront)
		{
			GNode* gn = gll->_pFront;
			while (gn)
			{
				gn->_pGItem->_fnDraw(gn->_pGItem, hdc, x0, y0);

				gn = gn->_pNext;
			}
		}
	}
}

// -------------------- GItemChar ---------------------------------------------

void GItemChar_free(GItem* _this)
{
	GItemChar* g = (GItemChar*)_this;
}

void GItemChar_toString(GItem* _this, String* str)
{
	GItemChar* g = (GItemChar*)_this;
	char gstr[] = {0, 0, 0, 0, 0};

	switch (g->_ch)
	{
	case L'\u00D7':
		String_cat(str, "*");
		break;
	case L'\u03C0':
		String_cat(str, "pi");
		break;
	case L'\u03B8':
		String_cat(str, "theta");
		break;
	case L'\u03C6':
		String_cat(str, "phi");
		break;
	case L'\u222B':
		String_cat(str, "Integral");
		break;
	case L'\u2192':
		String_cat(str, ">>");
		break;

	default:
		wcstombs(gstr, &g->_ch, 1);
		String_cat(str, gstr);
	}

	
}

void GItemChar_fontId(GItem* _this, int id)
{
	GItemChar* g = (GItemChar*)_this;

	_this->_fontId = (id >= (sizeof(g_fontList)/sizeof(g_fontList[0]))) 
		? (sizeof(g_fontList) / sizeof(g_fontList[0])) - 1 : id;
}

int GItemChar_width(GItem* _this, HDC hdc)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);

	_this->_width = s.cx;

	return _this->_width;
}

int GItemChar_topHeight(GItem* _this, HDC hdc)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	_this->_height = s.cy;

	return _this->_height / 2;
}

int GItemChar_bottomHeight(GItem* _this, HDC hdc)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	_this->_height = s.cy;

	return _this->_height / 2;
}

int GItemChar_baseline_y(GItem* _this, HDC hdc)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	_this->_baseLineY = s.cy / 2;

	return _this->_baseLineY;
}

void GItemChar_calcXY(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);

	_this->_x0 = x0;
	_this->_y0 = y0 - s.cy / 2;
}

static COLORREF GetChColor(const wchar_t ch)
{
	static const wchar_t op[] = L"+-*/^!<>=|&%,;~:_\u00d7";
	static const wchar_t smb[] = L"e\u03C0";

	if (iswdigit(ch) || ch == L'.')
		return RGB(0, 100, 0);
	else if(wcschr(op, ch))
		return RGB(0, 0, 255);
	else if (wcschr(smb, ch))
		return RGB(100, 0, 0);

	return RGB(0, 0, 0);
}

void GItemChar_draw(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch , 0};

	RECT rc;
	rc.left = x0 + _this->_x0;
	rc.top = y0 + _this->_y0;
	rc.right = rc.left + _this->_width;
	rc.bottom = rc.top + _this->_height - 1;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	{
		SetDCPenColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, GetChColor(g->_ch));
	}
		
	TextOutW(hdc, x0 + _this->_x0, y0 + _this->_y0, str, 1);
}

GItemChar* GItemChar_init(int pl, const wchar_t ch)
{
	GItemChar* g = (GItemChar*)malloc(sizeof(GItemChar));
	assert(g != NULL);

	g->_gitem._type = GITEM_CHAR;
	g->_gitem._procLevel = pl;

	g->_gitem._fnFree = GItemChar_free;
	g->_gitem._fnToString = GItemChar_toString;
	g->_gitem._fnFontId = GItemChar_fontId;
	g->_gitem._fnWidth = GItemChar_width;
	g->_gitem._fnY1 = GItemChar_topHeight;
	g->_gitem._fnY2 = GItemChar_bottomHeight;
	g->_gitem._fnBaseLineY = GItemChar_baseline_y;
	g->_gitem._fnCalcXY = GItemChar_calcXY;
	g->_gitem._fnDraw = GItemChar_draw;

	g->_ch = ch;

	return g;
}

// -------------------- GItemPower --------------------------------------------

void GItemPower_free(GItem* _this)
{
	GItemPower* g = (GItemPower*)_this;

	GList_free(g->_pExponent);
}

void GItemPower_toString(GItem* _this, String* str)
{
	GItemPower* g = (GItemPower*)_this;
	char gstr1[] = { '^', '(', 0};
	char gstr2[] = { ')', 0 };

	String_cat(str, gstr1);
	GList_toString(g->_pExponent, str);
	String_cat(str, gstr2);
}

void GItemPower_fontId(GItem* _this, int id)
{
	GItemPower* g = (GItemPower*)_this;

	_this->_fontId = (id >= (sizeof(g_fontList) / sizeof(g_fontList[0])))
		? (sizeof(g_fontList) / sizeof(g_fontList[0])) - 1 : id;

	GList_calcFontId(g->_pExponent, _this->_fontId + 1);
}

int GItemPower_width(GItem* _this, HDC hdc)
{
	GItemPower* g = (GItemPower*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);
	
	GList_calcWidth(g->_pExponent, hdc);

	_this->_width = g->_pExponent->_width;

	wchar_t str[2] = { L'?', 0 };
	SIZE s;

	GetTextExtentPoint32W(hdc, str, 1, &s);
	if (g->_pExponent->_pFront == g->_pExponent->_pRear)
		_this->_width += s.cx;

	return _this->_width;
}

int GItemPower_topHeight(GItem* _this, HDC hdc)
{
	GItemPower* g = (GItemPower*)_this;
	wchar_t str[2] = { L'A', 0};
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	_this->_height = s.cy;

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcHeight(g->_pExponent, hdc);

	_this->_height += g->_pExponent->_height - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;

	return s.cy / 2 + g->_pExponent->_height - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;
}

int GItemPower_bottomHeight(GItem* _this, HDC hdc)
{
	GItemPower* g = (GItemPower*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	_this->_height = s.cy;

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcHeight(g->_pExponent, hdc);

	_this->_height += g->_pExponent->_height - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;

	return s.cy / 2;
}

int GItemPower_baseline_y(GItem* _this, HDC hdc)
{
	GItemPower* g = (GItemPower*)_this;
	wchar_t str[2] = { L'A', 0};
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	_this->_baseLineY = s.cy / 2 + g->_pExponent->_height - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;

	GList_calcBaseLineY(g->_pExponent, hdc);

	return _this->_baseLineY;
}

void GItemPower_calcXY(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemPower* g = (GItemPower*)_this;
	wchar_t str[2] = { L'A', 0};
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	_this->_x0 = x0;
	_this->_y0 = y0 - s.cy / 2 - g->_pExponent->_height + tm.tmDescent + tm.tmInternalLeading + tm.tmExternalLeading;

	GList_calcXY(g->_pExponent, hdc, _this->_x0, _this->_y0 + g->_pExponent->_baseLineY);
}

void GItemPower_draw(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemPower* g = (GItemPower*)_this;

	RECT rc;
	rc.left = x0 + _this->_x0;
	rc.top = y0 + _this->_y0;
	rc.right = rc.left + _this->_width;
	rc.bottom = rc.top + _this->_height;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	{
		SetDCPenColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(0, 0, 0));
	}

	if (g->_pExponent->_pFront->_pGItem->_type == GITEM_CHAR &&
		((GItemChar*)g->_pExponent->_pFront->_pGItem)->_ch == 0)
	{
		SetTextColor(hdc, RGB(255, 0, 0));
		TextOutW(hdc, x0 + _this->_x0, y0 + _this->_y0, L"?", 1);
	}

	GList_draw(g->_pExponent, hdc, x0, y0);
}

GItemPower* GItemPower_init(int pl, GList* exponent)
{
	GItemPower* g = (GItemPower*)malloc(sizeof(GItemPower));
	assert(g != NULL);

	g->_gitem._type = GITEM_POWER;
	g->_gitem._procLevel = pl;
	
	g->_gitem._fnFree = GItemPower_free;
	g->_gitem._fnToString = GItemPower_toString;
	g->_gitem._fnFontId = GItemPower_fontId;
	g->_gitem._fnWidth = GItemPower_width;
	g->_gitem._fnY1 = GItemPower_topHeight;
	g->_gitem._fnY2 = GItemPower_bottomHeight;
	g->_gitem._fnBaseLineY = GItemPower_baseline_y;
	g->_gitem._fnCalcXY = GItemPower_calcXY;
	g->_gitem._fnDraw = GItemPower_draw;

	g->_pExponent = exponent;

	return g;
}

// -------------------- GItemSubscript ----------------------------------------

void GItemSubscript_free(GItem* _this)
{
	GItemSubscript* g = (GItemSubscript*)_this;

	GList_free(g->_pButtomExpr);
}

void GItemSubscript_toString(GItem* _this, String* str)
{
	GItemSubscript* g = (GItemSubscript*)_this;
	char gstr1[] = { '_', '(', 0 };
	char gstr2[] = { ')', 0 };

	String_cat(str, gstr1);
	GList_toString(g->_pButtomExpr, str);
	String_cat(str, gstr2);
}

void GItemSubscript_fontId(GItem* _this, int id)
{
	GItemSubscript* g = (GItemSubscript*)_this;

	_this->_fontId = (id >= (sizeof(g_fontList) / sizeof(g_fontList[0])))
		? (sizeof(g_fontList) / sizeof(g_fontList[0])) - 1 : id;

	GList_calcFontId(g->_pButtomExpr, _this->_fontId + 1);
}

int GItemSubscript_width(GItem* _this, HDC hdc)
{
	GItemSubscript* g = (GItemSubscript*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_calcWidth(g->_pButtomExpr, hdc);

	_this->_width = g->_pButtomExpr->_width;

	wchar_t str[2] = { L'?', 0 };
	SIZE s;

	GetTextExtentPoint32W(hdc, str, 1, &s);
	if (g->_pButtomExpr->_pFront == g->_pButtomExpr->_pRear)
		_this->_width += s.cx;

	return _this->_width;
}

int GItemSubscript_topHeight(GItem* _this, HDC hdc)
{
	GItemSubscript* g = (GItemSubscript*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	_this->_height = s.cy;

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcHeight(g->_pButtomExpr, hdc);

	_this->_height += g->_pButtomExpr->_height - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;

	return s.cy / 2; 
}

int GItemSubscript_bottomHeight(GItem* _this, HDC hdc)
{
	GItemSubscript* g = (GItemSubscript*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	_this->_height = s.cy;

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcHeight(g->_pButtomExpr, hdc);

	_this->_height += g->_pButtomExpr->_height - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;

	return s.cy / 2 + g->_pButtomExpr->_height - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;
}

int GItemSubscript_baseline_y(GItem* _this, HDC hdc)
{
	GItemSubscript* g = (GItemSubscript*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	_this->_baseLineY = s.cy / 2;

	GList_calcBaseLineY(g->_pButtomExpr, hdc);

	return _this->_baseLineY;
}

void GItemSubscript_calcXY(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemSubscript* g = (GItemSubscript*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	_this->_x0 = x0;
	_this->_y0 = y0 + s.cy / 2 - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;

	GList_calcXY(g->_pButtomExpr, hdc, _this->_x0, _this->_y0 + g->_pButtomExpr->_baseLineY);
}

void GItemSubscript_draw(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemSubscript* g = (GItemSubscript*)_this;

	RECT rc;
	rc.left = x0 + _this->_x0;
	rc.top = y0 + _this->_y0;
	rc.right = rc.left + _this->_width;
	rc.bottom = rc.top + _this->_height;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	{
		SetDCPenColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(0, 0, 0));
	}

	if (g->_pButtomExpr->_pFront->_pGItem->_type == GITEM_CHAR &&
		((GItemChar*)g->_pButtomExpr->_pFront->_pGItem)->_ch == 0)
	{
		SetTextColor(hdc, RGB(255, 0, 0));
		TextOutW(hdc, x0 + _this->_x0, y0 + _this->_y0, L"?", 1);
	}

	GList_draw(g->_pButtomExpr, hdc, x0, y0);
}

GItemSubscript* GItemSubscript_init(int pl, GList* bottomExpr)
{
	GItemSubscript* g = (GItemSubscript*)malloc(sizeof(GItemSubscript));
	assert(g != NULL);

	g->_gitem._type = GITEM_SUBSCRIPT;
	g->_gitem._procLevel = pl;

	g->_gitem._fnFree = GItemSubscript_free;
	g->_gitem._fnToString = GItemSubscript_toString;
	g->_gitem._fnFontId = GItemSubscript_fontId;
	g->_gitem._fnWidth = GItemSubscript_width;
	g->_gitem._fnY1 = GItemSubscript_topHeight;
	g->_gitem._fnY2 = GItemSubscript_bottomHeight;
	g->_gitem._fnBaseLineY = GItemSubscript_baseline_y;
	g->_gitem._fnCalcXY = GItemSubscript_calcXY;
	g->_gitem._fnDraw = GItemSubscript_draw;

	g->_pButtomExpr = bottomExpr;

	return g;
}

// -------------------- GItemFraction ---------------------------------------------

void GItemFraction_free(GItem* _this)
{
	GItemDivision* g = (GItemDivision*)_this;
	
	GList_free(g->_pNumerator);
	GList_free(g->_pDenominator);

	g->_pNumerator = NULL;
	g->_pDenominator = NULL;
}

void GItemFraction_toString(GItem* _this, String* str)
{
	GItemDivision* g = (GItemDivision*)_this;
	char gstr1[] = { '(', 0 };
	char gstr2[] = { ')', 0 };
	char gstr3[] = { '/', 0 };

	String_cat(str, gstr1);
	String_cat(str, gstr1);
	GList_toString(g->_pNumerator, str);
	String_cat(str, gstr2);
	String_cat(str, gstr3);
	String_cat(str, gstr1);
	GList_toString(g->_pDenominator, str);
	String_cat(str, gstr2);
	String_cat(str, gstr2);

}

void GItemFraction_fontId(GItem* _this, int id)
{
	GItemDivision* g = (GItemDivision*)_this;

	_this->_fontId = (id >= (sizeof(g_fontList) / sizeof(g_fontList[0])))
		? (sizeof(g_fontList) / sizeof(g_fontList[0])) - 1 : id;

	GList_calcFontId(g->_pNumerator, _this->_fontId);
	GList_calcFontId(g->_pDenominator, _this->_fontId);
}

int GItemFraction_width(GItem* _this, HDC hdc)
{
	GItemDivision* g = (GItemDivision*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_calcWidth(g->_pNumerator, hdc);
	GList_calcWidth(g->_pDenominator, hdc);

	_this->_width = FRACTION_PADDING * 2 + max(g->_pNumerator->_width, g->_pDenominator->_width);

	return _this->_width;
}

int GItemFraction_topHeight(GItem* _this, HDC hdc)
{
	GItemDivision* g = (GItemDivision*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_calcHeight(g->_pNumerator, hdc);
	GList_calcHeight(g->_pDenominator, hdc);

	_this->_height = g->_pNumerator->_height + g->_pDenominator->_height;

	return g->_pNumerator->_height;
}

int GItemFraction_bottomHeight(GItem* _this, HDC hdc)
{
	GItemDivision* g = (GItemDivision*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_calcHeight(g->_pNumerator, hdc);
	GList_calcHeight(g->_pDenominator, hdc);

	_this->_height = g->_pNumerator->_height + g->_pDenominator->_height;

	return g->_pDenominator->_height;
}

int GItemFraction_baseline_y(GItem* _this, HDC hdc)
{
	GItemDivision* g = (GItemDivision*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	_this->_baseLineY = g->_pNumerator->_height;

	GList_calcBaseLineY(g->_pNumerator, hdc);
	GList_calcBaseLineY(g->_pDenominator, hdc);

	return _this->_baseLineY;
}

void GItemFraction_calcXY(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemDivision* g = (GItemDivision*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	_this->_x0 = x0;
	_this->_y0 = y0 - g->_pNumerator->_height;

	int w1 = (_this->_width - g->_pNumerator->_width) / 2;
	int w2 = (_this->_width - g->_pDenominator->_width) / 2;

	GList_calcXY(g->_pNumerator, hdc, _this->_x0 + w1, _this->_y0 + g->_pNumerator->_baseLineY);
	GList_calcXY(g->_pDenominator, hdc, _this->_x0 + w2, _this->_y0 + g->_pNumerator->_height + g->_pDenominator->_baseLineY);
}

void GItemFraction_draw(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemDivision* g = (GItemDivision*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	RECT rc;
	rc.left = x0 + _this->_x0;
	rc.top = y0 + _this->_y0;
	rc.right = rc.left + _this->_width;
	rc.bottom = rc.top + _this->_height;

	{
		SetDCPenColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(0, 0, 0));
	}

	GList_draw(g->_pNumerator, hdc, x0, y0);
	GList_draw(g->_pDenominator, hdc, x0, y0);

	{
		SetDCPenColor(hdc, RGB(0, 0, 0));
	}

	MoveToEx(hdc, x0 + _this->_x0, y0 + _this->_y0 + g->_pNumerator->_height, NULL);
	LineTo(hdc, x0 + _this->_x0 + _this->_width, y0 + _this->_y0 + g->_pNumerator->_height);
}

GItemDivision* GItemDivision_init(int pl, GList* num, GList* den)
{
	GItemDivision* g = (GItemDivision*)malloc(sizeof(GItemDivision));
	assert(g != NULL);

	g->_gitem._type = GITEM_FRACTION;
	g->_gitem._procLevel = pl;

	g->_gitem._fnFree = GItemFraction_free;
	g->_gitem._fnToString = GItemFraction_toString;
	g->_gitem._fnFontId = GItemFraction_fontId;
	g->_gitem._fnWidth = GItemFraction_width;
	g->_gitem._fnY1 = GItemFraction_topHeight;
	g->_gitem._fnY2 = GItemFraction_bottomHeight;
	g->_gitem._fnBaseLineY = GItemFraction_baseline_y;
	g->_gitem._fnCalcXY = GItemFraction_calcXY;
	g->_gitem._fnDraw = GItemFraction_draw;

	g->_pNumerator = num;
	g->_pDenominator = den;

	return g;
}

// -------------------- GItemRoot ---------------------------------------------

void GItemRoot_free(GItem* _this)
{
	GItemRoot* g = (GItemRoot*)_this;

	GList_free(g->_pExpr);
	GList_free(g->_pRoot);
}

void GItemRoot_toString(GItem* _this, String* str)
{
	GItemRoot* g = (GItemRoot*)_this;
	char gstr1[] = { 'R', 'o', 'o', 't', '(', 0 };
	char gstr2[] = { ',', 0 };
	char gstr3[] = { ')', 0 };
	char gstr4[] = { '2', 0 };

	String_cat(str, gstr1);
	GList_toString(g->_pExpr, str);
	String_cat(str, gstr2);

	if (g->_pRoot->_pFront == g->_pRoot->_pRear)
	{
		String_cat(str ,gstr4);
	}
	else
	{
		GList_toString(g->_pRoot, str);
	}
	
	String_cat(str, gstr3);
}

void GItemRoot_fontId(GItem* _this, int id)
{
	GItemRoot* g = (GItemRoot*)_this;

	_this->_fontId = (id >= (sizeof(g_fontList) / sizeof(g_fontList[0])))
		? (sizeof(g_fontList) / sizeof(g_fontList[0])) - 1 : id;

	GList_calcFontId(g->_pExpr, _this->_fontId);
	GList_calcFontId(g->_pRoot, _this->_fontId + 1);
}

int GItemRoot_width(GItem* _this, HDC hdc)
{
	GItemRoot* g = (GItemRoot*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_calcWidth(g->_pExpr, hdc);
	GList_calcWidth(g->_pRoot, hdc);

	_this->_width = ROOT_PADDING_V * 2 + g->_pExpr->_width + g->_pRoot->_width;

	return _this->_width;
}

int GItemRoot_topHeight(GItem* _this, HDC hdc)
{
	GItemRoot* g = (GItemRoot*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	//_this->_height = s.cy;

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcHeight(g->_pExpr, hdc);
	GList_calcHeight(g->_pRoot, hdc);

	_this->_height = ROOT_PADDING_H + max(g->_pRoot->_height,g->_pExpr->_height);
	
	return _this->_height / 2;
}

int GItemRoot_bottomHeight(GItem* _this, HDC hdc)
{
	GItemRoot* g = (GItemRoot*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	//_this->_height = s.cy;

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcHeight(g->_pExpr, hdc);
	GList_calcHeight(g->_pRoot, hdc);

	_this->_height = ROOT_PADDING_H + max(g->_pRoot->_height, g->_pExpr->_height);

	return _this->_height / 2;
}

int GItemRoot_baseline_y(GItem* _this, HDC hdc)
{
	GItemRoot* g = (GItemRoot*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcBaseLineY(g->_pExpr, hdc);
	GList_calcBaseLineY(g->_pRoot, hdc);

	_this->_baseLineY = ROOT_PADDING_H + g->_pExpr->_baseLineY;

	return _this->_baseLineY;
}

void GItemRoot_calcXY(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemRoot* g = (GItemRoot*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32W(hdc, str, 1, &s);
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	_this->_x0 = x0;
	_this->_y0 = y0 - _this->_baseLineY;

	GList_calcXY(g->_pRoot, hdc, _this->_x0, _this->_y0 + _this->_height - g->_pRoot->_height + g->_pRoot->_baseLineY - ROOT_PADDING_V);
	GList_calcXY(g->_pExpr, hdc, _this->_x0 + g->_pRoot->_width + ROOT_PADDING_V, _this->_y0 + _this->_baseLineY);
}

void GItemRoot_draw(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemRoot* g = (GItemRoot*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	RECT rc;
	rc.left = x0 + _this->_x0;
	rc.top = y0 + _this->_y0;
	rc.right = rc.left + _this->_width;
	rc.bottom = rc.top + _this->_height;

	{
		SetDCPenColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(0, 0, 0));
	}

	GList_draw(g->_pRoot, hdc, x0, y0);
	GList_draw(g->_pExpr, hdc, x0, y0);

	{
		SetDCPenColor(hdc, RGB(0, 0, 0));
	}

	MoveToEx(hdc, x0 + _this->_x0 + g->_pRoot->_width - 5, y0 + _this->_y0 + _this->_height - 5, NULL);
	LineTo(hdc, x0 + _this->_x0 + g->_pRoot->_width, y0 + _this->_y0 + _this->_height);
	LineTo(hdc, x0 + _this->_x0 + g->_pRoot->_width + ROOT_PADDING_V, y0 + _this->_y0 + 5);
	LineTo(hdc, x0 + _this->_x0 + g->_pRoot->_width + g->_pExpr->_width + ROOT_PADDING_V * 2, y0 + _this->_y0 + 5);

}

GItemRoot* GItemRoot_init(int pl, GList* expr, GList* root)
{
	GItemRoot* g = (GItemRoot*)malloc(sizeof(GItemRoot));
	assert(g != NULL);

	g->_gitem._type = GITEM_ROOT;
	g->_gitem._procLevel = pl;

	g->_gitem._fnFree = GItemRoot_free;
	g->_gitem._fnToString = GItemRoot_toString;
	g->_gitem._fnFontId = GItemRoot_fontId;
	g->_gitem._fnWidth = GItemRoot_width;
	g->_gitem._fnY1 = GItemRoot_topHeight;
	g->_gitem._fnY2 = GItemRoot_bottomHeight;
	g->_gitem._fnBaseLineY = GItemRoot_baseline_y;
	g->_gitem._fnCalcXY = GItemRoot_calcXY;
	g->_gitem._fnDraw = GItemRoot_draw;

	g->_pExpr = expr;
	g->_pRoot = root;

	return g;
}

//  ----------------------------- mouse issues ----------------------------------------------------

GNode* GList_GetGNodeFromPoint(GList* gll, int mx0, int my0)
{
	if (gll)
	{
		if (gll->_pFront)
		{
			GNode* gn = gll->_pFront;

			while (gn)
			{
				switch (gn->_pGItem->_type)
				{
				case GITEM_CHAR:
				{
					GItemChar* i = (GItemChar*)gn->_pGItem;

					POINT pt;
					pt.x = mx0;
					pt.y = my0;

					RECT rc1;
					rc1.left = i->_gitem._x0;
					rc1.top = i->_gitem._y0;
					rc1.right = rc1.left + i->_gitem._width / 2;
					rc1.bottom = rc1.top + i->_gitem._height;

					RECT rc2;
					rc2.left = i->_gitem._x0 + i->_gitem._width / 2;
					rc2.top = i->_gitem._y0;
					rc2.right = rc2.left + i->_gitem._width / 2;
					rc2.bottom = rc2.top + i->_gitem._height;

					if (PtInRect(&rc1, pt))
					{
						return gn;
					}

					if (PtInRect(&rc2, pt))
					{
						return gn->_pNext;
					}
				}
				break;

				case GITEM_FRACTION:
				{
					GItemDivision* i = (GItemDivision*)gn->_pGItem;

					POINT pt;
					pt.x = mx0;
					pt.y = my0;

					RECT rc1;
					rc1.left = i->_gitem._x0;
					rc1.top = i->_gitem._y0;
					rc1.right = rc1.left + i->_gitem._width / 2;
					rc1.bottom = rc1.top + i->_gitem._height;

					RECT rc2;
					rc2.left = i->_gitem._x0 + i->_gitem._width / 2;
					rc2.top = i->_gitem._y0;
					rc2.right = rc2.left + i->_gitem._width / 2;
					rc2.bottom = rc2.top + i->_gitem._height;

					GNode* node = GList_GetGNodeFromPoint(i->_pNumerator, mx0, my0);
					if (node)
					{
						return node;
					}
					node = GList_GetGNodeFromPoint(i->_pDenominator, mx0, my0);
					if (node)
					{
						return node;
					}

					if (PtInRect(&rc1, pt))
					{
						return gn;
					}

					if (PtInRect(&rc2, pt))
					{
						return gn->_pNext;
					}
				}
				break;

				case GITEM_POWER:
				{
					GItemPower* i = (GItemPower*)gn->_pGItem;

					POINT pt;
					pt.x = mx0;
					pt.y = my0;

					RECT rc1;
					rc1.left = i->_gitem._x0;
					rc1.top = i->_gitem._y0;
					rc1.right = rc1.left + i->_gitem._width / 2;
					rc1.bottom = rc1.top + i->_gitem._height;

					RECT rc2;
					rc2.left = i->_gitem._x0 + i->_gitem._width / 2;
					rc2.top = i->_gitem._y0;
					rc2.right = rc2.left + i->_gitem._width / 2;
					rc2.bottom = rc2.top + i->_gitem._height;

					GNode* node = GList_GetGNodeFromPoint(i->_pExponent, mx0, my0);
					if (node)
					{
						return node;
					}

					if (PtInRect(&rc1, pt))
					{
						return gn;
					}

					if (PtInRect(&rc2, pt))
					{
						return gn->_pNext;
					}
				}
				break;

				case GITEM_SUBSCRIPT:
				{
					GItemSubscript* i = (GItemSubscript*)gn->_pGItem;

					POINT pt;
					pt.x = mx0;
					pt.y = my0;

					RECT rc1;
					rc1.left = i->_gitem._x0;
					rc1.top = i->_gitem._y0;
					rc1.right = rc1.left + i->_gitem._width / 2;
					rc1.bottom = rc1.top + i->_gitem._height;

					RECT rc2;
					rc2.left = i->_gitem._x0 + i->_gitem._width / 2;
					rc2.top = i->_gitem._y0;
					rc2.right = rc2.left + i->_gitem._width / 2;
					rc2.bottom = rc2.top + i->_gitem._height;

					GNode* node = GList_GetGNodeFromPoint(i->_pButtomExpr, mx0, my0);
					if (node)
					{
						return node;
					}

					if (PtInRect(&rc1, pt))
					{
						return gn;
					}

					if (PtInRect(&rc2, pt))
					{
						return gn->_pNext;
					}
				}
				break;

				case GITEM_ROOT:
				{
					GItemRoot* i = (GItemRoot*)gn->_pGItem;

					POINT pt;
					pt.x = mx0;
					pt.y = my0;

					RECT rc1;
					rc1.left = i->_gitem._x0;
					rc1.top = i->_gitem._y0;
					rc1.right = rc1.left + i->_gitem._width / 2;
					rc1.bottom = rc1.top + i->_gitem._height;

					RECT rc2;
					rc2.left = i->_gitem._x0 + i->_gitem._width / 2;
					rc2.top = i->_gitem._y0;
					rc2.right = rc2.left + i->_gitem._width / 2;
					rc2.bottom = rc2.top + i->_gitem._height;

					GNode* node = GList_GetGNodeFromPoint(i->_pRoot, mx0, my0);
					if (node)
					{
						return node;
					}
					node = GList_GetGNodeFromPoint(i->_pExpr, mx0, my0);
					if (node)
					{
						return node;
					}

					if (PtInRect(&rc1, pt))
					{
						return gn;
					}

					if (PtInRect(&rc2, pt))
					{
						return gn->_pNext;
					}
				}
				break;
				}

				gn = gn->_pNext;
			}
		}
	}

	return NULL;
}

GList* GList_UpdateCurrent(GList* gll, GNode* node)
{
	if (gll)
	{
		if (gll->_pFront)
		{
			GNode* gn = gll->_pFront;

			while (gn)
			{
				gll->_pCurrent = gn;

				switch (gn->_pGItem->_type)
				{
				case GITEM_CHAR:
				{
					GItemChar* i = (GItemChar*)gn->_pGItem;

					if (gn == node)
					{
						gll->_pCurrent = gn;
						return gll;
					}
				}
				break;

				case GITEM_FRACTION:
				{
					GItemDivision* i = (GItemDivision*)gn->_pGItem;

					GList* pg;

					pg = GList_UpdateCurrent(i->_pNumerator, node);
					if (pg)
						return pg;

					pg = GList_UpdateCurrent(i->_pDenominator, node);
					if (pg)
						return pg;

					if (gn == node)
					{
						gll->_pCurrent = gn;
						return gll;
					}
				}
				break;

				case GITEM_POWER:
				{
					GItemPower* i = (GItemPower*)gn->_pGItem;

					GList* pg;

					pg = GList_UpdateCurrent(i->_pExponent, node);
					if (pg)
						return pg;

					if (gn == node)
					{
						gll->_pCurrent = gn;
						return gll;
					}
				}
				break;

				case GITEM_SUBSCRIPT:
				{
					GItemSubscript* i = (GItemSubscript*)gn->_pGItem;

					GList* pg;

					pg = GList_UpdateCurrent(i->_pButtomExpr, node);
					if (pg)
						return pg;

					if (gn == node)
					{
						gll->_pCurrent = gn;
						return gll;
					}
				}
				break;

				case GITEM_ROOT:
				{
					GItemRoot* i = (GItemRoot*)gn->_pGItem;

					GList* pg;

					pg = GList_UpdateCurrent(i->_pRoot, node);
					if (pg)
						return pg;

					pg = GList_UpdateCurrent(i->_pExpr, node);
					if (pg)
						return pg;

					if (gn == node)
					{
						gll->_pCurrent = gn;
						return gll;
					}
				}
				break;
				}

				gn = gn->_pNext;
			}
		}
	}

	return NULL;
}
