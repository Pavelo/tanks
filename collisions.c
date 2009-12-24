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
