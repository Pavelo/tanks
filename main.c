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
#define TANK_RAD 3.0f

//STRUTTURE
//struttura palla di cannone
struct _bullet
{
	float radius;
	float pos[3];
	float v[3];
	float a[3];
	float mass;
};
typedef struct _bullet bullet;

//struttura palle di cannone
struct _bullets
{
	bullet bullet;
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
	float pos[3];
	float v[3];
	float a[3];
	float rot;
	tread userTreadL;
	tread userTreadR;
	turret userTurret;
	float enginePower;
	float throttle;
	float speed;
	float frictionForce;
	float mass;
	float iperBoostAccumul;
	float iperBoost;
	bullets *bulletRoot;
	int ammo;
	float life;
	int state;
	int animation;
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
//modalit� blur
float blur = 0.2f;
int blurActive = 0;
//carri armati
int nTanks = 3;
tank tanks[3];
float frictionCoeff = 0.8f;
float distanceFromCamera = 10.0f;
int turretView = 0;
int fixedView  = 0;
map* levelMap;
obj* objTank[20];
float2 repos;
//stampe a video
char stampe[80];

//FUNZIONI

float2 borderCollision(tank* t)
{
	float2 rp = {1.0f, 1.0f};
	float attenuation = 0.6f;
//	printf("w: %f\th: %f\td: %f\n", levelMap->obs[0][0].model->b.w, levelMap->obs[0][0].model->b.h, levelMap->obs[0][0].model->b.d);
//	printf("x: %f\tz: %f\n", t->pos[0], t->pos[2]);
	if (t->pos[0] > levelMap->width * DIM_TILE * 0.5f - TANK_RAD)
	{
		rp.x = -attenuation;
		rp.y = attenuation;
	}
	if (t->pos[0] < -levelMap->width * DIM_TILE * 0.5f + TANK_RAD)
	{
		rp.x = -attenuation;
		rp.y = attenuation;
	}
	if (t->pos[2] > levelMap->height * DIM_TILE * 0.5f - TANK_RAD)
	{
		rp.x = attenuation;
		rp.y = -attenuation;
	}
	if (t->pos[2] < -levelMap->height * DIM_TILE * 0.5f + TANK_RAD)
	{
		rp.x = attenuation;
		rp.y = -attenuation;
	}
	rp.x *= t->v[0];
	rp.y *= t->v[2];
	return rp;
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
	float light0Position[] = {0.0f, 1.0f, 0.0f, 0.0f};
	float light1Position[] = {1.0f, 0.0f, 1.0f, 0.0f};
	float light2Position[] = {-1.0f, 0.0f, -1.0f, 0.0f};	
	
	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2Position);
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
	glRasterPos2f(x,y);
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
	bullets *b;
	b = malloc(sizeof(bullets));
	b->bullet.a[0] = 0.0f;
	b->bullet.a[1] = -9.8f;
	b->bullet.a[2] = 0.0f;
	b->bullet.mass = 1.0f;
	b->bullet.pos[0] = tankT->pos[0] + (-0.83f + tankT->userTurret.pos[2] + tankT->userTurret.tankCannon.pos[2])* sin((tankT->rot+tankT->userTurret.rot)*M_PI/180.0f);
	b->bullet.pos[1] = tankT->pos[1] + tankT->userTurret.pos[1] + tankT->userTurret.tankCannon.pos[1] + 2.2f*sin((tankT->userTurret.tankCannon.rot)*M_PI/180.0f);
	b->bullet.pos[2] = tankT->pos[2] + (-0.83f + tankT->userTurret.pos[2] + tankT->userTurret.tankCannon.pos[2])* cos((tankT->rot+tankT->userTurret.rot)*M_PI/180.0f);
	b->bullet.radius = 1.0f;
	b->bullet.v[0] = 50.0f * sin((tankT->rot+tankT->userTurret.rot)*M_PI/180.0f) - tankT->v[0];
	b->bullet.v[1] = 12.0f * sin((tankT->userTurret.tankCannon.rot)*M_PI/180.0f);
	b->bullet.v[2] = 50.0f * cos((tankT->rot+tankT->userTurret.rot)*M_PI/180.0f) - tankT->v[2];
	
