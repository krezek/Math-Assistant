#ifndef _MAIN_WND_H_
#define _MAIN_WND_H_

#include <panel.h>
#include <calc_wnd.h>
#include <help_wnd.h>
#include <graphics_wnd.h>

typedef struct _PanelList PanelList;
typedef struct _MainWindow MainWindow;

typedef LRESULT(*FnHandleMessage) (MainWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct _MainWindow
{
	HWND _hWnd;
	HWND _hWndTabControl;
	HWND _hWndStatusBar;
	HWND _hWndAbout;
	
	int _clientWidth, _clientHeight;
	
	CalcWindow* _pCalcWnd;
	GraphicsWindow* _pGraphicsWnd;
	HelpWindow* _pHelpWnd;

	FnHandleMessage _fnHandleMessage;
} MainWindow;

ATOM MainWindow_RegisterClass();

MainWindow* MainWindow_init();
void MainWindow_free(MainWindow* mw);

BOOL MainWindow_Create(MainWindow* _this);

#endif /* _MAIN_WND_H_ */

