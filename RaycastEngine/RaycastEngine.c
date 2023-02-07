#include <GL/glut.h>
#include <math.h>
#define PI 3.14159
#define P2 PI / 2
#define P3 3 * PI / 2
#define DR 0.01745329252

int WindowX = 1024;
int WindowY = 512;
float px, py, deltax, deltay, ang, frame1, frame2, fps;
struct Buttons {
	int w;
	int a;
	int s;
	int d;
};
struct Buttons Keys;

float degToRad(float deg)
{
	return (deg * PI / 180);
}

int MapX = 8, MapY = 8, MapS = 64;

int Map[64] = {
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,3,1,0,0,0,1,
	1,0,1,0,0,2,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

void drawMap2D(int map[64], int x, int y, int size)
{
	// Escanear cada punto del mapa como cordenadas (x, y) si hay un 1 poner blanco en un bucle
	int mx, my, mp; // Posicion vertical y horizontal (mx, my) posicion exacta (mp)
	for (my = 0; my < y; my++)
	{
		for (mx = 0; mx < x; mx++)
		{
			mp = my * x + mx;

			if (Map[mp] >= 1) { glColor3f(1, 1, 1); }
			else { glColor3f(0, 0, 0); }

			int xo = mx * size, yo = my * size;
			glBegin(GL_QUADS);
			glVertex2i(xo + 1, yo + 1);
			glVertex2i(xo + size - 1, yo + 1);
			glVertex2i(xo + size - 1, yo + size - 1);
			glVertex2i(xo + 1, yo + size - 1);
			glEnd();
		}
	}
}
 
float dist(float ax, float ay, float bx, float by, float ang)
{
	return (sqrt((by - ay) * (by - ay) + (bx - ax) * (bx - ax)));
}

void drawRays2D()
{
	int rloop, wx, wy, wp, mv, mh, dof; float rang, xo, yo, rdx, rdy, disT;
	rang = ang - DR * 30;
	if (rang < 0) { rang += 2 * PI; }
	if (rang > 2 * PI) { rang -= 2 * PI; }

	for (rloop = 0; rloop < 90; rloop++)
	{
		// --------- LINEAS HORIZONTALES ------------
		dof = 0;
		float aTan = -1 / tan(rang);
		float disH = 1000000, ddx = px, ddy = py;

		if (rang > PI) { rdy = (((int)py >> 6) << 6) - 0.0001; rdx = (py - rdy) * aTan + px; yo = -64; xo = -yo * aTan; }
		if (rang < PI) { rdy = (((int)py >> 6) << 6) +64; rdx = (py - rdy) * aTan + px; yo = 64; xo = -yo * aTan; }
		if (rang == 0 || rang == PI) { rdx = px; rdy = py; dof = 8; }
		while (dof < 8)
		{
			wx = (int) rdx >> 6; wy = (int) rdy >> 6; wp = wy * MapX + wx;

			if (wp > 0 && wp < MapS && Map[wp] > 0) { ddx = rdx, ddy = rdy, mh = Map[wp], disH = dist(px, py, ddx, ddy, rang), dof = 8;
			}
			else { rdx += xo; rdy += yo; dof += 1; }
		}

		// --------- LINEAS VERTICALES ------------
		dof = 0;
		float nTan = -tan(rang);
		float disV = 1000000, vdx = px, vdy = py;

		if (rang > P2 || rang < P3) { rdx = (((int)px >> 6) << 6) - 0.0001; rdy = (px - rdx) * nTan + py; xo = -64; yo = -xo * nTan; }
		if (rang < P2 || rang > P3) { rdx = (((int)px >> 6) << 6) + 64; rdy = (px - rdx) * nTan + py; xo = 64; yo = -xo * nTan; }
		if (rang == 0 || rang == PI) { rdx = px; rdy = py; dof = 8; }
		while (dof < 8)
		{
			wx = (int)rdx >> 6; wy = (int)rdy >> 6; wp = wy * MapX + wx;

			if (wp > 0 && wp < MapS && Map[wp] > 0) { vdx = rdx, vdy = rdy, mv = Map[wp], disV = dist(px, py, vdx, vdy, rang), dof = 8; }
			else { rdx += xo; rdy += yo; dof += 1; }
		}

		if (disV < disH) 
		{ 
			rdx = vdx, rdy = vdy, disT = disV, glColor3f(0.9, 0, 0); 
			if (mv == 2) { glColor3f(0, 0, 0.9); }
			if (mv == 3) { glColor3f(0, 0.9, 0); }
		}
		if (disH < disV)
		{
			rdx = ddx, rdy = ddy, disT = disH, glColor3f(0.7, 0, 0);
			if (mh == 2) { glColor3f(0, 0, 0.7); }
			if (mh == 3) { glColor3f(0, 0.9, 0); }
		}


		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2i(px, py);
		glVertex2i(rdx, rdy);
		glEnd();
		
		float center = ang - rang;
		if (center < 0) { center += 2 * PI; }
		if (center > 2 * PI) { center -= 2 * PI; }

		disT = disT * cos(center);
		float lineH = (MapS * 320) / disT; 
		if (lineH > 320) { lineH = 320; }
		float lineO = 160 - lineH / 2;


		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2f(rloop * 8 + 530, lineO);
		glVertex2f(rloop * 8 + 530, lineH + lineO);
		glEnd();

		float fx, fy;

		fx = cos(rdx - deltax);
		fy = sin(rdy - deltay);

		glLineWidth(8);
		glBegin(GL_POINTS);
		glVertex2f(fx, fy);
		glEnd();

		rang += DR;

		if (rang < 0) { rang += 2 * PI; }
		if (rang > 2 * PI) { rang -= 2 * PI; }


	}
}
void drawPlayer()
{

	if (ang >= 2 * PI) { ang -= 2 * PI; }
	if (ang < 0) { ang += 2 * PI; }

	if (Keys.a == 1) {  ang -= degToRad(0.2); deltax = cos(ang) * 5; deltay = sin(ang) * 5; }
	if (Keys.d == 1) { ang += degToRad(0.2); deltax = cos(ang) * 5; deltay = sin(ang) * 5; }
	if (Keys.s == 1) { px -= degToRad(deltax); py -= degToRad(deltay); }
	if (Keys.w == 1) { px += degToRad(deltax); py += degToRad(deltay); }

	glutPostRedisplay();

	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px, py);
	glEnd();

	glColor3f(1, 1, 0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px, py);
	glVertex2i(px + deltax * 5, py + deltay * 5);
	glEnd();
}

