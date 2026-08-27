/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"
#include "platform.h"

#include <..\Math Assistant\resource.h>
#include <winutil.h>
#include <calc.h>
#include <simplify.h>
#include <editor.h>

#include <calc_wnd.h>


#define SCROLLBAR_WIDE 20
#define INITIAL_FONTSIZE 14

extern int g_ribbon_height;
extern int g_statusbar_height;

extern const int g_splitter_width;
extern int g_splitterX;

extern void init_rules_library();
extern void destroy_rules_library();

static TCHAR szWindowClass[] = _T("CalcWindowClass");

HFONT g_math_font;
HFONT g_fontList[5];

void Graphics_fontList_init(HANDLE hFont);
void Graphics_fontList_free();

static LRESULT CALLBACK DefaultWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT HandleMessage(CalcWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

static LRESULT OnCreate(CalcWindow* cw);
static LRESULT OnDestroy(CalcWindow* cw);
static LRESULT OnSize(CalcWindow* cw);

static LRESULT OnPaint(CalcWindow* cw);
static LRESULT OnSetFontSize(CalcWindow* cw, int fsize);

static void SetScrollbarInfo(CalcWindow* cw);
static LRESULT OnHScroll(CalcWindow* cw, WPARAM wParam);
static LRESULT OnVScroll(CalcWindow* cw, WPARAM wParam);

static LRESULT OnSetFocus(CalcWindow* cw);
static LRESULT OnKillFocus(CalcWindow* cw);

static LRESULT OnMousLButtonDown(CalcWindow* cw, int x, int y);
static LRESULT OnMouseWheel(CalcWindow* cw, WPARAM wParam);

static LRESULT OnSetFontSize(CalcWindow* cw, int fsize);

static LRESULT OnKeyDown(CalcWindow* cw, WPARAM wParam, LPARAM lParam);
static LRESULT OnChar(CalcWindow* cw, WPARAM wParam, LPARAM lParam);

static BOOL OnContextMenu(CalcWindow* cw, int x, int y);
static VOID DisplayContextMenu(CalcWindow* cw, POINT pt);

extern const wchar_t* MParser_get_last_error(); 
extern int parse_gitems(GList** ppGl, const char* s);

VOID OnCommand_PanelsNewGeneral(CalcWindow* cw);
VOID OnCommand_PanelsNewDraw2D(CalcWindow* cw);
VOID OnCommand_PanelsDelete(CalcWindow* cw);
VOID OnCommand_PanelsUp(CalcWindow* cw);
VOID OnCommand_PanelsDown(CalcWindow* cw);

static void SetStatusBarText(CalcWindow* cw, const wchar_t* txt);

void Calculate(CalcWindow* cw);
void Simplify(CalcWindow* cw);
void Drawing(CalcWindow* cw);

ATOM CalcWindow_RegisterClass()
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
    CalcWindow* pThis;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (CalcWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->_hWnd = hWnd;
    }
    else
    {
        pThis = (CalcWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    if (pThis)
    {
        return pThis->_fnHandleMessage(pThis, uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

CalcWindow* CalcWindow_init()
{
    CalcWindow* cw = (CalcWindow*)malloc(sizeof(CalcWindow));
    assert(cw != NULL);

    cw->_fnHandleMessage = HandleMessage;

    cw->_hWnd = NULL;
    cw->_xCurrentPos = cw->_yCurrent = 0;

    cw->_pPanelList = PanelList_init();
    cw->_selectedPanel = NULL;
    cw->_managedPanel = NULL;

    return cw;
}

void CalcWindow_free(CalcWindow* mw)
{
    PanelList_free(mw->_pPanelList);
    free(mw);
}

BOOL CalcWindow_Create(CalcWindow * _this, HWND hWndParent)
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

    return TRUE;
}

static LRESULT HandleMessage(CalcWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        return OnCreate(_this);

    case WM_DESTROY:
        return OnDestroy(_this);

    case WM_SIZE:
        return OnSize(_this);

    case WM_PAINT:
        return OnPaint(_this);

    case WM_SETFOCUS:
        return OnSetFocus(_this);

    case WM_KILLFOCUS:
        return OnKillFocus(_this);

    case WM_VSCROLL:
        return OnVScroll(_this, wParam);

    case WM_HSCROLL:
        return OnHScroll(_this, wParam);

    case WM_KEYDOWN:
        return OnKeyDown(_this, wParam, lParam);

    case WM_CHAR:
    {
        static int x = 2;
        if (wParam == L'^')
        {
            --x;
            if (x) return 0;
            x = 2;
        }

        return OnChar(_this, wParam, lParam);
    }
        
    case WM_CONTEXTMENU:
        if (OnContextMenu(_this, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
            return 0L;
        else
            return DefWindowProc(_this->_hWnd, uMsg, wParam, lParam);


    case WM_MOUSEWHEEL:
        return OnMouseWheel(_this, wParam);

    case WM_LBUTTONDOWN:
        return OnMousLButtonDown(_this, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    case WM_RBUTTONDOWN:
        return OnMousLButtonDown(_this, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_PANELS_NEW_GENERAL:
            OnCommand_PanelsNewGeneral(_this);
            break;
        case IDM_PANELS_NEW_DRAW2D:
            OnCommand_PanelsNewDraw2D(_this);
            break;
        case IDM_PANELS_DELETE:
            OnCommand_PanelsDelete(_this);
            break;
        case IDM_CALC:
            Calculate(_this);
            break;
        case IDM_SIMPLIFY:
            Simplify(_this);
            break;
        case IDM_DRAW:
            PostMessage(_this->_hWndParent, WM_DRAWING_MSG, 1, 0);
            break;
        case IDM_PANELS_UP:
            OnCommand_PanelsUp(_this);
            break;
        case IDM_PANELS_DOWN:
            OnCommand_PanelsDown(_this);
            break;
        }
        return 0;

    case WM_SETFONTSIZE:
        return OnSetFontSize(_this, (int)(wParam));    

    default:
        return DefWindowProc(_this->_hWnd, uMsg, wParam, lParam);
    }
}

static LRESULT OnCreate(CalcWindow* cw)
{
    init_rules_library();

    // Create Font
    HDC hdc = GetDC(cw->_hWnd);
    int lfHeight = -MulDiv(INITIAL_FONTSIZE, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    g_math_font = CreateFont(lfHeight, 0, 0, 0, FALSE,
        FALSE, 0, 0, 0, 0, 0, 0, 0, L"Cambria");
    if (!g_math_font)
    {
        ShowError(_T("MainWindow::OnCreate::unable to create math font"));
        return -1;
    }

    ReleaseDC(cw->_hWnd, hdc);

    Graphics_fontList_init(g_math_font);

    cw->_hWndVScrollBar = CreateWindowEx(
        0,
        L"SCROLLBAR",
        (PTSTR)NULL,
        WS_CHILD | WS_VISIBLE | SBS_VERT,
        0,
        0,
        0,
        0,
        cw->_hWnd,
        (HMENU)NULL,
        (HINSTANCE)GetWindowLongPtr(cw->_hWnd, GWLP_HINSTANCE),
        (PVOID)NULL
    );

    if (!cw->_hWndVScrollBar)
    {
        ShowError(_T("CalcWindow::OnCreate::Unable to Create V ScrollBar"));
        return -1;
    }

    cw->_hWndHScrollBar = CreateWindowEx(
        0,
        L"SCROLLBAR",
        (PTSTR)NULL,
        WS_CHILD | WS_VISIBLE | SBS_HORZ,
        0,
        0,
        0,
        0,
        cw->_hWnd,
        (HMENU)NULL,
        (HINSTANCE)GetWindowLongPtr(cw->_hWnd, GWLP_HINSTANCE),
        (PVOID)NULL
    );

    if (!cw->_hWndHScrollBar)
    {
        ShowError(_T("CalcWindow::OnCreate::Unable to Create H ScrollBar"));
        return -1;
    }

    cw->_hWndCorner = CreateWindowEx(
        0,
        L"BUTTON",
        (PTSTR)NULL,
        WS_CHILD | WS_VISIBLE | WS_DISABLED,
        0,
        0,
        0,
        0,
        cw->_hWnd,
        (HMENU)NULL,
        (HINSTANCE)GetWindowLongPtr(cw->_hWnd, GWLP_HINSTANCE),
        (PVOID)NULL
    );

    if (!cw->_hWndCorner)
    {
        ShowError(_T("CalcWindow::OnCreate::Unable to Create Corner Button"));
        return -1;
    }

    PanelList_AddNewDraw2DPanel(cw->_pPanelList, L"f(x)=", L"Interval:", L"Step:");
    cw->_selectedPanel = cw->_pPanelList->_front->_panel;
    
    {
        Editor* ed = ((Draw2DPanel*)cw->_selectedPanel)->_func_editor;
        Editor_OnChar_Default(ed, 'C', cw->_hWnd);
        Editor_OnChar_Default(ed, 'o', cw->_hWnd);
        Editor_OnChar_Default(ed, 's', cw->_hWnd);
        Editor_OnChar_Default(ed, '(', cw->_hWnd);
        Editor_OnChar_Default(ed, 'x', cw->_hWnd);
        Editor_OnChar_Default(ed, ')', cw->_hWnd);
        ed = ((Draw2DPanel*)cw->_selectedPanel)->_interval_editor;
        Editor_OnChar_Default(ed, '-', cw->_hWnd);
        Editor_OnChar_Default(ed, '5', cw->_hWnd);
        Editor_OnChar_Default(ed, ',', cw->_hWnd);
        Editor_OnChar_Default(ed, '5', cw->_hWnd);
        ed = ((Draw2DPanel*)cw->_selectedPanel)->_step_editor;
        Editor_OnChar_Default(ed, '/', cw->_hWnd);
        Editor_OnChar_Default(ed, '1', cw->_hWnd);
        Editor_OnKey_RightArrow(ed, cw->_hWnd, false, false);
        Editor_OnChar_Default(ed, '1', cw->_hWnd);
        Editor_OnChar_Default(ed, '0', cw->_hWnd);
        Editor_OnChar_Default(ed, '0', cw->_hWnd);
    }

    PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
        cw->_yCurrent);

    PanelList_AddNewGeneralPanel(cw->_pPanelList, L"In:", L"Out:");
    cw->_selectedPanel = cw->_pPanelList->_front->_next->_panel;
    PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
        cw->_yCurrent);


    Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
        cw->_xCurrentPos - cw->_selectedPanel->_x0,
        cw->_yCurrent - cw->_selectedPanel->_y0);
     
    InvalidateRect(cw->_hWnd, NULL, FALSE);
    SetScrollbarInfo(cw);

    return 0;
}

static LRESULT OnDestroy(CalcWindow* cw)
{
    destroy_rules_library();

    Graphics_fontList_free();
    DeleteFont(g_math_font);

    return 0;
}

static LRESULT OnSize(CalcWindow* cw)
{
    MoveWindow(cw->_hWndVScrollBar,
        cw->_clientWidth - SCROLLBAR_WIDE,
        0,
        SCROLLBAR_WIDE,
        cw->_clientHeight - SCROLLBAR_WIDE,
        TRUE);
    
    MoveWindow(cw->_hWndHScrollBar,
        0,
        cw->_clientHeight - SCROLLBAR_WIDE,
        cw->_clientWidth - SCROLLBAR_WIDE,
        SCROLLBAR_WIDE,
        TRUE);
    
    MoveWindow(cw->_hWndCorner,
        cw->_clientWidth - SCROLLBAR_WIDE,
        cw->_clientHeight - SCROLLBAR_WIDE,
        SCROLLBAR_WIDE,
        SCROLLBAR_WIDE,
        TRUE);
    
    SetScrollbarInfo(cw);

    return 0;
}

static LRESULT OnPaint(CalcWindow* cw)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(cw->_hWnd, &ps);

    //SetBkMode(hdc, TRANSPARENT);
    //SetROP2(hdc, R2_NOT);
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

    PanelList_Paint(cw->_pPanelList,
        hdc,
        &ps.rcPaint,
        cw->_xCurrentPos,
        cw->_yCurrent);

    EndPaint(cw->_hWnd, &ps);

    return 0;
}

static void SetScrollbarInfo(CalcWindow* cw)
{
    SCROLLINFO siv, sih;

    int v = PanelList_GetViewportHeight(cw->_pPanelList) -
        cw->_clientHeight + SCROLLBAR_WIDE;
    v = v > 0 ? v : 0;

    cw->_yMaxScroll = v;
    siv.cbSize = sizeof(siv);
    siv.fMask = SIF_RANGE | SIF_POS;
    siv.nMin = 0;
    siv.nMax = cw->_yMaxScroll;
    siv.nPos = cw->_yCurrent;
    SetScrollInfo(cw->_hWndVScrollBar, SB_CTL, &siv, TRUE);

    int h = PanelList_GetViewportWidth(cw->_pPanelList) -
        cw->_clientWidth + SCROLLBAR_WIDE;
    h = h > 0 ? h : 0;

    cw->_xMaxScroll = h;
    sih.cbSize = sizeof(sih);
    sih.fMask = SIF_RANGE | SIF_POS;
    sih.nMin = 0;
    sih.nMax = cw->_xMaxScroll;
    sih.nPos = cw->_xCurrentPos;
    SetScrollInfo(cw->_hWndHScrollBar, SB_CTL, &sih, TRUE);
}

static LRESULT OnVScroll(CalcWindow* cw, WPARAM wParam)
{
    int xDelta = 0;
    int yNewPos;
    int yDelta;

    switch (LOWORD(wParam))
    {
    case SB_PAGEUP:
        yNewPos = cw->_yCurrent - 60;
        break;

    case SB_PAGEDOWN:
        yNewPos = cw->_yCurrent + 60;
        break;

    case SB_LINEUP:
        yNewPos = cw->_yCurrent - 20;
        break;

    case SB_LINEDOWN:
        yNewPos = cw->_yCurrent + 20;
        break;

    case SB_THUMBPOSITION:
        yNewPos = HIWORD(wParam);
        break;

    default:
        yNewPos = cw->_yCurrent;
    }

    yNewPos = max(0, yNewPos);
    yNewPos = min(cw->_yMaxScroll, yNewPos);

    // If the current position does not change, do not scroll.
    if (yNewPos == cw->_yCurrent)
        return 0;

    // Determine the amount scrolled (in pixels). 
    yDelta = yNewPos - cw->_yCurrent;

    // Reset the current scroll position. 
    cw->_yCurrent = yNewPos;

    PanelList_PropertyChangedEvent(cw->_pPanelList,
        FALSE,
        NULL,
        cw->_hWnd,
        cw->_xCurrentPos,
        cw->_yCurrent);
    Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
        cw->_xCurrentPos - cw->_selectedPanel->_x0,
        cw->_yCurrent - cw->_selectedPanel->_y0);
        
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = cw->_clientWidth - SCROLLBAR_WIDE;
    rc.bottom = cw->_clientHeight - SCROLLBAR_WIDE;

    ScrollWindowEx(cw->_hWnd, -xDelta, -yDelta, &rc,
        &rc, (HRGN)NULL, (RECT*)NULL,
        SW_INVALIDATE);
    UpdateWindow(cw->_hWnd);

    // Reset the scroll bar. 
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    si.nPos = cw->_yCurrent;
    SetScrollInfo(cw->_hWndVScrollBar, SB_CTL, &si, TRUE);

    return 0;
}

static LRESULT OnHScroll(CalcWindow* cw, WPARAM wParam)
{
    int xDelta;
    int xNewPos;
    int yDelta = 0;

    switch (LOWORD(wParam))
    {
    case SB_PAGEUP:
        xNewPos = cw->_xCurrentPos - 60;
        break;

    case SB_PAGEDOWN:
        xNewPos = cw->_xCurrentPos + 60;
        break;

    case SB_LINEUP:
        xNewPos = cw->_xCurrentPos - 20;
        break;

    case SB_LINEDOWN:
        xNewPos = cw->_xCurrentPos + 20;
        break;

    case SB_THUMBPOSITION:
        xNewPos = HIWORD(wParam);
        break;

    default:
        xNewPos = cw->_xCurrentPos;
    }

    xNewPos = max(0, xNewPos);
    xNewPos = min(cw->_xMaxScroll, xNewPos);

    // If the current position does not change, do not scroll.
    if (xNewPos == cw->_xCurrentPos)
        return 0;

    // Determine the amount scrolled (in pixels). 
    xDelta = xNewPos - cw->_xCurrentPos;

    // Reset the current scroll position. 
    cw->_xCurrentPos = xNewPos;

    PanelList_PropertyChangedEvent(cw->_pPanelList,
        FALSE,
        NULL,
        cw->_hWnd,
        cw->_xCurrentPos,
        cw->_yCurrent);
    Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
        cw->_xCurrentPos - cw->_selectedPanel->_x0,
        cw->_yCurrent - cw->_selectedPanel->_y0);
        
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = cw->_clientWidth - SCROLLBAR_WIDE;
    rc.bottom = cw->_clientHeight - SCROLLBAR_WIDE;

    ScrollWindowEx(cw->_hWnd, -xDelta, -yDelta, &rc,
        &rc, (HRGN)NULL, (RECT*)NULL,
        SW_INVALIDATE);
    UpdateWindow(cw->_hWnd);

    // Reset the scroll bar. 
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    si.nPos = cw->_xCurrentPos;
    SetScrollInfo(cw->_hWndHScrollBar, SB_CTL, &si, TRUE);

    return 0;
}

static LRESULT OnSetFocus(CalcWindow* cw)
{
    if (cw->_selectedPanel)
        Editor_OnSetFocus(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd);

    return 0;
}

static LRESULT OnKillFocus(CalcWindow* cw)
{
    if (cw->_selectedPanel)
        Editor_OnKillFocus(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel));

    return 0;
}

