/***
CTIS164 - Template Source Program
----------
STUDENT : Mian Muhammad Umair Amer
SECTION : 3
HOMEWORK: 4
----------
PROBLEMS: NONE
----------
ADDITIONAL FEATURES:
1) Interactive Menu To Start Annimation
2) About showing info of Animation on new screen and Exit option
3) Anytime during Animation by pressing SpaceBar you could return to Main Menu
4) Colored Effects when light source is turned on/off (respective color or Grey)
***/

#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT 700

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

						  //For Timmer
bool activetimer = true;

typedef struct {
	float r, g, b;
} color_t;

//Structure for lights
typedef struct {
	vec_t   pos;
	color_t color;
	vec_t   vel;
	bool glow;
} light_t;

typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;

//Structure For Planets
typedef struct {
	color_t color;
	float angle;
	float Aspeed;
	bool direc;
	float radius;
} planet_t;

#define NUM 4				//No of light sources including Sun
#define BEGIN 0				// Begin State = Menu
#define RUN 1				// Animation Running State
int ann_state = BEGIN;		// state initialized to Begin
bool Aboutmsg = false;		// bool for showing About message

							//Array of 3 Planets
planet_t P[NUM - 1];

//Array Declaration and Initialization of 4 Light Sources
light_t light[NUM] =
{
	{ { -100, 0 },{ 1, 0, 0 },{ 3,  2 } ,true }, //RED
	{ { 200, 0 },{ 0, 1, 0 },{ -2, -1 },true }, //GREEN
	{ { -200, 0 },{ 0, 0, 1 },{ 3, -2 } ,true }, //BLUE
	{ { 0, 0 },{ 1, 1, 0 },{ 0, 0 } ,true } //YELLOW SUN
};

color_t mulColor(float k, color_t c)
{
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}

color_t addColor(color_t c1, color_t c2)
{
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}

// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d)
{
	return fabs((-1.0 / 350.0) * d + 1.0);
}

color_t calculateColor(light_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N) *distanceImpact(magV(L));
	return mulColor(factor, source.color);
}

//Used For Initializations
void Init();
//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
void vprint(int x, int y, void *font, char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

//Displays Menu
void displayBEGIN()
{
	//Gradient Background
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 0);
	glVertex2f(-400, 350);
	glVertex2f(400, 350);
	glColor3ub(200, 150, 0);
	glVertex2f(400, -350);
	glVertex2f(-400, -350);
	glEnd();

	if (!Aboutmsg) //If not showing About message
	{
		//Title
		glColor3ub(255, 255, 0);
		vprint2(-350, 230, 0.495, "REFLECTING PLANETS");
		vprint2(-260, 180, 0.2, "< ANIMATION BY M.M.UMAIR AMER >");

		//Start Button
		glBegin(GL_LINE_LOOP);
		glVertex2f(-100, 0);
		glVertex2f(100, 0);
		glVertex2f(100, -50);
		glVertex2f(-100, -50);
		glEnd();
		vprint(-40, -35, GLUT_BITMAP_TIMES_ROMAN_24, "START");

		//About Button
		glBegin(GL_LINE_LOOP);
		glVertex2f(-80, -70);
		glVertex2f(80, -70);
		glVertex2f(80, -120);
		glVertex2f(-80, -120);
		glEnd();
		vprint(-45, -105, GLUT_BITMAP_TIMES_ROMAN_24, "ABOUT");

		//Exit Button
		glBegin(GL_LINE_LOOP);
		glVertex2f(-50, -140);
		glVertex2f(50, -140);
		glVertex2f(50, -190);
		glVertex2f(-50, -190);
		glEnd();
		vprint(-28, -175, GLUT_BITMAP_TIMES_ROMAN_24, "EXIT");

		glColor3ub(255, 255, 255);
		vprint2(-290, -300, 0.3, "WELCOME TO UMAIR GALAXY");
	}
	else
	{
		glColor3ub(255, 255, 255);
		vprint(-150, 50, GLUT_BITMAP_9_BY_15, "   This Software is Licensed ");
		vprint(-150, 20, GLUT_BITMAP_9_BY_15, "Copyrights are Reserved (c) 2018");
		vprint(-150, -10, GLUT_BITMAP_9_BY_15, "       Manufactured By :");
		vprint(-150, -40, GLUT_BITMAP_9_BY_15, "   Mian Muhammad Umair Amer");

		//Exit Button
		glBegin(GL_LINE_LOOP);
		glVertex2f(-50, -140);
		glVertex2f(50, -140);
		glVertex2f(50, -190);
		glVertex2f(-50, -190);
		glEnd();
		vprint(-32, -175, GLUT_BITMAP_TIMES_ROMAN_24, "BACK");
	}
}

