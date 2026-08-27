#ifndef _G_ITEMS_H_
#define _G_ITEMS_H_

#include <strg.h>

#define FRACTION_PADDING 10
#define ROOT_PADDING_H 5
#define ROOT_PADDING_V 10

typedef struct _GItem GItem;
typedef struct _GNode GNode;
typedef struct _GList GList;

typedef enum {GITEM_CHAR, GITEM_POWER, GITEM_FRACTION, GITEM_ROOT, GITEM_SUBSCRIPT} GItemType;

typedef void   (*FnFree)			(GItem* _this);
typedef void   (*FnToString)		(GItem* _this, String* str);
typedef void   (*FnFontId)			(GItem* _this, int id);
typedef int    (*FnWidth)			(GItem* _this, HDC hdc);
typedef int    (*FnY1)				(GItem* _this, HDC hdc);
typedef int    (*FnY2)				(GItem* _this, HDC hdc);
typedef int    (*FnBaseLineY)		(GItem* _this, HDC hdc);
typedef void   (*FnCalcXY)			(GItem* _this, HDC hdc, int x0, int y0);
typedef void   (*FnDraw)			(GItem* _this, HDC hdc, int x0, int y0);

typedef struct _GNode
{
	GItem* _pGItem;

	struct _GNode* _pNext;
	struct _GNode* _pPrev;
} GNode;

typedef struct _GList
{
	GNode* _pFront;
	GNode* _pRear;
	GNode* _pCurrent; // used by editor

	GList* _pParent;
	int _width, _height, _baseLineY;
	int _x0, y0;

	int _item_type; // used to determine parsed glist type
} GList;

typedef struct _GItem
{
	GItemType _type;
	int _procLevel;

	int _fontId;
	int _width, _height;
	int _baseLineY;
	int _x0, _y0;

	FnFree			_fnFree;
	FnFontId		_fnFontId;
	FnWidth			_fnWidth;
	FnY1			_fnY1;
	FnY2			_fnY2;
	FnBaseLineY		_fnBaseLineY;
	FnCalcXY		_fnCalcXY;
	FnDraw			_fnDraw;
	FnToString		_fnToString;
} GItem;

typedef struct
{
	GItem _gitem;

	wchar_t _ch;
} GItemChar;

typedef struct
{
	GItem _gitem;

	GList* _pExponent;
} GItemPower;

typedef struct
{
	GItem _gitem;

	GList* _pButtomExpr;
} GItemSubscript;

typedef struct
{
	GItem _gitem;

	GList* _pNumerator;
	GList* _pDenominator;
} GItemDivision;

typedef struct
{
	GItem _gitem;

	GList* _pExpr;
	GList* _pRoot;
} GItemRoot;

GNode* GNode_init(GItem* pGItem, GNode* n, GNode* p);
void GNode_free(GNode* pn);

GList* GList_init(GList* parent);
void GList_free(GList* gll);
void GList_pushback(GList* gll, GItem* pGItem);

void GList_calcFontId(GList* gll, int id);
void GList_calcWidth(GList* gll, HDC hdc);
void GList_calcHeight(GList* gll, HDC hdc);
void GList_calcBaseLineY(GList* gll, HDC hdc);
void GList_calcXY(GList* gll, HDC hdc, int x0, int y0);
void GList_draw(GList* gll, HDC hdc, int x0, int y0);

void GList_toString(GList* gll, String* str);

GNode* GList_GetGNodeFromPoint(GList* gll, int mx0, int my0);
GList* GList_UpdateCurrent(GList* gll, GNode* node);

GItemChar* GItemChar_init(int pl, const wchar_t ch);
GItemPower* GItemPower_init(int pl, GList* exponent);
GItemSubscript* GItemSubscript_init(int pl, GList* bottomExpr);
GItemDivision* GItemDivision_init(int pl, GList* num, GList* den);
GItemRoot* GItemRoot_init(int pl, GList* expr, GList* root);

void GListTree_free(GList** ppGl);


#endif /* _G_ITEMS_H_ */
