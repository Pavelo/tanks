#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "levelLoader.h"

map* loadLevel(char* path)
{
	int loaded, dim, i=0, x, y=0;
	char line[100];
	obj* obs[20];
	map* myMap;
	float2 BBpos = {0.0f, 0.0f};
	
	myMap = (map*) malloc(sizeof(map));
	
	// Caricamento modelli
	myMap->sky = (obj*) loadOBJ("obj/sky.obj");
	myMap->background[0] = (obj*) loadOBJ("obj/des_wall.obj");
	myMap->background[1] = (obj*) loadOBJ("obj/des_mountain.obj");
	obs[0] = (obj*) loadOBJ("obj/des_ground.obj");
	obs[1] = (obj*) loadOBJ("obj/pwup_ammo.obj");
	obs[2] = (obj*) loadOBJ("obj/des_trench_ns.obj");
	obs[3] = (obj*) loadOBJ("obj/des_trench_we.obj");
	obs[4] = (obj*) loadOBJ("obj/des_trench_end_n.obj");
	obs[5] = (obj*) loadOBJ("obj/des_trench_end_s.obj");
	obs[6] = (obj*) loadOBJ("obj/des_trench_end_w.obj");
	obs[7] = (obj*) loadOBJ("obj/des_trench_end_e.obj");
	
	// Creo la bounding box di ogni modello
	createBoundingBox(obs[0]);
	createBoundingBox(obs[1]);
	createBoundingBox(obs[2]);
	createBoundingBox(obs[3]);
	createBoundingBox(obs[4]);
	createBoundingBox(obs[5]);
	createBoundingBox(obs[6]);
	createBoundingBox(obs[7]);
	
//	isColliding(obs[0].b.pivot, {1.0f, })
	
	FILE *fp = fopen(path,"r");
	
	if (fp != NULL)
	{
		while (fgets(line, 99, fp))
		{
			// numero di nemici
			if (line[0] == 'e')
			{
				sscanf(line, "%*c %d", &myMap->enemies);
			}
			// mappa
			else if (line[0] == 'm')
			{
				x=0;
				while (line[x+2] != 10) {	// finch� non trovo un a capo (LF)
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
					x++;
				}
				y++;
			}
		}

		fclose(fp);
		
		myMap->width = x;
		myMap->depth = y;
		// calcolo la posizione della mappa, diversa a seconda se il numero di tile � pari o dispari
		myMap->posX = -floor(myMap->width * 0.5f) * DIM_TILE + (1 - fmodf(myMap->width, 2)) * 0.5f * DIM_TILE;
		myMap->posY = -floor(myMap->depth * 0.5f) * DIM_TILE + (1 - fmodf(myMap->depth, 2)) * 0.5f * DIM_TILE;
		
		// imposto le posizioni delle bounding box degli ostacoli statici
//		for (y=0; y < myMap->depth; y++)
//		{
//			for (x=0; x < myMap->width; x++)
//			{
////				BBpos.x = x * DIM_TILE;
////				BBpos.y = myMap->obs[x][y].model->bb
////				BBpos.z = y * DIM_TILE;
////				placeBoundingBox(&myMap->obs[x][y].model->bb, BBpos);
//				myMap->obs[x][y].model->bb.min.x = myMap->posX * x - DIM_TILE * 0.5f;
//				myMap->obs[x][y].model->bb.min.z = myMap->posY * y - DIM_TILE * 0.5f;
//				myMap->obs[x][y].model->bb.max.x = myMap->posX * x + DIM_TILE * 0.5f;
//				myMap->obs[x][y].model->bb.max.z = myMap->posY * y + DIM_TILE * 0.5f;
//				printf("[%d][%d] min.x %f\t\tmin.z %f\t\tmax.x %f\t\tmax.z %f\n", x, y, myMap->obs[x][y].model->bb.min.x, myMap->obs[x][y].model->bb.min.z, myMap->obs[x][y].model->bb.max.x, myMap->obs[x][y].model->bb.max.z);
//			}
//		}
		
		
		myMap->loaded = 1;
	}
	else
		myMap->loaded = 0;
	
	return myMap;
}

void drawLevel(map* myMap)
{
	int i, x, y;
	
	glPushMatrix();
		glPushMatrix(); // disegno il muro
			glScalef(myMap->width * DIM_TILE, 155.0f, myMap->depth * DIM_TILE);
			glEnable(GL_RESCALE_NORMAL);
			drawOBJ(myMap->background[0]);
		glPopMatrix();
		glPushMatrix(); // disegno le montagne
			glScalef(myMap->width * DIM_TILE, fmin(myMap->width * DIM_TILE, myMap->depth *DIM_TILE), myMap->depth * DIM_TILE);
			glEnable(GL_RESCALE_NORMAL);
			drawOBJ(myMap->background[1]);
		glPopMatrix();
		glTranslatef(myMap->posX, 0.0f, myMap->posY);
		for (y=0; y < myMap->depth; y++)
		{
			for (x=0; x < myMap->width; x++)
			{
				glPushMatrix();
					glTranslatef(x * DIM_TILE, 0.0f, y * DIM_TILE);
					glScalef(DIM_TILE, DIM_TILE, DIM_TILE);
					drawOBJ(myMap->obs[x][y].model);
				glPopMatrix();
			}
		}
	glDisable(GL_RESCALE_NORMAL);
	glPopMatrix();
}

