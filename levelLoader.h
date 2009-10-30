typedef struct
{
	int x;
	int y;
	int type;
} obstacle;

typedef struct
{
	int lenght;
	int enemies;
	obstacle obs[100];
} map;

map* loadLevel(char* path);