static LRESULT OnKeyDown(CalcWindow* cw, WPARAM wParam, LPARAM lParam)
{
    bool bShift = GetKeyState(VK_SHIFT) < 0;
    bool bCtrl = GetKeyState(VK_CONTROL) < 0;

    switch (wParam)
    {
    case VK_F1:
    {
        HINSTANCE r = ShellExecute(NULL, L"open", L"https://krezek.github.io/Simple_Calc.html", NULL, NULL, SW_SHOWNORMAL);

        if (r <= (HINSTANCE)32)
        {
            ShowError(L"Could not open help file.");
        }
    }
    break;

    case VK_RETURN:
        if (cw->_selectedPanel->_type == PT_DRAW2D)
        {
            PostMessage(cw->_hWndParent, WM_DRAWING_MSG, 1, 0);
        }
        else
        {
            if (bShift)
            {
                Simplify(cw);
            }
            else
            {
                Calculate(cw);
            }
        }

        break;

    case VK_HOME:       // Home 
        break;

    case VK_END:        // End 
        break;

    case VK_PRIOR:      // Page Up 
        break;

    case VK_NEXT:       // Page Down 
        break;

    case VK_LEFT:       // Left arrow 
    {
        Editor_OnKey_LeftArrow(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), 
            cw->_hWnd, bShift, bCtrl);

        Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
            cw->_xCurrentPos - cw->_selectedPanel->_x0,
            cw->_yCurrent - cw->_selectedPanel->_y0);
           
        break;
    }

    case VK_RIGHT:      // Right arrow
    {
        Editor_OnKey_RightArrow(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel),
            cw->_hWnd, bShift, bCtrl);

        Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
            cw->_xCurrentPos - cw->_selectedPanel->_x0,
            cw->_yCurrent - cw->_selectedPanel->_y0);
      
        break;
    }
    case VK_UP:         // Up arrow 
        break;

    case VK_DOWN:       // Down arrow 
        break;

    case VK_DELETE:     // Delete 
    {
        RECT rc1, rc2;
        rc1 = Panel_GetRect(cw->_selectedPanel); 
        
        Editor_OnKey_Delete(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel));

        PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
            cw->_yCurrent);
        rc2 = Panel_GetRect(cw->_selectedPanel);

        Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
            cw->_xCurrentPos - cw->_selectedPanel->_x0,
            cw->_yCurrent - cw->_selectedPanel->_y0);
            
        if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
        {
            InvalidateRect(cw->_hWnd, NULL, FALSE);
        }
        else
        {
            OffsetRect(&rc1, -cw->_xCurrentPos, -cw->_yCurrent);
            OffsetRect(&rc2, -cw->_xCurrentPos, -cw->_yCurrent);

            RECT rc;
            UnionRect(&rc, &rc1, &rc2);

            rc.right = rc.right + 1;
            rc.bottom = rc.bottom + 1;

            InvalidateRect(cw->_hWnd, &rc, FALSE);
        }

        SetScrollbarInfo(cw);
    }
    break;
    }

    return 0;
}