void KeyboardDown(unsigned char key)
{
	if (key == 'w') { Keys.w = 1; }
	if (key == 'a') { Keys.a = 1; }
	if (key == 's') { Keys.s = 1; }
	if (key == 'd') { Keys.d = 1; }
	glutPostRedisplay();
}

void KeyboardUp(unsigned char key)
{
	if (key == 'w') { Keys.w = 0; }
	if (key == 'a') { Keys.a = 0; }
	if (key == 's') { Keys.s = 0; }
	if (key == 'd') { Keys.d = 0; }
	glutPostRedisplay();
}


void Display()
{
	frame2 = glutGet(GLUT_ELAPSED_TIME);
	fps = (frame2 * frame1);
	frame1 = glutGet(GLUT_ELAPSED_TIME);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D(Map, MapX, MapY, MapS);
	drawRays2D();
	drawPlayer();
	glutSwapBuffers();
}

void Reshape(int x, int y)
{
	if (x != WindowX || y != WindowY){
		glutReshapeWindow(WindowX, WindowY);
}
}

void Init()
{
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, WindowX, WindowY, 0); // poner bien o todo sale al reves
	px = 300, py = 300;
	ang = 0;
	deltax = cos(ang) * 5;
	deltay = sin(ang) * 5;
}

int main(int* argc, char** argv)
{
	glutInitWindowSize(WindowX, WindowY);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInit(&argc, argv);
	int MainWindow = glutCreateWindow("I Dunno");
	Init();
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyboardDown);
	glutKeyboardUpFunc(KeyboardUp);
	glutMainLoop();
}