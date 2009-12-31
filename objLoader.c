#include <GLUT/glut.h>
//#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tga.h"
#include "objLoader.h"

int txtId = 0;
float m[16];
BoundingBox b;

int loadMTL(char* path, obj* model)
{
	int loaded;
	char line[100];
	
	// inizializzo le stringhe
	int i;
	for (i=0; i<32; i++)
	{
		model->texturePath[i] = '\0';
	}
	
	//assegno un identificativo per la texture da caricare per ogni modello
	model->textureId = txtId;
	txtId++;
	
	FILE *fp = fopen(path,"r");
	
	if (fp != NULL)
	{
		while (fgets(line, 99, fp))
		{
			if (strstr(line, "map_Kd") != NULL)
			{
				strncpy(model->textureName, line + 7, strlen(line)-8);
				printf(" [%s ", model->textureName);
				sprintf(model->texturePath, "texture/%s", model->textureName);
				printf("%d]", loadTGA(model->texturePath, model->textureId));
				
				// reinizializzo le stringhe
				for (i=0; i<32; i++)
				{
					model->texturePath[i] = '\0';
				}
			}
			else if (strstr(line, "Kd") != NULL)
			{
				sscanf(line, "%*c%*c %f %f %f", &model->diffuse[0], &model->diffuse[1], &model->diffuse[2]);
			}
			else if (strstr(line, "Ka") != NULL)
			{
				sscanf(line, "%*c%*c %f %f %f", &model->ambient[0], &model->ambient[1], &model->ambient[2]);
			}
			else if (strstr(line, "Ks") != NULL)
			{
				sscanf(line, "%*c%*c %f %f %f", &model->specular[0], &model->specular[1], &model->specular[2]);
			}
			else if (strstr(line, "Ns") != NULL)
			{
				sscanf(line, "%*c%*c %f", &model->shininess);
			}
		}
		loaded = 1;
	}
	else
	{
		loaded = 0;
	}
	
	fclose(fp);
	
	return loaded;
}

obj* loadOBJ(char* path)
{
	int loaded;
	char line[100];
	obj* model;
	model = (obj*) malloc(sizeof(obj));
	
	model->vCount = 0;
	int vtCount = 0;
	int vnCount = 0;
	model->fCount = 0;
	model->f3Count = 0;
	
	FILE *fp = fopen(path,"r");
	
	char mtllibName[32];
	char mtllibPath[32];
	
	// inizializzo le stringhe
	int i;
	for (i=0; i<32; i++)
	{
		mtllibName[i] = '\0';
		mtllibPath[i] = '\0';
	}
	
	if (fp != NULL)
	{
		while (fgets(line, 99, fp))
		{
			if (line[0] == 'v')
			{
				// texture vertex
				if (line[1] == 't')
				{
					sscanf(line, "%*c%*c %f %f", &model->vt[vtCount].u, &model->vt[vtCount].v);
					vtCount++;
				}
				// normal vertex
				else if (line[1] == 'n')
				{
					sscanf(line, "%*c%*c %f %f %f", &model->vn[vnCount].x, &model->vn[vnCount].y, &model->vn[vnCount].z);
					vnCount++;
				}
				// vertex
				else
				{
					sscanf(line, "%*c %f %f %f", &model->v[model->vCount].x, &model->v[model->vCount].y, &model->v[model->vCount].z);
					model->vCount++;
				}
			}
			// face
			else if (line[0] == 'f')
			{
				int n = 0;
				int spaceCount = 0;
				while (line[n] != '\0')
				{
					if (line[n]==' ')
						spaceCount++;
					n++;
				}
				if (spaceCount == 3)
				{
					sscanf(line, "%*c %d/%d/%d %d/%d/%d %d/%d/%d",
						   &model->f3[model->f3Count].v1, &model->f3[model->f3Count].t1, &model->f3[model->f3Count].n1,
						   &model->f3[model->f3Count].v2, &model->f3[model->f3Count].t2, &model->f3[model->f3Count].n2,
						   &model->f3[model->f3Count].v3, &model->f3[model->f3Count].t3, &model->f3[model->f3Count].n3);
					model->f3Count++;
				}
				else if (spaceCount == 4)
				{
					sscanf(line, "%*c %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
						   &model->f[model->fCount].v1, &model->f[model->fCount].t1, &model->f[model->fCount].n1,
						   &model->f[model->fCount].v2, &model->f[model->fCount].t2, &model->f[model->fCount].n2,
						   &model->f[model->fCount].v3, &model->f[model->fCount].t3, &model->f[model->fCount].n3,
						   &model->f[model->fCount].v4, &model->f[model->fCount].t4, &model->f[model->fCount].n4);
					model->fCount++;
				}
			}
			else if (strstr(line, "mtllib") != NULL)
			{
				strncpy(mtllibName, line+7, strlen(line)-8);
				printf("%s", mtllibName);
				sprintf(mtllibPath, "obj/%s", mtllibName);
				printf(" %d\n",loadMTL(mtllibPath,model));
				
				// reinizializzo le stringhe
				for (i=0; i<32; i++)
				{
					mtllibName[i] = '\0';
					mtllibPath[i] = '\0';
				}
			}
			//        	printf("v %f %f %f\n", model.v[model->vCount].x, model.v[model->vCount].y, model.v[model->vCount].z);
			//        	printf("%f %f %f\n", model.vn[f[model->fCount].n1-1].x, model.vn[f[model->fCount].n1-1].y, model.vn[f[model->fCount].n1-1].z);
		}
		loaded = 1;
	}
	else
	{
		loaded = 0;
	}
	
	fclose(fp);
	
	
	return model;
}

