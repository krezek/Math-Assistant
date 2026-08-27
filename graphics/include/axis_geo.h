#ifndef _AXIS_GEO_H_
#define _AXIS_GEO_H_

typedef struct _Material Material;
typedef struct _RenderItem RenderItem;
typedef struct _V_RenderItem V_RenderItem;
typedef struct _MeshGeometry MeshGeometry;

typedef struct _AxisGeo
{
	MeshGeometry* _mesh_geometry;
} AxisGeo;

AxisGeo* AxisGeo_init();
void AxisGeo_free(AxisGeo* sg);

void AxisGeo_build(AxisGeo* sg, ID3D12Device* d3dDevice, ID3D12GraphicsCommandList* commandList);
void AxisGeo_add_render_items(AxisGeo* sg, V_RenderItem* v_all_RI);

#endif /* _AXIS_GEO_H_ */
