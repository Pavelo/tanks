#include "utils.h"

//float4* vres;

float dotProduct(float3 a, float3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// prodotto matrice 4x4 per vettore 4x1
float4 matrixVecMult(float *m, float4 v)
{
	int i, j;
	float4 vres;
	
	// scorro prima sulle righe poi sulle colonne per conformità alle matrici passate da OpenGL 
	vres.x = m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12] * v.w;
	vres.y = m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13] * v.w;
	vres.z = m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w;
	vres.w = m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w;
	
	return vres;
}