void drawOBJ(obj* model)
{
	//	printf("ambient %f %f %f\n", ambient[0], ambient[1], ambient[2]);
	//	printf("diffuse %f %f %f\n", diffuse[0], diffuse[1], diffuse[2]);
	//	printf("specular %f %f %f\n", specular[0], specular[1], specular[2]);
	//	printf("shininess %f\n", shininess);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, model->ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, model->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, model->specular);
	glMaterialf(GL_FRONT, GL_SHININESS, model->shininess);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, model->textureId);
	
	glBegin(GL_QUADS);
	
	int i;
	for (i=0; i<model->fCount; i++)
	{
		glNormal3f(model->vn[model->f[i].n1-1].x, model->vn[model->f[i].n1-1].y, model->vn[model->f[i].n1-1].z);
		glTexCoord2f(model->vt[model->f[i].t1-1].u, model->vt[model->f[i].t1-1].v);
		glVertex3f(model->v[model->f[i].v1-1].x, model->v[model->f[i].v1-1].y, model->v[model->f[i].v1-1].z);
		
		glNormal3f(model->vn[model->f[i].n2-1].x, model->vn[model->f[i].n2-1].y, model->vn[model->f[i].n2-1].z);
		glTexCoord2f(model->vt[model->f[i].t2-1].u, model->vt[model->f[i].t2-1].v);
		glVertex3f(model->v[model->f[i].v2-1].x, model->v[model->f[i].v2-1].y, model->v[model->f[i].v2-1].z);
		
		glNormal3f(model->vn[model->f[i].n3-1].x, model->vn[model->f[i].n3-1].y, model->vn[model->f[i].n3-1].z);
		glTexCoord2f(model->vt[model->f[i].t3-1].u, model->vt[model->f[i].t3-1].v);
		glVertex3f(model->v[model->f[i].v3-1].x, model->v[model->f[i].v3-1].y, model->v[model->f[i].v3-1].z);
		
		glNormal3f(model->vn[model->f[i].n4-1].x, model->vn[model->f[i].n4-1].y, model->vn[model->f[i].n4-1].z);
		glTexCoord2f(model->vt[model->f[i].t4-1].u, model->vt[model->f[i].t4-1].v);
		glVertex3f(model->v[model->f[i].v4-1].x, model->v[model->f[i].v4-1].y, model->v[model->f[i].v4-1].z);
		
		/*printf("v1 %f %f %f\n", v[f[i].v1-1].x, v[f[i].v1-1].y, v[f[i].v1-1].z);
		 printf("v2 %f %f %f\n", v[f[i].v2-1].x, v[f[i].v2-1].y, v[f[i].v2-1].z);
		 printf("v3 %f %f %f\n", v[f[i].v3-1].x, v[f[i].v3-1].y, v[f[i].v3-1].z);
		 printf("v4 %f %f %f\n", v[f[i].v4-1].x, v[f[i].v4-1].y, v[f[i].v4-1].z);
		 
		 printf("vn1 %f %f %f\n", vn[f[i].n1-1].x, vn[f[i].n1-1].y, vn[f[i].n1-1].z);
		 printf("vn2 %f %f %f\n", vn[f[i].n2-1].x, vn[f[i].n2-1].y, vn[f[i].n2-1].z);
		 printf("vn3 %f %f %f\n", vn[f[i].n3-1].x, vn[f[i].n3-1].y, vn[f[i].n3-1].z);
		 printf("vn4 %f %f %f\n", vn[f[i].n4-1].x, vn[f[i].n4-1].y, vn[f[i].n4-1].z);*/
	}
	
	glEnd();
	
	glBegin(GL_TRIANGLES);
	
	for (i=0; i<model->f3Count; i++)
	{
		glNormal3f(model->vn[model->f3[i].n1-1].x, model->vn[model->f3[i].n1-1].y, model->vn[model->f3[i].n1-1].z);
		glTexCoord2f(model->vt[model->f3[i].t1-1].u, model->vt[model->f3[i].t1-1].v);
		glVertex3f(model->v[model->f3[i].v1-1].x, model->v[model->f3[i].v1-1].y, model->v[model->f3[i].v1-1].z);
		
		glNormal3f(model->vn[model->f3[i].n2-1].x, model->vn[model->f3[i].n2-1].y, model->vn[model->f3[i].n2-1].z);
		glTexCoord2f(model->vt[model->f3[i].t2-1].u, model->vt[model->f3[i].t2-1].v);
		glVertex3f(model->v[model->f3[i].v2-1].x, model->v[model->f3[i].v2-1].y, model->v[model->f3[i].v2-1].z);
		
		glNormal3f(model->vn[model->f3[i].n3-1].x, model->vn[model->f3[i].n3-1].y, model->vn[model->f3[i].n3-1].z);
		glTexCoord2f(model->vt[model->f3[i].t3-1].u, model->vt[model->f3[i].t3-1].v);
		glVertex3f(model->v[model->f3[i].v3-1].x, model->v[model->f3[i].v3-1].y, model->v[model->f3[i].v3-1].z);
		
		/*            printf("v1 %f %f %f\n", v[f[i].v1-1].x, v[f[i].v1-1].y, v[f[i].v1-1].z);
		 printf("v2 %f %f %f\n", v[f[i].v2-1].x, v[f[i].v2-1].y, v[f[i].v2-1].z);
		 printf("v3 %f %f %f\n", v[f[i].v3-1].x, v[f[i].v3-1].y, v[f[i].v3-1].z);
		 
		 printf("vn1 %f %f %f\n", vn[f[i].n1-1].x, vn[f[i].n1-1].y, vn[f[i].n1-1].z);
		 printf("vn2 %f %f %f\n", vn[f[i].n2-1].x, vn[f[i].n2-1].y, vn[f[i].n2-1].z);
		 printf("vn3 %f %f %f\n", vn[f[i].n3-1].x, vn[f[i].n3-1].y, vn[f[i].n3-1].z);*/
	}
	
	glEnd();
	
