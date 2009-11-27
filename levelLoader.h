#include "objLoader.h"

#define DIM_TILE 5.0f

struct _obstacle
{
	obj* model;
	float3 boundingVertices[8];
};
typedef struct _obstacle obstacle;

struct _map
{
	int width;
	int depth;
	float posX;
	float posY;
	int enemies;
	obstacle obs[40][40];
	obj* sky;
	obj* background[2];
	int loaded;
};
typedef struct _map map;

map* loadLevel(char* path);
void drawLevel(map* myMap);
void placeBoundingBox(obstacle *o, float3 *vertices);
