/*
*	Copywrite reserved for REZEK
*/

#include "pch.h"

#include <..\..\CFG.h>
#include <framerc.h>
#include <winutil.h>
#include <utils.h>
#include <upload_buffer.h>

#include <axis_geo.h>

#include <graphics_wnd.h>

extern int g_ribbon_height;
extern int g_statusbar_height;

extern const int g_splitter_width;
extern int g_splitterX;

extern const int g_tab_control_height;

static TCHAR szWindowClass[] = _T("GraphicsWindowClass");

D3D12_INPUT_ELEMENT_DESC g_inputLayout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};

LRESULT CALLBACK DefaultWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT HandleMessage(GraphicsWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT OnCreate(GraphicsWindow* _this, CREATESTRUCT* pcs);
LRESULT OnDestroy(GraphicsWindow* _this);
LRESULT OnSize(GraphicsWindow* _this, WPARAM wParam, LPARAM lParam);
LRESULT OnSizing(GraphicsWindow* _this, RECT* prc);
LRESULT OnActivate(GraphicsWindow* _this, bool active);
void OnMouseDown(GraphicsWindow* _this, WPARAM btnState, int x, int y);
void OnMouseUp(GraphicsWindow* _this, WPARAM btnState, int x, int y);
void OnMouseMove(GraphicsWindow* _this, WPARAM btnState, int x, int y);
LRESULT OnKeyDown(GraphicsWindow* _this, WPARAM wParam, LPARAM lParam);
LRESULT OnChar(GraphicsWindow* _this, WPARAM wParam, LPARAM lParam);

void InitDevice(GraphicsWindow* _this);
void BuildRootSignature(GraphicsWindow* _this);
void BuildShadersAndInputLayout(GraphicsWindow* _this);
void BuildPSO(GraphicsWindow* _this);
void BuildGeometry(GraphicsWindow* _this);
void BuildRenderItems(GraphicsWindow* _this);
void Cleanup3D(GraphicsWindow* _this);
void FlushCommandQueue(GraphicsWindow* _this);
D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView(GraphicsWindow* _this);
float AspectRatio(GraphicsWindow* _this);
void GetStaticSamplers(GraphicsWindow* _this, D3D12_STATIC_SAMPLER_DESC* list);

void DrawRenderItems(GraphicsWindow* _this, 
    ID3D12GraphicsCommandList* cmdList, 
    const V_RenderItem* ritems,
    const D3D_PRIMITIVE_TOPOLOGY topology);
void DrawCurveRenderItems(GraphicsWindow* _this,
    ID3D12GraphicsCommandList* cmdList,
    const V_RenderItem* ritems,
    const D3D_PRIMITIVE_TOPOLOGY topology,
    int index);

void SetTextMessage(GraphicsWindow* _this, const wchar_t* msg);

LRESULT OnDrawing(GraphicsWindow* _this, void* points);

DrawingBlock* DrawingBlock_init()
{
    DrawingBlock* db = (DrawingBlock*)malloc(sizeof(DrawingBlock));

    db->_id = 0;

    db->_vVertices = V_Vertex_init();
    db->_vIndices = V_Index16_init();

    return db;
}

void DrawingBlock_free(DrawingBlock* db)
{
    V_Vertex_free(db->_vVertices);
    V_Index16_free(db->_vIndices);

    free(db);
}

ATOM GraphicsWindow_RegisterClass()
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DefaultWindow_Proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);
}

