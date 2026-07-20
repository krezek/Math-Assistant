#include "pch.h"

#include <general.h>

#define EXPAND 512

extern int g_NumFrameResources;

V_Vertex* V_Vertex_init()
{
	V_Vertex* vv = (V_Vertex*)malloc(sizeof(V_Vertex));
	assert(vv);

	vv->_size = 0;
	vv->_capacity = EXPAND;
	vv->_vertices = (Vertex*)malloc(sizeof(Vertex) * vv->_capacity);
	assert(vv->_vertices);

	return vv;
}

void V_Vertex_free(V_Vertex* vv)
{
	free(vv->_vertices);
	free(vv);
}

void V_Vertex_pushback(V_Vertex* vv, const Vertex v)
{
	if (vv->_size >= vv->_capacity)
	{
		vv->_capacity += EXPAND;
		Vertex* pv = (Vertex*)realloc(vv->_vertices, vv->_capacity * sizeof(Vertex));
		assert(pv);
		vv->_vertices = pv;
	}

	vv->_vertices[vv->_size++] = v;
}

V_Index16* V_Index16_init()
{
	V_Index16* vi = (V_Index16*)malloc(sizeof(V_Index16));
	assert(vi);

	vi->_size = 0;
	vi->_capacity = EXPAND;
	vi->_indices = (unsigned __int16*)malloc(sizeof(unsigned __int16) * vi->_capacity);
	assert(vi->_indices);

	return vi;
}

void V_Index16_free(V_Index16* vi)
{
	free(vi->_indices);
	free(vi);
}

void V_Index16_pushback(V_Index16* vi, const unsigned __int16 i)
{
	if (vi->_size >= vi->_capacity)
	{
		vi->_capacity += EXPAND;
		unsigned __int16* pi = (unsigned __int16*)realloc(vi->_indices, vi->_capacity * sizeof(unsigned __int16));
		assert(pi);
		vi->_indices = pi;
	}

	vi->_indices[vi->_size++] = i;
}

void BoundingBox_CreateFromPoints(BoundingBox* Out, size_t Count, const XFLOAT3* pPoints, size_t Stride)
{
	assert(Count > 0);
	assert(pPoints);

	// Find the minimum and maximum x, y, and z
	XVECTOR vMin, vMax;

	vMin = vMax = XLoadFloat3(pPoints);

	for (size_t i = 1; i < Count; ++i)
	{
		XVECTOR Point = XLoadFloat3((const XFLOAT3*)((const uint8_t*)(pPoints) + i * Stride));

		vMin = XVectorMin(vMin, Point);
		vMax = XVectorMax(vMax, Point);
	}

	// Store center and extents.
	XStoreFloat3(&Out->Center, XVectorScale(XVectorAdd(vMin, vMax), 0.5f));
	XStoreFloat3(&Out->Extents, XVectorScale(XVectorSubtract(vMax, vMin), 0.5f));
}

bool BoundingBox_Intersects(BoundingBox* In, XVECTOR Origin, XVECTOR Direction, float* Dist)
{
	assert(XVector3IsUnit(Direction));

	// Load the box.
	XVECTOR vCenter = XLoadFloat3(&In->Center);
	XVECTOR vExtents = XLoadFloat3(&In->Extents);

	// Adjust ray origin to be relative to center of the box.
	XVECTOR TOrigin = XVectorSubtract(vCenter, Origin);

	// Compute the dot product againt each axis of the box.
	// Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
	XVECTOR AxisDotOrigin = TOrigin;
	XVECTOR AxisDotDirection = Direction;

	// if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
	XVECTOR IsParallel = XVectorLessOrEqual(XVectorAbs(AxisDotDirection), g_RayEpsilon);

	// Test against all three axii simultaneously.
	XVECTOR InverseAxisDotDirection = XVectorReciprocal(AxisDotDirection);
	XVECTOR t1 = XVectorMultiply(XVectorSubtract(AxisDotOrigin, vExtents), InverseAxisDotDirection);
	XVECTOR t2 = XVectorMultiply(XVectorAdd(AxisDotOrigin, vExtents), InverseAxisDotDirection);

	// Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
	// use the results from any directions parallel to the slab.
	XVECTOR t_min = XVectorSelect(XVectorMin(t1, t2), g_FltMin, IsParallel);
	XVECTOR t_max = XVectorSelect(XVectorMax(t1, t2), g_FltMax, IsParallel);

	// t_min.x = maximum( t_min.x, t_min.y, t_min.z );
	// t_max.x = minimum( t_max.x, t_max.y, t_max.z );
	t_min = XVectorMax(t_min, XVectorSplatY(t_min));  // x = max(x,y)
	t_min = XVectorMax(t_min, XVectorSplatZ(t_min));  // x = max(max(x,y),z)
	t_max = XVectorMin(t_max, XVectorSplatY(t_max));  // x = min(x,y)
	t_max = XVectorMin(t_max, XVectorSplatZ(t_max));  // x = min(min(x,y),z)

	// if ( t_min > t_max ) return false;
	XVECTOR NoIntersection = XVectorGreater(XVectorSplatX(t_min), XVectorSplatX(t_max));

	// if ( t_max < 0.0f ) return false;
	NoIntersection = XVectorOrInt(NoIntersection, XVectorLess(XVectorSplatX(t_max), XVectorZero()));

	// if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
	XVECTOR ParallelOverlap = XVectorInBounds(AxisDotOrigin, vExtents);
	NoIntersection = XVectorOrInt(NoIntersection, XVectorAndCInt(IsParallel, ParallelOverlap));

	if (!XVector3AnyTrue(NoIntersection))
	{
		// Store the x-component to *pDist
		XStoreFloat(Dist, t_min);
		return true;
	}

	*Dist = 0.f;
	return false;
}

