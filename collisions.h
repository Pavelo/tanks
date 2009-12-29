#include "objLoader.h"

struct _collisionMap
{
	BoundingBox obsBB[40][40];
	OrientedBoundingBox tanksBB[3];
	BoundingBox border;
	//	BoundingSphere bulletsBB[10]; // meglio mettere una bb nella struttura bullet?
};
typedef struct _collisionMap collisionMap;

BoundingBox placeBoundingBox(BoundingBox* b, float* tMatrix);
OrientedBoundingBox placeOrientedBoundingBox(OrientedBoundingBox* ob, float* tMatrix);
int isColliding(float3 p, BoundingBox* a);