static LRESULT OnChar(CalcWindow* cw, WPARAM wParam, LPARAM lParam)
{
    HideCaret(cw->_hWnd);

    switch (wParam)
    {
    case 0x08:          // Backspace 
    {
        RECT rc1 = Panel_GetRect(cw->_selectedPanel); 
        
        Editor_OnChar_Backspace(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel));
        PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
            cw->_yCurrent);
        RECT rc2 = Panel_GetRect(cw->_selectedPanel);

        Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
            cw->_xCurrentPos - cw->_selectedPanel->_x0,
            cw->_yCurrent - cw->_selectedPanel->_y0);
            
        if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
        {
            InvalidateRect(cw->_hWnd, NULL, FALSE);
        }
        else
        {
            OffsetRect(&rc1, -cw->_xCurrentPos, -cw->_yCurrent);
            OffsetRect(&rc2, -cw->_xCurrentPos, -cw->_yCurrent);

            RECT rc;
            UnionRect(&rc, &rc1, &rc2);

            rc.right = rc.right + 1;
            rc.bottom = rc.bottom + 1;

            InvalidateRect(cw->_hWnd, &rc, FALSE);
        }

        SetScrollbarInfo(cw);

        break;
    }

    case 0x09:          // Tab 
    {
        // Todo: Implement Tab navigation
        if (GetKeyState(VK_SHIFT) < 0)
        {
        }
        else
        {
            cw->_selectedPanel->_nextEditorFunc(cw->_selectedPanel);

            Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
                cw->_xCurrentPos - cw->_selectedPanel->_x0,
                cw->_yCurrent - cw->_selectedPanel->_y0);
        }
    }
    break;

    case 0x0D:          // Carriage return 
        break;

    case 0x1B:        // Escape 
    case 0x0A:        // Linefeed 
        MessageBeep((UINT)-1);
        break;

    default:
    {
        RECT rc1, rc2;
        rc1 = Panel_GetRect(cw->_selectedPanel);

        Editor_OnChar_Default(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel),
            (char)wParam, cw->_hWnd);
        PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
            cw->_yCurrent);
        rc2 = Panel_GetRect(cw->_selectedPanel);

        Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
            cw->_xCurrentPos - cw->_selectedPanel->_x0,
            cw->_yCurrent - cw->_selectedPanel->_y0);
            
        if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
        {
            InvalidateRect(cw->_hWnd, NULL, FALSE);
        }
        else
        {
            OffsetRect(&rc1, -cw->_xCurrentPos, -cw->_yCurrent);
            OffsetRect(&rc2, -cw->_xCurrentPos, -cw->_yCurrent);

            RECT rc;
            UnionRect(&rc, &rc1, &rc2);

            rc.right = rc.right + 1;
            rc.bottom = rc.bottom + 1;

            InvalidateRect(cw->_hWnd, &rc, FALSE);
        }
        
        SetScrollbarInfo(cw);

        break;
    }
    }

    ShowCaret(cw->_hWnd);

    return 0;
}

