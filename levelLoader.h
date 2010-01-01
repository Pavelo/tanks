#include "collisions.h"

#define DIM_TILE 5.0f
#define PWUP_RESPAWN_TIME 2000

struct _powerup
{
	obj* model;
	char type;
	int active;
	int timer;
};
typedef struct _powerup powerup;

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
	powerup pwup[40][40];
	obj* sky;
	obj* background[2];
	int loaded;
	collisionMap cm;
	float pwupRot;
};
typedef struct _map map;

map* loadLevel(char* path);
void drawLevel(map* myMap);
