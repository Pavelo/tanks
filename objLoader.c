#include <GLUT/glut.h>
//#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tga.h"
#include "objLoader.h"

//obj model;
int txtId = 0;

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
	
	int vCount = 0;
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
					sscanf(line, "%*c %f %f %f", &model->v[vCount].x, &model->v[vCount].y, &model->v[vCount].z);
					vCount++;
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
			//        	printf("v %f %f %f\n", model.v[vCount].x, model.v[vCount].y, model.v[vCount].z);
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
	
	glDisable(GL_TEXTURE_2D);
}

void defBoundingBox(obj* model, float w, float h, float d)
{
	model->b.w = w;
	model->b.h = h;
	model->b.d = d;
	model->b.pivot.x = 0.0f;
	model->b.pivot.y = h * 0.5f;
	model->b.pivot.z = 0.0f;
}

void isColliding(float3 point, float3 normal)
{
	float re = fabsf( dotProduct(point, normal));
	printf("%f\n", re);
}