	b->next = tankT->bulletRoot;
	tankT->bulletRoot = b;
}

float bodyRotate(float angle,float angleSign,float currentRotation)
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

void attackEnemy(tank *tankT)
{
	float stepD = 15.0f;
	float distanceX = (tankT->pos[0]-tanks[0].pos[0]);
	float distanceZ = (tankT->pos[2]-tanks[0].pos[2]);
	float distance = sqrt(distanceX*distanceX + distanceZ*distanceZ);
	float XSign = 1.0f;
	if(distanceX<0.0f)
		XSign = -1.0f;
	float TSign = 1.0f;
	if(tankT->rot<0.0f)
		TSign = -1.0f;
	//trova l'angolo rispetto a Z. Moltiplicazione per il segno di distanceX per recuperare
	//la direzione della rotazione rispetto all'asse X(di cui non teniamo conto nell'acos)
	//troviamo valori di rotazioni che vanno da -M_PI a +M_PI
	float angle = acos(distanceZ/distance);// * XSign;
	float reduction = distance/stepD;
	if(reduction>1.0f)
		reduction=1.0f;
	if(reduction<-1.0f)
		reduction=-1.0f;
	
	//all'interno della zona di raggio step frena e gira la torretta verso il carro armato nemico
	if(((reduction>=0.0f && reduction<1.0f) || (reduction>-1.0f && reduction<0.0f)))
	{
		tankT->v[0] *= 0.94f;
		tankT->v[2] *= 0.94f;
		//bisognerebbe anche qui trattare la rotazione come movimento graduale
		//e considerare tutti i casi di rotazione
		tankT->userTurret.rot = -tankT->rot + angle*XSign*180.0f/M_PI;
		//tankT->enginePower = 100.0f*reduction;
	}
	//all'esterno della zona di raggio step ruota verso il nemico ed avvicinati
	else 
	{    
		//se la rotazione � entro lo step di rotazione del nemico
		float stepR = 2.0f;
		if( 
		   ((tankT->rot)>(angle*XSign*180.0f/M_PI)-stepR && (tankT->rot)<(angle*XSign*180.0f/M_PI)+stepR) 
		   ||
		   (fabs(tankT->rot)+stepR > 180.0f && fabs(tankT->rot)>(angle*180.0f/M_PI)-stepR && fabs(tankT->rot)<(angle*180.0f/M_PI)+stepR)
		   )
		{
			//accelera
			tankT->enginePower = 100.0f;
			//raddrizza torretta nel cammino
			float TSign = 1.0f;
			if(tankT->userTurret.rot<0.0f)
				TSign = -1.0f;
			if(fabs(tankT->userTurret.rot)>0.1f)
				tankT->userTurret.rot += -1.5f * TSign;
		}
		//altrimenti se la rotazione � al di fuori dello step di rotazione del nemico
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
				tankT->rot = bodyRotate(angle,XSign,tankT->rot);
			}
			
		}
	}
	//sprintf(stampe,"Rotazione nemico: %f Rotazione utente: %f",tankT->rot,(angle*180.0f/M_PI));
	
	
}

void runAway(tank *tankT)
{
	float hMapL = 50.0f;
	float hMapW = 50.0f;
	//equazione della circonferenza
	// X^2 + Y^2 = r^2
	//devi trovare un punto esterno alla circonferenza che sia entro la mappa
	
	float step = 30.0f;
	float distanceX = (tankT->pos[0]-tanks[0].pos[0]);
	float distanceZ = (tankT->pos[2]-tanks[0].pos[2]);
	float distance = sqrt(distanceX*distanceX + distanceZ*distanceZ);
	//trova l'angolo rispetto a Z. Moltiplicazione per il segno di distanceX per recuperare
	//la direzione della rotazione rispetto all'asse X(di cui non teniamo conto nell'acos)
	float angle = acos(distanceZ/distance) * distanceX/fabs(distanceX);
	float reduction = distance/step;
	if(reduction>1.0f)
		reduction=1.0f;
	if(reduction<-1.0f)
		reduction=-1.0f;
	
	if(((reduction>=0.0f && reduction<1.0f) || (reduction>-1.0f && reduction<0.0f)))
	{
		tankT->enginePower = 100.0f*reduction;
	}
	else 
	{
		tankT->v[0] *= 0.94f;
		tankT->v[2] *= 0.94f;
		//tankT->enginePower = 100.0f*reduction;
	}
	
	tankT->rot = (angle+M_PI)*180.0f/M_PI;
	sprintf(stampe,"Lontananza: %f Velocit�: %f",distance,tankT->speed);
}

