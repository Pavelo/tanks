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
#include "tga.h"
#include "levelLoader.h"
#include "objLoader.h"

//COSTANTI
#define WIDTH 800
#define HEIGHT 600

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
	float mass;
	float iperBoostAccumul;
	float iperBoost;
	bullets *bulletRoot;
	int ammo;
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
float frictionCoeff = 0.8f;
float distanceFromCamera = 10.0f;
int turretView = 0;
int fixedView  = 0;
map *levelMap;
int animation = 0;

//FUNZIONI

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

//reshape
void reshape ( int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);
	ar = (float)w/(float)h;
	gluPerspective(45.0, ar, 0.2f, 300.0f);


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
	//glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
	//glEnable(GL_LIGHT2);

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
	userTank.pos[0] = 0.0f;
	userTank.pos[1] = 1.0f;
	userTank.pos[2] = 0.0f;
	userTank.v[0] = 0.0f;
	userTank.v[1] = 0.0f;
	userTank.v[2] = 0.0f;
	userTank.a[0] = 0.0f;
	userTank.a[1] = 0.0f;
	userTank.a[2] = 0.0f;
	userTank.rot  = 0.0f;
	userTank.enginePower = 0.0f;
	userTank.throttle = 0.0f;
	userTank.mass = 1.0f;
	userTank.iperBoostAccumul = 0.0f;
	userTank.iperBoost = 0.0f;
	userTank.bulletRoot = NULL;
	userTank.ammo = 10;
	//torretta
	userTank.userTurret.pos[0] = 0.0f;
	userTank.userTurret.pos[1] = 0.72f;
	userTank.userTurret.pos[2] = 0.0f;
	userTank.userTurret.rot = 0.0f;
	//cannone
	userTank.userTurret.tankCannon.pos[0] = 0.0f;
	userTank.userTurret.tankCannon.pos[1] = -0.05f;
	userTank.userTurret.tankCannon.pos[2] = -2.2f;
	userTank.userTurret.tankCannon.rot = 0.0f;
	//cingolo sx
	userTank.userTreadL.pos[0] = -0.8f;
	userTank.userTreadL.pos[1] = -0.06f;
	userTank.userTreadL.pos[2] = 0.62f;
	//cingolo dx
	userTank.userTreadR.pos[0] = 0.8f;
	userTank.userTreadR.pos[1] = -0.06f;
	userTank.userTreadR.pos[2] = 0.62f;

	//carico i modelli

/*	loadOBJ("obj/tank_body.obj");
	loadOBJ("obj/tread.obj", 1);
	loadOBJ("obj/tread2.obj", 2);
	loadOBJ("obj/tread3.obj", 3);
	loadOBJ("obj/tank_turret.obj", 4);
	loadOBJ("obj/tank_cannon.obj", 5);
	loadOBJ("obj/skyl.obj", 6);
	loadOBJ("obj/tank_body.obj", 7);*/

	//carico il livello
	levelMap = loadLevel("levels/sample.lvl");
}

//visualizzazione
void display(void)
{
	float light0Position[] = {0.0f, 70.0f, 0.0f, 1.0f};
	float light1Position[] = {0.0f, 10.0f, 0.0f, 1.0f};
	float light2Position[] = {-20.0f, 0.0f, 0.0f, 1.0f};

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
	gluLookAt(userTank.pos[0],//Eye x
			3.5f,//Eye y
			userTank.pos[2] + distanceFromCamera,//Eye z
			userTank.pos[0], 3.0f,userTank.pos[2],//At
			0.0f, 1.0f, 0.0f);//Up
    }
    else if(turretView==0) //third person view
    {
	gluLookAt(userTank.pos[0] + distanceFromCamera*sin(userTank.rot*M_PI/180.0f),//Eye x
			3.5f,//Eye y
			userTank.pos[2] + distanceFromCamera*cos(userTank.rot*M_PI/180.0f),//Eye z
			userTank.pos[0], 3.0f,userTank.pos[2],//At
			0.0f, 1.0f, 0.0f);//Up
    }
    else //turret view
    {
          gluLookAt(userTank.pos[0] + 1.0f*sin((userTank.rot+userTank.userTurret.rot)*M_PI/180.0f),//Eye x
			2.5f,//Eye y
			userTank.pos[2] + 1.0f*cos((userTank.rot+userTank.userTurret.rot)*M_PI/180.0f),//Eye z
			userTank.pos[0] - 5.0f*sin((userTank.rot+userTank.userTurret.rot)*M_PI/180.0f), 2.5f + sin(userTank.userTurret.tankCannon.rot*M_PI/180.0f),userTank.pos[2] - 5.0f*cos((userTank.rot+userTank.userTurret.rot)*M_PI/180.0f),//At
			0.0f, 1.0f, 0.0f);//Up
    }

	//glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1Position);
	//glLightfv(GL_LIGHT2, GL_POSITION, light2Position);

	//disegno i modelli

	//GRIGLIA
	lightOff();
	makeGrid(100.0f,100.0f,20.0f);
	lightOn();

    //CARRO ARMATO UTENTE
