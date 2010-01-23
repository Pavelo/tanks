//CAMBIAMENTI
//fisica in idle
//enginepower nella struttura,nell'init e nei tasti
//aggiunti tasti per la rotazione
//aggiunta variabile frictionCoefficient
//aggiunta frenata in tasti
//aggiunta visuale torretta in tasti,camera,variabili(turretView)
//cingoli caricati esternamente,aggiunte posizioni nell'init,variabile x l'animazione e nella display

#include <GLUT/glut.h>
//#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
//#include <windows.h>
#include "levelLoader.h"

//COSTANTI
#define WIDTH 800
#define HEIGHT 600
#define LIFETIME 0.3f

//STRUTTURE
//struttura palla di cannone
struct _bullet
{
	float radius;
	float pos[3];
	float v[3];
	float a[3];
	float mass;
	float3 scale;
	int hit;
};
typedef struct _bullet bullet;

//struttura palle di cannone
struct _bullets
{
	bullet bullet;
	float lifetime;
	struct _bullets *next;
};
typedef struct _bullets bullets;

//struttura carro armato
struct _cannon
{
	float pos[3];
	float rot;
	float length;
};
typedef struct _cannon cannon;

struct _turret
{
	float pos[3];
	float rot;
	cannon tankCannon;
};
typedef struct _turret turret;

struct _tread
{
	float pos[3];
};
typedef struct _tread tread;

struct _tank
{
	float scale; 
	float pos[3];             //vettore per la posizione
	float3 lastPos;           //vettore per la posizione all'istante di tempo precedente
	float v[3];               //vettore per la velocità
	float a[3];               //vettore per l'accelerazione
	float rot;                //valore in gradi per la rotazione
	float lastRot;            //valore in gradi per la rotazione all'istante di tempo precedente
	tread userTreadL;         //struttura per il cingolo sinistro
	tread userTreadR;         //struttura per il cingolo destro
	turret userTurret;        //struttura per la torretta
	float frictionForce;      //forza di attrito
	float throttle;           //modulo accelerazione
	float speed;              //modulo velocità
	float enginePower;        //potenza erogata dal motore
	float mass;               //massa del carro armato
	float iperBoostAccumul;
	float iperBoost;
	int weaponPower;          //potenza di attacco
	bullets *bulletRoot;      //struttura per i proiettili
	int ammo;                 //munizioni disponibili
	float life;               //valore di vita
	float rechargeTime;       //tempo passato per la ricarica
	float rechargeNeeded;     //tempo richiesto per la ricarica
	int state;                //stato: [0]-morto [1]-difendi [2]-temporeggia [3]-attacca
	int animationL;            //valore per l'animazione dei cingoli
	int animationR;            //valore per l'animazione dei cingoli
	int collision;
	float collisionAngle;
	int directCollision;
	int randomMoves;
	float randomAngle;
	OrientedBoundingBox boundingVol;  //bounding box che racchiude l'intero carro armato (coordinate world)
	float boundingRad;        //raggio della sfera che racchiude il carro armato
};
typedef struct _tank tank;

//VARIABILI
//tempi e frame
double deltaT = 0.0f;
struct timeval old;
//zbuffer
GLubyte *pixels;
//ratio
float ar;
//modalitˆ blur
float blur = 0.2f;
int blurActive = 0;
//carri armati
tank* tanks;
float frictionCoeff = 0.8f;
float distanceFromCamera = 10.0f;
int turretView = 0;
int fixedView  = 0;
map* levelMap;
obj* objTank[20];
float enemiesDefeated;
//stampe a video
char stampe[80];
char stampe2[80];
char stampe3 [80];
char rech[10];
char printScreen[30][80];
//matrici e vettori
float mat[16];
float4 vec;
float4 res;
//colori
float myColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
//fisica proiettili
float timePassed = 0.0f;
//float kkk = 0.0f;
float xBullet = 0.0f;
float yBullet = 0.0f;
//menu
int showMenu = 1;
int menuAction = -1;
int selectBoxPlace = 0;
int entries = 4;
char levelFileName[10][32];
int listLenght;
int gameLoaded = 0;
int currentLevel = -1;
int gameMode; // 0: new game | 1: play level
//luci
float light0Position[] = {0.0f, 1.0f, 0.6f, 0.0f};
float light1Position[] = {1.0f, 0.0f, 1.0f, 0.0f};
float light2Position[] = {-1.0f, 0.0f, -1.0f, 0.0f};
float light3Position[] = {0.0f, 1.0f, 0.0f, 0.0f};
float light4Position[] = {0.0f, 1.0f, 0.0f, 0.0f};

//FUNZIONI

// Controlla se  avvenuta una collisione con i confini della mappa
void borderCollision(tank* t, int i)
{
	float dumping = 0.2f;
	
	// controllo su 4 vertici
	if (!isColliding(levelMap->cm.tanksBB[i].vert[2], &levelMap->cm.border) ||
		!isColliding(levelMap->cm.tanksBB[i].vert[3], &levelMap->cm.border) ||
		!isColliding(levelMap->cm.tanksBB[i].vert[6], &levelMap->cm.border) ||
		!isColliding(levelMap->cm.tanksBB[i].vert[7], &levelMap->cm.border))
	{
		t->pos[0] = t->lastPos.x;
		t->pos[2] = t->lastPos.z;
		t->rot = t->lastRot;
		
		if (fabsf(t->pos[0]) > fabsf(t->pos[2]))
			t->v[0] *= -1.0f;
		else
			t->v[2] *= -1.0f;
		
		t->v[0] *= dumping;
		t->v[2] *= dumping;
	}
}

// Controlla se  avvenuta una collisione con qualche ostacolo statico della mappa
void staticCollision(tank* t, int i, int x, int y)
{
	float dumping = 0.2f;
	
	if (isColliding(levelMap->cm.tanksBB[i].vert[0], &levelMap->cm.obsBB[x][y]) ||
		isColliding(levelMap->cm.tanksBB[i].vert[1], &levelMap->cm.obsBB[x][y]) ||
		isColliding(levelMap->cm.tanksBB[i].vert[2], &levelMap->cm.obsBB[x][y]) ||
		isColliding(levelMap->cm.tanksBB[i].vert[3], &levelMap->cm.obsBB[x][y]) ||
		isColliding(levelMap->cm.tanksBB[i].vert[4], &levelMap->cm.obsBB[x][y]) ||
		isColliding(levelMap->cm.tanksBB[i].vert[5], &levelMap->cm.obsBB[x][y]) ||
		isColliding(levelMap->cm.tanksBB[i].vert[6], &levelMap->cm.obsBB[x][y]) ||
		isColliding(levelMap->cm.tanksBB[i].vert[7], &levelMap->cm.obsBB[x][y]))
	{
		t->pos[0] = t->lastPos.x;
		t->pos[2] = t->lastPos.z;
		t->rot = t->lastRot;
		t->v[0] *= -dumping;
		t->v[2] *= -dumping;
		
		t->directCollision=5;
		if(isColliding(levelMap->cm.tanksBB[i].vert[0], &levelMap->cm.obsBB[x][y]))
		{
        	t->directCollision=10;
        }     
    	else if(isColliding(levelMap->cm.tanksBB[i].vert[1], &levelMap->cm.obsBB[x][y]))
    	{
        	t->directCollision=-10;
        }
    	else if(isColliding(levelMap->cm.tanksBB[i].vert[4], &levelMap->cm.obsBB[x][y]))
    	{
        	t->directCollision=10;
        }
    	else if(isColliding(levelMap->cm.tanksBB[i].vert[5], &levelMap->cm.obsBB[x][y]))
    	{
        	t->directCollision=-10;
        }
	}
}

// Controlla se  avvenuta una collisione con qualche powerup
void powerupCollision(tank* t, int i, int x, int y)
{
	if ( (sqrtf( (t->pos[0] - x*DIM_TILE-levelMap->posX) * (t->pos[0] - x*DIM_TILE-levelMap->posX) + (t->pos[2] - y*DIM_TILE-levelMap->posY) * (t->pos[2] - y*DIM_TILE-levelMap->posY)) < (DIM_TILE*0.5f)) && levelMap->pwup[x][y].active && levelMap->pwup[x][y].placed)
	{
		levelMap->pwup[x][y].active = 0;
		levelMap->pwup[x][y].timer = PWUP_RESPAWN_TIME;
		switch (levelMap->pwup[x][y].type)
		{
			case 'p':
				t->ammo += 5;
				break;
			case 'h':
				t->life += 50.0f;
				break;
			case '4':
				t->weaponPower = 4;
				break;

				
			default:
				break;
		}
	}
}

