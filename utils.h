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

struct _float4
{
	float x;
	float y;
	float z;
	float w;
};
typedef struct _float4 float4;

float dotProduct(float3 a, float3 b);
float4 matrixVecMult(float *m, float4 v);
