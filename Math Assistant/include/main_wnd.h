#ifndef _MAIN_WND_H_
#define _MAIN_WND_H_

#include <panel.h>
#include <calc_wnd.h>
#include <help_wnd.h>
#include <graphics_wnd.h>

typedef struct _PanelList PanelList;
typedef struct _MainWindow MainWindow;

typedef LRESULT(*HandleMessageFunc) (MainWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct _MainWindow
{
	HWND _hWnd;
	HWND _hWndTabControl;
	HWND _hWndStatusBar;
	HWND _hWndAbout;
	
	int _client_width, _client_height;
	
	CalcWindow* _calc_wnd;
	GraphicsWindow* _graphics_wnd;
	HelpWindow* _help_wnd;

	HandleMessageFunc _HandleMessageFunc;
} MainWindow;

ATOM MainWindow_RegisterClass();

MainWindow* MainWindow_init();
void MainWindow_free(MainWindow* mw);

BOOL MainWindow_Create(MainWindow* _this);

#endif /* _MAIN_WND_H_ */