LRESULT CALLBACK DefaultWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    GraphicsWindow* pThis;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (GraphicsWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->_hWnd = hWnd;
    }
    else
    {
        pThis = (GraphicsWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
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

BOOL GraphicsWindow_Create(GraphicsWindow* _this, HWND hWndParent)
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

GraphicsWindow* GraphicsWindow_init()
{
    GraphicsWindow* gw = (GraphicsWindow*)malloc(sizeof(GraphicsWindow));
    assert(gw != NULL);

    memset(gw, 0, sizeof(GraphicsWindow));

    gw->_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    gw->_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    gw->_pOpaquePSO = PipelineStateObject_init();
    gw->_pLinesPSO = PipelineStateObject_init();
    gw->_pPointsPSO = PipelineStateObject_init();
    
    gw->_radius = 13.0f;
    gw->_theta = 0.65f - X_PIDIV2;
    gw->_phi = 1.4f * X_PIDIV4;

    gw->_world      = Identity4x4();
    gw->_view       = Identity4x4();
    gw->_proj       = Identity4x4();

    gw->_pVRIAxis = V_RenderItem_init();

    gw->_axis_geometry = AxisGeo_init();

    memset(gw->_arrCurveID, 0, sizeof(uint64_t) * MAX_CURVES_COUNT);
    memset(gw->_arrCurveMeshGeo, 0, sizeof(MeshGeometry*) * MAX_CURVES_COUNT);
    memset(gw->_arrCurveVRI, 0, sizeof(V_RenderItem*) * MAX_CURVES_COUNT);
    memset(gw->_arrCurveCB, 0, sizeof(UploadBuffer*) * MAX_CURVES_COUNT);
    
    gw->_fnHandleMessage = HandleMessage;

    return gw;
}

void GraphicsWindow_free(GraphicsWindow* _this)
{
    for (int ix = 0; ix < MAX_CURVES_COUNT; ++ix)
    {
        if (!_this->_arrCurveCB[ix])
            continue;
        UploadBuffer_free(_this->_arrCurveCB[ix]);
    }

    for (int ix = 0; ix < MAX_CURVES_COUNT; ++ix)
    {
        if (!_this->_arrCurveMeshGeo[ix])
            continue;
        MeshGeometry_free(_this->_arrCurveMeshGeo[ix]);
    }

    for (int ix = 0; ix < MAX_CURVES_COUNT; ++ix)
    {
        if (!_this->_arrCurveVRI[ix])
            continue;
        V_RenderItem_free(_this->_arrCurveVRI[ix]);
    }

    AxisGeo_free(_this->_axis_geometry);

    V_RenderItem_free(_this->_pVRIAxis);

    if (_this->_pD3dDevice)
        FlushCommandQueue(_this);

    Cleanup3D(_this);
    free(_this);
}

static LRESULT HandleMessage(GraphicsWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        return OnCreate(_this, (CREATESTRUCT*)lParam);

    case WM_DESTROY:
        return OnDestroy(_this);

    case WM_SIZE:
        return OnSize(_this, wParam, lParam);

    case WM_SIZING:
        return OnSizing(_this, (RECT*)lParam);

    case WM_ACTIVATE:
        return OnActivate(_this, LOWORD(wParam) != WA_INACTIVE);

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
		OnMouseDown(_this, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        OnMouseUp(_this, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_MOUSEMOVE:
        OnMouseMove(_this, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    case WM_KEYDOWN:
        return OnKeyDown(_this, wParam, lParam);

    case WM_CHAR:
        return OnChar(_this, wParam, lParam);

    case WM_3D_DRAWING_MSG:
        return OnDrawing(_this, (void*)lParam);

    default:
        return DefWindowProc(_this->_hWnd, uMsg, wParam, lParam);
    }
}

LRESULT OnCreate(GraphicsWindow* _this, CREATESTRUCT* pcs)
{
    _this->_clientX = pcs->x;
    _this->_clientY = pcs->y;
    _this->_clientWidth = pcs->cx;
    _this->_clientHeight = pcs->cy;

    return 0;
}

LRESULT OnDestroy(GraphicsWindow* _this)
{
    FlushCommandQueue(_this);
    return 0;
}

LRESULT OnSize(GraphicsWindow* _this, WPARAM wParam, LPARAM lParam)
{
    if (!IsWindowVisible(_this->_hWnd))
        return 0;


    assert(_this->_pD3dDevice);
    assert(_this->_pSwapChain);
    assert(_this->_pDirectCmdListAlloc);

    FlushCommandQueue(_this);

    ThrowIfFailed(_this->_pCommandList->lpVtbl->Reset(_this->_pCommandList, _this->_pDirectCmdListAlloc, NULL));

    for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
    {
        if (_this->_arrSwapChainBuffer[i])
            _this->_arrSwapChainBuffer[i]->lpVtbl->Release(_this->_arrSwapChainBuffer[i]);
    }

    if (_this->_pDepthStencilBuffer)
        _this->_pDepthStencilBuffer->lpVtbl->Release(_this->_pDepthStencilBuffer);

    ThrowIfFailed(_this->_pSwapChain->lpVtbl->ResizeBuffers(
        _this->_pSwapChain,
        SWAP_CHAIN_BUFFER_COUNT,
        _this->_clientWidth, _this->_clientHeight,
        _this->_backBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

    _this->_currBackBuffer = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = { 0 };
    _this->_pRtvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart(_this->_pRtvHeap, &rtvHeapHandle);
    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
    {
        ThrowIfFailed(_this->_pSwapChain->lpVtbl->GetBuffer(_this->_pSwapChain, i, &IID_ID3D12Resource, (void**)&_this->_arrSwapChainBuffer[i]));

        _this->_pD3dDevice->lpVtbl->CreateRenderTargetView(_this->_pD3dDevice,
            _this->_arrSwapChainBuffer[i], 
            NULL, 
            rtvHeapHandle);

        rtvHeapHandle.ptr += 1 * _this->_rtvDescriptorSize;
    }

    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = _this->_clientWidth;
    depthStencilDesc.Height = _this->_clientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_HEAP_PROPERTIES hp = { 0 };
    hp.Type = D3D12_HEAP_TYPE_DEFAULT;
    hp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    hp.CreationNodeMask = 1;
    hp.VisibleNodeMask = 1;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = _this->_depthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateCommittedResource(
        _this->_pD3dDevice,
        &hp,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        &IID_ID3D12Resource, (void**)&_this->_pDepthStencilBuffer));

    D3D12_CPU_DESCRIPTOR_HANDLE dsViewHandle = { 0 };

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = _this->_depthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    _this->_pD3dDevice->lpVtbl->CreateDepthStencilView(
        _this->_pD3dDevice,
        _this->_pDepthStencilBuffer, 
        &dsvDesc, 
        DepthStencilView(_this));

    // Transition the resource from its initial state to be used as a depth buffer.
    D3D12_RESOURCE_BARRIER barrier = { 0 };
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = _this->_pDepthStencilBuffer;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    _this->_pCommandList->lpVtbl->ResourceBarrier(_this->_pCommandList, 1, &barrier);

    // Execute the resize commands.
    ThrowIfFailed(_this->_pCommandList->lpVtbl->Close(_this->_pCommandList));

    ID3D12CommandList* cmdsLists[] = { (ID3D12CommandList*)_this->_pCommandList };
    _this->_pCommandQueue->lpVtbl->ExecuteCommandLists(_this->_pCommandQueue, _countof(cmdsLists), cmdsLists);

    // Wait until resize is complete.
    FlushCommandQueue(_this);

    // Update the viewport transform to cover the client area.
    _this->_screenViewport.TopLeftX = 0;
    _this->_screenViewport.TopLeftY = 0;
    _this->_screenViewport.Width = (float)_this->_clientWidth;
    _this->_screenViewport.Height = (float)_this->_clientHeight;
    _this->_screenViewport.MinDepth = 0.0f;
    _this->_screenViewport.MaxDepth = 1.0f;

    _this->_scissorRect.left = 0;
    _this->_scissorRect.top = 0;
    _this->_scissorRect.right = _this->_clientWidth;
    _this->_scissorRect.bottom = _this->_clientHeight;

    XMATRIX P = XMatrixPerspectiveFovLH(X_PIDIV4, AspectRatio(_this), 1.0f, 1000.0f);
    XStoreFloat4x4(&_this->_proj, P);

    return 0;
}

LRESULT OnSizing(GraphicsWindow* _this, RECT* prc)
{
    LONG nWidth = prc->right - prc->left;
    LONG nHeight = prc->bottom - prc->top;

    if ((nWidth < 600) || (nHeight < 500))
    {
        prc->right = max(prc->right, prc->left + 600);
        prc->bottom = max(prc->bottom, prc->top + 500);
    }

    return TRUE;
}

LRESULT OnActivate(GraphicsWindow* _this, bool active)
{
    if (!active)
    {
        _this->_paused = true;
        GameTimer_Stop(&_this->_gameTimer);
    }
    else
    {
        _this->_paused = false;
        GameTimer_Start(&_this->_gameTimer);
    }

    return 0;
}

void OnMouseDown(GraphicsWindow* _this, WPARAM btnState, int x, int y)
{
    //if (GetKeyState(VK_CONTROL) < 0)
    {
        _this->_LastMousePos.x = x;
        _this->_LastMousePos.y = y;

        SetCapture(_this->_hWnd);
    }
}

void OnMouseUp(GraphicsWindow* _this, WPARAM btnState, int x, int y)
{
    //if (GetKeyState(VK_CONTROL) < 0)
        ReleaseCapture();
}

void OnMouseMove(GraphicsWindow* _this, WPARAM btnState, int x, int y)
{
    if (GetCapture() != _this->_hWnd)
        return;

    
    //if (GetKeyState(VK_CONTROL) < 0)
    {
        if ((btnState & MK_LBUTTON) != 0)
        {
            float dx = XConvertToRadians(0.25f * (float)(x - _this->_LastMousePos.x));
            float dy = XConvertToRadians(0.25f * (float)(y - _this->_LastMousePos.y));

            _this->_theta += dx;
            _this->_phi += dy;

            _this->_phi = Clamp(_this->_phi, 0.1f, X_PIDIV2 - 0.1f);
        }
        else if ((btnState & MK_RBUTTON) != 0)
        {
            float dx = 0.2f * (float)(x - _this->_LastMousePos.x);
            float dy = 0.2f * (float)(y - _this->_LastMousePos.y);

            _this->_radius += dx - dy;

            _this->_radius = Clamp(_this->_radius, 6.0f, 20.0f);
        }

        _this->_LastMousePos.x = x;
        _this->_LastMousePos.y = y;
    }
}

LRESULT OnKeyDown(GraphicsWindow* _this, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case VK_F1:
        break;

    case VK_RETURN:
        break;

    case VK_HOME:       // Home 
        break;

        //case VK_END:        // End 
        //	return OnKeyDown_end();

    case VK_PRIOR:      // Page Up 
        break;

    case VK_NEXT:       // Page Down 
        break;

    case VK_LEFT:       // Left arrow 
        break;

    case VK_RIGHT:      // Right arrow
        break;

    case VK_UP:         // Up arrow 
        break;

    case VK_DOWN:       // Down arrow 
        break;

    case VK_DELETE:     // Delete 
        break;
    }

    return 0;
}

LRESULT OnChar(GraphicsWindow* _this, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case VK_BACK:          // Backspace 
        break;

    case VK_TAB:          // Tab 
        break;

    case VK_RETURN:          // Carriage return 
        break;

    case VK_ESCAPE:        // Escape 
    case 0x0A:        // Linefeed 
        MessageBeep((UINT)-1);
        break;

    case VK_SPACE:
        break;

    default:
        break;
    }

    return 0;
}

void GraphicsWindow_Initial3D(GraphicsWindow* _this)
{
    InitDevice(_this);
    
    BuildRootSignature(_this);
    BuildShadersAndInputLayout(_this);
    BuildPSO(_this);
    
    BuildGeometry(_this);
    BuildRenderItems(_this);
    
    _this->_frameResource = FrameResource_init(_this->_pD3dDevice,
        1,
        (UINT)_this->_pVRIAxis->_size + 1);

    _this->_pCommandList->lpVtbl->Close(_this->_pCommandList);
    ID3D12CommandList* cmdsLists[] = { (ID3D12CommandList*)_this->_pCommandList };
    _this->_pCommandQueue->lpVtbl->ExecuteCommandLists(_this->_pCommandQueue, _countof(cmdsLists), cmdsLists);

    FlushCommandQueue(_this);
}

void InitDevice(GraphicsWindow* _this)
{
#ifdef _DEBUG
    // Enable the D3D12 debug layer.
    {
        ID3D12Debug* debugController = NULL;
        ThrowIfFailed(D3D12GetDebugInterface((REFIID)&IID_ID3D12Debug, (LPVOID*)(&debugController)));
        debugController->lpVtbl->EnableDebugLayer(debugController);
    }
#endif

    ThrowIfFailed(CreateDXGIFactory(&IID_IDXGIFactory, (void**)&_this->_pFactory));
    ThrowIfFailed(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_1, &IID_ID3D12Device, (void**)&_this->_pD3dDevice));
    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateFence(_this->_pD3dDevice, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**)&_this->_pFence));

    _this->_rtvDescriptorSize = _this->_pD3dDevice->lpVtbl->GetDescriptorHandleIncrementSize(_this->_pD3dDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    _this->_dsvDescriptorSize = _this->_pD3dDevice->lpVtbl->GetDescriptorHandleIncrementSize(_this->_pD3dDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    _this->_cbvSrvUavDescriptorSize = _this->_pD3dDevice->lpVtbl->GetDescriptorHandleIncrementSize(_this->_pD3dDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = _this->_backBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;

    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CheckFeatureSupport(
        _this->_pD3dDevice,
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels,
        sizeof(msQualityLevels)));

    _this->_4xMsaaQuality = msQualityLevels.NumQualityLevels;
    assert(_this->_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

    // Create Command Objects
    D3D12_COMMAND_QUEUE_DESC queueDesc = {0};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    
    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateCommandQueue(_this->_pD3dDevice,
        &queueDesc, &IID_ID3D12CommandQueue, (void**)&_this->_pCommandQueue));

    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateCommandAllocator(
        _this->_pD3dDevice,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        &IID_ID3D12CommandAllocator, (void**)&_this->_pDirectCmdListAlloc));

    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateCommandList(
        _this->_pD3dDevice,
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        _this->_pDirectCmdListAlloc,
        NULL,
        &IID_ID3D12GraphicsCommandList, (void**)&_this->_pCommandList));

    // Create Swap Chain
    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = _this->_clientWidth;
    sd.BufferDesc.Height = _this->_clientHeight;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = _this->_backBufferFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
    sd.OutputWindow = _this->_hWnd;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ThrowIfFailed(_this->_pFactory->lpVtbl->CreateSwapChain(
        _this->_pFactory,
        (IUnknown*)_this->_pCommandQueue,
        &sd,
        &_this->_pSwapChain));

    // Create Rtv and Dsv Descriptor Heaps
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateDescriptorHeap(
        _this->_pD3dDevice,
        &rtvHeapDesc, 
        &IID_ID3D12DescriptorHeap,
        (void**)&_this->_pRtvHeap));


    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateDescriptorHeap(
        _this->_pD3dDevice,
        &dsvHeapDesc, 
        &IID_ID3D12DescriptorHeap,
        (void**)&_this->_pDsvHeap));

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateDescriptorHeap(
        _this->_pD3dDevice,
        &cbvHeapDesc,
        &IID_ID3D12DescriptorHeap, (void**)&_this->_pSrvHeap));

    D3D12_CPU_DESCRIPTOR_HANDLE hDescriptor = { 0 };
    _this->_pSrvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart(_this->_pSrvHeap, &hDescriptor);
}