void planAction(tank *tankT)
{
	//controlla il nuovo stato
	if(
	   (tankT->life>50.0f && tankT->life<=70.0f && tankT->ammo>7) ||
	   (tankT->life>70.0f && tankT->ammo>5)
	   )
		attackEnemy(tankT);
	else
		runAway(tankT);
}

//reshape
void reshape ( int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glViewport(0, 0, w, h);
	ar = (float)w/(float)h;
	gluPerspective(45.0, ar, 0.2f, 500.0f);
	
	
	glMatrixMode(GL_MODELVIEW);
	
	//zbuffer
	//free(pixels);
	//pixels = malloc(sizeof(GLubyte)*w*h);
}

//inizializzazione
void init(void)
{
	//carico il livello
	levelMap = loadLevel("levels/sample.lvl");

	setDesertLights();
	
	// da usare solo insieme a glColor per definire un colore indipendente dalle sorgenti di luce!
	//	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	//	glEnable(GL_COLOR_MATERIAL);
	
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
	
	//definizione strutture
	//carro armato
	int i = 0;
	for(i=0;i<nTanks;i++)
	{
    	tanks[i].scale = 1.0f;
    	if(i==0)
    	{
			tanks[i].pos[0] = 0.0f;
			tanks[i].pos[1] = 1.0f;
			tanks[i].pos[2] = 0.0f;
        } else
			if(i==1)
			{
				tanks[i].pos[0] = 25.0f;
				tanks[i].pos[1] = 1.0f;
				tanks[i].pos[2] = 10.0f;
			} else
				if(i==2)
				{
					tanks[i].pos[0] = -12.0f;
					tanks[i].pos[1] = 1.0f;
					tanks[i].pos[2] = -30.0f;
				}
    	tanks[i].v[0] = 0.0f;
    	tanks[i].v[1] = 0.0f;
    	tanks[i].v[2] = 0.0f;
    	tanks[i].a[0] = 0.0f;
    	tanks[i].a[1] = 0.0f;
    	tanks[i].a[2] = 0.0f;
    	tanks[i].rot  = 0.0f;
    	tanks[i].speed = 0.0f;
    	tanks[i].enginePower = 0.0f;
    	tanks[i].frictionForce = 0.0f;
    	tanks[i].throttle = 0.0f;
    	tanks[i].mass = 1.0f;
    	tanks[i].iperBoostAccumul = 0.0f;
    	tanks[i].iperBoost = 0.0f;
    	tanks[i].bulletRoot = NULL;
    	tanks[i].ammo = 10;
    	tanks[i].life = 100.0f;
    	tanks[i].state = 2;
    	tanks[i].animation = 0;
    	//torretta
    	tanks[i].userTurret.pos[0] = 0.0f;
    	tanks[i].userTurret.pos[1] = 0.72f;
    	tanks[i].userTurret.pos[2] = 0.0f;
    	tanks[i].userTurret.rot = 0.0f;
    	//cannone
    	tanks[i].userTurret.tankCannon.length = 1.00f;
    	tanks[i].userTurret.tankCannon.pos[0] = 0.0f;
    	tanks[i].userTurret.tankCannon.pos[1] = -0.05f;
    	tanks[i].userTurret.tankCannon.pos[2] = -2.2f;
    	tanks[i].userTurret.tankCannon.rot = 0.0f;
    	//cingolo sx
    	tanks[i].userTreadL.pos[0] = -0.8f;
    	tanks[i].userTreadL.pos[1] = -0.06f;
    	tanks[i].userTreadL.pos[2] = 0.62f;
    	//cingolo dx
    	tanks[i].userTreadR.pos[0] = 0.8f;
    	tanks[i].userTreadR.pos[1] = -0.06f;
    	tanks[i].userTreadR.pos[2] = 0.62f;
    }
	
	//carico i modelli
	objTank[0] = (obj*) loadOBJ("obj/tank_body.obj");
	objTank[1] = (obj*) loadOBJ("obj/tread.obj");
	objTank[2] = (obj*) loadOBJ("obj/tread2.obj");
	objTank[3] = (obj*) loadOBJ("obj/tread3.obj");
	objTank[4] = (obj*) loadOBJ("obj/tank_turret.obj");
	objTank[5] = (obj*) loadOBJ("obj/tank_cannon.obj");
}

