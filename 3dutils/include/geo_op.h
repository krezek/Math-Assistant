#ifndef _GEO_OP_H_
#define _GEO_OP_H_

bool is_point_inside(XFLOAT3* p, V_Vertex* vVertices, V_Index16* vIndices);
void subtract(V_Vertex* result_vVertices, V_Index16* result_vIndices,
	V_Vertex* origin_vVertices, V_Index16* origin_vIndices,
	V_Vertex* minus_vVertices, V_Index16* minus_vIndices);

#endif // _GEO_OP_H_
