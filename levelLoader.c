#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "levelLoader.h"

//map myMap;

map* loadLevel(char* path)
{
	int loaded, dim, i=0, x, y=0;
	char line[100];
	obj* obs[20];
	
	obs[0] = (obj*) loadOBJ("obj/des_ground.obj");
	obs[1] = (obj*) loadOBJ("obj/des_cactus.obj");
	obs[2] = (obj*) loadOBJ("obj/des_trench_ns.obj");
	obs[3] = (obj*) loadOBJ("obj/des_trench_we.obj");
	obs[4] = (obj*) loadOBJ("obj/des_trench_end_n.obj");
	obs[5] = (obj*) loadOBJ("obj/des_trench_end_s.obj");
	obs[6] = (obj*) loadOBJ("obj/des_trench_end_w.obj");
	obs[7] = (obj*) loadOBJ("obj/des_trench_end_e.obj");
	
	map* myMap;
	myMap = (map*) malloc(sizeof(map));
	
	FILE *fp = fopen(path,"r");
	
	if (fp != NULL)
	{
		while (fgets(line, 99, fp))
		{
			// dimensioni della mappa
			if (line[0] == 'd')
			{
				sscanf(line, "%*c %d", &dim);
				switch (dim)
				{
					case 0:
						myMap->side = 5;
						break;
					case 1:
						myMap->side = 30;
						break;
					case 2:
						myMap->side = 40;
						break;
					default:
						myMap->side = 20;
						break;
				}
			}
			// numero di nemici
			else if (line[0] == 'e')
			{
				sscanf(line, "%*c %d", &myMap->enemies);
			}
			// ostacolo
			else if (line[0] == 'o')
			{
//				sscanf(line, "%*c%d %d %d", &myMap->obs[i].type, &myMap->obs[i].x, &myMap->obs[i].y);
//				myMap->obs[i].model = obs[myMap->obs[i].type];
				
				i++;
			}
			else if (line[0] == 'm')
			{
				for (x=0; x<myMap->side; x++) {
					switch (line[x+2]) {
						case 'o':
							myMap->obs[x][y].model = obs[0];
							break;
						case 'Y':
							myMap->obs[x][y].model = obs[1];
							break;
						case 'I':
							myMap->obs[x][y].model = obs[2];
							break;
						case '=':
							myMap->obs[x][y].model = obs[3];
							break;
						case 'n':
							myMap->obs[x][y].model = obs[4];
							break;
						case 'u':
							myMap->obs[x][y].model = obs[5];
							break;
						case '(':
							myMap->obs[x][y].model = obs[6];
							break;
						case ')':
							myMap->obs[x][y].model = obs[7];
							break;
						default:
							break;
					}
				}
				y++;
			}
		}
		
		fclose(fp);
		
		loaded = 1;
	}
	else
		loaded = 0;
	
	return myMap;
}

void drawLevel(map* myMap)
{
	int i, x, y;
	float mapPos = -DIM_TILE * myMap->side;
	
	glPushMatrix();
	{
		glTranslatef(mapPos * 0.5f, 0.0f, mapPos);
		for (y=0; y < myMap->side; y++)
		{
			for (x=0; x < myMap->side; x++) {
				glPushMatrix();
				{
					glTranslatef(x * DIM_TILE, 0.0f, y * DIM_TILE);
					glScalef(DIM_TILE, DIM_TILE, DIM_TILE);
					glEnable(GL_RESCALE_NORMAL);
					drawOBJ(myMap->obs[x][y].model);
					glDisable(GL_RESCALE_NORMAL);
					glPopMatrix();
				}
			}
		}
	}
	glPopMatrix();
}
