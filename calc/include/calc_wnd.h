#ifndef _CALC_WND_H_
#define _CALC_WND_H_

#include <panel.h>

#define WM_SETFONTSIZE WM_USER + 1
#define WM_STATUS_BAR_MSG WM_USER + 2

#define WM_DRAWING_MSG WM_USER + 3

typedef struct _PanelList PanelList;
typedef struct _CalcWindow CalcWindow;

typedef LRESULT(*FnHandleCalcMessage) (CalcWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct _CalcWindow
{
	HWND _hWnd, _hWndParent;
	HWND _hWndVScrollBar, _hWndHScrollBar, _hWndCorner;

	int _clientX, _clientY;
	int _clientWidth, _clientHeight;
	int _xCurrentPos, _yCurrent;
	int _xMaxScroll, _yMaxScroll;

	PanelList* _pPanelList;
	Panel* _selectedPanel;
	Panel* _managedPanel;

	FnHandleCalcMessage _fnHandleMessage;
} CalcWindow;

ATOM CalcWindow_RegisterClass();

CalcWindow* CalcWindow_init();
void CalcWindow_free(CalcWindow* mw);

BOOL CalcWindow_Create(CalcWindow* _this, HWND hWndParent);

LRESULT CalcWindowRibbonCommand(CalcWindow* cw, int cmd);

#endif /* _CALC_WND_H */