// Controlla se  avvenuta una collisione tra i carri armati
void tanksCollision(tank* t1, tank* t2)
{
	float dumping = 0.2f;
	
	if ( sqrtf( (t2->pos[0] - t1->pos[0]) * (t2->pos[0] - t1->pos[0]) + (t2->pos[1] - t1->pos[1]) * (t2->pos[1] - t1->pos[1]) + (t2->pos[2] - t1->pos[2]) * (t2->pos[2] - t1->pos[2])) < (t1->boundingRad + t2->boundingRad) )
	{
		t1->pos[0] = t1->lastPos.x;
		t1->pos[2] = t1->lastPos.z;
		t1->v[0] *= -dumping;
		t1->v[2] *= dumping;
		
	}
}

// Controlla se  avvenuta una collisione tra un proiettile e un ostacolo
int bulletObsCollision(bullet* bul)
{
	int collide = 0;
	int x, y;
	float3 bulletPos = { bul->pos[0], bul->pos[1], bul->pos[2] };
	
	for (y=0; y < levelMap->depth; y++)
	{
		for (x=0; x < levelMap->width; x++)
		{
			if (isColliding(bulletPos, &levelMap->cm.obsBB[x][y]))
			{
				collide = 1;
				break;
			}
		}
	}
				
	return collide;
}

// Controlla se  avvenuta una collisione tra un proiettile e un carro armato
int bulletTankCollision(bullet* bul, tank* t)
{
	return sqrtf( (bul->pos[0] - t->pos[0]) * (bul->pos[0] - t->pos[0]) + (bul->pos[2] - t->pos[2]) * (bul->pos[2] - t->pos[2]) + (bul->pos[1] - t->pos[1]) * (bul->pos[1] - t->pos[1]) ) < (bul->radius + t->boundingRad);
}

int checkCollision(tank* t,float angle) 
{
    int result = 0;
    
    int x;
    int z;
    
    float distance = 20.0f;
    
    float3 tankPos;
    tankPos.x = -t->pos[0];
    tankPos.y = 0.0f;
    tankPos.z = -t->pos[2];
    
    float3 tankToObstacle;
    float3 tankToStraight;
    float pf;
    float3 p;
    float3 b;
    
    glPushMatrix();
    //identifica tutti gli ostacoli presenti nella mappa
    for (z=0; z < levelMap->depth; z++)
	{
		for (x=0; x < levelMap->width; x++) {
			//se è un ostacolo
			if(levelMap->obs[x][z].type>=1)
			{
				float xObs = x * DIM_TILE+levelMap->posX;
				float zObs = z * DIM_TILE+levelMap->posY;
				float radius = DIM_TILE;
				tankToObstacle.x = xObs - t->pos[0];
				tankToObstacle.y = 0.0f;
				tankToObstacle.z = zObs - t->pos[2];
				tankToStraight.x = -distance*sin(radians(angle));
				tankToStraight.y = 0.0f;
				tankToStraight.z = -distance*cos(radians(angle));
				
				pf = dotProduct(normalize(tankToStraight),normalize(tankToObstacle));
				pf = pf*magnitude(tankToObstacle);
				p  = scalProduct(pf,normalize(tankToStraight));
				b  = subtract(tankToObstacle,p);
				
				//verifica se c'è collisione
				float check=dotProduct(p,tankToStraight); //per controllare il segno di p
				if(magnitude(p)<magnitude(tankToStraight) && check>=0.0f && magnitude(b)<radius)
				{
                    result = 1;
				}
				
			}//END IF
		}//END FOR
	}//END FOR
    glPopMatrix();
    
//    if(result==1)
//		sprintf(stampe2,"Collisione rilevataXY!");
//    else
//		sprintf(stampe2,"Via liberaXY!");
    
    return result;
}

//Crea l'illuminazione per il livello DESERTO
void setDesertLights(void)
{
	float ambient0[] = {1.0f, 1.0f, 1.0f};
	float diffuse0[] = {1.0f, 1.0f, 1.0f};
	float specular0[] = {1.0f, 1.0f, 1.0f};
	
	float ambient1[] = {1.0f, 1.0f, 1.0f};
	float diffuse1[] = {0.7f, 0.7f, 0.7f};
	float specular1[] = {1.0f, 1.0f, 1.0f};
	
	float ambient2[] = {1.0f, 1.0f, 1.0f};
	float diffuse2[] = {0.5f, 0.5f, 0.5f};
	float specular2[] = {1.0f, 1.0f, 1.0f};
	
	glEnable(GL_LIGHTING);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glEnable(GL_LIGHT1);
	
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);
	glEnable(GL_LIGHT2);
}

//Crea l'illuminazione per il livello URBANO
void setUrbanLights(void)
{
	float ambient0[] = {0.5f, 0.5f, 0.5f};
	float diffuse0[] = {1.0f, 1.0f, 1.0f};
	float specular0[] = {1.0f, 1.0f, 1.0f};
	
	float ambient1[] = {0.9f, 0.9f, 0.9f};
	float diffuse1[] = {1.0f, 1.0f, 1.0f};
	float specular1[] = {0.0f, 0.0f, 0.0f};
	
	float ambient2[] = {0.0f, 0.0f, 0.0f};
	float diffuse2[] = {1.0f, 1.0f, 1.0f};
	float specular2[] = {0.0f, 0.0f, 0.0f};
	
    float ambient3[] = {0.0f, 0.0f, 0.0f};
	float diffuse3[] = {1.0f, 1.0f, 1.0f};
	float specular3[] = {0.0f, 0.0f, 0.0f};
	
	float ambient4[] = {1.0f, 1.0f, 1.0f};
	float diffuse4[] = {0.4f, 0.4f, 0.4f};
	float specular4[] = {0.0f, 0.0f, 0.0f};
	
	glEnable(GL_LIGHTING);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glEnable(GL_LIGHT1);
	
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);
	glEnable(GL_LIGHT2);
	
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient3);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse3);
	glLightfv(GL_LIGHT3, GL_SPECULAR, specular3);
	glEnable(GL_LIGHT3);
	
	glLightfv(GL_LIGHT4, GL_AMBIENT, ambient4);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuse4);
	glLightfv(GL_LIGHT4, GL_SPECULAR, specular4);
	glEnable(GL_LIGHT4);
	
	int z;int x;int c=0;
	for (z=0; z < levelMap->depth; z++)
	{
		for (x=0; x < levelMap->width; x++) 
		{
			//se è un ostacolo
			if(levelMap->obs[x][z].type==2)
			{
				if(c==0)
				{
                    light0Position[0] = x * DIM_TILE + levelMap->posX;
                    light0Position[1] = 2.0f * DIM_TILE;
                    light0Position[2] = z * DIM_TILE + levelMap->posY;
                    light0Position[3] = 1.0f;
				}
				if(c==1)
				{
                    light1Position[0] = x * DIM_TILE + levelMap->posX;
                    light1Position[1] = 2.0f * DIM_TILE;
                    light1Position[2] = z * DIM_TILE + levelMap->posY;
                    light1Position[3] = 1.0f;
				}
				if(c==2)
				{
                    light2Position[0] = x * DIM_TILE + levelMap->posX;
                    light2Position[1] = 2.0f * DIM_TILE;
                    light2Position[2] = z * DIM_TILE + levelMap->posY;
                    light2Position[3] = 1.0f;
				}
				if(c==3)
				{
                    light3Position[0] = x * DIM_TILE + levelMap->posX;
                    light3Position[1] = 2.0f * DIM_TILE;
                    light3Position[2] = z * DIM_TILE + levelMap->posY;
                    light3Position[3] = 1.0f;
				}
                c++;
			}
		}
    }
}

//Attiva le luci nel livello DESERTO
void displayDesertLights(void)
{
	float light0Position[] = {0.0f, 1.0f, 0.6f, 0.0f};
	float light1Position[] = {1.0f, 0.0f, 1.0f, 0.0f};
	float light2Position[] = {-1.0f, 0.0f, -1.0f, 0.0f};	
	
	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2Position);
}

//Attiva le luci nel livello URBANO
void displayUrbanLights(void)
{
    GLfloat spotDir[]={0.0,-1.0,0.0};
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,120.0);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT,1.0);	
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,0.1);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF,120.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,1.0);	
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,0.1);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDir);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF,120.0);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT,1.0);	
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION,0.1);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spotDir);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF,120.0);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT,1.0);	
	glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION,0.1);	
	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2Position);
	glLightfv(GL_LIGHT3, GL_POSITION, light3Position);
	
	glLightfv(GL_LIGHT4, GL_POSITION, light4Position);
}

//Abilita l'illuminazione della scena
void lightOn(void)
{
	glEnable(GL_LIGHTING);
}

//Disabilita l'illuminazione della scena
void lightOff(void)
{
	glDisable(GL_LIGHTING);
}

//Disabilita l'illuminzione e sposta in proiezione ortogonale
void orthogonalStart (void) {
	lightOff();
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, 0, HEIGHT);
	glScalef(1, -1, 1);
	glTranslatef(0, -HEIGHT, 0);
	glMatrixMode(GL_MODELVIEW);
}

//Ritorna in proiezione precedente e riabilita l'illuminazione
void orthogonalEnd (void) {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	lightOn();
}