void Calculate(CalcWindow* cw)
{
    SetStatusBarText(cw, L"");

    String* inStr = String_init();
    GList_toString(((GeneralPanel*)cw->_selectedPanel)->_in_gitems_list, inStr);

    char* outStr = do_calc(inStr->_str);

    if (outStr)
    {
        GList* gl = NULL;
        parse_gitems(&gl, outStr);
        if (gl)
        {
            RECT rc1 = Panel_GetRect(cw->_selectedPanel);

            GList_free(((GeneralPanel*)cw->_selectedPanel)->_out_gitems_list);
            ((GeneralPanel*)cw->_selectedPanel)->_out_gitems_list = gl;

            PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
                cw->_yCurrent);
            RECT rc2 = Panel_GetRect(cw->_selectedPanel);

            Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
                cw->_xCurrentPos - cw->_selectedPanel->_x0,
                cw->_yCurrent - cw->_selectedPanel->_y0);
                
            if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
            {
                InvalidateRect(cw->_hWnd, NULL, FALSE);
            }
            else
            {
                OffsetRect(&rc1, -cw->_xCurrentPos, -cw->_yCurrent);
                OffsetRect(&rc2, -cw->_xCurrentPos, -cw->_yCurrent);

                RECT rc;
                UnionRect(&rc, &rc1, &rc2);

                rc.right = rc.right + 1;
                rc.bottom = rc.bottom + 1;

                InvalidateRect(cw->_hWnd, &rc, FALSE);
            }

            SetScrollbarInfo(cw);
        }

        free(outStr);
    }
    else
    {
        SetStatusBarText(cw, MParser_get_last_error());
    }


    String_free(inStr);
}

