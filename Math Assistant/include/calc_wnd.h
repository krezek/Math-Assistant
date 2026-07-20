#ifndef _CALC_WND_H_
#define _CALC_WND_H_

#include <panel.h>

#define WM_SETFONTSIZE WM_USER + 1
#define WM_STATUS_BAR_MSG WM_USER + 2

#define WM_DRAWING_MSG WM_USER + 3

typedef struct _PanelList PanelList;
typedef struct _CalcWindow CalcWindow;

typedef LRESULT(*HandleCalcMessageFunc) (CalcWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct _CalcWindow
{
	HWND _hWnd, _hWndParent;
	HWND _hWndVScrollBar, _hWndHScrollBar, _hWndCorner;

	int _client_x, _client_y;
	int _client_width, _client_height;
	int _x_current_pos, _y_current_pos;
	int _xMaxScroll, _yMaxScroll;

	PanelList* _panelList;
	Panel* _selected_panel;
	Panel* _managed_panel;

	HandleCalcMessageFunc _HandleMessageFunc;
} CalcWindow;

ATOM CalcWindow_RegisterClass();

CalcWindow* CalcWindow_init();
void CalcWindow_free(CalcWindow* mw);

BOOL CalcWindow_Create(CalcWindow* _this, HWND hWndParent);

LRESULT CalcWindowRibbonCommand(CalcWindow* cw, int cmd);

#endif /* _CALC_WND_H */

