#include "pch.h"

#include <general.h>
#include <xmath.h>
#include <geometry.h>


void Sphere_populate(GSphere* s, V_Vertex* vVertices, V_Index16* vIndices)
{
	float phiStep = X_PI / s->_stack_count;
	float thetaStep = X_2PI / s->_slice_count;

	V_Vertex_pushback(vVertices, (Vertex) { { 0.0f, s->_radius, 0.0f }, { 1.0f, 0.0f, 0.0f } });

	for (int i = 1; i < s->_stack_count; ++i)
	{
		float phi = i * phiStep;
		for (int j = 0; j <= s->_slice_count; ++j)
		{
			float theta = j * thetaStep;
			XVECTOR p =
			{
				s->_radius * sinf(phi) * cosf(theta),
				s->_radius * cosf(phi),
				s->_radius * sinf(phi) * sinf(theta)
			};

			XFLOAT3 pf;
			XStoreFloat3(&pf, p);

			V_Vertex_pushback(vVertices, (Vertex) { pf, { 1.0f, 0.0f, 0.0f } });
		}
	}

	V_Vertex_pushback(vVertices, (Vertex) { { 0.0f, -s->_radius, 0.0f }, { 1.0f, 0.0f, 0.0f } });

	for (int i = 1; i <= s->_slice_count; ++i)
	{
		V_Index16_pushback(vIndices, i + 1);
		V_Index16_pushback(vIndices, i);
		V_Index16_pushback(vIndices, 0);
	}

	int baseIndex = 1;
	int ringVertexCount = s->_slice_count + 1;
	for (int i = 0; i < s->_stack_count - 2; ++i)
	{
		for (int j = 0; j < s->_slice_count; ++j)
		{
			V_Index16_pushback(vIndices, baseIndex + i * ringVertexCount + j);
			V_Index16_pushback(vIndices, baseIndex + i * ringVertexCount + j + 1);
			V_Index16_pushback(vIndices, baseIndex + (i + 1) * ringVertexCount + j);
			V_Index16_pushback(vIndices, baseIndex + (i + 1) * ringVertexCount + j);
			V_Index16_pushback(vIndices, baseIndex + i * ringVertexCount + j + 1);
			V_Index16_pushback(vIndices, baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	int southPoleIndex = (int)vVertices->_size - 1;
	baseIndex = southPoleIndex - ringVertexCount;

	for (int i = 0; i <= s->_slice_count; ++i)
	{
		V_Index16_pushback(vIndices, baseIndex + i);
		V_Index16_pushback(vIndices, baseIndex + i + 1);
		V_Index16_pushback(vIndices, southPoleIndex);
	}
}
