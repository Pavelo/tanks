#include "objLoader.h"

struct _collisionMap
{
	BoundingBox obsBB[40][40];
	OrientedBoundingBox tanksBB[10];
	BoundingBox border;
};
typedef struct _collisionMap collisionMap;

BoundingBox placeBoundingBox(BoundingBox* b, float* tMatrix);
OrientedBoundingBox placeOrientedBoundingBox(OrientedBoundingBox* ob, float* tMatrix);
int isColliding(float3 p, BoundingBox* a);