void renderBitmapString(float x, float y, void *font, char *string)
{
    char *c;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x,y);
    for (c=string; *c != '\0'; c++)
    {
		glutBitmapCharacter(font, *c);
    }
    
    glColor3f(0.5f, 0.5f, 0.5f);
	glRasterPos2f(x+1,y+1);
    for (c=string; *c != '\0'; c++)
    {
		glutBitmapCharacter(font, *c);
    }
    glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2f(x+2,y+2);
    for (c=string; *c != '\0'; c++)
    {
		glutBitmapCharacter(font, *c);
    }
}

void renderLvlMenuString(float x, float y, void *font, char *string)
{
	char* c;
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}
    glColor3f(0.7f, 0.0f, 0.0f);
	glRasterPos2f(x+1,y);
    for (c=string; *c != '\0'; c++)
    {
		glutBitmapCharacter(font, *c);
    }
    glColor3f(0.3f, 0.0f, 0.0f);
	glRasterPos2f(x+2,y);
    for (c=string; *c != '\0'; c++)
    {
		glutBitmapCharacter(font, *c);
    }
}

void makeGrid(float lenght, float width, float nol)
{
	float l = lenght * 0.5;
	float w = width  * 0.5;
	float stepl = lenght/nol;
	float stepw = width/nol;
	int i;
	
	for (i=0; i<=nol; i++)
	{
		glBegin(GL_LINES);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(-w+(stepw*i),0.0f,-l);
		glVertex3f(-w+(stepw*i),0.0f,l);
		glEnd();
		glBegin(GL_LINES);
		glNormal3f(0.0f,1.0f,0.0f);
		glVertex3f(-w,0.0f,-l+(stepl*i));
		glVertex3f(w,0.0f,-l+(stepl*i));
		glEnd();
	}
}

