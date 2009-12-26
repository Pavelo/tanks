#include "collisions.h"

BoundingBox tb;
OrientedBoundingBox otb;

BoundingBox placeBoundingBox(BoundingBox* b, float* tMatrix)
{

	float4 vMin = threeToFour(b->min);
	float4 vMax = threeToFour(b->max);
	
	vMin = matrixVecMult(tMatrix, vMin);
	vMax = matrixVecMult(tMatrix, vMax);
	tb.min = fourToThree(vMin);
	tb.max = fourToThree(vMax);
	
	return tb;
}

OrientedBoundingBox placeOrientedBoundingBox(OrientedBoundingBox* ob, float* tMatrix)
{
	int i;
	float4 v[8];
	
	for (i=0; i<8; i++)
	{
		v[i] = threeToFour(ob->vert[i]);
		v[i] = matrixVecMult(tMatrix, v[i]);
		otb.vert[i] = fourToThree(v[i]);
	}
	
	return otb;
}

int isColliding(float3 p, BoundingBox* a)
{
	return p.x <= a->max.x && p.x >= a->min.x && p.y <= a->max.y && p.y >= a->min.y && p.z <= a->max.z && p.z >= a->min.z;
}
