#ifndef _GRAPHICS_WND_H_
#define _GRAPHICS_WND_H_

#include <xmath.h>
#include <general.h>
#include <gtimer.h>

#define WM_3D_DRAWING_MSG WM_USER + 1

typedef struct _PipelineStateObject PipelineStateObject;
typedef struct _FrameResource FrameResource;
typedef struct _AxisGeo AxisGeo;


#define SWAP_CHAIN_BUFFER_COUNT 2
#define FRAME_RESOURCE_COUNT	3
#define TEXTURE_COUNT			1
#define MATERIAL_COUNT			TEXTURE_COUNT
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

	int _client_x, _client_y, _client_width, _client_height;

	POINT _LastMousePos;

	DXGI_FORMAT _backBufferFormat;
	DXGI_FORMAT _depthStencilFormat;

	UINT _4xMsaaQuality;

	IDXGIFactory* _factory;
	ID3D12Device* _d3dDevice;
	ID3D12Fence* _fence;

	UINT64 _currentFence;

	UINT _rtvDescriptorSize;
	UINT _dsvDescriptorSize;
	UINT _cbvSrvUavDescriptorSize;

	ID3D12CommandQueue* _commandQueue;
	ID3D12CommandAllocator* _directCmdListAlloc;
	ID3D12GraphicsCommandList* _commandList;

	IDXGISwapChain* _swapChain;
	ID3D12Resource* _swapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];
	int _currBackBuffer;
	ID3D12Resource* _depthStencilBuffer;

	ID3D12DescriptorHeap* _rtvHeap;
	ID3D12DescriptorHeap* _dsvHeap;
	ID3D12DescriptorHeap* _srvHeap;

	D3D12_VIEWPORT _screenViewport;
	D3D12_RECT _scissorRect;

	ID3D12RootSignature* _rootSignature;

	PipelineStateObject* _opaque_pso;
	PipelineStateObject* _lines_pso;
	PipelineStateObject* _points_pso;
	
	FrameResource* _frameResource[FRAME_RESOURCE_COUNT];
	int _currFrameResourceIndex;

	Texture _textures[TEXTURE_COUNT];
	Material _materials[MATERIAL_COUNT];

	PassConstants _mainPassConstant;

	float _radius;
	float _theta;
	float _phi;

	XFLOAT4X4 _world;
	XFLOAT4X4 _view;
	XFLOAT4X4 _proj;

	XFLOAT3 _eyePos;

	V_RenderItem* _v_opaque_RI;
	V_RenderItem* _v_all_RI;

	AxisGeo* _sphere_geometry;
	MeshGeometry* _mesh_geometry[MAX_CURVES_COUNT];

	bool _paused;
	GameTimer _game_timer;

	wchar_t _text_msg[MSG_TEXT_LENGTH];

	DrawingBlock* _curves[MAX_CURVES_COUNT];

	HandleGraphicsMessageFunc _HandleMessageFunc;
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
