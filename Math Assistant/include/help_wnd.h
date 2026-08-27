#ifndef _HELP_WND_H_
#define _HELP_WND_H_

typedef struct _HelpWindow HelpWindow;

typedef LRESULT(*HandleHelpMessageFunc) (HelpWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct _HelpWindow
{
	HWND _hWnd, _hWndParent;
	HWND _hWndBookLabel;
	HWND _hWndBookCombobox;
	HWND _hWndTree;
	HWND _hWndText;

	int _clientX, _clientY;
	int _clientWidth, _clientHeight;

	HandleHelpMessageFunc _fnHandleMessage;
} HelpWindow;

ATOM HelpWindow_RegisterClass();

HelpWindow* HelpWindow_init();
void HelpWindow_free(HelpWindow* hw);

BOOL HelpWindow_Create(HelpWindow* _this, HWND hWndParent);

#endif /* _HELP_WND_H_ */