//visualizzazione
void display(void)
{
	int i = 0;
	
	displayDesertLights();
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    }
    else if(turretView==0) //third person view
    {
		gluLookAt(tanks[0].pos[0] + distanceFromCamera*sin(tanks[0].rot*M_PI/180.0f),//Eye x
				  3.5f,//Eye y
				  tanks[0].pos[2] + distanceFromCamera*cos(tanks[0].rot*M_PI/180.0f),//Eye z
				  tanks[0].pos[0], 3.0f,tanks[0].pos[2],//At
				  0.0f, 1.0f, 0.0f);//Up
    }
    else //turret view
    {
		gluLookAt(tanks[0].pos[0] + 1.0f*sin((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f),//Eye x
				  2.5f,//Eye y
				  tanks[0].pos[2] + 1.0f*cos((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f),//Eye z
				  tanks[0].pos[0] - 5.0f*sin((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f), 2.5f + sin(tanks[0].userTurret.tankCannon.rot*M_PI/180.0f),tanks[0].pos[2] - 5.0f*cos((tanks[0].rot+tanks[0].userTurret.rot)*M_PI/180.0f),//At
				  0.0f, 1.0f, 0.0f);//Up
    }
	
	//disegno i modelli
	
	//GRIGLIA
//	lightOff();
//	makeGrid(100.0f,100.0f,20.0f);
//	lightOn();
	
	//LIVELLO
	drawLevel(levelMap);
	
	//CARRI ARMATI
    bullets *p = NULL;
    for(i=0;i<nTanks;i++)
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
		if(tanks[i].animation<10)
			drawOBJ(objTank[1]);
		else if(tanks[i].animation<20)
			drawOBJ(objTank[2]);
		else if(tanks[i].animation<30)
			drawOBJ(objTank[3]);
		else
		{
			drawOBJ(objTank[1]);
			tanks[i].animation = 0;
		}
		glPopMatrix();
		//left tread
		glPushMatrix();
		glTranslatef(tanks[i].userTreadL.pos[0],tanks[i].userTreadL.pos[1],tanks[i].userTreadL.pos[2]);
		glRotatef(90.0f,0.0f,1.0f,0.0f);
		if(tanks[i].animation<10)
			drawOBJ(objTank[1]);
		else if(tanks[i].animation<20)
			drawOBJ(objTank[2]);
		else if(tanks[i].animation<30)
			drawOBJ(objTank[3]);
		else
		{
			drawOBJ(objTank[1]);
			tanks[i].animation = 0;
		}
		glPopMatrix();
		//tank body
		drawOBJ(objTank[0]);
    	glPopMatrix();
		
    	//float kkk = 0.0f;
    	for(p=tanks[i].bulletRoot; p!=NULL; p=p->next)
        {
            //proiettile
            glPushMatrix();
			glTranslatef(p->bullet.pos[0],p->bullet.pos[1],p->bullet.pos[2]);
			glRotatef(tanks[i].userTurret.rot + tanks[i].rot,0.0f,1.0f,0.0f);
			glRotatef(tanks[i].userTurret.tankCannon.rot,1.0f,0.0f,0.0f);
			glTranslatef(0.0f,0.0f,-tanks[i].userTurret.tankCannon.length);
			glutSolidSphere(1.2f, 20, 20);
            glPopMatrix();
            /*glPushMatrix(); disegna sfere per controllare cancellazione da struttura corretta
			 glTranslatef(2.0f*kkk,0.0f,0.0f);
			 glutSolidSphere(1.2f, 20, 20);
			 glPopMatrix();
			 kkk += 1.0f;
			 */
        }
        p=NULL;
    }//END FOR
	
	// diesegno il cielo
	glPushMatrix();
	glTranslatef(tanks[0].pos[0], tanks[0].pos[1], tanks[0].pos[2]);
	glScalef(150.0f, 150.0f, 150.0f);
	drawOBJ(levelMap->sky);
	glPopMatrix();
	
	
	glutSwapBuffers();
}

//lettura input
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27: //Esc
			exit(0);
			break;
		case 'w': //forward
			tanks[0].enginePower = 100.0f;
			break;
		case 's': //back
			tanks[0].enginePower = -100.0f;
			break;
		case 'a': //left
			if(tanks[0].v[2]==0.0f)
			{
				tanks[0].rot += 1.5f;
			}
			break;
		case 'd': //right
			if(tanks[0].v[2]==0.0f)
			{
				tanks[0].rot -= 1.5f;
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
			shoot(&tanks[0]);
			break;
	}
}

