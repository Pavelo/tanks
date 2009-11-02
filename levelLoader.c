#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "levelLoader.h"

map myMap;

map* loadLevel(char* path)
{
	//map myMap;
	int loaded;
	char line[100];
	int dim;
	
	FILE *fp = fopen(path,"r");
	
	if (fp != NULL)
	{
		int i = 0;
		while (fgets(line, 99, fp))
		{
			// dimensioni della mappa
			if (line[0] == 'd')
			{
				sscanf(line, "%*c %d", &dim);
				switch (dim)
				{
					case 0:
						myMap.lenght = 20;
						break;
					case 1:
						myMap.lenght = 25;
						break;
					case 2:
						myMap.lenght = 30;
						break;
					default:
						myMap.lenght = 20;
						break;
				}
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
