#include "pch.h"
#include "platform.h"

#include <resource.h>
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
extern void parse_gitems(GList** ppGl, const char* s);

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
        return pThis->_HandleMessageFunc(pThis, uMsg, wParam, lParam);
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

    cw->_HandleMessageFunc = HandleMessage;

    cw->_hWnd = NULL;
    cw->_x_current_pos = cw->_y_current_pos = 0;

    cw->_panelList = PanelList_init();
    cw->_selected_panel = NULL;
    cw->_managed_panel = NULL;

    return cw;
}

void CalcWindow_free(CalcWindow* mw)
{
    PanelList_free(mw->_panelList);
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

    PanelList_AddNewDraw2DPanel(cw->_panelList, L"f(x)=", L"Interval:", L"Step:");
    cw->_selected_panel = cw->_panelList->_front->_panel;
    
    {
        Editor* ed = ((Draw2DPanel*)cw->_selected_panel)->_func_editor;
        Editor_OnChar_Default(ed, 'C', cw->_hWnd);
        Editor_OnChar_Default(ed, 'o', cw->_hWnd);
        Editor_OnChar_Default(ed, 's', cw->_hWnd);
        Editor_OnChar_Default(ed, '(', cw->_hWnd);
        Editor_OnChar_Default(ed, 'x', cw->_hWnd);
        Editor_OnChar_Default(ed, ')', cw->_hWnd);
        ed = ((Draw2DPanel*)cw->_selected_panel)->_interval_editor;
        Editor_OnChar_Default(ed, '-', cw->_hWnd);
        Editor_OnChar_Default(ed, '5', cw->_hWnd);
        Editor_OnChar_Default(ed, ',', cw->_hWnd);
        Editor_OnChar_Default(ed, '5', cw->_hWnd);
        ed = ((Draw2DPanel*)cw->_selected_panel)->_step_editor;
        Editor_OnChar_Default(ed, '/', cw->_hWnd);
        Editor_OnChar_Default(ed, '1', cw->_hWnd);
        Editor_OnKey_RightArrow(ed, cw->_hWnd, false, false);
        Editor_OnChar_Default(ed, '1', cw->_hWnd);
        Editor_OnChar_Default(ed, '0', cw->_hWnd);
        Editor_OnChar_Default(ed, '0', cw->_hWnd);
    }

    PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
        cw->_y_current_pos);

    PanelList_AddNewGeneralPanel(cw->_panelList, L"In:", L"Out:");
    cw->_selected_panel = cw->_panelList->_front->_next->_panel;
    PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
        cw->_y_current_pos);


    Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
        cw->_x_current_pos - cw->_selected_panel->_x0,
        cw->_y_current_pos - cw->_selected_panel->_y0);
     
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
        cw->_client_width - SCROLLBAR_WIDE,
        0,
        SCROLLBAR_WIDE,
        cw->_client_height - SCROLLBAR_WIDE,
        TRUE);
    
    MoveWindow(cw->_hWndHScrollBar,
        0,
        cw->_client_height - SCROLLBAR_WIDE,
        cw->_client_width - SCROLLBAR_WIDE,
        SCROLLBAR_WIDE,
        TRUE);
    
    MoveWindow(cw->_hWndCorner,
        cw->_client_width - SCROLLBAR_WIDE,
        cw->_client_height - SCROLLBAR_WIDE,
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

    PanelList_Paint(cw->_panelList,
        hdc,
        &ps.rcPaint,
        cw->_x_current_pos,
        cw->_y_current_pos);

    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

    EndPaint(cw->_hWnd, &ps);

    return 0;
}

