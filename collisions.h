#include "objLoader.h"

struct _collisionMap
{
	BoundingBox obsBB[40][40];
	BoundingBox tanksBB[3];
	//	BoundingSphere bulletsBB[10]; // meglio mettere una bb nella struttura bullet?
};
typedef struct _collisionMap collisionMap;

BoundingBox* placeBoundingBox(BoundingBox* b, float* tMatrix);
