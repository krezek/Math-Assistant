#ifndef _PANEL_H_
#define _PANEL_H_

#include <gitems.h>
#include <editor.h>

typedef struct _Panel Panel;

typedef void (*PanelFreeFunc) (Panel* p);
typedef void (*PanelPaintFunc) (Panel* p, HDC hdc, int x0, int y0);
typedef void (*PanelPropertyChangedFunc) (Panel* p, HWND hWnd);
typedef Editor* (*PanelGetEditorFunc) (Panel* p);
typedef void (*PanelNextEditor) (Panel* p);
typedef void (*PanelSelectEditorFromPointFunc) (Panel* p, int mx0, int my0);

typedef enum { PT_GENERAL, PT_DRAW2D } PanelType;

typedef struct _Panel
{
	PanelType _type;
	int _x0, _y0, _width, _height;

	PanelFreeFunc _freeFunc;
	PanelPaintFunc _paintFunc;
	PanelPropertyChangedFunc _propertyChangedFunc;
	PanelGetEditorFunc _getEditorFunc;
	PanelNextEditor _nextEditorFunc;
	PanelSelectEditorFromPointFunc _selectEditorFromPointFunc;
} Panel;

typedef struct _GeneralPanel
{
	Panel _panel;

	wchar_t* _inStr, * _outStr;
	SIZE _inStrSize, _outStrSize, _paddingSize;

	GList* _in_gitems_list;
	GList* _out_gitems_list;

	Editor* _editor;
} GeneralPanel;

typedef struct _Draw2DPanel
{
	Panel _panel;

	wchar_t* _funcStr, * _intervalStr, *_stepStr;
	SIZE _funcStrSize, _intervalStrSize, _stepStrSize, _paddingSize;

	GList* _func_gitems_list;
	GList* _interval_gitems_list;
	GList* _step_gitems_list;

	Editor* _func_editor;
	Editor* _interval_editor;
	Editor* _step_editor;

	Editor* _current_editor;
} Draw2DPanel;

typedef struct _PanelNode
{
	Panel* _panel;

	struct _PanelNode* _next;
	struct _PanelNode* _prev;
} PanelNode;

typedef struct _PanelList
{
	PanelNode* _front;
	PanelNode* _rear;
} PanelList;

RECT Panel_GetRect(Panel* p);

GeneralPanel* GeneralPanel_init(const wchar_t* inStr, const wchar_t* outStr);
Draw2DPanel* Draw2DPanel_init(const wchar_t* funcStr, const wchar_t* intervalStr, const wchar_t* stepStr);

PanelList* PanelList_init();
void PanelList_free(PanelList* pl);
void PanelList_AddNewGeneralPanel(PanelList* pl, const wchar_t* inStr, const wchar_t* outStr);
void PanelList_AddNewDraw2DPanel(PanelList* pl, const wchar_t* funcStr, const wchar_t* intervalStr, const wchar_t* stepStr);
int PanelList_GetViewportWidth(PanelList* pl);
int PanelList_GetViewportHeight(PanelList* pl);
void PanelList_Paint(PanelList* pl, HDC hdc, RECT* rcPaint, int x0, int y0);
void PanelList_PropertyChangedEvent(PanelList* pl, BOOL all, Panel* effected, HWND hWnd, int x0, int y0);
Panel* PanelList_GetPanelFromPoint(PanelList* pl, int px, int py);

void PanelList_DeletePanel(PanelList* pl, Panel* p);

#endif /* _PANEL_H_ */