void BuildRootSignature(GraphicsWindow* _this)
{
    D3D12_ROOT_PARAMETER slotRootParameter[2];

    slotRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    slotRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    slotRootParameter[0].Descriptor.ShaderRegister = 0; // Per ObjectCB
    slotRootParameter[0].Descriptor.RegisterSpace = 0;

    slotRootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    slotRootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    slotRootParameter[1].Descriptor.ShaderRegister = 1; // PassCB
    slotRootParameter[1].Descriptor.RegisterSpace = 0;

    D3D12_STATIC_SAMPLER_DESC samplers[6];
    GetStaticSamplers(_this, samplers);

    D3D12_ROOT_SIGNATURE_DESC rootSigDesc;
    rootSigDesc.NumParameters = _countof(slotRootParameter);
    rootSigDesc.pParameters = slotRootParameter;
    rootSigDesc.NumStaticSamplers = _countof(samplers);
    rootSigDesc.pStaticSamplers = samplers;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* serializedRootSig = NULL;
    ID3DBlob* errorBlob = NULL;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig, &errorBlob));

    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateRootSignature(
        _this->_pD3dDevice,
        0,
        serializedRootSig->lpVtbl->GetBufferPointer(serializedRootSig),
        serializedRootSig->lpVtbl->GetBufferSize(serializedRootSig),
        &IID_ID3D12RootSignature, (void**)&_this->_pRootSignature));
}

