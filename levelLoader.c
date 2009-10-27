#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	int x;
	int y;
	int type;
} obstacle;

typedef struct
{
	int lenght;
	int width;
	int enemies;
	obstacle obs[100];
} map;

map myMap;

map* loadLevel(char* path)
{
	//map myMap;
	int loaded;
	char line[100];

	FILE *fp = fopen(path,"r");

	if (fp != NULL)
	{
		int i = 0;
		while (fgets(line, 99, fp))
		{
			// dimensioni della mappa
			if (line[0] == 'd')
			{
				sscanf(line, "%*c %d %d", &myMap.lenght, &myMap.width);
			}
			// numero di nemici
			else if (line[0] == 'e')
			{
				sscanf(line, "%*c %d", &myMap.enemies);
			}
			// ostacolo
			else if (line[0] == 'o')
			{
				sscanf(line, "%*c%d %d %d", &myMap.obs[i].type, &myMap.obs[i].x, &myMap.obs[i].y);
				i++;
			}
		}

		loaded = 1;
	}
	else
		loaded = 0;

	return &myMap;
}
