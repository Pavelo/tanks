#include "utils.h"
#include <stdio.h>

float vres[4];

float dotProduct(float3 a, float3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// prodotto matrice 4x4 per vettore 4x1
float* matrixVecMult(float m[][4], float *v)
{
	int i, j;
	
	for (i=0; i<4; i++)
		vres[i] = 0;
	
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			vres[i] += m[i][j] * v[j];
	
	return vres;
}
