#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

typedef struct _V_Vertex V_Vertex;
typedef struct _V_Index16 V_Index16;

typedef struct _Sphere {
	float _radius;
	int _slice_count, _stack_count;
} GSphere;

void Sphere_populate(GSphere* s, V_Vertex* vVertices, V_Index16* vIndices);

#endif // _GEOMETRY_H_