static void SetScrollbarInfo(CalcWindow* cw)
{
    SCROLLINFO siv, sih;

    int v = PanelList_GetViewportHeight(cw->_panelList) -
        cw->_client_height + SCROLLBAR_WIDE;
    v = v > 0 ? v : 0;

    cw->_yMaxScroll = v;
    siv.cbSize = sizeof(siv);
    siv.fMask = SIF_RANGE | SIF_POS;
    siv.nMin = 0;
    siv.nMax = cw->_yMaxScroll;
    siv.nPos = cw->_y_current_pos;
    SetScrollInfo(cw->_hWndVScrollBar, SB_CTL, &siv, TRUE);

    int h = PanelList_GetViewportWidth(cw->_panelList) -
        cw->_client_width + SCROLLBAR_WIDE;
    h = h > 0 ? h : 0;

    cw->_xMaxScroll = h;
    sih.cbSize = sizeof(sih);
    sih.fMask = SIF_RANGE | SIF_POS;
    sih.nMin = 0;
    sih.nMax = cw->_xMaxScroll;
    sih.nPos = cw->_x_current_pos;
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
        yNewPos = cw->_y_current_pos - 60;
        break;

    case SB_PAGEDOWN:
        yNewPos = cw->_y_current_pos + 60;
        break;

    case SB_LINEUP:
        yNewPos = cw->_y_current_pos - 20;
        break;

    case SB_LINEDOWN:
        yNewPos = cw->_y_current_pos + 20;
        break;

    case SB_THUMBPOSITION:
        yNewPos = HIWORD(wParam);
        break;

    default:
        yNewPos = cw->_y_current_pos;
    }

    yNewPos = max(0, yNewPos);
    yNewPos = min(cw->_yMaxScroll, yNewPos);

    // If the current position does not change, do not scroll.
    if (yNewPos == cw->_y_current_pos)
        return 0;

    // Determine the amount scrolled (in pixels). 
    yDelta = yNewPos - cw->_y_current_pos;

    // Reset the current scroll position. 
    cw->_y_current_pos = yNewPos;

    PanelList_PropertyChangedEvent(cw->_panelList,
        FALSE,
        NULL,
        cw->_hWnd,
        cw->_x_current_pos,
        cw->_y_current_pos);
    Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
        cw->_x_current_pos - cw->_selected_panel->_x0,
        cw->_y_current_pos - cw->_selected_panel->_y0);
        
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = cw->_client_width - SCROLLBAR_WIDE;
    rc.bottom = cw->_client_height - SCROLLBAR_WIDE;

    ScrollWindowEx(cw->_hWnd, -xDelta, -yDelta, &rc,
        &rc, (HRGN)NULL, (RECT*)NULL,
        SW_INVALIDATE);
    UpdateWindow(cw->_hWnd);

    // Reset the scroll bar. 
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    si.nPos = cw->_y_current_pos;
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
        xNewPos = cw->_x_current_pos - 60;
        break;

    case SB_PAGEDOWN:
        xNewPos = cw->_x_current_pos + 60;
        break;

    case SB_LINEUP:
        xNewPos = cw->_x_current_pos - 20;
        break;

    case SB_LINEDOWN:
        xNewPos = cw->_x_current_pos + 20;
        break;

    case SB_THUMBPOSITION:
        xNewPos = HIWORD(wParam);
        break;

    default:
        xNewPos = cw->_x_current_pos;
    }

    xNewPos = max(0, xNewPos);
    xNewPos = min(cw->_xMaxScroll, xNewPos);

    // If the current position does not change, do not scroll.
    if (xNewPos == cw->_x_current_pos)
        return 0;

    // Determine the amount scrolled (in pixels). 
    xDelta = xNewPos - cw->_x_current_pos;

    // Reset the current scroll position. 
    cw->_x_current_pos = xNewPos;

    PanelList_PropertyChangedEvent(cw->_panelList,
        FALSE,
        NULL,
        cw->_hWnd,
        cw->_x_current_pos,
        cw->_y_current_pos);
    Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
        cw->_x_current_pos - cw->_selected_panel->_x0,
        cw->_y_current_pos - cw->_selected_panel->_y0);
        
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = cw->_client_width - SCROLLBAR_WIDE;
    rc.bottom = cw->_client_height - SCROLLBAR_WIDE;

    ScrollWindowEx(cw->_hWnd, -xDelta, -yDelta, &rc,
        &rc, (HRGN)NULL, (RECT*)NULL,
        SW_INVALIDATE);
    UpdateWindow(cw->_hWnd);

    // Reset the scroll bar. 
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    si.nPos = cw->_x_current_pos;
    SetScrollInfo(cw->_hWndHScrollBar, SB_CTL, &si, TRUE);

    return 0;
}

static LRESULT OnSetFocus(CalcWindow* cw)
{
    if (cw->_selected_panel)
        Editor_OnSetFocus(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd);

    return 0;
}