//Draws a single Planet
void drawPlanet(planet_t planet, float radius)
{
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.3, 0.3, 0.3);
	//gets the x and y points of the circle
	float x = radius * cos(planet.angle * D2R), y = radius * sin(planet.angle * D2R);
	glVertex2f(x, y);

	for (int i = 0; i <= 100; i++)
	{
		float angle = 2 * PI * i / 100; //increasing angle to form circle

		vec_t v = { x + planet.radius*cos(angle), y + planet.radius*sin(angle) };
		vertex_t P = { { v.x, v.y } };

		P.N = unitV(subV({ v.x, v.y }, { x, y }));

		//Calculating Color of all the sources on the planet
		color_t Color_result = { 0, 0, 0 };
		for (int k = 0; k < NUM; k++)
			if (light[k].glow == true)
				Color_result = addColor(Color_result, calculateColor(light[k], P));

		glColor3f(Color_result.r, Color_result.g, Color_result.b);
		glVertex2f(v.x, v.y);
	}
	glEnd();
}

//To return the state according to the light sources if they are glowing or not
char* state(bool r)
{
	return (r == false) ? "Off" : "On";
}

//Displays Information Board at the top of Animation
void displayInfo()
{
	glColor3ub(155, 255, 50);
	vprint(-350, 330, GLUT_BITMAP_9_BY_15, "Mian Muhammad Umair");
	vprint(-300, 310, GLUT_BITMAP_9_BY_15, "Amer");

	//For Red ON/OFF
	glColor3f(1, 0, 0);
	vprint(-130, 330, GLUT_BITMAP_9_BY_15, "<F1> Red");
	if (!light[0].glow)
		glColor3f(0.7, 0.7, 0.7);
	vprint(-50, 330, GLUT_BITMAP_9_BY_15, "%s", state(light[0].glow));

	//For Green ON/OFF
	glColor3f(0, 1, 0);
	vprint(30, 330, GLUT_BITMAP_9_BY_15, "<F2> Green");
	if (!light[1].glow)
		glColor3f(0.7, 0.7, 0.7);
	vprint(130, 330, GLUT_BITMAP_9_BY_15, "%s", state(light[1].glow));

	//For Blue ON/OFF
	glColor3f(0, 0, 1);
	vprint(210, 330, GLUT_BITMAP_9_BY_15, "<F3> Blue");
	if (!light[2].glow)
		glColor3f(0.7, 0.7, 0.7);
	vprint(300, 330, GLUT_BITMAP_9_BY_15, "%s", state(light[2].glow));

	//For Sun ON/OFF
	glColor3f(1, 1, 0);
	vprint(-130, 310, GLUT_BITMAP_9_BY_15, "<F4> Sun");
	if (!light[3].glow)
		glColor3f(0.7, 0.7, 0.7);
	vprint(-50, 310, GLUT_BITMAP_9_BY_15, "%s", state(light[3].glow));

	//To Restart / Pause Game
	glColor3ub(255, 20, 255);
	vprint(30, 310, GLUT_BITMAP_9_BY_15, "<F5> Pause");
	if (!activetimer)
		glColor3f(0.7, 0.7, 0.7);
	vprint(130, 310, GLUT_BITMAP_9_BY_15, "%s", state(activetimer));

	glColor3ub(155, 255, 150);
	vprint(210, 310, GLUT_BITMAP_9_BY_15, "<F6> Restart");

	//Line To separate the info bar and animation
	glColor3ub(255, 255, 0);
	glBegin(GL_LINES);
	glVertex2f(-350, 300);
	glVertex2f(350, 300);
	glEnd();
	//Message showing instruction to exit animation
	vprint(-200, -348, GLUT_BITMAP_9_BY_15, "Press <SpaveBar> To Go Back To Main Menu");
}

