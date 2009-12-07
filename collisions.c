#include "collisions.h"

BoundingBox tb;

BoundingBox* placeBoundingBox(BoundingBox* b, float* tMatrix)
{

	float4 vMin = threeToFour(b->min);
	float4 vMax = threeToFour(b->max);
	
	vMin = matrixVecMult(tMatrix, vMin);
	vMax = matrixVecMult(tMatrix, vMax);
	tb.min = fourToThree(vMin);
	tb.max = fourToThree(vMax);
	
	return &tb;
}