//	drawBoundingBox(&model->bb);
	
	glDisable(GL_TEXTURE_2D);
}

// ritorna la bounding box che racchiude tutte le bb nell'array passato come argomento; n è il numero di elementi dell'array
BoundingBox* BBUnion(BoundingBox *barr, int n)
{
	b.max.x = -10000.0f;
	b.max.y = -10000.0f;
	b.max.z = -10000.0f;
	b.min.x = 10000.0f;
	b.min.y = 10000.0f;
	b.min.z = 10000.0f;
	
	while (n >= 0)
	{
		n--;
		b.max.x = fmaxf(b.max.x, barr[n].max.x);
		b.max.y = fmaxf(b.max.y, barr[n].max.y);
		b.max.z = fmaxf(b.max.z, barr[n].max.z);
		b.min.x = fminf(b.min.x, barr[n].min.x);
		b.min.y = fminf(b.min.y, barr[n].min.y);
		b.min.z = fminf(b.min.z, barr[n].min.z);
	}
	
	return &b;
}

// crea una bounding box che rappresenta il volume in cui è inscritto il modello
void createBoundingBox(obj* model)
{
	int i;
	
	model->bb.max.x = -10000.0f;
	model->bb.max.y = -10000.0f;
	model->bb.max.z = -10000.0f;
	
	model->bb.min.x = 10000.0f;
	model->bb.min.y = 10000.0f;
	model->bb.min.z = 10000.0f;
	
	for (i=0; i < model->vCount; i++) {
		if (model->v[i].x > model->bb.max.x) model->bb.max.x = model->v[i].x;
		if (model->v[i].y > model->bb.max.y) model->bb.max.y = model->v[i].y;
		if (model->v[i].z > model->bb.max.z) model->bb.max.z = model->v[i].z;
		
		if (model->v[i].x < model->bb.min.x) model->bb.min.x = model->v[i].x;
		if (model->v[i].y < model->bb.min.y) model->bb.min.y = model->v[i].y;
		if (model->v[i].z < model->bb.min.z) model->bb.min.z = model->v[i].z;
	}
}



// rende visibile la bounding box associata al modello renderizzandola in wireframe
void drawBoundingBox(BoundingBox *b)
{
	
    glBegin (GL_LINE_LOOP);
    glVertex3f(b->max.x,b->max.y,b->min.z); // 0
    glVertex3f(b->min.x,b->max.y,b->min.z); // 1
    glVertex3f(b->min.x,b->min.y,b->min.z); // 2
    glVertex3f(b->max.x,b->min.y,b->min.z); // 3
    glEnd();
	
    glBegin (GL_LINE_LOOP);
    glVertex3f(b->max.x,b->min.y,b->max.z); // 4
    glVertex3f(b->max.x,b->max.y,b->max.z); // 5
    glVertex3f(b->min.x,b->max.y,b->max.z); // 6
    glVertex3f(b->min.x,b->min.y,b->max.z); // 7
    glEnd();
	
    glBegin (GL_LINE_LOOP);
    glVertex3f(b->max.x,b->max.y,b->min.z); // 0
    glVertex3f(b->max.x,b->max.y,b->max.z); // 5
    glVertex3f(b->min.x,b->max.y,b->max.z); // 6
    glVertex3f(b->min.x,b->max.y,b->min.z); // 1
    glEnd();
	
    glBegin (GL_LINE_LOOP);
    glVertex3f(b->max.x,b->min.y,b->max.z); // 4
    glVertex3f(b->min.x,b->min.y,b->max.z); // 7
    glVertex3f(b->min.x,b->min.y,b->min.z); // 2
    glVertex3f(b->max.x,b->min.y,b->min.z); // 3
	
    glEnd();
	
} 