/*	glPushMatrix();
		glTranslatef(userTank.pos[0], userTank.pos[1], userTank.pos[2]);
		glRotatef(userTank.rot,0.0f,1.0f,0.0f);
		//turret
		glPushMatrix();
		   glTranslatef(userTank.userTurret.pos[0],userTank.userTurret.pos[1],userTank.userTurret.pos[2]);
           glRotatef(userTank.userTurret.rot,0.0f,1.0f,0.0f);
              //cannon
		      glPushMatrix();
		      glTranslatef(userTank.userTurret.tankCannon.pos[0],userTank.userTurret.tankCannon.pos[1],userTank.userTurret.tankCannon.pos[2]+1.0f);
              glRotatef(userTank.userTurret.tankCannon.rot,1.0f,0.0f,0.0f);
              glTranslatef(0.0f,0.0f,-1.0f);
              drawOBJ(5);
           glPopMatrix();
           drawOBJ(4);
        glPopMatrix();
        //right tread
		glPushMatrix();
		   glTranslatef(userTank.userTreadR.pos[0],userTank.userTreadR.pos[1],userTank.userTreadR.pos[2]);
           glRotatef(90.0f,0.0f,1.0f,0.0f);
           if(animation<10)
           drawOBJ(1);
           else if(animation<20)
           drawOBJ(2);
           else if(animation<30)
           drawOBJ(3);
           else
           {
           drawOBJ(1);
           animation = 0;
           }
        glPopMatrix();
        //left tread
		glPushMatrix();
		   glTranslatef(userTank.userTreadL.pos[0],userTank.userTreadL.pos[1],userTank.userTreadL.pos[2]);
           glRotatef(90.0f,0.0f,1.0f,0.0f);
           if(animation<10)
           drawOBJ(1);
           else if(animation<20)
           drawOBJ(2);
           else if(animation<30)
           drawOBJ(3);
           else
           {
           drawOBJ(1);
           animation = 0;
           }
        glPopMatrix();
        //tank body
		drawOBJ(0);
	glPopMatrix();

	bullets *p;
	//float kkk = 0.0f;
	for(p=userTank.bulletRoot; p!=NULL; p=p->next)
    {
        //proiettile
        glPushMatrix();
           glTranslatef(p->bullet.pos[0],p->bullet.pos[1],p->bullet.pos[2]);
           glutSolidSphere(0.2f, 20, 20);
        glPopMatrix();
*/        /*glPushMatrix(); disegna sfere per controllare cancellazione da struttura corretta
           glTranslatef(2.0f*kkk,0.0f,0.0f);
           glutSolidSphere(1.2f, 20, 20);
        glPopMatrix();
        kkk += 1.0f;
        */