void Simplify(CalcWindow* cw)
{
    SetStatusBarText(cw, L"");

    String* inStr = String_init();
    GList_toString(((GeneralPanel*)cw->_selectedPanel)->_in_gitems_list, inStr);

    char* outStr = do_simplify(inStr->_str);
    
    if (outStr)
    {
        GList* gl = NULL;
        parse_gitems(&gl, outStr);
        if (gl)
        {
            RECT rc1 = Panel_GetRect(cw->_selectedPanel);

            GList_free(((GeneralPanel*)cw->_selectedPanel)->_out_gitems_list);
            ((GeneralPanel*)cw->_selectedPanel)->_out_gitems_list = gl;

            PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
                cw->_yCurrent);
            RECT rc2 = Panel_GetRect(cw->_selectedPanel);

            Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
                cw->_xCurrentPos - cw->_selectedPanel->_x0,
                cw->_yCurrent - cw->_selectedPanel->_y0);
                
            if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
            {
                InvalidateRect(cw->_hWnd, NULL, FALSE);
            }
            else
            {
                OffsetRect(&rc1, -cw->_xCurrentPos, -cw->_yCurrent);
                OffsetRect(&rc2, -cw->_xCurrentPos, -cw->_yCurrent);

                RECT rc;
                UnionRect(&rc, &rc1, &rc2);

                rc.right = rc.right + 1;
                rc.bottom = rc.bottom + 1;

                InvalidateRect(cw->_hWnd, &rc, FALSE);
            }

            SetScrollbarInfo(cw);
        }

        free(outStr);
    }
    else
    {
        SetStatusBarText(cw, MParser_get_last_error());
    }

    String_free(inStr);
}