void shoot(tank *tankT)
{
	tankT->ammo--;
	tankT->rechargeTime = 0.0f;
	bullets *b;
	b = malloc(sizeof(bullets));
	b->bullet.a[0] = 0.0f;
	b->bullet.a[1] = -9.8f;
	b->bullet.a[2] = 0.0f;
	b->bullet.mass = 1.0f;
	
	// calcolo della posizione iniziale del proiettile
	float mBullet[16];
	float3 bulletPos = { 0.0f, 0.0f, 0.0f };
	glPushMatrix();
	glLoadIdentity();
	glTranslatef( (-tankT->userTurret.tankCannon.length*cos(tankT->userTurret.tankCannon.rot*M_PI/180.0f)-tankT->userTurret.tankCannon.length*0.5f)*sin((tankT->rot+tankT->userTurret.rot)*M_PI/180.0f),
				  tankT->userTurret.tankCannon.length*sin(tankT->userTurret.tankCannon.rot*M_PI/180.0f),
				  (-tankT->userTurret.tankCannon.length*cos(tankT->userTurret.tankCannon.rot*M_PI/180.0f)-tankT->userTurret.tankCannon.length*0.5f)*cos((tankT->rot+tankT->userTurret.rot)*M_PI/180.0f) );
	glTranslatef(tankT->pos[0],tankT->pos[1]+tankT->userTurret.pos[1],tankT->pos[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, mBullet);
	glPopMatrix();
	bulletPos = fourToThree(matrixVecMult(mBullet, threeToFour(bulletPos)));
	b->bullet.pos[0] = bulletPos.x;
	b->bullet.pos[1] = bulletPos.y;
	b->bullet.pos[2] = bulletPos.z;

	float cannonSpeed = 35.0f;
    //la velocità di sparo viene separata in componente verticale e orizzontale rispetto alla rotazione del cannone
    //a questo punto l'orizzonatale viene separata in componente X e Z rispetto alla rotazione del carro e torretta
	b->bullet.v[0] = cannonSpeed*cos(radians(tankT->userTurret.tankCannon.rot))*sin(M_PI/180.0f*(tankT->rot+tankT->userTurret.rot));
	b->bullet.v[1] = cannonSpeed*sin(radians(tankT->userTurret.tankCannon.rot));
	b->bullet.v[2] = cannonSpeed*cos(radians(tankT->userTurret.tankCannon.rot))*cos(M_PI/180.0f*(tankT->rot+tankT->userTurret.rot));
	
	b->bullet.radius = 0.1f;
	b->bullet.scale.x = 1.0f;
	b->bullet.scale.y = 1.0f;
	b->bullet.scale.z = 1.0f;
	b->bullet.hit = -1;
	b->lifetime = 0.0f;
	
	b->next = tankT->bulletRoot;
	tankT->bulletRoot = b;
}

void animateLeftTurn(tank *t)
{
    t->animationR+=3;
	t->animationL-=3;
	if(t->animationL<0)
	    t->animationL=30;
}

void animateRightTurn(tank *t)
{
    t->animationL+=3;
	t->animationR-=3;
	if(t->animationR<0)
		t->animationR=30;
}

//controlla se un angolo di rotazione è all'interno di un certo step
int checkRotationInStep(float angle,float angleSign,float currentRotation,float step)
{
    if
		(
		 (currentRotation>(angle*angleSign*180.0f/M_PI)-step && currentRotation<(angle*angleSign*180.0f/M_PI)+step) 
		 ||
		 (fabs(currentRotation)+step > 180.0f && fabs(currentRotation)>(angle*180.0f/M_PI)-step && fabs(currentRotation)<(angle*180.0f/M_PI)+step)
		 )
        return 1;
    else
        return 0;
}

//restituisce un valore da aggiungere alla rotazione per avvicinarsi alla rotazione 
//desiderata(angle*angleSign) dall'angolo attuale di rotazione(currentRotation)
float rotateTowards(float angle,float angleSign,float currentRotation)
{
    float limitR = angle*angleSign - M_PI*angleSign;
    if(
	   (currentRotation > limitR*180.0f/M_PI && currentRotation > angle*angleSign*180.0f/M_PI) ||
	   (currentRotation < limitR*180.0f/M_PI && currentRotation < angle*angleSign*180.0f/M_PI)
	   )
    {
        if(angleSign>0.0f)
        {
            currentRotation -= 1.5f;
            if(currentRotation<-180.0f)
                currentRotation = currentRotation+360.0f;
            return currentRotation;
        }
        else
        {
            currentRotation += 1.5f;
            if(currentRotation>180.0f)
                currentRotation = currentRotation-360.0f;
            return currentRotation;
        }
    } 
    else if(currentRotation > limitR*180.0f/M_PI)
    {
        currentRotation += 1.5f;
        if(currentRotation>180.0f)
            currentRotation = currentRotation-360.0f;
        return currentRotation;
    }
    else
    {
        currentRotation -= 1.5f;
        if(currentRotation<-180.0f)
            currentRotation = currentRotation+360.0f;
        return currentRotation;
    }
}

void attackEnemy(tank *tankT,float angle,float XSign, float distance)
{   
	float stepD = 15.0f;
	float reduction = distance/stepD;
	
	tankT->state=1;
	if(reduction>1.0f)
		reduction=1.0f;
	if(reduction<-1.0f)
		reduction=-1.0f;
	
	//all'interno della zona di raggio step frena e gira la torretta verso il carro armato nemico
	if(((reduction>=0.0f && reduction<1.0f) || (reduction>-1.0f && reduction<0.0f)))
	{
		float stepR = 2.0f;         
		float actualRotation = (tankT->userTurret.rot + tankT->rot);
		actualRotation = normalize180(actualRotation); 
		tankT->v[0] *= 0.90f;
		tankT->v[2] *= 0.90f;   
		if(checkRotationInStep(angle,XSign,actualRotation,stepR)==1)
		{
			if(tankT->rechargeTime >= tankT->rechargeNeeded)
			{
				shoot(tankT);
			}
		}
		else
		{
			//actualRotation = rotateTowards(...,actualRotation)
			//torret.rot + tank.rot = rotateTowards(...,actualRotation)
			//torret.rot = rotateTowards(...,actualRotation) - tank.rot
			tankT->userTurret.rot = rotateTowards(angle,XSign,actualRotation) - tankT->rot;
		}
	}
	//all'esterno della zona di raggio step ruota verso il nemico ed avvicinati
	else 
	{    
		//se non c'è collisione nella direzione verso cui dovrei ruotare
		if(checkCollision(tankT,degrees(angle*XSign))==0)
		{                                      
			float stepR = 2.0f;
			//se la rotazione è entro lo step di rotazione del nemico
			if(checkRotationInStep(angle,XSign,tankT->rot,stepR)==1)
			{
				float TSign = getSign(tankT->userTurret.rot);         
				//accelera
				tankT->enginePower = 100.0f*reduction;
				//raddrizza torretta nel cammino
				if(fabs(tankT->userTurret.rot)>2.0f)
					tankT->userTurret.rot += -1.5f * TSign;     
			}
			//altrimenti se la rotazione è al di fuori dello step di rotazione del nemico
			else
			{
				//se sei in movimento fermati
				if((tankT->speed<=-0.1f) || (tankT->speed>=0.1f))
				{
					tankT->v[0] *= 0.94f;
					tankT->v[2] *= 0.94f;
				}
				//se sei fermo ruota verso la nuova posizione
				else
				{
					tankT->rot = rotateTowards(angle,XSign,tankT->rot);
					animateLeftTurn(tankT);
				}
				//raddrizza torretta nel cammino
				float TSign = getSign(tankT->userTurret.rot);
				if(fabs(tankT->userTurret.rot)>2.0f)
					tankT->userTurret.rot += -1.5f * TSign;
				
			}
		}
		//se c'è collisione
		else
		{
			//entra in modalità collision avoidance
			tankT->collision = 10;
			float stepC = 30.0f;
			int i;
			for(i=1;i<5;i++)
			{
				//se non c'è collisione all'angolo di collisione + 10*i
				if(checkCollision(tankT,normalize180(degrees(angle*XSign)+stepC*i))==0)
				{
					tankT->collisionAngle = normalize180(degrees(angle*XSign)+stepC*i);
					break;
				}
			}
		}
	}
}

void runAway(tank *tankT,float angle,float XSign,float distance)
{
    
    float step = 30.0f;
    float reduction = distance/step;
	
    tankT->state=3;                                                                   
    //trovo l'angolo inverso per scappare
    angle = normalizeMPI(angle*XSign+M_PI);
    XSign = getSign(angle);
    angle = fabs(angle);
    //distanza percentuale sullo step
    if(reduction>1.0f)
		reduction=1.0f;
    if(reduction<-1.0f)
		reduction=-1.0f;
    //a seconda della vicinanza cambia il valore di forza al motore
    float reductionInverse = step/distance;
    if(reductionInverse>1.0f)
        reductionInverse=1.0f;
    if(reductionInverse<-1.0f)
        reductionInverse=-1.0f;
    
    //se non c'è collisione nella direzione verso cui dovrei ruotare
    if(checkCollision(tankT,degrees(angle*XSign))==0)
    {
        if(checkRotationInStep(angle,XSign,tankT->rot,2.0f)==1)
        {
            if(((reduction>=0.0f && reduction<1.0f) || (reduction>-1.0f && reduction<0.0f)))
            {
                tankT->enginePower = 100.0f*reductionInverse;
            }
            else 
            {
                tankT->v[0] *= 0.94f;
                tankT->v[2] *= 0.94f;
            }      
        }
        else
        {
            //se sei in movimento fermati
            if((tankT->speed<=-0.1f) || (tankT->speed>=0.1f))
            {
				tankT->v[0] *= 0.94f;
				tankT->v[2] *= 0.94f;
            }
            else
				//se sei fermo ruota verso la nuova posizione
				tankT->rot = rotateTowards(angle,XSign,tankT->rot);
        }
    }
    else
    {
		//entra in modalità collision avoidance      
		float stepC = 30.0f;
		tankT->collision = 10;
		int i;
		for(i=1;i<5;i++)
		{
			//se non c'è collisione all'angolo di collisione + 10*i
			if(checkCollision(tankT,normalize180(degrees(angle*XSign)+stepC*i))==0)
			{
				tankT->collisionAngle = normalize180(degrees(angle*XSign)+stepC*i);
				break;
			}
		}
    }
}

void randomMove(tank *tankT)
{
	if(tankT->state!=2)
	{
		srand(time(NULL));
		float rA = (float)(rand() % 180 + 1);
		float XSign = (float)(rand() % 2);
		srand(time(NULL));
		if(XSign==0.0f)
			XSign=-1.0f;
		tankT->randomAngle = rA*XSign;
		tankT->state=2;
//		sprintf(stampe2,"Cambio angolo: %f",XSign);
	}
	//se non c'è collisione nella direzione verso cui dovrei ruotare
	if(checkCollision(tankT,tankT->randomAngle)==0)
	{
		if(checkRotationInStep(radians(fabs(tankT->randomAngle)),getSign(tankT->randomAngle),tankT->rot,2.0f)==1)
		{
			//cambiando lo stato in null(4),se si rientra in random move si è forzati
			//a cercare un altro angolo
			if(tankT->randomMoves>=20)
			{
				
				tankT->randomMoves=0;
				tankT->state=4;         
			}
			else
			{
				tankT->enginePower = 50.0f;
				tankT->randomMoves++;
//				sprintf(stampe2,"Random moves: %i",tankT->randomMoves);
			}       
		}
		else
		{
			//se sei in movimento fermati
			if((tankT->speed<=-0.1f) || (tankT->speed>=0.1f))
			{
				tankT->v[0] *= 0.94f;
				tankT->v[2] *= 0.94f;
			}
			else
				//se sei fermo ruota verso la nuova posizione
				tankT->rot = rotateTowards(radians(fabs(tankT->randomAngle)),getSign(tankT->randomAngle),tankT->rot);
		}
	}
	else
	{
		//entra in modalità collision avoidance
		float stepC = 30.0f;
		tankT->collision = 10;
		int i;
		for(i=1;i<5;i++)
		{
			//se non c'è collisione all'angolo di collisione + 10*i
			if(checkCollision(tankT,normalize180(tankT->randomAngle+stepC*i))==0)
			{
				tankT->collisionAngle = normalize180(tankT->randomAngle+stepC*i);
				break;
			}
		}
	}
}

void planAction(tank *tankT)
{
	//se non è stata rilevata una collisione
	//PLAN ACTION
	//se non hai abbastanza vita o munizioni
	//scappa
	//altrimenti se il nemico è in visuale
	//attacca
	//altrimenti
	//movimento casuale
	//se è stata rilevata collisione
	//EVITA COLLISIONE
	
	//variabili:
	//distanceX distanza sull'asse delle X tra tankT e carro utente
	//distanceZ distanza sull'asse delle Z tra tankT e carro utente
	//distance  vettore distanza
	//angle     angolo senza segno tra tankT e carro utente
	//XSign     segno dell'angolo angle
	
	//trova l'angolo rispetto a Z. Moltiplicazione per il segno di distanceX per recuperare
	//la direzione della rotazione rispetto all'asse X(di cui non teniamo conto nell'acos)
	//troviamo valori di rotazioni che vanno da -M_PI a +M_PI
	float distanceX = (tankT->pos[0]-tanks[0].pos[0]);
	float distanceZ = (tankT->pos[2]-tanks[0].pos[2]);
	float distance = sqrt(distanceX*distanceX + distanceZ*distanceZ);
	float XSign = getSign(distanceX);
	float angle = acos(distanceZ/distance);
	
	//se non è stata rilevata una collisione in precedenza
	if(tankT->collision==0 && tankT->directCollision==0)
	{
        //se non hai abbastanza vita o munizioni
        if
			(
			 (tankT->life<50.0f) ||
			 (tankT->life>=50.0f && tankT->life<=70.0f && tankT->ammo<7) ||
			 (tankT->life>70.0f && tankT->ammo<3)
			 )
			//scappa
			runAway(tankT,angle,XSign,distance);
        //altrimenti se il nemico è in visuale
    	else if(checkRotationInStep(angle,XSign,tankT->rot,30.0f)==1)
			//attacca
			attackEnemy(tankT,angle,XSign,distance);
        //altrimenti
    	else
			//movimento casuale
			randomMove(tankT);
    }
    //se in precedenza hai colliso (modalità collision avoidance)
    else if(tankT->directCollision!=0)
    {
		
		if(tankT->directCollision>0)
		{
			tankT->enginePower=-150.0f;
			tankT->directCollision--;
//			sprintf(printScreen[7],"Ho colliso avanti! %i",tankT->directCollision);
		}
		else if(tankT->directCollision<0)
		{
			tankT->enginePower=150.0f;
			tankT->directCollision++;
//			sprintf(printScreen[7],"Ho colliso dietro! %i",tankT->directCollision);
		}
    }
    //se in precedenza hai rilevato una collisione (modalità collision avoidance)
    else
    {
        //sprintf(stampe2,"Collision avoidance pattern!");
        //se sei in movimento fermati
        int check = checkRotationInStep(fabs(radians(tankT->collisionAngle)),getSign(tankT->collisionAngle),tankT->rot,2.0f);
        if(((tankT->speed<=-0.1f) || (tankT->speed>=0.1f)) && check==0)
        {
            //tankT->v[0] *= 0.94f;
            //tankT->v[2] *= 0.94f;
            tankT->v[0] *= 0.1f;
            tankT->v[2] *= 0.1f;
        }
        else
        {
			if(check==1)
			{
				tankT->enginePower = 100.0f;
				tankT->collision--;
			}
			else 
			{
				tankT->rot = rotateTowards(fabs(radians(tankT->collisionAngle)),getSign(tankT->collisionAngle),tankT->rot);
			}
        }
    }
}

//reshape
void reshape ( int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glViewport(0, 0, w, h);
	
	if (showMenu)
	{
		gluOrtho2D(0, 1.0f, 0, 1.0f);
	}
	else
	{
		ar = (float)w/(float)h;
		gluPerspective(45.0, ar, 0.2f, 500.0f);
	}
	
	glMatrixMode(GL_MODELVIEW);
	
	//zbuffer
	//free(pixels);
	//pixels = malloc(sizeof(GLubyte)*w*h);
}

void initMenu(void)
{
	loadTGA("texture/menu_main.tga", 1000);
	loadTGA("texture/menu_levels.tga", 1001);
	loadTGA("texture/menu_credits.tga", 1002);
	loadTGA("texture/menu_loading.tga", 1003);
	loadTGA("texture/menu_gameover.tga", 1004);
	
	DIR* directory = opendir("levels");
	int i;
	
	listLenght = levelList(directory, levelFileName);
	
	for (i=0; i<listLenght; i++)
		printf("%s\n", levelFileName[i]);
}

//inizializzazione
void init(void)
{
	//calcolo l'aspect ratio iniziale
	ar = (float)WIDTH/(float)HEIGHT;
	
	initMenu();
	
	setDesertLights();
	
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	if(blurActive==1)
	{
		glClearAccum(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_ACCUM_BUFFER_BIT);
		glReadBuffer(GL_BACK);
		glDrawBuffer(GL_BACK);
	}
	
	pixels = malloc(sizeof(GLubyte)*WIDTH*HEIGHT);
	
	gettimeofday(&old, NULL);
	srand(old.tv_sec);
	
	//carico i modelli del carro armato
	objTank[0] = (obj*) loadOBJ("obj/tank_body.obj");
	objTank[1] = (obj*) loadOBJ("obj/tread.obj");
	objTank[2] = (obj*) loadOBJ("obj/tread2.obj");
	objTank[3] = (obj*) loadOBJ("obj/tread3.obj");
	objTank[4] = (obj*) loadOBJ("obj/tank_turret.obj");
	objTank[5] = (obj*) loadOBJ("obj/tank_cannon.obj");
	objTank[6] = (obj*) loadOBJ("obj/crosshair.obj");
	
	//calcolo le bounding box delle parti principali del carro armato
	createBoundingBox(objTank[0]);
	createBoundingBox(objTank[4]);
	createBoundingBox(objTank[5]);
	
	objTank[5]->bb.min.z -= 1.7f;
	
}

void printLvlList(float x, float y)
{
	int i;
	
	orthogonalStart();
	glPushMatrix();
	glLoadIdentity();
	for (i=0; i<10; i++) {
		renderLvlMenuString(x, y+i*20.0f, GLUT_BITMAP_9_BY_15, levelFileName[i]);
	}
	glPopMatrix();
	orthogonalEnd();
}

void drawQuad(int textureId)
{
	float rgb[] = {1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT, rgb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, rgb);
	glMaterialfv(GL_FRONT, GL_SPECULAR, rgb);
	glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
	if (textureId != -1)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
	else
		glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.0f , 1.0f, 0.0f);
	glEnd();
	if (textureId != -1)
		glDisable(GL_TEXTURE_2D);
	else
		glEnable(GL_LIGHTING);
}