/*    }

    //LANDSCAPE
	lightOff();
	glPushMatrix();
		glScalef(150.0f, 150.0f, 150.0f);
		drawOBJ(6);
	glPopMatrix();
	lightOn();
*/

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
		userTank.enginePower = 100.0f;
		break;
	case 's': //back
		userTank.enginePower = -100.0f;
		break;
    case 'a': //left
		if(userTank.v[2]==0.0f)
		{
            userTank.rot += 1.5f;
        }
		break;
    case 'd': //right
		if(userTank.v[2]==0.0f)
		{
            userTank.rot -= 1.5f;
        }
		break;
    case 't': //enable/disable turret view
		if(turretView==0)
            turretView = 1;
        else
            turretView = 0;
		break;
    case 'c': //turret right
        userTank.userTurret.rot -= 1.5f;
		break;

    case 'z': //turret left
        userTank.userTurret.rot += 1.5f;
		break;
    case 'e': //cannon up
        if(userTank.userTurret.tankCannon.rot<15.0f)
        userTank.userTurret.tankCannon.rot += 1.5f;
		break;
    case 'q': //cannon down
        if(userTank.userTurret.tankCannon.rot>0.0f)
        userTank.userTurret.tankCannon.rot -= 1.5f;
		break;
    case 35: //Brakes
		userTank.v[0] *= 0.5f;
		userTank.v[2] *= 0.5f;
		break;
	case 32: //Space
		shoot(&userTank);
		break;
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

    //calculating friction force [Fk = fC * g * m]
    float frictionForce = 9.8f * frictionCoeff * userTank.mass;
    float speed = userTank.v[0]*sin(userTank.rot*M_PI/180.0f) + userTank.v[2]*cos(userTank.rot*M_PI/180.0f);
    if((speed>=-0.1f) && (speed<=0.1f) && (userTank.enginePower<=0.0f))
    {
       frictionForce = 0.0f;
    }
    else if(speed<-0.1)
    {
       frictionForce = 9.8f * frictionCoeff * userTank.mass;
    }
    else
    {
       frictionForce = (9.8f * frictionCoeff * userTank.mass)*-1;
    }

    //stop from continous movement
    if((speed>=-0.1f) && (speed<=0.1f))
    {
       userTank.v[0]=0.0f;
       userTank.v[2]=0.0f;
    }

    //calculating throttle [a = (Fe - Fk) / m]
    userTank.a[0] = ((userTank.enginePower - frictionForce)*sin(userTank.rot*M_PI/180.0f)/userTank.mass)*-1;
    userTank.a[2] = ((userTank.enginePower - frictionForce)*cos(userTank.rot*M_PI/180.0f)/userTank.mass)*-1;
    userTank.throttle = ((userTank.enginePower - frictionForce)/userTank.mass)*-1;

    // S = So + v*t
	//userTank.v[2] += userTank.throttle * deltaT;
	//userTank.pos[2] += userTank.v[2] * deltaT;

	// S = So + vo*t + a*t*t*0.5
	userTank.pos[0] += userTank.v[0]*deltaT + userTank.a[0]*deltaT*deltaT*0.5f;
	userTank.v[0] += userTank.a[0] * deltaT;
	userTank.pos[2] += userTank.v[2]*deltaT + userTank.a[2]*deltaT*deltaT*0.5f;
	userTank.v[2] += userTank.a[2] * deltaT;

	userTank.enginePower = 0.0f;

	//FISICA PALLOTTOLE
	bullets *p;
	//elimina dalla struttura la radice con posizione Y negativa
	//radice diventa primo nodo con Y positiva o NULL se non presente
	while(userTank.bulletRoot!=NULL && userTank.bulletRoot->bullet.pos[1]<0.0f)
            userTank.bulletRoot = userTank.bulletRoot->next;
    //scorre tutta la struttura di pallottole
	for(p=userTank.bulletRoot; p!=NULL; p=p->next)
	{
        //toglie dalla struttura le pallottole con Y negativa
        while(p->next!=NULL && p->next->bullet.pos[1]<0.0f)
            p->next = p->next->next;
        //calcola la fisica delle pallottole
        int j;
        for(j=0;j<3;j++)
        {
            if(j==1)
            p->bullet.pos[j] += p->bullet.v[j]*deltaT;
            else
            p->bullet.pos[j] -= p->bullet.v[j]*deltaT;
            p->bullet.v[j] += p->bullet.a[j]*deltaT;
        }

    }

	//animation settings
	if(fabs(speed)<6.0f)
	animation += 1*fabs(speed)*0.5;
	else
	animation += 3;

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