void BuildShadersAndInputLayout(GraphicsWindow* _this)
{
    _this->_pOpaquePSO->_vsByteCode = LoadBinary(SHADER_PATH L"default_vs.cso");
    _this->_pOpaquePSO->_psByteCode = LoadBinary(SHADER_PATH L"default_ps.cso");
}

void BuildPSO(GraphicsWindow* _this)
{
    //
    // PSO for sky objects.
    //
    
    D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;
    ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    opaquePsoDesc.InputLayout.NumElements = _countof(g_inputLayout);
    opaquePsoDesc.InputLayout.pInputElementDescs = g_inputLayout;
    opaquePsoDesc.pRootSignature = _this->_pRootSignature;
    
    opaquePsoDesc.VS.pShaderBytecode = _this->_pOpaquePSO->_vsByteCode->lpVtbl->GetBufferPointer(_this->_pOpaquePSO->_vsByteCode);
    opaquePsoDesc.VS.BytecodeLength = _this->_pOpaquePSO->_vsByteCode->lpVtbl->GetBufferSize(_this->_pOpaquePSO->_vsByteCode);
    
    opaquePsoDesc.PS.pShaderBytecode = _this->_pOpaquePSO->_psByteCode->lpVtbl->GetBufferPointer(_this->_pOpaquePSO->_psByteCode);
    opaquePsoDesc.PS.BytecodeLength = _this->_pOpaquePSO->_psByteCode->lpVtbl->GetBufferSize(_this->_pOpaquePSO->_psByteCode);

    D3D12_RASTERIZER_DESC ras_desc;
    ras_desc.FillMode = D3D12_FILL_MODE_SOLID;
    //ras_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
    ras_desc.CullMode = D3D12_CULL_MODE_NONE;
    ras_desc.FrontCounterClockwise = FALSE;
    ras_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    ras_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    ras_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    ras_desc.DepthClipEnable = TRUE;
    ras_desc.MultisampleEnable = FALSE;
    ras_desc.AntialiasedLineEnable = FALSE;
    ras_desc.ForcedSampleCount = 0;
    ras_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    opaquePsoDesc.RasterizerState = ras_desc;

    D3D12_BLEND_DESC blend_desc;
    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blend_desc.RenderTarget[i] = defaultRenderTargetBlendDesc;
    opaquePsoDesc.BlendState = blend_desc;

    D3D12_DEPTH_STENCIL_DESC stencil_desc;
    stencil_desc.DepthEnable = TRUE;
    stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    stencil_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    stencil_desc.StencilEnable = FALSE;
    stencil_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    stencil_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    stencil_desc.FrontFace = defaultStencilOp;
    stencil_desc.BackFace = defaultStencilOp;
    opaquePsoDesc.DepthStencilState = stencil_desc;

    opaquePsoDesc.SampleMask = UINT_MAX;
    opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    opaquePsoDesc.NumRenderTargets = 1;
    opaquePsoDesc.RTVFormats[0] = _this->_backBufferFormat;
    opaquePsoDesc.SampleDesc.Count = 1;
    opaquePsoDesc.SampleDesc.Quality = 0;
    opaquePsoDesc.DSVFormat = _this->_depthStencilFormat;
    
    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateGraphicsPipelineState(
        _this->_pD3dDevice,
        &opaquePsoDesc, 
        &IID_ID3D12PipelineState,
        (void**)&_this->_pOpaquePSO->_PSO));

    D3D12_GRAPHICS_PIPELINE_STATE_DESC linesPsoDesc = opaquePsoDesc;
    linesPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateGraphicsPipelineState(
        _this->_pD3dDevice,
        &linesPsoDesc,
        &IID_ID3D12PipelineState,
        (void**)&_this->_pLinesPSO->_PSO));

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pointsPsoDesc = opaquePsoDesc;
    pointsPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    ThrowIfFailed(_this->_pD3dDevice->lpVtbl->CreateGraphicsPipelineState(
        _this->_pD3dDevice,
        &pointsPsoDesc,
        &IID_ID3D12PipelineState,
        (void**)&_this->_pPointsPSO->_PSO));
}

