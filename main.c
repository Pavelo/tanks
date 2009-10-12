//#include <GLUT/gl.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "tga.h"

//COSTANTI
#define WIDTH 800
#define HEIGHT 600

//STRUTTURE
//struttura palla di cannone
typedef struct
{
	float radius;
	float pos[3];
	float v[3];
	float a[3];
	float mass;
} myBullet;

//struttura palle di cannone
struct _bullets
{
	myBullet bullet;
	struct _bullets *next;
};
typedef struct _bullets bullets;

//struttura carro armato
struct _cannon
{
	float pos[3];
	float rot;
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
	float throttle;
	float mass;
	float iperBoostAccumul;
	float iperBoost;
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
//carro armato
tank userTank;
float distanceFromCamera = 10.0f;

//FUNZIONI

//reshape
void reshape ( int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);
	ar = (float)w/(float)h;
	gluPerspective(45.0, ar, 0.2f, 100.0f);


	glMatrixMode(GL_MODELVIEW);

	//zbuffer
	//free(pixels);
	//pixels = malloc(sizeof(GLubyte)*w*h);
}

//inizializzazione
void init(void)
{
	float ambient[] = {1.0f, 1.0f, 1.0f};
	float diffuse[] = {1.0f, 1.0f, 1.0f};
	float specular[] = {1.0f, 1.0f, 1.0f};

	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
	glEnable(GL_LIGHT2);

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
	userTank.scale = 1.0f;
	userTank.v[0] = 0.0f;
	userTank.v[1] = 0.0f;
	userTank.v[2] = 0.0f;
	userTank.a[0] = 0.0f;
	userTank.a[1] = 0.0f;
	userTank.a[2] = 0.0f;
	userTank.rot  = 0.0f;
	userTank.throttle = 0.0f;
	userTank.mass = 1.0f;
	userTank.iperBoostAccumul = 0.0f;
	userTank.iperBoost = 0.0f;
	//torretta
	userTank.userTurret.pos[0] = 0.0f;
	userTank.userTurret.pos[1] = 0.0f;
	userTank.userTurret.pos[2] = 0.0f;
	userTank.userTurret.rot = 0.0f;
	//cannone
	userTank.userTurret.tankCannon.pos[0] = 0.0f;
	userTank.userTurret.tankCannon.pos[1] = 0.0f;
	userTank.userTurret.tankCannon.pos[2] = 0.0f;
	userTank.userTurret.tankCannon.rot = 0.0f;
	//cingolo sx
	userTank.userTreadL.pos[0] = 0.0f;
	userTank.userTreadL.pos[1] = 0.0f;
	userTank.userTreadL.pos[2] = 0.0f;
	//cingolo dx
	userTank.userTreadR.pos[0] = 0.0f;
	userTank.userTreadR.pos[1] = 0.0f;
	userTank.userTreadR.pos[2] = 0.0f;

	//carico i modelli
	loadOBJ("obj/tank_camo.obj", 0); //fixare il forced mtl nell'objloader
}

//visualizzazione
void display(void)
{
	float light0Position[] = {0.0f, 10.0f, 20.0f, 1.0f};
	float light1Position[] = {20.0f, -10.0f, 0.0f, 1.0f};
	float light2Position[] = {-20.0f, 0.0f, -20.0f, 1.0f};

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(userTank.pos[0] + distanceFromCamera*sin(userTank.rot*M_PI/180.0f),//Eye x
			3.5f,//Eye y
			userTank.pos[2] + distanceFromCamera*cos(userTank.rot*M_PI/180.0f),//Eye z
			userTank.pos[0], 3.0f,userTank.pos[2],//At
			0.0f, 1.0f, 0.0f);//Up

	glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2Position);

	//disegno i modelli
	drawOBJ(0);

	glutSwapBuffers();
}

//lettura input
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:
		exit(0);
		break;
	}
}

void idle(void)
{
	float deltaTeta;
	struct timeval newTime;

	gettimeofday(&newTime, NULL);

	deltaT = (((double)(newTime.tv_sec) + (double)(newTime.tv_usec)/1000000.0) -
			((double)(old.tv_sec) + (double)(old.tv_usec)/1000000.0));

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