static LRESULT OnKillFocus(CalcWindow* cw)
{
    if (cw->_selected_panel)
        Editor_OnKillFocus(cw->_selected_panel->_getEditorFunc(cw->_selected_panel));

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
        if (cw->_selected_panel->_type == PT_DRAW2D)
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
        Editor_OnKey_LeftArrow(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), 
            cw->_hWnd, bShift, bCtrl);

        Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
            cw->_x_current_pos - cw->_selected_panel->_x0,
            cw->_y_current_pos - cw->_selected_panel->_y0);
           
        break;
    }

    case VK_RIGHT:      // Right arrow
    {
        Editor_OnKey_RightArrow(cw->_selected_panel->_getEditorFunc(cw->_selected_panel),
            cw->_hWnd, bShift, bCtrl);

        Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
            cw->_x_current_pos - cw->_selected_panel->_x0,
            cw->_y_current_pos - cw->_selected_panel->_y0);
      
        break;
    }
    case VK_UP:         // Up arrow 
        break;

    case VK_DOWN:       // Down arrow 
        break;

    case VK_DELETE:     // Delete 
    {
        RECT rc1, rc2;
        rc1 = Panel_GetRect(cw->_selected_panel); 
        
        Editor_OnKey_Delete(cw->_selected_panel->_getEditorFunc(cw->_selected_panel));

        PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
            cw->_y_current_pos);
        rc2 = Panel_GetRect(cw->_selected_panel);

        Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
            cw->_x_current_pos - cw->_selected_panel->_x0,
            cw->_y_current_pos - cw->_selected_panel->_y0);
            
        if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
        {
            InvalidateRect(cw->_hWnd, NULL, FALSE);
        }
        else
        {
            OffsetRect(&rc1, -cw->_x_current_pos, -cw->_y_current_pos);
            OffsetRect(&rc2, -cw->_x_current_pos, -cw->_y_current_pos);

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
        RECT rc1 = Panel_GetRect(cw->_selected_panel); 
        
        Editor_OnChar_Backspace(cw->_selected_panel->_getEditorFunc(cw->_selected_panel));
        PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
            cw->_y_current_pos);
        RECT rc2 = Panel_GetRect(cw->_selected_panel);

        Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
            cw->_x_current_pos - cw->_selected_panel->_x0,
            cw->_y_current_pos - cw->_selected_panel->_y0);
            
        if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
        {
            InvalidateRect(cw->_hWnd, NULL, FALSE);
        }
        else
        {
            OffsetRect(&rc1, -cw->_x_current_pos, -cw->_y_current_pos);
            OffsetRect(&rc2, -cw->_x_current_pos, -cw->_y_current_pos);

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
            cw->_selected_panel->_nextEditorFunc(cw->_selected_panel);

            Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
                cw->_x_current_pos - cw->_selected_panel->_x0,
                cw->_y_current_pos - cw->_selected_panel->_y0);
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
        rc1 = Panel_GetRect(cw->_selected_panel);

        Editor_OnChar_Default(cw->_selected_panel->_getEditorFunc(cw->_selected_panel),
            (char)wParam, cw->_hWnd);
        PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
            cw->_y_current_pos);
        rc2 = Panel_GetRect(cw->_selected_panel);

        Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
            cw->_x_current_pos - cw->_selected_panel->_x0,
            cw->_y_current_pos - cw->_selected_panel->_y0);
            
        if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
        {
            InvalidateRect(cw->_hWnd, NULL, FALSE);
        }
        else
        {
            OffsetRect(&rc1, -cw->_x_current_pos, -cw->_y_current_pos);
            OffsetRect(&rc2, -cw->_x_current_pos, -cw->_y_current_pos);

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
    GList_toString(((GeneralPanel*)cw->_selected_panel)->_in_gitems_list, inStr);

    char* outStr = do_calc(inStr->_str);

    if (outStr)
    {
        GList* gl = NULL;
        parse_gitems(&gl, outStr);
        if (gl)
        {
            RECT rc1 = Panel_GetRect(cw->_selected_panel);

            GList_free(((GeneralPanel*)cw->_selected_panel)->_out_gitems_list);
            ((GeneralPanel*)cw->_selected_panel)->_out_gitems_list = gl;

            PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
                cw->_y_current_pos);
            RECT rc2 = Panel_GetRect(cw->_selected_panel);

            Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
                cw->_x_current_pos - cw->_selected_panel->_x0,
                cw->_y_current_pos - cw->_selected_panel->_y0);
                
            if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
            {
                InvalidateRect(cw->_hWnd, NULL, FALSE);
            }
            else
            {
                OffsetRect(&rc1, -cw->_x_current_pos, -cw->_y_current_pos);
                OffsetRect(&rc2, -cw->_x_current_pos, -cw->_y_current_pos);

                RECT rc;
                UnionRect(&rc, &rc1, &rc2);

                rc.right = rc.right + 1;
                rc.bottom = rc.bottom + 1;

                InvalidateRect(cw->_hWnd, &rc, FALSE);
            }

            SetScrollbarInfo(cw);

            free(outStr);
        }
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
    GList_toString(((GeneralPanel*)cw->_selected_panel)->_in_gitems_list, inStr);

    char* outStr = do_simplify(inStr->_str);

    if (outStr)
    {
        GList* gl = NULL;
        parse_gitems(&gl, outStr);
        if (gl)
        {
            RECT rc1 = Panel_GetRect(cw->_selected_panel);

            GList_free(((GeneralPanel*)cw->_selected_panel)->_out_gitems_list);
            ((GeneralPanel*)cw->_selected_panel)->_out_gitems_list = gl;

            PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
                cw->_y_current_pos);
            RECT rc2 = Panel_GetRect(cw->_selected_panel);

            Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
                cw->_x_current_pos - cw->_selected_panel->_x0,
                cw->_y_current_pos - cw->_selected_panel->_y0);
                
            if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
            {
                InvalidateRect(cw->_hWnd, NULL, FALSE);
            }
            else
            {
                OffsetRect(&rc1, -cw->_x_current_pos, -cw->_y_current_pos);
                OffsetRect(&rc2, -cw->_x_current_pos, -cw->_y_current_pos);

                RECT rc;
                UnionRect(&rc, &rc1, &rc2);

                rc.right = rc.right + 1;
                rc.bottom = rc.bottom + 1;

                InvalidateRect(cw->_hWnd, &rc, FALSE);
            }

            SetScrollbarInfo(cw);

            free(outStr);
        }
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

        cw->_managed_panel = PanelList_GetPanelFromPoint(cw->_panelList,
            rpt.x + cw->_x_current_pos,
            rpt.y + cw->_y_current_pos);
        if (cw->_managed_panel)
        {
            if(cw->_panelList->_front != cw->_panelList->_rear)
                EnableMenuItem(hmenuTrackPopup, IDM_PANELS_DELETE, MF_ENABLED);
            else
                EnableMenuItem(hmenuTrackPopup, IDM_PANELS_DELETE, MF_DISABLED);

            if (cw->_managed_panel->_type != PT_DRAW2D)
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

    Panel* p = PanelList_GetPanelFromPoint(cw->_panelList,
        x + cw->_x_current_pos,
        y + cw->_y_current_pos);
    if (p)
    {
        cw->_selected_panel = p;

        cw->_selected_panel->_selectEditorFromPointFunc(cw->_selected_panel,
            x + cw->_x_current_pos - cw->_selected_panel->_x0,
            y + cw->_y_current_pos - cw->_selected_panel->_y0);

        Editor_MouseClicked(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
            cw->_x_current_pos - cw->_selected_panel->_x0,
            cw->_y_current_pos - cw->_selected_panel->_y0,
            x + cw->_x_current_pos - cw->_selected_panel->_x0,
            y + cw->_y_current_pos - cw->_selected_panel->_y0);
            
        Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
            cw->_x_current_pos - cw->_selected_panel->_x0,
            cw->_y_current_pos - cw->_selected_panel->_y0);
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

    PanelList_PropertyChangedEvent(cw->_panelList, TRUE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
        cw->_y_current_pos);

    Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
        cw->_x_current_pos - cw->_selected_panel->_x0,
        cw->_y_current_pos - cw->_selected_panel->_y0);
       
    InvalidateRect(cw->_hWnd, NULL, FALSE);
    SetScrollbarInfo(cw);
    
    return 0;
}