static XVECTOR g_BoxOffset[8] =
{
	{ { { -1.0f, -1.0f,  1.0f, 0.0f } } },
	{ { {  1.0f, -1.0f,  1.0f, 0.0f } } },
	{ { {  1.0f,  1.0f,  1.0f, 0.0f } } },
	{ { { -1.0f,  1.0f,  1.0f, 0.0f } } },
	{ { { -1.0f, -1.0f, -1.0f, 0.0f } } },
	{ { {  1.0f, -1.0f, -1.0f, 0.0f } } },
	{ { {  1.0f,  1.0f, -1.0f, 0.0f } } },
	{ { { -1.0f,  1.0f, -1.0f, 0.0f } } },
};

void BoundingBox_Transform(BoundingBox* In, BoundingBox* Out, XMATRIX M)
{
	// Load center and extents.
	XVECTOR vCenter = XLoadFloat3(&In->Center);
	XVECTOR vExtents = XLoadFloat3(&In->Extents);

	// Compute and transform the corners and find new min/max bounds.
	XVECTOR Corner = XVectorMultiplyAdd(vExtents, g_BoxOffset[0], vCenter);
	Corner = XVector3Transform(Corner, M);

	XVECTOR Min, Max;
	Min = Max = Corner;

	for (size_t i = 1; i < 8; ++i)
	{
		Corner = XVectorMultiplyAdd(vExtents, g_BoxOffset[i], vCenter);
		Corner = XVector3Transform(Corner, M);

		Min = XVectorMin(Min, Corner);
		Max = XVectorMax(Max, Corner);
	}

	// Store center and extents.
	XStoreFloat3(&Out->Center, XVectorScale(XVectorAdd(Min, Max), 0.5f));
	XStoreFloat3(&Out->Extents, XVectorScale(XVectorSubtract(Max, Min), 0.5f));
}

V_SubmeshGeometry* V_SubmeshGeometry_init()
{
	V_SubmeshGeometry* vsg = (V_SubmeshGeometry*)malloc(sizeof(V_SubmeshGeometry));
	assert(vsg);

	vsg->_size = 0;
	vsg->_capacity = EXPAND;
	vsg->_submeshes = (SubmeshGeometry*)malloc(sizeof(SubmeshGeometry) * vsg->_capacity);
	assert(vsg->_submeshes);

	return vsg;
}
void V_SubmeshGeometry_free(V_SubmeshGeometry* vsg)
{
	free(vsg->_submeshes);
	free(vsg);
}

void V_SubmeshGeometry_pushback(V_SubmeshGeometry* vsg, const SubmeshGeometry sg)
{
	if (vsg->_size >= vsg->_capacity)
	{
		vsg->_capacity += EXPAND;
		SubmeshGeometry* psg = (SubmeshGeometry*)realloc(vsg->_submeshes, vsg->_capacity * sizeof(SubmeshGeometry));
		assert(psg);
		vsg->_submeshes = psg;
	}

	vsg->_submeshes[vsg->_size++] = sg;
}

