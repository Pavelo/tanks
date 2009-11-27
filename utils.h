struct _float2
{
	float x;
	float y;
};
typedef struct _float2 float2;

struct _float3
{
	float x;
	float y;
	float z;
};
typedef struct _float3 float3;

float dotProduct(float3 a, float3 b);
float* matrixVecMult(float m[][4], float* v);