VOID OnCommand_PanelsNewGeneral(CalcWindow* cw)
{
    PanelList_AddNewGeneralPanel(cw->_panelList, L"In:", L"Out:");
    cw->_selected_panel = cw->_panelList->_rear->_panel;

    PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
        cw->_y_current_pos);
    Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
        cw->_x_current_pos - cw->_selected_panel->_x0,
        cw->_y_current_pos - cw->_selected_panel->_y0);
        
    InvalidateRect(cw->_hWnd, NULL, FALSE);
    SetScrollbarInfo(cw);
}

VOID OnCommand_PanelsNewDraw2D(CalcWindow* cw)
{
    PanelList_AddNewDraw2DPanel(cw->_panelList, L"f(x)=", L"Interval:", L"Step:");
    cw->_selected_panel = cw->_panelList->_rear->_panel;

    {
        Editor* ed = ((Draw2DPanel*)cw->_selected_panel)->_func_editor;
        Editor_OnChar_Default(ed, 'C', cw->_hWnd);
        Editor_OnChar_Default(ed, 'o', cw->_hWnd);
        Editor_OnChar_Default(ed, 's', cw->_hWnd);
        Editor_OnChar_Default(ed, '(', cw->_hWnd);
        Editor_OnChar_Default(ed, 'x', cw->_hWnd);
        Editor_OnChar_Default(ed, ')', cw->_hWnd);
        ed = ((Draw2DPanel*)cw->_selected_panel)->_interval_editor;
        Editor_OnChar_Default(ed, '-', cw->_hWnd);
        Editor_OnChar_Default(ed, '5', cw->_hWnd);
        Editor_OnChar_Default(ed, ',', cw->_hWnd);
        Editor_OnChar_Default(ed, '5', cw->_hWnd);
        ed = ((Draw2DPanel*)cw->_selected_panel)->_step_editor;
        Editor_OnChar_Default(ed, '/', cw->_hWnd);
        Editor_OnChar_Default(ed, '1', cw->_hWnd);
        Editor_OnKey_RightArrow(ed, cw->_hWnd, false, false);
        Editor_OnChar_Default(ed, '1', cw->_hWnd);
        Editor_OnChar_Default(ed, '0', cw->_hWnd);
        Editor_OnChar_Default(ed, '0', cw->_hWnd);
    }

    PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
        cw->_y_current_pos);

    Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
        cw->_x_current_pos - cw->_selected_panel->_x0,
        cw->_y_current_pos - cw->_selected_panel->_y0);

    InvalidateRect(cw->_hWnd, NULL, FALSE);
    SetScrollbarInfo(cw);

}