void BuildGeometry(GraphicsWindow* _this)
{
    AxisGeo_build(_this->_axis_geometry, _this->_pD3dDevice, _this->_pCommandList);
}

void BuildRenderItems(GraphicsWindow* _this)
{
    AxisGeo_add_render_items(_this->_axis_geometry, _this->_pVRIAxis);
}

void Cleanup3D(GraphicsWindow* _this)
{
    FrameResource_free(_this->_frameResource);

    if (_this->_pOpaquePSO)
        PipelineStateObject_free(_this->_pOpaquePSO);
    if (_this->_pLinesPSO)
        PipelineStateObject_free(_this->_pLinesPSO);
    if (_this->_pPointsPSO)
        PipelineStateObject_free(_this->_pPointsPSO);


    if (_this->_pRootSignature)
        _this->_pRootSignature->lpVtbl->Release(_this->_pRootSignature);

    if (_this->_pSrvHeap)
        _this->_pSrvHeap->lpVtbl->Release(_this->_pSrvHeap);

    if (_this->_pDsvHeap)
        _this->_pDsvHeap->lpVtbl->Release(_this->_pDsvHeap);
    
    if (_this->_pRtvHeap)
        _this->_pRtvHeap->lpVtbl->Release(_this->_pRtvHeap);

    if (_this->_pDepthStencilBuffer)
        _this->_pDepthStencilBuffer->lpVtbl->Release(_this->_pDepthStencilBuffer);
        
    for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
        if(_this->_arrSwapChainBuffer[i])
            _this->_arrSwapChainBuffer[i]->lpVtbl->Release(_this->_arrSwapChainBuffer[i]);
        
    if (_this->_pSwapChain)
        _this->_pSwapChain->lpVtbl->Release(_this->_pSwapChain);

    if (_this->_pCommandList)
        _this->_pCommandList->lpVtbl->Release(_this->_pCommandList);

    if (_this->_pDirectCmdListAlloc)
        _this->_pDirectCmdListAlloc->lpVtbl->Release(_this->_pDirectCmdListAlloc);
    
    if (_this->_pCommandQueue)
        _this->_pCommandQueue->lpVtbl->Release(_this->_pCommandQueue);

    if (_this->_pFence)
        _this->_pFence->lpVtbl->Release(_this->_pFence);

    if (_this->_pD3dDevice)
        _this->_pD3dDevice->lpVtbl->Release(_this->_pD3dDevice);

    if (_this->_pFactory)
        _this->_pFactory->lpVtbl->Release(_this->_pFactory);
}

