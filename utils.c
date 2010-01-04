#include "utils.h"

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

float4 threeToFour(float3 v)
{
	float4 v4;
	
	v4.x = v.x;
	v4.y = v.y;
	v4.z = v.z;
	v4.w = 1.0f;
	
	return v4;
}

float3 fourToThree(float4 v)
{
	float3 v3;
	
	v3.x = v.x;
	v3.y = v.y;
	v3.z = v.z;
	
	return v3;
}

float3 scalProduct(float s, float3 u)
{
    float3 temp;
    temp.x=u.x*s;
    temp.y=u.y*s;
    temp.z=u.z*s;
	return temp;
}

float magnitude(float3 m)
{
    return sqrt(m.x*m.x + m.y*m.y + m.z*m.z);
}

float3 normalize(float3 n)
{
    float3 temp;
    temp.x=n.x/magnitude(n);
    temp.y=n.y/magnitude(n);
    temp.z=n.z/magnitude(n);
	return temp;
}
float3 subtract(float3 u, float3 v)
{
    float3 temp;
    temp.x=u.x - v.x;
    temp.y=u.y - v.y;
    temp.z=u.z - v.z;
	return temp;
}

//angoli
float normalize180(float angle)
{
      if(angle>180.0f)
         angle -= 360.0f;
      else if(angle<-180.0f)
         angle += 360.0f;
      return angle;
}
float getSign(float f)
{
      if(f<0.0f)
      return -1.0f;
      else
      return 1.0f;
}
