#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <xmath.h>

#define MAX_FIE_NAME 255
#define BONE_COUNT 100

typedef struct _PassConstants
{
    XFLOAT4X4 View;
    XFLOAT4X4 InvView;
    XFLOAT4X4 Proj;

    XFLOAT4X4 InvProj;
    XFLOAT4X4 ViewProj;
    XFLOAT4X4 InvViewProj;
    
    XFLOAT3 EyePosW;    
    float cbPerObjectPad1;

    XFLOAT4 AmbientLight;
} PassConstants;

typedef struct _ObjectConstants
{
    XFLOAT4X4 World;
} ObjectConstants;

typedef struct _Vertex
{
    XFLOAT3 Position;
    XFLOAT3 Color;
} Vertex;

typedef struct _V_Vertex
{
    size_t  _size;
    size_t  _capacity;
    Vertex* _vertices;
} V_Vertex;

typedef struct _V_Index16
{
    size_t              _size;
    size_t              _capacity;
    unsigned __int16*   _indices;
} V_Index16;

typedef struct _BoundingBox
{
    XFLOAT3 Center;            // Center of the box.
    XFLOAT3 Extents;           // Distance from the center to each side.
} BoundingBox;

typedef struct _SubmeshGeometry
{
    UINT    IndexCount;
    UINT    StartIndexLocation;
    INT     BaseVertexLocation;

    BoundingBox Bounds;
} SubmeshGeometry;

typedef struct _V_SubmeshGeometry
{
    size_t              _size;
    size_t              _capacity;
    SubmeshGeometry*    _submeshes;
} V_SubmeshGeometry;

typedef struct _MeshGeometry
{
    ID3D12Resource* _VertexBufferGPU;
    ID3D12Resource* _IndexBufferGPU;

    ID3D12Resource* _VertexDefaultBuffer;
    ID3D12Resource* _IndexDefaultBuffer;

    V_SubmeshGeometry* _v_submeshes;

    UINT        VertexByteStride;
    UINT        VertexBufferByteSize;
    DXGI_FORMAT IndexFormat;
    UINT        IndexBufferByteSize;
} MeshGeometry;

typedef struct _PipelineStateObject
{
    ID3DBlob* _vsByteCode;
    ID3DBlob* _psByteCode;

    ID3D12PipelineState* _PSO;
} PipelineStateObject;

typedef struct _Texture
{
    wchar_t Filename[MAX_FIE_NAME];

    ID3D12Resource* Resource;
    ID3D12Resource* UploadHeap;

    uint8_t* _ddsData;
} Texture;

typedef struct _Material
{
    int _srvHeapIndex;

    char _name[255];

    XFLOAT4 DiffuseAlbedo;
    XFLOAT3 FresnelR0;
    float Roughness;
    XFLOAT4X4 MatTransform;

    int NumFramesDirty;
} Material;

typedef struct _RenderItem
{
    XFLOAT4X4 World;
    XFLOAT4X4 TexTransform;

    int NumFramesDirty;

    UINT ObjCBIndex;

    MeshGeometry* Geo;
    Material* Mat;

    BoundingBox Bounds;

    D3D_PRIMITIVE_TOPOLOGY PrimitiveType;

    UINT IndexCount;
    UINT StartIndexLocation;
    int BaseVertexLocation;
} RenderItem;

typedef struct _V_RenderItem
{
    size_t              _size;
    size_t              _capacity;
    RenderItem**        _ris;
} V_RenderItem;

V_Vertex* V_Vertex_init();
void V_Vertex_free(V_Vertex* vv);
void V_Vertex_pushback(V_Vertex* vv, const Vertex v);

V_Index16* V_Index16_init();
void V_Index16_free(V_Index16* vi);
void V_Index16_pushback(V_Index16* vi, const unsigned __int16 i);

void BoundingBox_CreateFromPoints(BoundingBox* Out, size_t Count, const XFLOAT3* pPoints, size_t Stride);
bool BoundingBox_Intersects(BoundingBox* In, XVECTOR Origin, XVECTOR Direction, float* Dist);
void BoundingBox_Transform(BoundingBox* In, BoundingBox* Out, XMATRIX M);

V_SubmeshGeometry* V_SubmeshGeometry_init();
void V_SubmeshGeometry_free(V_SubmeshGeometry* vsg);
void V_SubmeshGeometry_pushback(V_SubmeshGeometry* vsg, const SubmeshGeometry sg);

MeshGeometry* MeshGeometry_init();
void MeshGeometry_free(MeshGeometry* mg);

D3D12_VERTEX_BUFFER_VIEW MeshGeometry_VertexBufferView(MeshGeometry* mg);
D3D12_INDEX_BUFFER_VIEW MeshGeometry_IndexBufferView(MeshGeometry* mg);

PipelineStateObject* PipelineStateObject_init();
void PipelineStateObject_free(PipelineStateObject* pso);

int Material_getIndex(Material* lst, int count, const char* n);

RenderItem* RenderItem_init();
void RenderItem_free(RenderItem* ri);

V_RenderItem* V_RenderItem_init();
void V_RenderItem_free(V_RenderItem* vri);
void V_RenderItem_pushback(V_RenderItem* vri, RenderItem* ri);

inline static float Clamp(const float x, const float low, const float high)
{
    return x < low ? low : (x > high ? high : x);
}


#endif /* _GENERAL_H_ */
