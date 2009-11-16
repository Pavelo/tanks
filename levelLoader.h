#include "objLoader.h"

#define DIM_TILE 5.0f

typedef struct
{
	int type;
	obj* model;
} obstacle;

typedef struct
{
	int width;
	int height;
	float posX;
	float posY;
	int enemies;
	obstacle obs[40][40];
	obj* sky;
	obj* background[2];
} map;

map* loadLevel(char* path);
void drawLevel(map* myMap);