void FlushCommandQueue(GraphicsWindow* _this)
{
    _this->_currentFence++;

    ThrowIfFailed(_this->_pCommandQueue->lpVtbl->Signal(_this->_pCommandQueue, _this->_pFence, _this->_currentFence));

    if (_this->_pFence->lpVtbl->GetCompletedValue(_this->_pFence) < _this->_currentFence)
    {
        HANDLE eventHandle = CreateEventEx(NULL, NULL, false, EVENT_ALL_ACCESS);
        assert(eventHandle);

        ThrowIfFailed(_this->_pFence->lpVtbl->SetEventOnCompletion(_this->_pFence, _this->_currentFence, eventHandle));

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

float AspectRatio(GraphicsWindow* _this)
{
    return (float)(_this->_clientWidth) / _this->_clientHeight;
}

ID3D12Resource* CurrentBackBuffer(GraphicsWindow* _this)
{
    return _this->_arrSwapChainBuffer[_this->_currBackBuffer];
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView(GraphicsWindow* _this)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc;
    _this->_pDsvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart(_this->_pDsvHeap, &cpu_desc);

    return cpu_desc;
}

D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView(GraphicsWindow* _this)
{
    D3D12_CPU_DESCRIPTOR_HANDLE desc_handle;
    _this->_pRtvHeap->lpVtbl->GetCPUDescriptorHandleForHeapStart(_this->_pRtvHeap, &desc_handle);
    desc_handle.ptr += _this->_currBackBuffer * _this->_rtvDescriptorSize;

    return desc_handle;
}

void GetStaticSamplers(GraphicsWindow* _this, D3D12_STATIC_SAMPLER_DESC* list)
{
    const D3D12_STATIC_SAMPLER_DESC pointWrap = 
    {
        D3D12_FILTER_MIN_MAG_MIP_POINT,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        0,
        16,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
        0.f,
        D3D12_FLOAT32_MAX,
        0, // ShaderRegister
        0,
        D3D12_SHADER_VISIBILITY_ALL
    };

    const D3D12_STATIC_SAMPLER_DESC pointClamp =
    {
        D3D12_FILTER_MIN_MAG_MIP_POINT,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        0,
        16,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
        0.f,
        D3D12_FLOAT32_MAX,
        1, // ShaderRegister
        0,
        D3D12_SHADER_VISIBILITY_ALL
    };

    const D3D12_STATIC_SAMPLER_DESC linearWrap =
    {
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        0,
        16,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
        0.f,
        D3D12_FLOAT32_MAX,
        2, // ShaderRegister
        0,
        D3D12_SHADER_VISIBILITY_ALL
    };

    const D3D12_STATIC_SAMPLER_DESC linearClamp =
    {
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        0,
        16,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
        0.f,
        D3D12_FLOAT32_MAX,
        3, // ShaderRegister
        0,
        D3D12_SHADER_VISIBILITY_ALL
    };

    const D3D12_STATIC_SAMPLER_DESC anisotropicWrap =
    {
        D3D12_FILTER_ANISOTROPIC,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        0,
        8,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
        0.f,
        D3D12_FLOAT32_MAX,
        4, // ShaderRegister
        0,
        D3D12_SHADER_VISIBILITY_ALL
    };

    const D3D12_STATIC_SAMPLER_DESC anisotropicClamp =
    {
        D3D12_FILTER_ANISOTROPIC,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        0,
        8,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
        0.f,
        D3D12_FLOAT32_MAX,
        5, // ShaderRegister
        0,
        D3D12_SHADER_VISIBILITY_ALL
    };

    list[0] = pointWrap;
    list[1] = pointClamp;
    list[2] = linearWrap;
    list[3] = linearClamp;
    list[4] = anisotropicWrap;
    list[5] = anisotropicClamp;
}

void Update_Camera(GraphicsWindow* _this)
{
    _this->_eyePos.x = _this->_radius * sinf(_this->_phi) * cosf(_this->_theta);
    _this->_eyePos.z = _this->_radius * sinf(_this->_phi) * sinf(_this->_theta);
    _this->_eyePos.y = _this->_radius * cosf(_this->_phi);

    XVECTOR pos = XVectorSet(_this->_eyePos.x, _this->_eyePos.y, _this->_eyePos.z, 1.0f);
    XVECTOR target = XVectorZero();
    XVECTOR up = XVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XStoreFloat4x4(&_this->_view, view);
}

void Update_MainPassCB(GraphicsWindow* _this)
{
    XMATRIX view = XLoadFloat4x4(&_this->_view);
    XMATRIX proj = XLoadFloat4x4(&_this->_proj);
    XMATRIX viewProj = XMatrixMultiply(view, proj);

    XVECTOR vd = XMatrixDeterminant(view);
    XMATRIX invView = XMatrixInverse(&vd, view);

    XVECTOR pd = XMatrixDeterminant(proj);
    XMATRIX invProj = XMatrixInverse(&pd, proj);

    XVECTOR vpd = XMatrixDeterminant(viewProj);
    XMATRIX invViewProj = XMatrixInverse(&vpd, viewProj);

    XStoreFloat4x4(&_this->_mainPassConstant.View, XMatrixTranspose(view));
    XStoreFloat4x4(&_this->_mainPassConstant.InvView, XMatrixTranspose(invView));
    XStoreFloat4x4(&_this->_mainPassConstant.Proj, XMatrixTranspose(proj));
    XStoreFloat4x4(&_this->_mainPassConstant.InvProj, XMatrixTranspose(invProj));
    XStoreFloat4x4(&_this->_mainPassConstant.ViewProj, XMatrixTranspose(viewProj));
    XStoreFloat4x4(&_this->_mainPassConstant.InvViewProj, XMatrixTranspose(invViewProj));
    
    _this->_mainPassConstant.EyePosW = _this->_eyePos;

    UploadBuffer* currPassCB = _this->_frameResource->_passCB;
    UploadBuffer_CopyData(currPassCB, 0, &_this->_mainPassConstant, sizeof(PassConstants));
}

void Update_ObjectCBs(GraphicsWindow* _this)
{
    for (int ix = 0; ix < _this->_pVRIAxis->_size; ++ix)
    {
        RenderItem* e = _this->_pVRIAxis->_ris[ix];

        XMATRIX world = XLoadFloat4x4(&e->World);

        ObjectConstants objConstants;
        XStoreFloat4x4(&objConstants.World, XMatrixTranspose(world));

        UploadBuffer_CopyData(_this->_frameResource->_objectCB, e->ObjCBIndex, &objConstants, sizeof(ObjectConstants));

    }
}

void Update_CurveCBs(GraphicsWindow* _this)
{
    for (int iy = 0; iy < MAX_CURVES_COUNT; ++iy)
    {
        if (!_this->_arrCurveVRI[iy])
            continue;

        V_RenderItem* vRI = _this->_arrCurveVRI[iy];
        for (int ix = 0; ix < vRI->_size; ++ix)
        {
            RenderItem* e = vRI->_ris[ix];

            XMATRIX world = XLoadFloat4x4(&e->World);

            ObjectConstants objConstants;
            XStoreFloat4x4(&objConstants.World, XMatrixTranspose(world));

            UploadBuffer_CopyData(_this->_arrCurveCB[iy], e->ObjCBIndex, &objConstants, sizeof(ObjectConstants));

        }
    }
}

void GraphicsWindow_Update(GraphicsWindow* _this)
{
    FlushCommandQueue(_this);

    Update_Camera(_this);

    Update_MainPassCB(_this);
    Update_ObjectCBs(_this);

    Update_CurveCBs(_this);
}

void GraphicsWindow_Draw(GraphicsWindow* _this)
{
    ThrowIfFailed(_this->_pDirectCmdListAlloc->lpVtbl->Reset(_this->_pDirectCmdListAlloc));

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    ThrowIfFailed(_this->_pCommandList->lpVtbl->Reset(_this->_pCommandList, _this->_pDirectCmdListAlloc, NULL));

    _this->_pCommandList->lpVtbl->RSSetViewports(_this->_pCommandList, 1, &_this->_screenViewport);
    _this->_pCommandList->lpVtbl->RSSetScissorRects(_this->_pCommandList, 1, &_this->_scissorRect);

    // Indicate a state transition on the resource usage.
    D3D12_RESOURCE_BARRIER barrierRTAsTexture =
    {
        D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        D3D12_RESOURCE_BARRIER_FLAG_NONE,
        { CurrentBackBuffer(_this), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET }
    };
    _this->_pCommandList->lpVtbl->ResourceBarrier(_this->_pCommandList, 1, &barrierRTAsTexture);

    // Clear the back buffer and depth buffer.
    float LightSteelBlue[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    _this->_pCommandList->lpVtbl->ClearRenderTargetView(_this->_pCommandList, CurrentBackBufferView(_this), 
        (float*) &LightSteelBlue, 0, NULL);
    _this->_pCommandList->lpVtbl->ClearDepthStencilView(_this->_pCommandList,
        DepthStencilView(_this), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

    // Specify the buffers we are going to render to.
    D3D12_CPU_DESCRIPTOR_HANDLE desc_handle = CurrentBackBufferView(_this);
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc = DepthStencilView(_this);
    _this->_pCommandList->lpVtbl->OMSetRenderTargets(_this->_pCommandList,
        1, &desc_handle, true, &cpu_desc);

    ID3D12DescriptorHeap* descriptorHeaps[] = {_this->_pSrvHeap };
    _this->_pCommandList->lpVtbl->SetDescriptorHeaps(_this->_pCommandList, _countof(descriptorHeaps), descriptorHeaps);

    _this->_pCommandList->lpVtbl->SetGraphicsRootSignature(_this->_pCommandList, _this->_pRootSignature);

    _this->_pCommandList->lpVtbl->SetGraphicsRootConstantBufferView(_this->_pCommandList,
        1,
        _this->_frameResource->_passCB->_uploadBuffer->lpVtbl->GetGPUVirtualAddress(
            _this->_frameResource->_passCB->_uploadBuffer));

    // draw curves
    for (int ix = 0; ix < MAX_CURVES_COUNT; ++ix)
    {
        if (!_this->_arrCurveVRI[ix])
            continue;

        _this->_pCommandList->lpVtbl->SetPipelineState(_this->_pCommandList, _this->_pOpaquePSO->_PSO);
        DrawCurveRenderItems(_this, _this->_pCommandList, _this->_arrCurveVRI[ix], D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, ix);

        _this->_pCommandList->lpVtbl->SetPipelineState(_this->_pCommandList, _this->_pLinesPSO->_PSO);
        DrawCurveRenderItems(_this, _this->_pCommandList, _this->_arrCurveVRI[ix], D3D_PRIMITIVE_TOPOLOGY_LINELIST, ix);

        _this->_pCommandList->lpVtbl->SetPipelineState(_this->_pCommandList, _this->_pPointsPSO->_PSO);
        DrawCurveRenderItems(_this, _this->_pCommandList, _this->_arrCurveVRI[ix], D3D_PRIMITIVE_TOPOLOGY_POINTLIST, ix);
    }
            
    _this->_pCommandList->lpVtbl->SetPipelineState(_this->_pCommandList, _this->_pOpaquePSO->_PSO);
    DrawRenderItems(_this, _this->_pCommandList, _this->_pVRIAxis, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    _this->_pCommandList->lpVtbl->SetPipelineState(_this->_pCommandList, _this->_pLinesPSO->_PSO);
    DrawRenderItems(_this, _this->_pCommandList, _this->_pVRIAxis, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    _this->_pCommandList->lpVtbl->SetPipelineState(_this->_pCommandList, _this->_pPointsPSO->_PSO);
    DrawRenderItems(_this, _this->_pCommandList, _this->_pVRIAxis, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

    // Indicate a state transition on the resource usage.
    D3D12_RESOURCE_BARRIER barrierRTForPresent =
    {
        D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        D3D12_RESOURCE_BARRIER_FLAG_NONE,
        { CurrentBackBuffer(_this), D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT }
    };
    _this->_pCommandList->lpVtbl->ResourceBarrier(_this->_pCommandList, 1, &barrierRTForPresent);

    // Done recording commands.
    ThrowIfFailed(_this->_pCommandList->lpVtbl->Close(_this->_pCommandList));

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = { (ID3D12CommandList*)_this->_pCommandList };
    _this->_pCommandQueue->lpVtbl->ExecuteCommandLists(_this->_pCommandQueue, _countof(cmdsLists), cmdsLists);

    //RenderText(_this->_currBackBuffer, _this->_arrTextMsg);

    // swap the back and front buffers
    ThrowIfFailed(_this->_pSwapChain->lpVtbl->Present(_this->_pSwapChain, 0, 0));
    _this->_currBackBuffer = (_this->_currBackBuffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

    _this->_pCommandQueue->lpVtbl->Signal(_this->_pCommandQueue, _this->_pFence, _this->_currentFence);

    FlushCommandQueue(_this);
}

void DrawRenderItems(GraphicsWindow* _this, 
    ID3D12GraphicsCommandList* cmdList, 
    const V_RenderItem* ritems,
    const D3D_PRIMITIVE_TOPOLOGY topology)
{
    UINT objCBByteSize = Utils_CalcConstantBufferByteSize(sizeof(ObjectConstants));

    UploadBuffer* objectCB = _this->_frameResource->_objectCB;

    for (size_t i = 0; i < ritems->_size; ++i)
    {
        RenderItem* ri = ritems->_ris[i];

        if (ri->PrimitiveType != topology)
            continue;

        D3D12_VERTEX_BUFFER_VIEW vbv = MeshGeometry_VertexBufferView(ri->Geo);
        D3D12_INDEX_BUFFER_VIEW ibv = MeshGeometry_IndexBufferView(ri->Geo);

        cmdList->lpVtbl->IASetVertexBuffers(cmdList, 0, 1, &vbv);
        cmdList->lpVtbl->IASetIndexBuffer(cmdList, &ibv);
        cmdList->lpVtbl->IASetPrimitiveTopology(cmdList, ri->PrimitiveType);

        D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->_uploadBuffer->lpVtbl->GetGPUVirtualAddress(objectCB->_uploadBuffer) + 
            ri->ObjCBIndex * objCBByteSize;
        
        cmdList->lpVtbl->SetGraphicsRootConstantBufferView(cmdList, 0, objCBAddress);
        
        cmdList->lpVtbl->DrawIndexedInstanced(cmdList, ri->IndexCount, 1, 
            ri->StartIndexLocation, ri->BaseVertexLocation, 0);
    }
}

void DrawCurveRenderItems(GraphicsWindow* _this,
    ID3D12GraphicsCommandList* cmdList,
    const V_RenderItem* ritems,
    const D3D_PRIMITIVE_TOPOLOGY topology, int ix)
{
    UINT objCBByteSize = Utils_CalcConstantBufferByteSize(sizeof(ObjectConstants));

    UploadBuffer* objectCB = _this->_arrCurveCB[ix];

    for (size_t i = 0; i < ritems->_size; ++i)
    {
        RenderItem* ri = ritems->_ris[i];

        if (ri->PrimitiveType != topology)
            continue;

        D3D12_VERTEX_BUFFER_VIEW vbv = MeshGeometry_VertexBufferView(ri->Geo);
        D3D12_INDEX_BUFFER_VIEW ibv = MeshGeometry_IndexBufferView(ri->Geo);

        cmdList->lpVtbl->IASetVertexBuffers(cmdList, 0, 1, &vbv);
        cmdList->lpVtbl->IASetIndexBuffer(cmdList, &ibv);
        cmdList->lpVtbl->IASetPrimitiveTopology(cmdList, ri->PrimitiveType);

        D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->_uploadBuffer->lpVtbl->GetGPUVirtualAddress(
            objectCB->_uploadBuffer) +
            ri->ObjCBIndex * objCBByteSize;

        cmdList->lpVtbl->SetGraphicsRootConstantBufferView(cmdList, 0, objCBAddress);

        cmdList->lpVtbl->DrawIndexedInstanced(cmdList, ri->IndexCount, 1,
            ri->StartIndexLocation, ri->BaseVertexLocation, 0);
    }
}


void SetTextMessage(GraphicsWindow* _this, const wchar_t* msg)
{
    wcscpy_s(_this->_arrTextMsg, MSG_TEXT_LENGTH, msg);
}

LRESULT OnDrawing(GraphicsWindow* _this, void* lparam)
{
    if (!lparam)
        return 0;

    DrawingBlock* db = (DrawingBlock*)lparam;

    for (int ix = 0; ix < MAX_CURVES_COUNT; ++ix)
    {
        if (!_this->_arrCurveID[ix])
            continue;

        if (_this->_arrCurveID[ix] == db->_id)
        {
            _this->_arrCurveID[ix] = 0;

            UploadBuffer_free(_this->_arrCurveCB[ix]);
            _this->_arrCurveCB[ix] = NULL;

            MeshGeometry_free(_this->_arrCurveMeshGeo[ix]);
            _this->_arrCurveMeshGeo[ix] = NULL;

            V_RenderItem_free(_this->_arrCurveVRI[ix]);
            _this->_arrCurveVRI[ix] = NULL;

            break;
        }
    }

    if (db->_vVertices->_size <= 0)
    {
        DrawingBlock_free(db);
        return -1;
    }
        

    printf("Drawing...\n");

    ThrowIfFailed(_this->_pDirectCmdListAlloc->lpVtbl->Reset(_this->_pDirectCmdListAlloc));
    ThrowIfFailed(_this->_pCommandList->lpVtbl->Reset(_this->_pCommandList, _this->_pDirectCmdListAlloc, NULL));
    // do drawing staff

    for (int iy = 0; iy < MAX_CURVES_COUNT; ++iy)
    {
        if (_this->_arrCurveID[iy] == 0)
        {
            _this->_arrCurveID[iy] = db->_id;

            _this->_arrCurveMeshGeo[iy] = MeshGeometry_init();

            SubmeshGeometry smg;

            smg.IndexCount = (UINT)db->_vIndices->_size;
            smg.BaseVertexLocation = 0;
            smg.StartIndexLocation = 0;
            V_SubmeshGeometry_pushback(_this->_arrCurveMeshGeo[iy]->_v_submeshes, smg);

            _this->_arrCurveMeshGeo[iy]->VertexBufferByteSize = (UINT)(sizeof(Vertex) * db->_vVertices->_size);
            _this->_arrCurveMeshGeo[iy]->IndexBufferByteSize = (UINT)(sizeof(unsigned __int16) * db->_vIndices->_size);
            _this->_arrCurveMeshGeo[iy]->VertexByteStride = sizeof(Vertex);

            _this->_arrCurveMeshGeo[iy]->_VertexBufferGPU = Utils_CreateDefaultBuffer(_this->_pD3dDevice,
                _this->_pCommandList,
                db->_vVertices->_vertices,
                _this->_arrCurveMeshGeo[iy]->VertexBufferByteSize,
                &_this->_arrCurveMeshGeo[iy]->_VertexDefaultBuffer);

            _this->_arrCurveMeshGeo[iy]->_IndexBufferGPU = Utils_CreateDefaultBuffer(_this->_pD3dDevice,
                _this->_pCommandList,
                db->_vIndices->_indices,
                _this->_arrCurveMeshGeo[iy]->IndexBufferByteSize,
                &_this->_arrCurveMeshGeo[iy]->_IndexDefaultBuffer);

            _this->_arrCurveVRI[iy] = V_RenderItem_init();

            RenderItem* ri = RenderItem_init();
            ri->IndexCount = _this->_arrCurveMeshGeo[iy]->_v_submeshes->_submeshes[0].IndexCount;
            ri->BaseVertexLocation = _this->_arrCurveMeshGeo[iy]->_v_submeshes->_submeshes[0].BaseVertexLocation;
            ri->StartIndexLocation = _this->_arrCurveMeshGeo[iy]->_v_submeshes->_submeshes[0].StartIndexLocation;

            ri->Geo = _this->_arrCurveMeshGeo[iy];

            ri->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

            ri->ObjCBIndex = (UINT)_this->_arrCurveVRI[iy]->_size;
            ri->World = Identity4x4();

            V_RenderItem_pushback(_this->_arrCurveVRI[iy], ri);

            _this->_arrCurveCB[iy] = UploadBuffer_init(sizeof(ObjectConstants), _this->_pD3dDevice,
                (UINT)_this->_arrCurveVRI[iy]->_size, true);

            break;
        }
    }

    DrawingBlock_free(db);

    _this->_pCommandList->lpVtbl->Close(_this->_pCommandList);
    ID3D12CommandList* cmdsLists[] = { (ID3D12CommandList*)_this->_pCommandList };
    _this->_pCommandQueue->lpVtbl->ExecuteCommandLists(_this->_pCommandQueue, _countof(cmdsLists), cmdsLists);

    FlushCommandQueue(_this);

    return 0;
}
