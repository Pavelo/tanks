#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "levelLoader.h"

map* loadLevel(char* path)
{
	int loaded, dim, i, x, y=0;
	char line[100];
	obj* obs[20];
	obj* pwup[5];
	map* myMap;
	float3 BBpos = {0.0f, 0.0f, 0.0f};
	float mat[16];
	
	myMap = (map*) malloc(sizeof(map));
	
	// Caricamento modelli powerup
	pwup[0] = (obj*) loadOBJ("obj/pwup_ammo.obj");
	pwup[1] = (obj*) loadOBJ("obj/pwup_health.obj");
	pwup[2] = (obj*) loadOBJ("obj/pwup_quad_damage.obj");
	
	FILE *fp = fopen(path,"r");
	
	if (fp != NULL)
	{
		while (fgets(line, 99, fp))
		{
			//tipo di livello
            if (line[0] == 'l')
            {
                sscanf(line, "%*c %d", &myMap->levelType);
                if(myMap->levelType==1) //deserto
                {
                	myMap->sky = (obj*) loadOBJ("obj/sky.obj");
               		myMap->background[0] = (obj*) loadOBJ("obj/des_wall.obj");
                	myMap->background[1] = (obj*) loadOBJ("obj/des_dunes.obj");
                    obs[0] = (obj*) loadOBJ("obj/des_ground.obj");
                    obs[1] = (obj*) loadOBJ("obj/des_cactus.obj");
                    obs[2] = (obj*) loadOBJ("obj/des_trench_ns.obj");
                	obs[3] = (obj*) loadOBJ("obj/des_trench_we.obj");
                	obs[4] = (obj*) loadOBJ("obj/des_trench_end_n.obj");
                	obs[5] = (obj*) loadOBJ("obj/des_trench_end_s.obj");
                	obs[6] = (obj*) loadOBJ("obj/des_trench_end_w.obj");
                	obs[7] = (obj*) loadOBJ("obj/des_trench_end_e.obj");
                }
                else if(myMap->levelType==2) //urbano
                {
                	myMap->sky = (obj*) loadOBJ("obj/sky_night.obj");
               		myMap->background[0] = (obj*) loadOBJ("obj/urb_wall.obj");
                	myMap->background[1] = (obj*) loadOBJ("obj/urb_building_big.obj");
                    obs[0] = (obj*) loadOBJ("obj/urb_ground.obj");
                    obs[1] = (obj*) loadOBJ("obj/urb_lamp.obj");
                    obs[2] = (obj*) loadOBJ("obj/urb_crate.obj");
                	obs[3] = (obj*) loadOBJ("obj/urb_crate.obj");
                	obs[4] = (obj*) loadOBJ("obj/urb_crate.obj");
                	obs[5] = (obj*) loadOBJ("obj/urb_crate.obj");
                	obs[6] = (obj*) loadOBJ("obj/urb_crate.obj");
                	obs[7] = (obj*) loadOBJ("obj/urb_crate.obj");
                }
				
            }
			// numero di nemici
			else if (line[0] == 'e')
			{
				sscanf(line, "%*c %d", &myMap->enemies);
				myMap->enemies = fminf(myMap->enemies, 9);
			}
			// mappa
			else if (line[0] == 'm')
			{
				x=0;
				while (line[x+2] != 10) {	// finch non trovo un a capo (LF)
					switch (line[x+2]) {
						case '.':
							myMap->obs[x][y].model = obs[0];
							myMap->obs[x][y].type = 0;
							myMap->pwup[x][y].placed = 0;
							break;
						case 'Y':
							myMap->obs[x][y].model = obs[1];
							myMap->obs[x][y].type = 2;
							myMap->pwup[x][y].placed = 0;
							break;
						case 'I':
							myMap->obs[x][y].model = obs[2];
							myMap->obs[x][y].type = 1;
							myMap->pwup[x][y].placed = 0;
							break;
						case '=':
							myMap->obs[x][y].model = obs[3];
							myMap->obs[x][y].type = 1;
							myMap->pwup[x][y].placed = 0;
							break;
						case 'n':
							myMap->obs[x][y].model = obs[4];
							myMap->obs[x][y].type = 1;
							myMap->pwup[x][y].placed = 0;
							break;
						case 'u':
							myMap->obs[x][y].model = obs[5];
							myMap->obs[x][y].type = 1;
							myMap->pwup[x][y].placed = 0;
							break;
						case '(':
							myMap->obs[x][y].model = obs[6];
							myMap->obs[x][y].type = 1;
							myMap->pwup[x][y].placed = 0;
							break;
						case ')':
							myMap->obs[x][y].model = obs[7];
							myMap->obs[x][y].type = 1;
							myMap->pwup[x][y].placed = 0;
							break;
						case 'p':
							myMap->obs[x][y].model = obs[0];
							myMap->pwup[x][y].model = pwup[0];
							myMap->pwup[x][y].type = line[x+2];
							myMap->pwup[x][y].placed = 1;
							myMap->pwup[x][y].active = 1;
							break;
						case 'h':
							myMap->obs[x][y].model = obs[0];
							myMap->obs[x][y].type = 0;
							myMap->pwup[x][y].model = pwup[1];
							myMap->pwup[x][y].type = line[x+2];
							myMap->pwup[x][y].placed = 1;
							myMap->pwup[x][y].active = 1;
							break;
						case '4':
							myMap->obs[x][y].model = obs[0];
							myMap->obs[x][y].type = 0;
							myMap->pwup[x][y].model = pwup[2];
							myMap->pwup[x][y].type = line[x+2];
							myMap->pwup[x][y].placed = 1;
							myMap->pwup[x][y].active = 1;
							break;
							
						default:
							myMap->obs[x][y].model = obs[0];
							myMap->obs[x][y].type = 0;
							myMap->pwup[x][y].placed = 0;
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
		// calcolo la posizione della mappa, diversa a seconda se il numero di tile  pari o dispari
		myMap->posX = -floor(myMap->width * 0.5f) * DIM_TILE + (1 - fmodf(myMap->width, 2)) * 0.5f * DIM_TILE;
		myMap->posY = -floor(myMap->depth * 0.5f) * DIM_TILE + (1 - fmodf(myMap->depth, 2)) * 0.5f * DIM_TILE;
		
		// Creo le bounding box dei confini per delimitare la mappa
		myMap->cm.border.min.x = -myMap->width * DIM_TILE * 0.5f;
		myMap->cm.border.min.y = -100.0f;
		myMap->cm.border.min.z = -myMap->depth * DIM_TILE * 0.5f;
		myMap->cm.border.max.x = myMap->width * DIM_TILE * 0.5f;
		myMap->cm.border.max.y = 100.0f;
		myMap->cm.border.max.z = myMap->depth * DIM_TILE * 0.5f;
		
		// Creo la bounding box di ogni modello
		createBoundingBox(obs[0]);
		createBoundingBox(obs[1]);
		createBoundingBox(obs[2]);
		createBoundingBox(obs[3]);
		createBoundingBox(obs[4]);
		createBoundingBox(obs[5]);
		createBoundingBox(obs[6]);
		createBoundingBox(obs[7]);
		
		for (i=2; i<8; i++)
		{
			if (myMap->levelType == 1)
				obs[i]->bb.max.y += 0.05f;
		}
		
		// Imposto le posizioni delle bounding box degli ostacoli statici
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(myMap->posX, 0.0f, myMap->posY);
		for (y=0; y < myMap->depth; y++)
		{
			for (x=0; x < myMap->width; x++)
			{
				glPushMatrix();
				glTranslatef(x * DIM_TILE, 0.0f, y * DIM_TILE);
				glScalef(DIM_TILE, DIM_TILE, DIM_TILE);
				glGetFloatv(GL_MODELVIEW_MATRIX, mat);
				myMap->cm.obsBB[x][y] = placeBoundingBox(&myMap->obs[x][y].model->bb, mat);
				glPopMatrix();
			}
		}
		glPopMatrix();
		
		myMap->loaded = 1;
	}
	else
		myMap->loaded = 0;
	
	return myMap;
}

void drawLevel(map* myMap)
{
	int i, x, y;
	
	glEnable(GL_RESCALE_NORMAL);
	glPushMatrix();
		glPushMatrix(); // disegno il muro
			glScalef(myMap->width * DIM_TILE, 155.0f, myMap->depth * DIM_TILE);
			drawOBJ(myMap->background[0]);
		glPopMatrix();
		glPushMatrix(); // disegno le montagne
			glScalef(myMap->width * DIM_TILE, fmin(myMap->width * DIM_TILE, myMap->depth *DIM_TILE), myMap->depth * DIM_TILE);
			drawOBJ(myMap->background[1]);
		glPopMatrix();
		// disegno il terreno, gli ostacoli e i powerup
		glTranslatef(myMap->posX, 0.0f, myMap->posY);
		for (x=0; x < myMap->width; x++)
		{
			for (y=0; y < myMap->depth; y++)
			{
				glPushMatrix();
					glTranslatef(x * DIM_TILE, 0.0f, y * DIM_TILE);
					glScalef(DIM_TILE, DIM_TILE, DIM_TILE);
					drawOBJ(myMap->obs[x][y].model); // disegno gli ostacoli o il terreno
					if (myMap->pwup[x][y].placed && myMap->pwup[x][y].active) // disegno i powerup
					{
						glRotatef(myMap->pwupRot, 0.0f, 1.0f, 0.0f);
						glScalef(0.5f, 0.5f, 0.5f);
						drawOBJ(myMap->pwup[x][y].model);
					}
				glPopMatrix();
			}
		}
	glDisable(GL_RESCALE_NORMAL);
	glPopMatrix();
}

int levelList(DIR* directory, char fileName[][32])
{
	struct dirent* dent;
	int i = 0;
	
	if(directory)
	{
		while((dent=readdir(directory)))
		{
			if (dent->d_name[0] != '.')
			{
				sprintf(fileName[i], "%s", dent->d_name);
				i++;
			}
		}
		closedir(directory);
	}
	else
		printf("Failed to open \"levels\" directory\n");
	
	return i;
}
