#include "objLoader.h"

#define DIM_TILE 5.0f

typedef struct
{
	int x;
	int y;
	int type;
	obj* model;
} obstacle;

typedef struct
{
	int side;
	int enemies;
	obstacle obs[40][40];
} map;

map* loadLevel(char* path);
void drawLevel(map* myMap);