void loadGame(char* levelName)
{
	//carico il livello
	char path[32];
	char dir[] = "levels";
	sprintf(path, "%s/%s", dir, levelName);
	levelMap = loadLevel(path);
	
	// spengo tutte le luci per evitare che rimangano luci attive dal livello precedente
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);
	
	if(levelMap->levelType==1)
    	setDesertLights();
	if(levelMap->levelType==2)
    	setUrbanLights();

	levelMap->pwupRot = 0.0f;
	
	tanks = (tank*) malloc((levelMap->enemies+1) * sizeof(tank));
	
	//definizione strutture
	//carro armato
	int i, j;
	for(i=0;i<levelMap->enemies+1;i++)
	{
    	tanks[i].scale = 1.0f;
		
		if (i==0) {
			tanks[i].pos[0] = 0.0f;
			tanks[i].pos[1] = 0.5f;
			tanks[i].pos[2] = 0.0f;
		}
		else
		{
			tanks[i].pos[0] = i * 5;
			tanks[i].pos[1] = 0.5f;
			tanks[i].pos[2] = -levelMap->depth * DIM_TILE * 0.5f + 20;
		}
		tanks[i].lastPos.x = 0.0f;
		tanks[i].lastPos.y = 0.0f;
		tanks[i].lastPos.z = 0.0f;
    	tanks[i].v[0] = 0.0f;
    	tanks[i].v[1] = 0.0f;
    	tanks[i].v[2] = 0.0f;
    	tanks[i].a[0] = 0.0f;
    	tanks[i].a[1] = 0.0f;
    	tanks[i].a[2] = 0.0f;
    	tanks[i].rot  = normalize180(180.0f * (float)i);
		tanks[i].lastRot = 0.0f;
    	tanks[i].speed = 0.0f;
    	tanks[i].enginePower = 0.0f;
    	tanks[i].frictionForce = 0.0f;
    	tanks[i].throttle = 0.0f;
    	tanks[i].mass = 3.0f;
    	tanks[i].iperBoostAccumul = 0.0f;
    	tanks[i].iperBoost = 0.0f;
		tanks[i].weaponPower = 1;
    	tanks[i].bulletRoot = NULL;
    	tanks[i].ammo = 10;
    	tanks[i].life = 100.0f;
    	tanks[i].rechargeTime = 0.0f;
    	tanks[i].rechargeNeeded = 1.0f;
    	tanks[i].state = 2;
    	tanks[i].animationL = 0;
    	tanks[i].animationR = 0;
    	//torretta
    	tanks[i].userTurret.pos[0] = 0.0f;
    	tanks[i].userTurret.pos[1] = 0.72f;
    	tanks[i].userTurret.pos[2] = 0.0f;
    	tanks[i].userTurret.rot = 0.0f;
    	//cannone
    	tanks[i].userTurret.tankCannon.length = 2.20f;
    	tanks[i].userTurret.tankCannon.pos[0] = 0.0f;
    	tanks[i].userTurret.tankCannon.pos[1] = -0.06f;
		tanks[i].userTurret.tankCannon.pos[2] = -2.0f;
    	tanks[i].userTurret.tankCannon.rot = 0.0f;
    	//cingolo sx
    	tanks[i].userTreadL.pos[0] = -0.8f;
    	tanks[i].userTreadL.pos[1] = -0.06f;
    	tanks[i].userTreadL.pos[2] = 0.62f;
    	//cingolo dx
    	tanks[i].userTreadR.pos[0] = 0.8f;
    	tanks[i].userTreadR.pos[1] = -0.06f;
    	tanks[i].userTreadR.pos[2] = 0.62f;
		//bounding box in coordinate locali
		BoundingBox bbarr[] = {objTank[0]->bb, objTank[4]->bb, objTank[5]->bb};
		BoundingBox* tanksbb = BBUnion(bbarr, 3);
		tanks[i].boundingVol.vert[0].x = tanksbb->min.x;
		tanks[i].boundingVol.vert[0].y = tanksbb->min.y;
		tanks[i].boundingVol.vert[0].z = tanksbb->min.z;
		
		tanks[i].boundingVol.vert[1].x = tanksbb->min.x;
		tanks[i].boundingVol.vert[1].y = tanksbb->min.y;
		tanks[i].boundingVol.vert[1].z = tanksbb->max.z;
		
		tanks[i].boundingVol.vert[2].x = tanksbb->min.x;
		tanks[i].boundingVol.vert[2].y = tanksbb->max.y;
		tanks[i].boundingVol.vert[2].z = tanksbb->min.z;
		
		tanks[i].boundingVol.vert[3].x = tanksbb->min.x;
		tanks[i].boundingVol.vert[3].y = tanksbb->max.y;
		tanks[i].boundingVol.vert[3].z = tanksbb->max.z;
		
		tanks[i].boundingVol.vert[4].x = tanksbb->max.x;
		tanks[i].boundingVol.vert[4].y = tanksbb->min.y;
		tanks[i].boundingVol.vert[4].z = tanksbb->min.z;
		
		tanks[i].boundingVol.vert[5].x = tanksbb->max.x;
		tanks[i].boundingVol.vert[5].y = tanksbb->min.y;
		tanks[i].boundingVol.vert[5].z = tanksbb->max.z;
		
		tanks[i].boundingVol.vert[6].x = tanksbb->max.x;
		tanks[i].boundingVol.vert[6].y = tanksbb->max.y;
		tanks[i].boundingVol.vert[6].z = tanksbb->min.z;
		
		tanks[i].boundingVol.vert[7].x = tanksbb->max.x;
		tanks[i].boundingVol.vert[7].y = tanksbb->max.y;
		tanks[i].boundingVol.vert[7].z = tanksbb->max.z;
		//bounding box in coordinate world
		for (j=0; j<8; j++)
		{
			levelMap->cm.tanksBB[i].vert[j].x = 0.0f;
			levelMap->cm.tanksBB[i].vert[j].y = 0.0f;
			levelMap->cm.tanksBB[i].vert[j].z = 0.0f;
		}
		tanks[i].boundingRad = 2.2f;

    	tanks[i].collision = 0;
    	tanks[i].collisionAngle = 0.0f;
    	tanks[i].directCollision = 0;
    	tanks[i].randomMoves = 0;
    	tanks[i].randomAngle = 0.0f;
		turretView = 0;

		gameLoaded = 1;
    }
}