void idle(void)
{
	int i = 0;
	
	struct timeval newTime;
	
	gettimeofday(&newTime, NULL);
	
	deltaT = (((double)(newTime.tv_sec) + (double)(newTime.tv_usec)/1000000.0) -
			  ((double)(old.tv_sec) + (double)(old.tv_usec)/1000000.0));
	
	old.tv_sec = newTime.tv_sec;
	old.tv_usec = newTime.tv_usec;
	
	for(i=0;i<nTanks;i++)
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
    	tanks[i].v[0] += tanks[i].a[0] * deltaT;
    	tanks[i].pos[2] += tanks[i].v[2]*deltaT + tanks[i].a[2]*deltaT*deltaT*0.5f;
    	tanks[i].v[2] += tanks[i].a[2] * deltaT;
		
    	tanks[i].enginePower = 0.0f;
        
		// collisioni
		repos = borderCollision(&tanks[i]);
		tanks[i].v[0] = repos.x;
		tanks[i].v[2] = repos.y;
		
		
    	//FISICA PALLOTTOLE
    	bullets *p;
    	//elimina dalla struttura la radice con posizione Y negativa
    	//radice diventa primo nodo con Y positiva o NULL se non presente
    	while(tanks[i].bulletRoot!=NULL && tanks[i].bulletRoot->bullet.pos[1]<0.0f)
			tanks[i].bulletRoot = tanks[i].bulletRoot->next;
        //scorre tutta la struttura di pallottole
    	for(p=tanks[i].bulletRoot; p!=NULL; p=p->next)
    	{
            //toglie dalla struttura le pallottole con Y negativa
            while(p->next!=NULL && p->next->bullet.pos[1]<0.0f)
                p->next = p->next->next;
            //calcola la fisica delle pallottole
            int j = 0;
            for(j=0;j<nTanks;j++)
            {
				
                if(j!=i) 
                {
					if(p->bullet.pos[0]>tanks[j].pos[0]-1.0f && p->bullet.pos[0]<tanks[j].pos[0]+1.0f && p->bullet.pos[2]>tanks[j].pos[2]-1.0f && p->bullet.pos[2]<tanks[j].pos[2]+1.0f)
					{
						p->bullet.v[0] *= -1.0f;
						p->bullet.v[2] *= -1.0f;
						tanks[j].life -= 10.0f;
						//sprintf(stampe,"COLPITO carrarmato %i ! Vita rimasta: %f",j,tanks[j].life);
					}
                }
                p->bullet.pos[0] -= p->bullet.v[0]*deltaT;
                p->bullet.v[0] += p->bullet.a[0]*deltaT;
                p->bullet.pos[1] += p->bullet.v[1]*deltaT;
                p->bullet.v[1] += p->bullet.a[1]*deltaT;
                p->bullet.pos[2] -= p->bullet.v[2]*deltaT;
                p->bullet.v[2] += p->bullet.a[2]*deltaT;
            }
        }
		
    	//settings per le animazioni dei cingoli
    	if(fabs(tanks[i].speed)<6.0f)
            tanks[i].animation += 1*fabs(tanks[i].speed)*0.5;
    	else
    	    tanks[i].animation += 3;
		
    }//END FOR
	
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
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	
	
	init();
	
	glutMainLoop();
	return 0;
}
