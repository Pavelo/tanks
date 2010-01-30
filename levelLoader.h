#include "collisions.h"
#include <dirent.h>

#define DIM_TILE 5.0f
#define PWUP_RESPAWN_TIME 10.0f

struct _powerup
{
	obj* model;
	char type;
	int active;
	int placed;
	float timer;
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
	int levelType;
};
typedef struct _map map;

int levelList(DIR* directory, char fileName[][32]);
map* loadLevel(char* path, obj* mapObs[], obj* pwup[]);
void drawLevel(map* myMap);