// visualizza i menu
void displayMenu(void)
{
	float3 selectBoxCoord;
	float gap;
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1.0f, 0, 1.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	switch (menuAction)
	{
		case -1:
			// sfondo
			drawQuad(1000);
			
			//rettangolo di selezione
			entries = 4;
			selectBoxCoord.x = 0.115f;
			selectBoxCoord.y = 0.33f;
			selectBoxCoord.z = 0.1f;
			gap = -0.1f;
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1.0f, 0.0f, 0.0f, 0.18f);
			glPushMatrix();
			glTranslatef(selectBoxCoord.x, selectBoxCoord.y + (selectBoxPlace * gap), selectBoxCoord.z);
			glScalef(0.3f, 0.1f, 1.0f);
			drawQuad(-1);
			glPopMatrix();
			glDisable(GL_BLEND);				
			break;
			
		case 0: // new game
			drawQuad(1003);
			gameMode = 0;
			currentLevel++;
			currentLevel = fmodf(currentLevel, listLenght);
			loadGame(levelFileName[currentLevel]);
			menuAction = -1;
			selectBoxPlace = 0;
			showMenu = 0;
			break;
			
		case 1: // play level
			printLvlList(WIDTH/2, 373);
			drawQuad(1001);
			gameMode = 1;
			
			//rettangolo di selezione
			selectBoxCoord.x = 0.480f;
			selectBoxCoord.y = 0.372f;
			selectBoxCoord.z = 0.1f;
			gap = -0.0333f;
			entries = listLenght;
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1.0f, 0.0f, 0.0f, 0.30f);
			glPushMatrix();
			glTranslatef(selectBoxCoord.x, selectBoxCoord.y + (selectBoxPlace * gap), selectBoxCoord.z);
			glScalef(0.5f, 0.03f, 1.0f);
			drawQuad(-1);
			glPopMatrix();
			glDisable(GL_BLEND);				
			break;
		
		case 2: // credits
			if (gameMode == 2)
				drawQuad(1004);
			else
				drawQuad(1002);
			break;

		case 3:
			exit(0);
			break;
			
		default:
			menuAction = -1;
			break;
	}
	
	glutSwapBuffers();
}

