#include "utils.h"

struct _Vertex
{
	
	float x;
	float y;
	float z;
	
};
typedef struct _Vertex Vertex;

struct _VTexture
{
	
	float u;
	float v;
	
};
typedef struct _VTexture VTexture;

struct _VNormal
{
	
	float x;
	float y;
	float z;
	
};
typedef struct _VNormal VNormal;

struct _Face
{
	
	int v1;
	int t1;
	int n1;
	
	int v2;
	int t2;
	int n2;
	
	int v3;
	int t3;
	int n3;
	
	int v4;
	int t4;
	int n4;
	
};
typedef struct _Face Face;

struct _Face3
{
	
	int v1;
	int t1;
	int n1;
	
	int v2;
	int t2;
	int n2;
	
	int v3;
	int t3;
	int n3;
	
};
typedef struct _Face3 Face3;

struct _BoundingBox
{
	float3 max;
	float3 min;
};
typedef struct _BoundingBox BoundingBox;

struct _OrientedBoundingBox
{
	float3 vert[8];
};
typedef struct _OrientedBoundingBox OrientedBoundingBox;

struct _obj
{
	Vertex v[50000];
	VTexture vt[50000];
	VNormal vn[50000];
	Face f[50000];
	Face3 f3[50000];
	int vCount;
	int fCount;
	int f3Count;
	
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float shininess;
	
	int textureId;
	char textureName[32];
	char texturePath[32];
	
	BoundingBox bb;
};
typedef struct _obj obj;

obj* loadOBJ(char* path);
void drawOBJ(obj* model);
void createBoundingBox(obj* model);
BoundingBox* BBUnion(BoundingBox *barr, int n);
void drawBoundingBox(BoundingBox *b);
