#include "collisions.h"

#define DIM_TILE 5.0f

struct _obstacle
{
	obj* model;
	int type;
	BoundingBox obsBB;
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
	collisionMap cm;
};
typedef struct _map map;

map* loadLevel(char* path);
void drawLevel(map* myMap);
