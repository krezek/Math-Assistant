#include "pch.h"

#include <general.h>
#include <xmath.h>
#include <geo_op.h>


static bool check_point_side(XFLOAT3* p, XFLOAT3* p1, XFLOAT3* p2, XFLOAT3* p3)
{
	bool r = true;

	XVECTOR v1, v2, v3, v;

	v1 = XLoadFloat3(p1);
	v2 = XLoadFloat3(p2);
	v3 = XLoadFloat3(p3);
	v = XLoadFloat3(p);

	v2 = XVectorSubtract(v2, v1);
	v3 = XVectorSubtract(v3, v1);
	v = XVectorSubtract(v, v1);

	v = XVector3Dot(v, XVector3Cross(v2, v3));
	r = r && (XVectorGetX(v) <= 0);

	return r;
}

bool is_point_inside(XFLOAT3* p, V_Vertex* vVertices, V_Index16* vIndices)
{
	bool r = true;

	for (int ix = 0; ix < vIndices->_size; ix += 3)
	{
		XFLOAT3 p1 = vVertices->_vertices[vIndices->_indices[ix + 0]].Position;
		XFLOAT3 p2 = vVertices->_vertices[vIndices->_indices[ix + 1]].Position;
		XFLOAT3 p3 = vVertices->_vertices[vIndices->_indices[ix + 2]].Position;

		r = r && check_point_side(p, &p1, &p2, &p3);
	}

	return r;
}

bool get_intersect_point(XFLOAT3 poly_p1, XFLOAT3 poly_p2, XFLOAT3 poly_p3,
	XFLOAT3 p1, XFLOAT3 p2,
	XFLOAT3* p)
{
	XVECTOR pv1, pv2, n, d;

	pv1 = XVectorSubtract(XLoadFloat3(&poly_p2), XLoadFloat3(&poly_p1));
	pv2 = XVectorSubtract(XLoadFloat3(&poly_p3), XLoadFloat3(&poly_p1));
	n = XVector3Normalize(XVector3Cross(pv1, pv2));

	d = XVector3Normalize(XVectorSubtract(XLoadFloat3(&p2), XLoadFloat3(&p1)));

	if (XVectorGetX(XVector3Dot(n, d)) == 0)
	{
		printf("error get_intersect_point...\n");
	}

	float t = XVectorGetX(XVectorDivide(XVector3Dot(n, XVectorSubtract(XLoadFloat3(&poly_p1), XLoadFloat3(&p1))),
		XVector3Dot(n, d)));
		
	XStoreFloat3(p ,
		XVectorAdd(XLoadFloat3(&p1), 
			XVectorSet(t * XVectorGetX(d), t * XVectorGetY(d), t * XVectorGetZ(d), 0.0f)));

	XVECTOR v0, v1, v2;
	v0 = XVectorSubtract(XLoadFloat3(&poly_p3), XLoadFloat3(&poly_p1));
	v1 = XVectorSubtract(XLoadFloat3(&poly_p2), XLoadFloat3(&poly_p1));
	v2 = XVectorSubtract(XLoadFloat3(p), XLoadFloat3(&poly_p1));

	XVECTOR d00, d01, d11, d20, d21;
	d00 = XVector3Dot(v0, v0);
	d01 = XVector3Dot(v0, v1);
	d11 = XVector3Dot(v1, v1);
	d20 = XVector3Dot(v2, v0);
	d21 = XVector3Dot(v2, v1);

	float denom = XVectorGetX(d00)*XVectorGetX(d11) - XVectorGetX(d01)*XVectorGetX(d01);
	float u = (XVectorGetX(d11) * XVectorGetX(d20) - XVectorGetX(d01) * XVectorGetX(d21)) / denom;
	float v = (XVectorGetX(d00) * XVectorGetX(d21) - XVectorGetX(d01) * XVectorGetX(d20)) / denom;
	float w = 1 - u - v;

	if (u >= 0 && v >= 0 && w >= 0)
	{
		return true;
	}

	return false;
}

bool get_intersect_point_all(V_Vertex* minus_vVertices, V_Index16* minus_vIndices,
	XFLOAT3 p1, XFLOAT3 p2,
	XFLOAT3* p)
{
	for (int ix = 0; ix < minus_vIndices->_size; ix += 3)
	{
		XFLOAT3 mp1 = minus_vVertices->_vertices[minus_vIndices->_indices[ix + 0]].Position;
		XFLOAT3 mp2 = minus_vVertices->_vertices[minus_vIndices->_indices[ix + 1]].Position;
		XFLOAT3 mp3 = minus_vVertices->_vertices[minus_vIndices->_indices[ix + 2]].Position;

		bool s1 = check_point_side(&p1, &mp1, &mp2, &mp3);
		bool s2 = check_point_side(&p2, &mp1, &mp2, &mp3);

		if ((s1 && !s2) || (!s1 && s2))
		{
			if (get_intersect_point(mp1, mp2, mp3, p1, p2, p))
				return true;
		}
	}

	return false;
}

