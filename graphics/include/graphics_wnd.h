#ifndef _GRAPHICS_WND_H_
#define _GRAPHICS_WND_H_

#include <xmath.h>
#include <general.h>
#include <gtimer.h>

#define WM_3D_DRAWING_MSG WM_USER + 1

typedef struct _PipelineStateObject PipelineStateObject;
typedef struct _FrameResource FrameResource;
typedef struct _AxisGeo AxisGeo;
typedef struct _UploadBuffer UploadBuffer;


#define SWAP_CHAIN_BUFFER_COUNT 2
#define MSG_TEXT_LENGTH			1024

#define MAX_CURVES_COUNT		10

typedef struct _DrawingBlock
{
	size_t _id;

	V_Vertex* _vVertices;
	V_Index16* _vIndices;
} DrawingBlock;

typedef struct _GraphicsWindow GraphicsWindow;

typedef LRESULT(*HandleGraphicsMessageFunc) (GraphicsWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct _GraphicsWindow
{
	HWND _hWnd, _hWndParent;

	int _clientX, _clientY, _clientWidth, _clientHeight;

	POINT _LastMousePos;

	DXGI_FORMAT _backBufferFormat;
	DXGI_FORMAT _depthStencilFormat;

	UINT _4xMsaaQuality;

	IDXGIFactory* _pFactory;
	ID3D12Device* _pD3dDevice;
	ID3D12Fence* _pFence;

	UINT64 _currentFence;

	UINT _rtvDescriptorSize;
	UINT _dsvDescriptorSize;
	UINT _cbvSrvUavDescriptorSize;

	ID3D12CommandQueue* _pCommandQueue;
	ID3D12CommandAllocator* _pDirectCmdListAlloc;
	ID3D12GraphicsCommandList* _pCommandList;

	IDXGISwapChain* _pSwapChain;
	ID3D12Resource* _arrSwapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];
	int _currBackBuffer;
	ID3D12Resource* _pDepthStencilBuffer;

	ID3D12DescriptorHeap* _pRtvHeap;
	ID3D12DescriptorHeap* _pDsvHeap;
	ID3D12DescriptorHeap* _pSrvHeap;

	D3D12_VIEWPORT _screenViewport;
	D3D12_RECT _scissorRect;

	ID3D12RootSignature* _pRootSignature;

	PipelineStateObject* _pOpaquePSO;
	PipelineStateObject* _pLinesPSO;
	PipelineStateObject* _pPointsPSO;
	
	FrameResource* _frameResource;
	int _currFrameResourceIndex;

	PassConstants _mainPassConstant;

	float _radius;
	float _theta;
	float _phi;

	XFLOAT4X4 _world;
	XFLOAT4X4 _view;
	XFLOAT4X4 _proj;

	XFLOAT3 _eyePos;

	AxisGeo* _axis_geometry;
	V_RenderItem* _pVRIAxis;

	uint64_t _arrCurveID[MAX_CURVES_COUNT];
	MeshGeometry* _arrCurveMeshGeo[MAX_CURVES_COUNT];
	V_RenderItem* _arrCurveVRI[MAX_CURVES_COUNT];
	UploadBuffer* _arrCurveCB[MAX_CURVES_COUNT];

	bool _paused;
	GameTimer _gameTimer;

	wchar_t _arrTextMsg[MSG_TEXT_LENGTH];

	
	HandleGraphicsMessageFunc _fnHandleMessage;
} GraphicsWindow;

DrawingBlock* DrawingBlock_init();
void DrawingBlock_free(DrawingBlock* db);

ATOM GraphicsWindow_RegisterClass();

GraphicsWindow* GraphicsWindow_init();
void GraphicsWindow_free(GraphicsWindow* _this);

BOOL GraphicsWindow_Create(GraphicsWindow* _this, HWND hWndParent);

void GraphicsWindow_Initial3D(GraphicsWindow* _this);
void GraphicsWindow_Update(GraphicsWindow* _this);
void GraphicsWindow_Draw(GraphicsWindow* _this);

#endif /* _GRAPHICS_WND_H_ */