MeshGeometry* MeshGeometry_init()
{
	MeshGeometry* mg = (MeshGeometry*)malloc(sizeof(MeshGeometry));
	assert(mg);

	mg->_IndexBufferGPU = NULL;
	mg->_VertexBufferGPU = NULL;

	mg->_IndexDefaultBuffer = NULL;
	mg->_VertexDefaultBuffer = NULL;

	mg->_v_submeshes = V_SubmeshGeometry_init();

	mg->IndexFormat = DXGI_FORMAT_R16_UINT;
	mg->IndexBufferByteSize = 0;
	mg->VertexByteStride = 0;
	mg->VertexBufferByteSize = 0;

	return mg;
}

void MeshGeometry_free(MeshGeometry* mg)
{
	if (mg->_VertexDefaultBuffer)
		mg->_VertexDefaultBuffer->lpVtbl->Release(mg->_VertexDefaultBuffer);

	if (mg->_IndexDefaultBuffer)
		mg->_IndexDefaultBuffer->lpVtbl->Release(mg->_IndexDefaultBuffer);
	
	if (mg->_VertexBufferGPU)
		mg->_VertexBufferGPU->lpVtbl->Release(mg->_VertexBufferGPU);

	if (mg->_IndexBufferGPU)
		mg->_IndexBufferGPU->lpVtbl->Release(mg->_IndexBufferGPU);
		

	V_SubmeshGeometry_free(mg->_v_submeshes);
	free(mg);
}

D3D12_VERTEX_BUFFER_VIEW MeshGeometry_VertexBufferView(MeshGeometry* mg)
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = mg->_VertexBufferGPU->lpVtbl->GetGPUVirtualAddress(mg->_VertexBufferGPU);
	vbv.StrideInBytes = mg->VertexByteStride;
	vbv.SizeInBytes = mg->VertexBufferByteSize;

	return vbv;
}

D3D12_INDEX_BUFFER_VIEW MeshGeometry_IndexBufferView(MeshGeometry* mg)
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = mg->_IndexBufferGPU->lpVtbl->GetGPUVirtualAddress(mg->_IndexBufferGPU);
	ibv.Format = mg->IndexFormat;
	ibv.SizeInBytes = mg->IndexBufferByteSize;

	return ibv;
}

PipelineStateObject* PipelineStateObject_init()
{
	PipelineStateObject* pso = (PipelineStateObject*)malloc(sizeof(PipelineStateObject));
	assert(pso);

	memset(pso, 0, sizeof(PipelineStateObject));

	return pso;
}

void PipelineStateObject_free(PipelineStateObject* pso)
{
	if (pso->_PSO)
		pso->_PSO->lpVtbl->Release(pso->_PSO);

	if (pso->_vsByteCode)
		pso->_vsByteCode->lpVtbl->Release(pso->_vsByteCode);

	if (pso->_psByteCode)
		pso->_psByteCode->lpVtbl->Release(pso->_psByteCode);

	free(pso);
}

int Material_getIndex(Material* mats,int count, const char* n)
{
	for (int ix = 0; ix < count; ++ix)
	{
		if (_stricmp(n, mats[ix]._name) == 0)
			return ix;
	}

	return -1;
}

RenderItem* RenderItem_init()
{
	RenderItem* ri = (RenderItem*)malloc(sizeof(RenderItem));
	assert(ri);

	memset(ri, 0, sizeof(RenderItem));

	ri->NumFramesDirty = g_NumFrameResources;
	ri->ObjCBIndex = -1;
	ri->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	return ri;
}

void RenderItem_free(RenderItem* ri)
{
	free(ri);
}

V_RenderItem* V_RenderItem_init()
{
	V_RenderItem* vri = (V_RenderItem*)malloc(sizeof(V_RenderItem));
	assert(vri);

	vri->_size = 0;
	vri->_capacity = EXPAND;
	vri->_ris = (RenderItem**)malloc(sizeof(RenderItem*) * vri->_capacity);
	assert(vri->_ris);

	return vri;
}

void V_RenderItem_free(V_RenderItem* vri)
{
	free(vri->_ris);
	free(vri);
}

void V_RenderItem_pushback(V_RenderItem* vri, RenderItem* ri)
{
	if (vri->_size >= vri->_capacity)
	{
		vri->_capacity += EXPAND;
		RenderItem** ppv = (RenderItem**)realloc(vri->_ris, vri->_capacity * sizeof(RenderItem*));
		assert(ppv);
		vri->_ris = ppv;
	}

	vri->_ris[vri->_size++] = ri;
}
