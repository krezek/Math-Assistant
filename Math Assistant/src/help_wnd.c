#include "pch.h"
#include "platform.h"

#include <winutil.h>

#include <help_wnd.h>

#define IDL_BOOK_LABEL      1001
#define IDC_BOOK_COMBOBOX   1002
#define IDT_BOOK_TREE       1003
#define IDR_BOOK_EDIT       1004

extern int g_ribbon_height;
extern int g_statusbar_height;

extern const int g_splitter_width;
extern int g_splitterX;

extern const int g_tab_control_height;

extern int Populate_Books(HWND hWndBookCombobox);
extern int Populate_Tree(HWND hWndTree, int index);
extern void LoadText(HWND hWndText, const wchar_t* text, int index);
extern void OnDBClick_treeView(HWND hWndTree, HWND hWndText, WPARAM wParam, LPARAM lParam, int index);

void OnNotify_treeView(HelpWindow* hw, WPARAM wParam, LPARAM lParam);

int g_book_index = 0;

static TCHAR szWindowClass[] = _T("HelpWindowClass");

static LRESULT CALLBACK DefaultWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT HandleMessage(HelpWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

static LRESULT OnCreate(HelpWindow* hw);
static LRESULT OnDestroy(HelpWindow* hw);
static LRESULT OnSize(HelpWindow* hw);

ATOM HelpWindow_RegisterClass()
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_ENABLE;
    wcex.lpfnWndProc = DefaultWindow_Proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);
}

static LRESULT CALLBACK DefaultWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HelpWindow* pThis;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (HelpWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->_hWnd = hWnd;
    }
    else
    {
        pThis = (HelpWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        return pThis->_HandleMessageFunc(pThis, uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

HelpWindow* HelpWindow_init()
{
    HelpWindow* hw = (HelpWindow*)malloc(sizeof(HelpWindow));
    assert(hw != NULL);

    hw->_HandleMessageFunc = HandleMessage;

    hw->_hWnd = NULL;

    return hw;
}

void HelpWindow_free(HelpWindow* mw)
{
    free(mw);
}

BOOL HelpWindow_Create(HelpWindow * _this, HWND hWndParent)
{
    _this->_hWndParent = hWndParent;

    HWND hWnd = CreateWindow(
        szWindowClass,
        NULL,
        WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
        0, 0,
        0, 0,
        _this->_hWndParent,
        NULL,
        GetModuleHandle(NULL),
        _this
    );

    if (!hWnd)
        return FALSE;

    _this->_hWnd = hWnd;

    ShowWindow(_this->_hWnd, SW_HIDE);

    return TRUE;
}

static LRESULT HandleMessage(HelpWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        return OnCreate(_this);

    case WM_DESTROY:
        return OnDestroy(_this);

    case WM_SIZE:
        return OnSize(_this);

    case WM_COMMAND:
        if ((HIWORD(wParam) == CBN_SELCHANGE) && (LOWORD(wParam) == IDC_BOOK_COMBOBOX))
        {
            HWND hCombo = (HWND)lParam;
            int index = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
            if (index != CB_ERR)
            {
                Populate_Tree(_this->_hWndTree, g_book_index = index);
            }
        }
        return 0;

    case WM_NOTIFY:
        if (((LPNMHDR)lParam)->hwndFrom == _this->_hWndTree)
            OnNotify_treeView(_this, wParam, lParam);
        return 0;



    default:
        return DefWindowProc(_this->_hWnd, uMsg, wParam, lParam);
    }
}

static LRESULT OnCreate(HelpWindow* hw)
{
    // Create Book label and Combobox

    hw->_hWndBookLabel = CreateWindowEx(
        0,
        WC_STATIC,
        L"Book",
        SBARS_SIZEGRIP |
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS |
        SS_CENTER | SS_CENTERIMAGE,
        0, 0, 0, 0,
        hw->_hWnd,
        (HMENU)IDL_BOOK_LABEL,
        NULL,
        NULL);
    if (!hw->_hWndBookLabel)
    {
        ShowError(_T("HelpWindow::OnCreate::Unable to Create Book Label"));
        return -1;
    }

    hw->_hWndBookCombobox = CreateWindowEx(
        0,
        WC_COMBOBOX,
        (PCTSTR)NULL,
        SBARS_SIZEGRIP | CBS_DROPDOWN | CBS_HASSTRINGS |
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS |
        SS_CENTER | SS_CENTERIMAGE,
        0, 0, 0, 0,
        hw->_hWnd,
        (HMENU)IDC_BOOK_COMBOBOX,
        NULL,
        NULL);
    if (!hw->_hWndBookCombobox)
    {
        ShowError(_T("HelpWindow::OnCreate::Unable to Create Book Combobox"));
        return -1;
    }

    // Create Tree
    hw->_hWndTree = CreateWindowEx(
        0,
        WC_TREEVIEW,
        (PCTSTR)NULL,
        WS_VISIBLE | WS_CHILD | TVS_HASLINES | WS_BORDER | WS_CLIPSIBLINGS,
        0, 0, 0, 0,
        hw->_hWnd,
        (HMENU)IDT_BOOK_TREE,
        NULL,
        NULL);

    if (!hw->_hWndTree)
    {
        ShowError(_T("HelpWindow::OnCreate::Unable to Create Tree View"));
        return -1;
    }

    // Create RichEdit information
    hw->_hWndText = CreateWindowEx(
        0,
        MSFTEDIT_CLASS,
        (PCTSTR)NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_TABSTOP | WS_VSCROLL,
        0, 0, 0, 0,
        hw->_hWnd,
        (HMENU)IDR_BOOK_EDIT,
        NULL,
        NULL);

    if (!hw->_hWndText)
    {
        ShowError(_T("HelpWindow::OnCreate::Unable to Create RichEdit Info"));
        return -1;
    }

    Populate_Books(hw->_hWndBookCombobox);

    PostMessage(hw->_hWndBookCombobox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
    Populate_Tree(hw->_hWndTree, g_book_index = 0);
    LoadText(hw->_hWndText, L"About", 0);


    return 0;
}

static LRESULT OnDestroy(HelpWindow* hw)
{
    return 0;
}

static LRESULT OnSize(HelpWindow* hw)
{
    const int book_row_height = 25;
    const double view_vfactor = 0.33;

    MoveWindow(hw->_hWnd,
        hw->_client_x,
        hw->_client_y,
        hw->_client_width,
        hw->_client_height,
        TRUE);

    MoveWindow(hw->_hWndBookLabel, 0, 0,
        75, book_row_height, TRUE);

    MoveWindow(hw->_hWndBookCombobox, 75, 0,
        hw->_client_width - 75, book_row_height, TRUE);

    MoveWindow(hw->_hWndTree, 0, book_row_height,
        hw->_client_width,
        (int)((hw->_client_height - book_row_height) * view_vfactor), TRUE);

    MoveWindow(hw->_hWndText, 0, book_row_height +
        (int)((hw->_client_height - book_row_height) * view_vfactor),
        hw->_client_width,
        (int)((hw->_client_height - book_row_height) * (1.0 - view_vfactor)), TRUE);
    return 0;
}

static void OnNotify_treeView(HelpWindow* hw, WPARAM wParam, LPARAM lParam)
{
    switch (((LPNMHDR)lParam)->code)
    {
    case NM_DBLCLK:
    case NM_RETURN:
        OnDBClick_treeView(hw->_hWndTree, hw->_hWndText, wParam, lParam, g_book_index);
        break;
    }
}
