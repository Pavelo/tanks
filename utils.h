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

float3 scalProduct(float a, float3 b);
float dotProduct(float3 a, float3 b);
float magnitude(float3 m);
float3 normalize(float3 n);
float3 subtract(float3 u, float3 v);
float normalize180(float angle);
float normalizeMPI(float angle);
float getSign(float f);
float4 matrixVecMult(float *m, float4 v);
float4 threeToFour(float3 v);
float3 fourToThree(float4 v);
float degrees(float radians);
float radians(float degrees);