//
// To display onto window using OpenGL commands
//
void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	//Checking State
	if (ann_state == BEGIN)
		displayBEGIN();
	else if (ann_state == RUN)
	{
		// Draws 4 light source (Red,Green,Blue & Yellow Sun)
		for (int i = 0; i < NUM; i++)
			if (light[i].glow)
			{
				glColor3f(light[i].color.r, light[i].color.g, light[i].color.b);
				if (i == NUM - 1) //For Sun
					circle(light[i].pos.x, light[i].pos.y, 32);
				else
					circle(light[i].pos.x, light[i].pos.y, 10);
			}

		//Drawing 3 Planets
		float radius = 150; //initializing radius
		for (int i = 0; i < 3; i++)
		{
			drawPlanet(P[i], radius);
			radius += 60;
		}

		// keyboard instructions
		displayInfo();
	}


	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == 32)
	{
		ann_state = BEGIN;
		Init(); //intialize
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// TO control the light sources / To turn them ON and OFF
	if (key == GLUT_KEY_F1) //RED
		light[0].glow = !light[0].glow;
	if (key == GLUT_KEY_F2) //GREEN
		light[1].glow = !light[1].glow;
	if (key == GLUT_KEY_F3) //BLUE
		light[2].glow = !light[2].glow;
	if (key == GLUT_KEY_F4) //YELLOW SUN
		light[3].glow = !light[3].glow;
	if (key == GLUT_KEY_F5) //Pause/Start Animation
		activetimer = !activetimer;
	if (key == GLUT_KEY_F6) // Restart
		Init();
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	//changing cordinate system
	int x2 = x - winWidth / 2;
	int y2 = winHeight / 2 - y;
	//Displaying Main Menu Screen and Its Buttons Functions
	if (ann_state == BEGIN &&  button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
		if (x2 >= -100 && x2 <= 100 && y2 >= -50 && y2 <= 0) //if Start Button is pressed
			ann_state = RUN;
		else if (x2 >= -80 && x2 <= 80 && y2 >= -120 && y2 <= -70) //if About Button is pressed
			Aboutmsg = true; // so it's contents are not shown
		else if (x2 >= -50 && x2 <= 50 && y2 >= -190 && y2 <= -140 && Aboutmsg == false) // if Exit Button is pressed
			exit(0);
		else
			Aboutmsg = false; //To return Back to menu by clicking Back Button

							  // to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

void onMove(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v)
{

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	if (activetimer)
	{
		for (int i = 0; i < NUM; i++)
		{
			light[i].pos = addV(light[i].pos, light[i].vel);

			// Bouncing Back from Walls.
			if (light[i].pos.x > winWidth / 2 - 10 || light[i].pos.x < -winWidth / 2 + 10)
				light[i].vel.x *= -1;
			if (light[i].pos.y > 300 - 10 || light[i].pos.y < -winHeight / 2 + 10) // Bounce Back from the Scoreboardline at top
				light[i].vel.y *= -1;
		}

		//clockwise or Anti-clockwise movement of Planets
		for (int i = 0; i < 3; i++)
			if (P[i].direc)
				P[i].angle += P[i].Aspeed;
			else
				P[i].angle -= P[i].Aspeed;
	}


	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Initializing for the Restart of Animation
	light[0] = { { -100, 0 },{ 1, 0, 0 } };
	light[1] = { { 200, 0 },{ 0, 1, 0 } };
	light[2] = { { -200, 0 },{ 0, 0, 1 } };

	for (int i = 0; i < 3; i++)
	{
		P[i].color = { 150 , 150 , 150 };
		P[i].direc = rand() % 2;
		P[i].angle = rand() % 361;
		P[i].Aspeed = rand() % 2 + 1;
		P[i].radius = rand() % 6 + 22;

		//Randon vector(direction)
		light[i].vel = { (double)(rand() % 7) + 1, (double)(rand() % 7) + 1 };
		light[i].glow = true;
	}
	light[3].glow = true; //Sun is always glowing in start
	activetimer = true; //game is always working in start
}

void main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Reflecting Planets (HW # 4) - Mian Muhammad Umair Amer");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}