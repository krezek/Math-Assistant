#include "pch.h"

#include <..\..\CFG.h>
#include <utils.h>
#include <xmath.h>
#include <geometry.h>
#include <general.h>
#include <axis_geo.h>

AxisGeo* AxisGeo_init()
{
    AxisGeo* sg = (AxisGeo*)malloc(sizeof(AxisGeo));
	assert(sg);

	memset(sg, 0, sizeof(AxisGeo));

	sg->_mesh_geometry = MeshGeometry_init();
	sg->_mesh_geometry->VertexByteStride = sizeof(Vertex);

	return sg;
}

void AxisGeo_free(AxisGeo* vg)
{
	MeshGeometry_free(vg->_mesh_geometry);

	free(vg);
}

void AxisGeo_build(AxisGeo* sg, ID3D12Device* d3dDevice, ID3D12GraphicsCommandList* commandList)
{
    V_Vertex* vVertices = V_Vertex_init();
    V_Index16* vIndices = V_Index16_init();

    const float length = 4.0f;
    const float arraw = 0.1f;

    V_Vertex_pushback(vVertices, (Vertex) { {-length, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { length, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { length, -arraw, -arraw }, { 1.0f, 0.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { length, -arraw, arraw }, { 1.0f, 0.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { length, arraw, arraw }, { 1.0f, 0.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { length, arraw, -arraw }, { 1.0f, 0.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { length + 2 * arraw, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } });

    V_Vertex_pushback(vVertices, (Vertex) { { 0.0f, -length, 0.0f }, { 0.0f, 1.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { 0.0f, length, 0.0f }, { 0.0f, 1.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { -arraw, length, -arraw }, { 0.0f, 1.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { -arraw, length, arraw }, { 0.0f, 1.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { arraw, length, arraw }, { 0.0f, 1.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { arraw, length, -arraw }, { 0.0f, 1.0f, 0.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { 0.0f, length + 2 * arraw, 0.0f }, { 0.0f, 1.0f, 0.0f } });

    V_Vertex_pushback(vVertices, (Vertex) { { 0.0f, 0.0f, -length }, { 0.0f, 0.0f, 1.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { 0.0f, 0.0f, length }, { 0.0f, 0.0f, 1.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { -arraw, -arraw, length }, { 0.0f, 0.0f, 1.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { -arraw, arraw, length }, { 0.0f, 0.0f, 1.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { arraw, arraw, length }, { 0.0f, 0.0f, 1.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { arraw, -arraw, length }, { 0.0f, 0.0f, 1.0f } });
    V_Vertex_pushback(vVertices, (Vertex) { { 0.0f, 0.0f, length + 2 * arraw }, { 0.0f, 0.0f, 1.0f } });

    V_Index16_pushback(vIndices, 0);
    V_Index16_pushback(vIndices, 1);

    V_Index16_pushback(vIndices, 2);
    V_Index16_pushback(vIndices, 3);
    V_Index16_pushback(vIndices, 6);

    V_Index16_pushback(vIndices, 3);
    V_Index16_pushback(vIndices, 4);
    V_Index16_pushback(vIndices, 6);

    V_Index16_pushback(vIndices, 4);
    V_Index16_pushback(vIndices, 5);
    V_Index16_pushback(vIndices, 6);

    V_Index16_pushback(vIndices, 5);
    V_Index16_pushback(vIndices, 2);
    V_Index16_pushback(vIndices, 6);

    V_Index16_pushback(vIndices, 7);
    V_Index16_pushback(vIndices, 8);

    V_Index16_pushback(vIndices, 9);
    V_Index16_pushback(vIndices, 10);
    V_Index16_pushback(vIndices, 13);

    V_Index16_pushback(vIndices, 10);
    V_Index16_pushback(vIndices, 11);
    V_Index16_pushback(vIndices, 13);

    V_Index16_pushback(vIndices, 11);
    V_Index16_pushback(vIndices, 12);
    V_Index16_pushback(vIndices, 13);

    V_Index16_pushback(vIndices, 12);
    V_Index16_pushback(vIndices, 9);
    V_Index16_pushback(vIndices, 13);

    V_Index16_pushback(vIndices, 14);
    V_Index16_pushback(vIndices, 15);

    V_Index16_pushback(vIndices, 16);
    V_Index16_pushback(vIndices, 17);
    V_Index16_pushback(vIndices, 20);

    V_Index16_pushback(vIndices, 17);
    V_Index16_pushback(vIndices, 18);
    V_Index16_pushback(vIndices, 20);

    V_Index16_pushback(vIndices, 18);
    V_Index16_pushback(vIndices, 19);
    V_Index16_pushback(vIndices, 20);

    V_Index16_pushback(vIndices, 19);
    V_Index16_pushback(vIndices, 16);
    V_Index16_pushback(vIndices, 20);
    
    SubmeshGeometry smg;

    smg.IndexCount = 2;
    smg.BaseVertexLocation = 0;
    smg.StartIndexLocation = 0;
    V_SubmeshGeometry_pushback(sg->_mesh_geometry->_v_submeshes, smg);

    smg.IndexCount = 12;
    smg.BaseVertexLocation = 0;
    smg.StartIndexLocation = 2;
    V_SubmeshGeometry_pushback(sg->_mesh_geometry->_v_submeshes, smg);

    smg.IndexCount = 2;
    smg.BaseVertexLocation = 0;
    smg.StartIndexLocation = 14;
    V_SubmeshGeometry_pushback(sg->_mesh_geometry->_v_submeshes, smg);

    smg.IndexCount = 12;
    smg.BaseVertexLocation = 0;
    smg.StartIndexLocation = 16;
    V_SubmeshGeometry_pushback(sg->_mesh_geometry->_v_submeshes, smg);

    smg.IndexCount = 2;
    smg.BaseVertexLocation = 0;
    smg.StartIndexLocation = 28;
    V_SubmeshGeometry_pushback(sg->_mesh_geometry->_v_submeshes, smg);

    smg.IndexCount = 12;
    smg.BaseVertexLocation = 0;
    smg.StartIndexLocation = 30;
    V_SubmeshGeometry_pushback(sg->_mesh_geometry->_v_submeshes, smg);
    
    sg->_mesh_geometry->VertexBufferByteSize = (UINT)(sizeof(Vertex) * vVertices->_size);
    sg->_mesh_geometry->IndexBufferByteSize = (UINT)(sizeof(unsigned __int16) * vIndices->_size);
    sg->_mesh_geometry->VertexByteStride = sizeof(Vertex);

    sg->_mesh_geometry->_VertexBufferGPU = Utils_CreateDefaultBuffer(d3dDevice,
        commandList,
        vVertices->_vertices,
        sg->_mesh_geometry->VertexBufferByteSize,
        &sg->_mesh_geometry->_VertexDefaultBuffer);

    sg->_mesh_geometry->_IndexBufferGPU = Utils_CreateDefaultBuffer(d3dDevice,
        commandList,
        vIndices->_indices,
        sg->_mesh_geometry->IndexBufferByteSize,
        &sg->_mesh_geometry->_IndexDefaultBuffer);

    V_Index16_free(vIndices);
    V_Vertex_free(vVertices);
}

void AxisGeo_add_render_items(AxisGeo* sg, Material* materials, int matCount, V_RenderItem* v_all_RI, V_RenderItem* v_opaque_RI)
{
    for (int ix = 0; ix < sg->_mesh_geometry->_v_submeshes->_size; ++ix)
    {
        RenderItem* ri = RenderItem_init();

        ri->IndexCount = sg->_mesh_geometry->_v_submeshes->_submeshes[ix].IndexCount;
        ri->BaseVertexLocation = sg->_mesh_geometry->_v_submeshes->_submeshes[ix].BaseVertexLocation;
        ri->StartIndexLocation = sg->_mesh_geometry->_v_submeshes->_submeshes[ix].StartIndexLocation;

        ri->Geo = sg->_mesh_geometry;

        if(ix%2 == 0)
            ri->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        else
            ri->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        ri->ObjCBIndex = (UINT)v_all_RI->_size;
        ri->World = Identity4x4();
        //XMATRIX RST = XMatrixTranslation(0.0f, -40.0f, 0.0f);
        //XStoreFloat4x4(&ri->World, RST);

        ri->TexTransform = Identity4x4();

        V_RenderItem_pushback(v_all_RI, ri);
        V_RenderItem_pushback(v_opaque_RI, ri);
    }
}