void displayGame(void)
{
	int i = 0;
	
	if(levelMap->levelType==1)
        displayDesertLights();
	if(levelMap->levelType==2)
    	displayUrbanLights();
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, ar, 0.2f, 500.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
    //third person view [turretView==0]
    //-camera is distant distanceFromCamera multiplied by sin or cos on dependency to tank rotation
    //-from tank position
    //third person view [turretView==0]
    //-camera is distant distanceFromCamera multiplied by sin or cos on dependency to tank rotation
    //-from tank position
    if(fixedView==1) //fixed view
    {
		gluLookAt(tanks[0].pos[0],//Eye x
				  3.5f,//Eye y
				  tanks[0].pos[2] + distanceFromCamera,//Eye z
				  tanks[0].pos[0], 3.0f,tanks[0].pos[2],//At
				  0.0f, 1.0f, 0.0f);//Up
//		gluLookAt(tanks[0].pos[0]+5.0f,//Eye x
//				  1.5f,//Eye y
//				  tanks[0].pos[2]-2.0f,//Eye z
//				  tanks[0].pos[0], 3.0f,tanks[0].pos[2]-2.0f,//At
//				  0.0f, 1.0f, 0.0f);//Up
    }
    else if(turretView==0) //third person view
    {
		gluLookAt(tanks[0].pos[0] + distanceFromCamera*sin(tanks[0].rot*M_PI/180.0f),//Eye x
				  3.5f,//Eye y
				  tanks[0].pos[2] + distanceFromCamera*cos(tanks[0].rot*M_PI/180.0f),//Eye z
				  tanks[0].pos[0], 3.0f,tanks[0].pos[2],//At
				  0.0f, 1.0f, 0.0f);//Up
    }
    else if (turretView) //turret view
    {
		gluLookAt(tanks[0].pos[0] + 1.0f*sin((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f),//Eye x
				  2.5f,//Eye y
				  tanks[0].pos[2] + 1.0f*cos((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f),//Eye z
				  tanks[0].pos[0] - 5.0f*sin((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f), 2.5f + sin(tanks[0].userTurret.tankCannon.rot*M_PI/180.0f),tanks[0].pos[2] - 5.0f*cos((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f),//At
				  0.0f, 1.0f, 0.0f);//Up
    }
	
	//disegno i modelli
	
	//LIVELLO
	drawLevel(levelMap);
	
	//CARRI ARMATI
    bullets *p = NULL;
    for(i=0;i<levelMap->enemies+1;i++)
    {
    	glPushMatrix();
		glTranslatef(tanks[i].pos[0], tanks[i].pos[1], tanks[i].pos[2]);
		glRotatef(tanks[i].rot,0.0f,1.0f,0.0f);
		//turret
		glPushMatrix();
		glTranslatef(tanks[i].userTurret.pos[0],tanks[i].userTurret.pos[1],tanks[i].userTurret.pos[2]);
		glRotatef(tanks[i].userTurret.rot,0.0f,1.0f,0.0f);
		//cannon
		glPushMatrix();
		glTranslatef(tanks[i].userTurret.tankCannon.pos[0],tanks[i].userTurret.tankCannon.pos[1],tanks[i].userTurret.tankCannon.pos[2]+1.0f);
		glRotatef(tanks[i].userTurret.tankCannon.rot,1.0f,0.0f,0.0f);
		glTranslatef(0.0f,0.0f,-tanks[i].userTurret.tankCannon.length/2);
		drawOBJ(objTank[5]);
		glPopMatrix();
		drawOBJ(objTank[4]);
		glPopMatrix();
		//right tread
		glPushMatrix();
		glTranslatef(tanks[i].userTreadR.pos[0],tanks[i].userTreadR.pos[1],tanks[i].userTreadR.pos[2]);
		glRotatef(90.0f,0.0f,1.0f,0.0f);
		if(tanks[i].animationR<10)
			drawOBJ(objTank[1]);
		else if(tanks[i].animationR<20)
			drawOBJ(objTank[2]);
		else if(tanks[i].animationR<=30)
			drawOBJ(objTank[3]);
		else
		{
			drawOBJ(objTank[1]);
			tanks[i].animationR = 0;
		}
		glPopMatrix();
		//left tread
		glPushMatrix();
		glTranslatef(tanks[i].userTreadL.pos[0],tanks[i].userTreadL.pos[1],tanks[i].userTreadL.pos[2]);
		glRotatef(90.0f,0.0f,1.0f,0.0f);
		if(tanks[i].animationL<10)
			drawOBJ(objTank[1]);
		else if(tanks[i].animationL<20)
			drawOBJ(objTank[2]);
		else if(tanks[i].animationL<=30)
			drawOBJ(objTank[3]);
		else
		{
			drawOBJ(objTank[1]);
			tanks[i].animationL = 0;
		}
		glPopMatrix();
		//tank body
		drawOBJ(objTank[0]);
    	glPopMatrix();
		
		glEnable(GL_RESCALE_NORMAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    	//float kkk = 0.0f;
    	for(p=tanks[i].bulletRoot; p!=NULL; p=p->next)
        {
            //proiettile
			if (p->lifetime < LIFETIME)
			{
				glPushMatrix();
				myColor[3]=1.0-p->lifetime/LIFETIME;
				glColor4fv(myColor);
				glTranslatef(p->bullet.pos[0],p->bullet.pos[1],p->bullet.pos[2]);
				glScalef(p->bullet.scale.x, p->bullet.scale.y, p->bullet.scale.z);
				glutSolidSphere(p->bullet.radius, 10, 10);
				glPopMatrix();
			}
            
            //disegna sfere per controllare cancellazione da struttura corretta
            //si deve aggiungere un kkk=0.0f fuori da for
            /*
			 glPushMatrix(); 
			 glTranslatef(2.0f*kkk,0.0f,0.0f);
			 glutSolidSphere(1.2f, 20, 20);
			 glPopMatrix();
			 kkk += 1.0f;
			 */
        }
        p=NULL;
		glDisable(GL_BLEND);
		glDisable(GL_RESCALE_NORMAL);
    }//END FOR
	
	// diesegno il cielo
	glPushMatrix();
	glTranslatef(tanks[0].pos[0], tanks[0].pos[1], tanks[0].pos[2]);
	glScalef(300.0f, 300.0f, 300.0f);
	drawOBJ(levelMap->sky);
	glPopMatrix();
	
	//sovraimpressioni ortogonali
	orthogonalStart();
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(5.0f,30.0f,GLUT_BITMAP_9_BY_15,stampe);
	renderBitmapString(5.0f,50.0f,GLUT_BITMAP_9_BY_15,stampe2);
	renderBitmapString(5.0f,70.0f,GLUT_BITMAP_9_BY_15,stampe3);
	for (i=0; i<30; i++) {
		renderBitmapString(5.0f,i*20+90.0f,GLUT_BITMAP_9_BY_15,printScreen[i]);
	}
	glPopMatrix();
	orthogonalEnd();
	
	//disegno ostacolone
	glPushMatrix();
	glTranslatef(20.0f, 0.0f, 20.0f);
	//glutWireSphere(5.0f, 10, 10);
	glPopMatrix();
	
	//disegno mirino
	if(turretView==1)
	{
    	float translateB = (tanks[0].userTurret.tankCannon.pos[2] - tanks[0].userTurret.tankCannon.length);
    	float initX = translateB*sin((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f);
    	float initY = translateB*cos((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f);
    	glPushMatrix();
		lightOff();
		glColor3f(1.0f, 0.0f, 0.0f);
    	glTranslatef(xBullet+ tanks[0].pos[0]  + initX, 0.3f, yBullet+ tanks[0].pos[2] + initY);
    	glRotatef(tanks[0].rot+tanks[0].userTurret.rot,0.0f,1.0f,0.0f);
    	drawOBJ(objTank[6]);
		lightOn();
    	glPopMatrix();
    }
	
	//checkCollision(&tanks[0],tanks[0].rot);
	glutSwapBuffers();
}

//visualizzazione
void display(void)
{
	if (showMenu)
	{
		displayMenu();
	}
	else
	{
		displayGame();
	}
}

void specialKeys(int key, int x, int y)
{
	if (showMenu && menuAction == -1) // menu principale
	{
		switch (key)
		{
			case GLUT_KEY_DOWN:
				selectBoxPlace = fmod(selectBoxPlace+1, entries);
				break;
			case GLUT_KEY_UP:
				selectBoxPlace = fmod(selectBoxPlace+entries-1, entries);
				break;

			default:
				break;
		}
	}
	else if (showMenu && menuAction == 1) // menu play level
	{
		switch (key)
		{
			case GLUT_KEY_DOWN:
				selectBoxPlace = fmod(selectBoxPlace+1, entries);
				break;
			case GLUT_KEY_UP:
				selectBoxPlace = fmod(selectBoxPlace+entries-1, entries);
				break;
			case GLUT_KEY_LEFT:
				menuAction = -1;
				selectBoxPlace = 1;
				break;

			default:
				break;
		}
	}
	else if (showMenu && menuAction == 2) // menu credits
	{
		switch (key)
		{
			case GLUT_KEY_LEFT:
				menuAction = -1;
				selectBoxPlace = 2;
				break;

			default:
				break;
		}
	}
}

//lettura input
void keyboard(unsigned char key, int x, int y)
{
	if (showMenu && menuAction == -1) // menu principale
	{
		switch (key)
		{
			case 27: // esc
				if (gameLoaded)
					showMenu = showMenu ? 0 : 1;
				break;
			case 'm':
				exit(0);
				break;
			case 13: // CR (carriage return - corrisponde a INVIO su mac)
				menuAction = selectBoxPlace;
				selectBoxPlace = 0;
				currentLevel = -1;
				break;

			default:
				break;
		}
	}
	else if (showMenu && menuAction == 1) // play level
	{
		switch (key) {
			case 27: // esc
				menuAction = -1;
				selectBoxPlace = 1;
				break;
			case 13: // CR
				loadGame(levelFileName[selectBoxPlace]);
				menuAction = -1;
				selectBoxPlace = 0;
				showMenu = 0;
				break;

			default:
				break;
		}
	}
	else if (showMenu && menuAction == 2) // credits
	{
		switch (key)
		{
			default:
				menuAction = -1;
				selectBoxPlace = 2;
				gameMode = 0;
				break;
		}
	}
	else if (!showMenu)
	{
		switch(key)
		{
			case 27: //Esc
				showMenu = showMenu ? 0 : 1;
				break;
			case 'w': //forward
				tanks[0].enginePower = 300.0f;
				break;
			case 's': //back
				tanks[0].enginePower = -300.0f;
				break;
			case 'a': //left
				if(tanks[0].v[2]==0.0f)
				{
					tanks[0].rot += 1.5f;
					animateLeftTurn(&tanks[0]);
				}
				break;
			case 'd': //right
				if(tanks[0].v[2]==0.0f)
				{
					tanks[0].rot -= 1.5f;
					animateRightTurn(&tanks[0]);
				}
				break;
			case 't': //enable/disable turret view
				if(turretView==0)
					turretView = 1;
				else
					turretView = 0;
				break;
			case 'c': //turret right
				tanks[0].userTurret.rot -= 1.5f;
				break;
				
			case 'z': //turret left
				tanks[0].userTurret.rot += 1.5f;
				break;
			case 'e': //cannon up
				if(tanks[0].userTurret.tankCannon.rot<15.0f)
					tanks[0].userTurret.tankCannon.rot += 1.5f;
				break;
			case 'q': //cannon down
				if(tanks[0].userTurret.tankCannon.rot>0.0f)
					tanks[0].userTurret.tankCannon.rot -= 1.5f;
				break;
			case 35: //Brakes
				tanks[0].v[0] *= 0.5f;
				tanks[0].v[2] *= 0.5f;
				break;
			case 32: //Space
				if(tanks[0].rechargeTime>=tanks[0].rechargeNeeded)
					shoot(&tanks[0]);
				break;
			case '<':
				fixedView = fixedView ? 0 : 1;
				break;
			case '0':
				tanks[0].pos[0] = 0.0f;
				tanks[0].pos[1] = 0.5f;
				tanks[0].pos[2] = 0.0f;
				break;
			case 'm':
				exit(0);
				break;
				
		}
	}
}

void idle(void)
{
	struct timeval newTime;
	
	gettimeofday(&newTime, NULL);
	
	deltaT = (((double)(newTime.tv_sec) + (double)(newTime.tv_usec)/1000000.0) -
			  ((double)(old.tv_sec) + (double)(old.tv_usec)/1000000.0));
	
	old.tv_sec = newTime.tv_sec;
	old.tv_usec = newTime.tv_usec;
	
	if (showMenu)
	{
	}
	else
	{
		levelMap->pwupRot = fmodf(levelMap->pwupRot + 3.0f, 360.0f);
		
		int i, x, y;
		
		for(i=0;i<levelMap->enemies+1;i++)
		{
			if(i!=0)
			{
				//intelligenza artificiale nemici
				//azzera valori
				tanks[i].enginePower = 0.0f;
				planAction(&tanks[i]);
			}
			//calcolo della fisica dei carri armati
			//calcolo della forza di attrito [Fk = fC * g * m]
			tanks[i].frictionForce = 9.8f * frictionCoeff * tanks[i].mass;
			tanks[i].speed = tanks[i].v[0]*sin(tanks[i].rot*M_PI/180.0f) + tanks[i].v[2]*cos(tanks[i].rot*M_PI/180.0f);
			if((tanks[i].speed>=-0.1f) && (tanks[i].speed<=0.1f) && (tanks[i].enginePower<=0.0f))
			{
				tanks[i].frictionForce = 0.0f;
			}
			else if(tanks[i].speed<-0.1)
			{
				tanks[i].frictionForce = 9.8f * frictionCoeff * tanks[i].mass;
			}
			else
			{
				tanks[i].frictionForce = (9.8f * frictionCoeff * tanks[i].mass)* -1.0f;
			}
			
			//ferma il movimento continuo, simula attrito statico
			if((tanks[i].speed>=-0.1f) && (tanks[i].speed<=0.1f))
			{
				tanks[i].v[0]=0.0f;
				tanks[i].v[2]=0.0f;
			}
			
			//calcolo dell'accelerazione derivata dalle forze in gioco(motore e attrito) e 
			//inversamente proporzionale alla massa del carro armato [a = (Fe - Fk) / m]
			tanks[i].a[0] = ((tanks[i].enginePower - tanks[i].frictionForce)*sin(tanks[i].rot*M_PI/180.0f)/tanks[i].mass)* -1.0f;
			tanks[i].a[2] = ((tanks[i].enginePower - tanks[i].frictionForce)*cos(tanks[i].rot*M_PI/180.0f)/tanks[i].mass)* -1.0f;
			tanks[i].throttle = ((tanks[i].enginePower - tanks[i].frictionForce)/tanks[i].mass)* -1.0f;
			
			// S = So + v*t
			//tanks[i].v[2] += tanks[i].throttle * deltaT;
			//tanks[i].pos[2] += tanks[i].v[2] * deltaT;
			
			// S = So + vo*t + a*t*t*0.5
			//calcolo delle equazioni del moto
			tanks[i].pos[0] += tanks[i].v[0]*deltaT + tanks[i].a[0]*deltaT*deltaT*0.5f;
			tanks[i].pos[2] += tanks[i].v[2]*deltaT + tanks[i].a[2]*deltaT*deltaT*0.5f;
			tanks[i].v[0] += tanks[i].a[0] * deltaT; //velocità finale
			tanks[i].v[2] += tanks[i].a[2] * deltaT; //velocità finale
			
			tanks[i].enginePower = 0.0f;
			
			// COLLISIONI
			
			// riposiziona la bounding box di ogni carro armato su di esso
			glPushMatrix();
			glLoadIdentity();
			glTranslatef(tanks[i].pos[0], 0.5f, tanks[i].pos[2]);
			glRotatef(tanks[i].rot, 0.0f, 1.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, mat);
			levelMap->cm.tanksBB[i] = placeOrientedBoundingBox(&tanks[i].boundingVol, mat);
			glPopMatrix();
			
			// gestistisce le collisioni con i confini della mappa
			borderCollision(&tanks[i], i);
			// gestisce le collisioni con gli ostacoli statici e i powerup
			for (y=0; y < levelMap->depth; y++)
			{
				for (x=0; x < levelMap->width; x++)
				{
					staticCollision(&tanks[i], i, x, y);
					powerupCollision(&tanks[i], i, x, y);
				}
			}
			// gestisce le collisioni tra i carri armati
			for (x=0; x < levelMap->enemies; x++)
				tanksCollision(&tanks[i], &tanks[(int)fmodf(i+x+1, levelMap->enemies+1)]);
			
			tanks[i].lastPos.x = tanks[i].pos[0];
			tanks[i].lastPos.z = tanks[i].pos[2];
			tanks[i].lastRot = tanks[i].rot;
			
			//FISICA PALLOTTOLE
			bullets *p;
			//elimina dalla struttura la radice con posizione Y negativa
			//radice diventa primo nodo con Y positiva o NULL se non presente
			float bulletY = 0.0f;
			while(tanks[i].bulletRoot!=NULL && tanks[i].bulletRoot->bullet.pos[1] + tanks[i].pos[1] + tanks[i].userTurret.pos[1]<0.0f)
				tanks[i].bulletRoot = tanks[i].bulletRoot->next;
			//scorre tutta la struttura di pallottole
			for(p=tanks[i].bulletRoot; p!=NULL; p=p->next)
			{
				//bulletY = p->next->bullet.pos[1] + tanks[i].pos[1] + tanks[i].userTurret.pos[1];
				//toglie dalla struttura le pallottole con Y negativa
				while(p->next!=NULL && (p->next->bullet.pos[1]+tanks[i].pos[1]+tanks[i].userTurret.pos[1])<0.0f)
					p->next = p->next->next;
				//calcola la fisica delle pallottole
				int j = 0;
				for(j=0;j<levelMap->enemies+1;j++)
				{
					if(j!=i && bulletTankCollision(&p->bullet, &tanks[j]))
					{
						p->bullet.v[0] = 0.0f;
						p->bullet.v[1] = 0.0f;
						p->bullet.v[2] = 0.0f;
						p->bullet.a[1] = 0.0f;
						p->bullet.hit = j;
					}
				}
				if (p->bullet.hit >= 0)
				{
					p->lifetime += (float)deltaT;
					p->bullet.scale.x += 5.0f;
					p->bullet.scale.y += 0.5f;
					p->bullet.scale.z += 5.0f;
					if (p->bullet.scale.x == 6.0f)
						tanks[p->bullet.hit].life -= 10.0f * tanks[i].weaponPower;
				}
				if (bulletObsCollision(&p->bullet))
				{
					p->lifetime += (float)deltaT;
					p->bullet.scale.x += 5.0f;
					p->bullet.scale.y += 0.5f;
					p->bullet.scale.z += 5.0f;
				}
				else
				{
					p->bullet.pos[0] -= p->bullet.v[0]*deltaT;
					p->bullet.pos[1] += p->bullet.v[1]*deltaT + 0.5*p->bullet.a[1]*deltaT*deltaT;
					p->bullet.v[1] += p->bullet.a[1]*deltaT;
					p->bullet.pos[2] -= p->bullet.v[2]*deltaT;
				}
			}
			
			//tempo di ricarica per le pallottole
			if(tanks[i].rechargeTime<tanks[i].rechargeNeeded && tanks[i].ammo>0)
				tanks[i].rechargeTime += deltaT;
			
			//settings per le animazioni dei cingoli
			if(fabs(tanks[i].speed)<6.0f) 
			{
				tanks[i].animationL += 1*fabs(tanks[i].speed)*0.5;
				tanks[i].animationR += 1*fabs(tanks[i].speed)*0.5;
			}
			else
			{
				tanks[i].animationL += 3;
				tanks[i].animationR += 3;
			}
			
			//morte carrarmato nemico
			if (i!=0 && tanks[i].life <= 0.0f && tanks[i].pos[1] > -5.0f)
			{
				tanks[i].pos[1] -= 0.1f;
			}
		}//END FOR
		
		enemiesDefeated = 0;
		for (i=1; i <= levelMap->enemies; i++)
		{
			if (tanks[i].life <= 0.0f)
				enemiesDefeated++;
		}
		
		//vittoria giocatore
		if (enemiesDefeated == levelMap->enemies)
		{
			if (gameMode == 0)
			{
				selectBoxPlace = 2;
				menuAction = 0;
				showMenu = 1;
			}
			else if (gameMode == 1)
			{
				menuAction = -1;
				selectBoxPlace = 2;
				showMenu = 1;
			}
		}
		
		//sconfitta giocatore
		if (tanks[0].life <= 0.0f)
		{
			//game over
			gameMode = 2;
			menuAction = 2;
			selectBoxPlace = 0;
			showMenu = 1;
		}
		
		//ricarica
		//	int x;
		float yf = tanks[0].rechargeNeeded*0.1f;
		float yf2 = 0.0f;
		for(x=0;x<10;x++)
		{
			yf2 += yf;
			if(yf2<=tanks[0].rechargeTime)
				rech[(int)x]='O';
			else
				rech[(int)x]='-';
		}
		
		
		//mirino
		if(turretView==1)
		{
			float cannonSpeed = 35.0f;
			float initialHigh = tanks[0].pos[1]+tanks[0].userTurret.pos[1];
			initialHigh += tanks[0].userTurret.tankCannon.length*sin(tanks[0].userTurret.tankCannon.rot*M_PI/180.0f);;
			float initialVel0 = cannonSpeed*cos(radians(tanks[0].userTurret.tankCannon.rot))*sin(M_PI/180.0f*(tanks[0].rot+tanks[0].userTurret.rot));
			float initialVel1 = cannonSpeed*sin(radians(tanks[0].userTurret.tankCannon.rot));
			float initialVel2 = cannonSpeed*cos(radians(tanks[0].userTurret.tankCannon.rot))*cos(M_PI/180.0f*(tanks[0].rot+tanks[0].userTurret.rot));
			float timeAtFloor = (-initialVel1 - sqrt(initialVel1*initialVel1 + 2.0f*9.81*initialHigh))/-9.81f;
			xBullet=-initialVel0*timeAtFloor;
			yBullet=-initialVel2*timeAtFloor;
		}
		
		
		// Timer powerup
		for (y=0; y < levelMap->depth; y++)
		{
			for (x=0; x < levelMap->width; x++)
			{
				if (!levelMap->pwup[x][y].active && levelMap->pwup[x][y].placed)
				{
					levelMap->pwup[x][y].timer -= (float)deltaT;
					if (levelMap->pwup[x][y].timer < 0)
					{
						levelMap->pwup[x][y].active = 1;
						if (levelMap->pwup[x][y].type == '4')
						{
							for (i=0; i<levelMap->enemies+1; i++) {
								tanks[i].weaponPower = 1;
							}
						}
					}
				}
			}
		}
		
		//HUD
		if (tanks[0].weaponPower == 4)
			sprintf(printScreen[2], "4x QUAD-DAMAGE 4x");
		else if (tanks[0].weaponPower == 1)
			sprintf(printScreen[2], "");
		
		sprintf(stampe,"Shoot Recharge |%s|",rech);
		sprintf(stampe2,"Ammo |%i|",tanks[0].ammo);
		sprintf(stampe3,"Speed |%i|",(int)fabs(tanks[0].speed));
		sprintf(printScreen[0], "HP |%d|", (int)tanks[0].life);
	}
	glutPostRedisplay();
}

int main (int argc, char** argv)
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ACCUM);
	
	glutInitWindowSize( WIDTH, HEIGHT );
	glutCreateWindow("Tanks");
	
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutSpecialFunc(specialKeys);
	
	init();
	
	glutMainLoop();
	return 0;
}