VOID OnCommand_PanelsDelete(CalcWindow* cw)
{
    if (cw->_managed_panel->_type == PT_DRAW2D)
    {
        PostMessage(cw->_hWndParent, WM_DRAWING_MSG, 0, (LPARAM)cw->_selected_panel);
    }

    PanelList_DeletePanel(cw->_panelList, cw->_managed_panel);

    PanelList_PropertyChangedEvent(cw->_panelList, TRUE, NULL, cw->_hWnd, cw->_x_current_pos,
        cw->_y_current_pos);

    InvalidateRect(cw->_hWnd, NULL, FALSE);
    SetScrollbarInfo(cw);

    if (cw->_managed_panel == cw->_selected_panel)
        cw->_selected_panel = cw->_panelList->_front->_panel;
    cw->_managed_panel = NULL;

    Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
        cw->_x_current_pos - cw->_selected_panel->_x0,
        cw->_y_current_pos - cw->_selected_panel->_y0);
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
    RECT rc1 = Panel_GetRect(cw->_selected_panel);

    Editor_OnCmd(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cmd, cw->_hWnd);
    PanelList_PropertyChangedEvent(cw->_panelList, FALSE, cw->_selected_panel, cw->_hWnd, cw->_x_current_pos,
        cw->_y_current_pos);
    RECT rc2 = Panel_GetRect(cw->_selected_panel);

    Editor_UpdateCaret(cw->_selected_panel->_getEditorFunc(cw->_selected_panel), cw->_hWnd,
        cw->_x_current_pos - cw->_selected_panel->_x0,
        cw->_y_current_pos - cw->_selected_panel->_y0);
       
    if (rc1.top - rc1.bottom != rc2.top - rc2.bottom)
    {
        InvalidateRect(cw->_hWnd, NULL, FALSE);
    }
    else
    {
        OffsetRect(&rc1, -cw->_x_current_pos, -cw->_y_current_pos);
        OffsetRect(&rc2, -cw->_x_current_pos, -cw->_y_current_pos);

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