BOOL OnContextMenu(CalcWindow* cw, int x, int y)
{
    RECT rc;                    // client area of window
    POINT pt = { x, y };        // location of mouse click

    // Get the bounding rectangle of the client area.

    GetClientRect(cw->_hWnd, &rc);

    // Convert the mouse position to client coordinates.

    ScreenToClient(cw->_hWnd, &pt);

    // If the position is in the client area, display a
    // shortcut menu.

    if (PtInRect(&rc, pt))
    {
        ClientToScreen(cw->_hWnd, &pt);
        DisplayContextMenu(cw, pt);
        return TRUE;
    }

    // Return FALSE if no menu is displayed.
    
    return FALSE;
}

static VOID DisplayContextMenu(CalcWindow* cw, POINT pt)
{
    HMENU hmenu;            // top-level menu 
    HMENU hmenuTrackPopup;  // shortcut menu 

    // Load the menu resource. 

    if ((hmenu = LoadMenu(NULL, L"PopupMenu")) == NULL)
        return;

    // TrackPopupMenu cannot display the menu bar so get
    // a handle to the first shortcut menu.

    hmenuTrackPopup = GetSubMenu(hmenu, 0);

    // Enable / Disable Submenu
    {
        POINT rpt = { pt.x, pt.y };

        ScreenToClient(cw->_hWnd, &rpt);

        cw->_managedPanel = PanelList_GetPanelFromPoint(cw->_pPanelList,
            rpt.x + cw->_xCurrentPos,
            rpt.y + cw->_yCurrent);
        if (cw->_managedPanel)
        {
            if(cw->_pPanelList->_front != cw->_pPanelList->_rear)
                EnableMenuItem(hmenuTrackPopup, IDM_PANELS_DELETE, MF_ENABLED);
            else
                EnableMenuItem(hmenuTrackPopup, IDM_PANELS_DELETE, MF_DISABLED);

            if (cw->_managedPanel->_type != PT_DRAW2D)
            {
                EnableMenuItem(hmenuTrackPopup, IDM_CALC, MF_ENABLED);
                EnableMenuItem(hmenuTrackPopup, IDM_SIMPLIFY, MF_ENABLED);
                EnableMenuItem(hmenuTrackPopup, IDM_DRAW, MF_DISABLED);
            }
            else
            {
                EnableMenuItem(hmenuTrackPopup, IDM_CALC, MF_DISABLED);
                EnableMenuItem(hmenuTrackPopup, IDM_SIMPLIFY, MF_DISABLED);
                EnableMenuItem(hmenuTrackPopup, IDM_DRAW, MF_ENABLED); 
            }
            

            //EnableMenuItem(hmenuTrackPopup, IDM_PANELS_UP, MF_ENABLED);
            //EnableMenuItem(hmenuTrackPopup, IDM_PANELS_DOWN, MF_ENABLED);
        }
        else
        {
            EnableMenuItem(hmenuTrackPopup, IDM_PANELS_DELETE, MF_DISABLED);
            
            EnableMenuItem(hmenuTrackPopup, IDM_CALC, MF_DISABLED);
            EnableMenuItem(hmenuTrackPopup, IDM_SIMPLIFY, MF_DISABLED);
            EnableMenuItem(hmenuTrackPopup, IDM_DRAW, MF_DISABLED);
            
            //EnableMenuItem(hmenuTrackPopup, IDM_PANELS_UP, MF_DISABLED);
            //EnableMenuItem(hmenuTrackPopup, IDM_PANELS_DOWN, MF_DISABLED);
        }
    }

    // Display the shortcut menu. Track the right mouse
    // button.

    TrackPopupMenu(hmenuTrackPopup,
        TPM_LEFTALIGN | TPM_RIGHTBUTTON,
        pt.x, pt.y, 0, cw->_hWnd, NULL);

    // Destroy the menu.

    DestroyMenu(hmenu);
    
}