void subtract(V_Vertex* result_vVertices, V_Index16* result_vIndices, 
	V_Vertex* origin_vVertices, V_Index16* origin_vIndices,
	V_Vertex* minus_vVertices, V_Index16* minus_vIndices)
{
	V_Index16* tmp_vIndices = V_Index16_init();

	const size_t msize = origin_vIndices->_size;
	for (int ix = 0; ix < msize; ix += 3)
	{
		XFLOAT3 p1 = origin_vVertices->_vertices[origin_vIndices->_indices[ix + 0]].Position;
		XFLOAT3 p2 = origin_vVertices->_vertices[origin_vIndices->_indices[ix + 1]].Position;
		XFLOAT3 p3 = origin_vVertices->_vertices[origin_vIndices->_indices[ix + 2]].Position;

		bool p1_inside = is_point_inside(&p1, minus_vVertices, minus_vIndices);
		bool p2_inside = is_point_inside(&p2, minus_vVertices, minus_vIndices);
		bool p3_inside = is_point_inside(&p3, minus_vVertices, minus_vIndices);

		if (p1_inside && p2_inside && p3_inside)
		{
			// all points are inside: do nothing(ignore adding points to result vector)
		}
		else
		{
			if (!p1_inside && !p2_inside && !p3_inside)
			{
				// Todo: all outside but there is intersection
				V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 0]);
				V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 1]);
				V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 2]);
			}
			else
			{
				XFLOAT3 intersectP1, intersectP2;

				if (!p1_inside && p2_inside && p3_inside)
				{
					bool s1 = get_intersect_point_all(minus_vVertices, minus_vIndices, p1, p2, &intersectP1);
					bool s2 = get_intersect_point_all(minus_vVertices, minus_vIndices, p1, p3, &intersectP2);

					if (s1 && s2 == false)
						printf("subtract error...\n");

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP1.x, intersectP1.y, intersectP1.z }
					});

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP2.x, intersectP2.y, intersectP2.z }
					});

					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 0]);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 2);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 1);

				}
				else if (p1_inside && !p2_inside && p3_inside)
				{
					bool s1 = get_intersect_point_all(minus_vVertices, minus_vIndices, p2, p1, &intersectP1);
					bool s2 = get_intersect_point_all(minus_vVertices, minus_vIndices, p2, p3, &intersectP2);

					if (s1 && s2 == false)
						printf("subtract error...\n");

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP1.x, intersectP1.y, intersectP1.z }
					});

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP2.x, intersectP2.y, intersectP2.z }
					});

					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 1]);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 2);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 1);
				}
				else if (!p1_inside && !p2_inside && p3_inside)
				{
					bool s1 = get_intersect_point_all(minus_vVertices, minus_vIndices, p1, p3, &intersectP1);
					bool s2 = get_intersect_point_all(minus_vVertices, minus_vIndices, p2, p3, &intersectP2);

					if (s1 && s2 == false)
						printf("subtract error...\n");

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP1.x, intersectP1.y, intersectP1.z }
					});

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP2.x, intersectP2.y, intersectP2.z }
					});

					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 0]);
					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 1]);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 2);

					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 2);
					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 1]);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 1);
				}
				else if (p1_inside && p2_inside && !p3_inside)
				{
					bool s1 = get_intersect_point_all(minus_vVertices, minus_vIndices, p3, p1, &intersectP1);
					bool s2 = get_intersect_point_all(minus_vVertices, minus_vIndices, p3, p2, &intersectP2);

					if (s1 && s2 == false)
						printf("subtract error...\n");

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP1.x, intersectP1.y, intersectP1.z }
					});

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP2.x, intersectP2.y, intersectP2.z }
					});

					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 2]);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 2);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 1);
				}
				else if (!p1_inside && p2_inside && !p3_inside)
				{
					bool s1 = get_intersect_point_all(minus_vVertices, minus_vIndices, p1, p2, &intersectP1);
					bool s2 = get_intersect_point_all(minus_vVertices, minus_vIndices, p3, p2, &intersectP2);

					if (s1 && s2 == false)
						printf("subtract error...\n");

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP1.x, intersectP1.y, intersectP1.z }
					});

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP2.x, intersectP2.y, intersectP2.z }
					});

					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 0]);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 2);
					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 2]);

					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 2]);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 1);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 2);
				}
				else if (p1_inside && !p2_inside && !p3_inside)
				{
					bool s1 = get_intersect_point_all(minus_vVertices, minus_vIndices, p1, p2, &intersectP1);
					bool s2 = get_intersect_point_all(minus_vVertices, minus_vIndices, p1, p3, &intersectP2);

					if (s1 && s2 == false)
						printf("subtract error...\n");

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP1.x, intersectP1.y, intersectP1.z }
					});

					V_Vertex_pushback(origin_vVertices,
						(Vertex) { { intersectP2.x, intersectP2.y, intersectP2.z }
					});

					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 2);
					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 1]);
					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 2]);

					V_Index16_pushback(tmp_vIndices, origin_vIndices->_indices[ix + 2]);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 2);
					V_Index16_pushback(tmp_vIndices, (unsigned short)origin_vVertices->_size - 1);
				}
			}
			
		}
	}

	for (int iy = 0; iy < tmp_vIndices->_size; ++iy)
	{
		V_Index16_pushback(result_vIndices, tmp_vIndices->_indices[iy]);
	}

	for (int ix = 0; ix < origin_vVertices->_size; ++ix)
	{
		bool hasRef = false;

		for (int iy = 0; iy < tmp_vIndices->_size; ++iy)
		{
			if (ix == tmp_vIndices->_indices[iy])
			{
				hasRef = true;
				break;
			}
		}

		if (hasRef)
		{
			V_Vertex_pushback(result_vVertices, origin_vVertices->_vertices[ix]);
		}
		else
		{
			for (int iy = 0; iy < tmp_vIndices->_size; ++iy)
			{
				if (tmp_vIndices->_indices[iy] > ix)
				{
					result_vIndices->_indices[iy] = result_vIndices->_indices[iy] - 1;
				}
			}
		}
	}

	V_Index16_free(tmp_vIndices);
}
