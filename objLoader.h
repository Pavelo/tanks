typedef struct
{

	float x;
	float y;
	float z;

} Vertex;

typedef struct
{

	float u;
	float v;

} VTexture;

typedef struct
{

	float x;
	float y;
	float z;

} VNormal;

typedef struct
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

} Face;

typedef struct
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

} Face3;

typedef struct
{
	Vertex v[50000];
	VTexture vt[50000];
	VNormal vn[50000];
	Face f[50000];
	Face3 f3[50000];

	float ambient[3];
	float diffuse[3];
	float specular[3];
	float shininess;

	int textureId;
	char textureName[32];
	char texturePath[32];
} obj;

obj* loadOBJ(char* path);
void drawOBJ(obj* model);