static LRESULT OnMousLButtonDown(CalcWindow* cw, int x, int y)
{
    SetFocus(cw->_hWnd);

    Panel* p = PanelList_GetPanelFromPoint(cw->_pPanelList,
        x + cw->_xCurrentPos,
        y + cw->_yCurrent);
    if (p)
    {
        cw->_selectedPanel = p;

        cw->_selectedPanel->_selectEditorFromPointFunc(cw->_selectedPanel,
            x + cw->_xCurrentPos - cw->_selectedPanel->_x0,
            y + cw->_yCurrent - cw->_selectedPanel->_y0);

        Editor_MouseClicked(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
            cw->_xCurrentPos - cw->_selectedPanel->_x0,
            cw->_yCurrent - cw->_selectedPanel->_y0,
            x + cw->_xCurrentPos - cw->_selectedPanel->_x0,
            y + cw->_yCurrent - cw->_selectedPanel->_y0);
            
        Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
            cw->_xCurrentPos - cw->_selectedPanel->_x0,
            cw->_yCurrent - cw->_selectedPanel->_y0);
    }

    return 0;
}

static LRESULT OnMouseWheel(CalcWindow* cw, WPARAM wParam)
{
    int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

    if (GetKeyState(VK_CONTROL) < 0)
    {
        static int fsize = INITIAL_FONTSIZE;

        if (zDelta < 0)
        {
            if (fsize > 12)
                --fsize;

            PostMessage(cw->_hWnd, WM_SETFONTSIZE, MAKEWPARAM(fsize, 0), 0);
        }
        else
        {
            if (fsize < 72)
                ++fsize;

            PostMessage(cw->_hWnd, WM_SETFONTSIZE, MAKEWPARAM(fsize, 0), 0);
        }
    }
    else
    {
        if (zDelta < 0)
            PostMessage(cw->_hWnd, WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), 0);
        else
            PostMessage(cw->_hWnd, WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), 0);
    }
    
    return 0;
}

static LRESULT OnSetFontSize(CalcWindow* cw, int fsize)
{
    if (g_math_font)
        DeleteFont(g_math_font);

    Graphics_fontList_free();

    // Create Font
    HDC hdc = GetDC(cw->_hWnd);
    int lfHeight = -MulDiv(fsize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    g_math_font = CreateFont(lfHeight, 0, 0, 0, FALSE,
        FALSE, 0, 0, 0, 0, 0, 0, 0, L"Cambria");
    if (!g_math_font)
    {
        ShowError(_T("MainWindow::OnCreate::unable to create math font"));
        return -1;
    }

    Graphics_fontList_init(g_math_font);

    PanelList_PropertyChangedEvent(cw->_pPanelList, TRUE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
        cw->_yCurrent);

    Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
        cw->_xCurrentPos - cw->_selectedPanel->_x0,
        cw->_yCurrent - cw->_selectedPanel->_y0);
       
    InvalidateRect(cw->_hWnd, NULL, FALSE);
    SetScrollbarInfo(cw);
    
    return 0;
}

VOID OnCommand_PanelsNewGeneral(CalcWindow* cw)
{
    PanelList_AddNewGeneralPanel(cw->_pPanelList, L"In:", L"Out:");
    cw->_selectedPanel = cw->_pPanelList->_rear->_panel;

    PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
        cw->_yCurrent);
    Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
        cw->_xCurrentPos - cw->_selectedPanel->_x0,
        cw->_yCurrent - cw->_selectedPanel->_y0);
        
    InvalidateRect(cw->_hWnd, NULL, FALSE);
    SetScrollbarInfo(cw);
}

VOID OnCommand_PanelsNewDraw2D(CalcWindow* cw)
{
    PanelList_AddNewDraw2DPanel(cw->_pPanelList, L"f(x)=", L"Interval:", L"Step:");
    cw->_selectedPanel = cw->_pPanelList->_rear->_panel;

    {
        Editor* ed = ((Draw2DPanel*)cw->_selectedPanel)->_func_editor;
        Editor_OnChar_Default(ed, 'C', cw->_hWnd);
        Editor_OnChar_Default(ed, 'o', cw->_hWnd);
        Editor_OnChar_Default(ed, 's', cw->_hWnd);
        Editor_OnChar_Default(ed, '(', cw->_hWnd);
        Editor_OnChar_Default(ed, 'x', cw->_hWnd);
        Editor_OnChar_Default(ed, ')', cw->_hWnd);
        ed = ((Draw2DPanel*)cw->_selectedPanel)->_interval_editor;
        Editor_OnChar_Default(ed, '-', cw->_hWnd);
        Editor_OnChar_Default(ed, '5', cw->_hWnd);
        Editor_OnChar_Default(ed, ',', cw->_hWnd);
        Editor_OnChar_Default(ed, '5', cw->_hWnd);
        ed = ((Draw2DPanel*)cw->_selectedPanel)->_step_editor;
        Editor_OnChar_Default(ed, '/', cw->_hWnd);
        Editor_OnChar_Default(ed, '1', cw->_hWnd);
        Editor_OnKey_RightArrow(ed, cw->_hWnd, false, false);
        Editor_OnChar_Default(ed, '1', cw->_hWnd);
        Editor_OnChar_Default(ed, '0', cw->_hWnd);
        Editor_OnChar_Default(ed, '0', cw->_hWnd);
    }

    PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
        cw->_yCurrent);

    Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
        cw->_xCurrentPos - cw->_selectedPanel->_x0,
        cw->_yCurrent - cw->_selectedPanel->_y0);

    InvalidateRect(cw->_hWnd, NULL, FALSE);
    SetScrollbarInfo(cw);

}

VOID OnCommand_PanelsDelete(CalcWindow* cw)
{
    if (cw->_managedPanel->_type == PT_DRAW2D)
    {
        PostMessage(cw->_hWndParent, WM_DRAWING_MSG, 0, (LPARAM)cw->_selectedPanel);
    }

    PanelList_DeletePanel(cw->_pPanelList, cw->_managedPanel);

    PanelList_PropertyChangedEvent(cw->_pPanelList, TRUE, NULL, cw->_hWnd, cw->_xCurrentPos,
        cw->_yCurrent);

    InvalidateRect(cw->_hWnd, NULL, FALSE);
    SetScrollbarInfo(cw);

    if (cw->_managedPanel == cw->_selectedPanel)
        cw->_selectedPanel = cw->_pPanelList->_front->_panel;
    cw->_managedPanel = NULL;

    Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
        cw->_xCurrentPos - cw->_selectedPanel->_x0,
        cw->_yCurrent - cw->_selectedPanel->_y0);
}

VOID OnCommand_PanelsUp(CalcWindow* cw)
{

}

VOID OnCommand_PanelsDown(CalcWindow* cw)
{

}

void Graphics_fontList_init(HANDLE hFont)
{
    g_fontList[0] = hFont;

    for (int ix = 1; ix < sizeof(g_fontList) / sizeof(g_fontList[0]); ++ix)
    {
        LOGFONT logFont;
        GetObject(hFont, sizeof(LOGFONT), &logFont);
        logFont.lfHeight = (logFont.lfHeight + ix * 5 < 0) ? logFont.lfHeight + ix * 5 : -3;

        g_fontList[ix] = CreateFontIndirect(&logFont);
    }
}

void Graphics_fontList_free()
{
    for (int ix = 1; ix < sizeof(g_fontList) / sizeof(g_fontList[0]); ++ix)
    {

        DeleteObject(g_fontList[ix]);
    }
}

LRESULT CalcWindowRibbonCommand(CalcWindow* cw, int cmd)
{
    RECT rc1 = Panel_GetRect(cw->_selectedPanel);

    Editor_OnCmd(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cmd, cw->_hWnd);
    PanelList_PropertyChangedEvent(cw->_pPanelList, FALSE, cw->_selectedPanel, cw->_hWnd, cw->_xCurrentPos,
        cw->_yCurrent);
    RECT rc2 = Panel_GetRect(cw->_selectedPanel);

    Editor_UpdateCaret(cw->_selectedPanel->_getEditorFunc(cw->_selectedPanel), cw->_hWnd,
        cw->_xCurrentPos - cw->_selectedPanel->_x0,
        cw->_yCurrent - cw->_selectedPanel->_y0);
       
    if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
    {
        InvalidateRect(cw->_hWnd, NULL, FALSE);
    }
    else
    {
        OffsetRect(&rc1, -cw->_xCurrentPos, -cw->_yCurrent);
        OffsetRect(&rc2, -cw->_xCurrentPos, -cw->_yCurrent);

        RECT rc;
        UnionRect(&rc, &rc1, &rc2);

        rc.right = rc.right + 1;
        rc.bottom = rc.bottom + 1;

        InvalidateRect(cw->_hWnd, &rc, FALSE);
    }

    SetScrollbarInfo(cw);

    return 0;
}

static void SetStatusBarText(CalcWindow* cw, const wchar_t* txt)
{
    PostMessage(cw->_hWndParent, WM_STATUS_BAR_MSG, (WPARAM)0, (LPARAM)txt);
}
