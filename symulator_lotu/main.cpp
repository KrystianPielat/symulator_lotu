#include <iostream>			
#include <utilities.h>
#include <sstream>
#include <algorithm>
#include <Windows.h>


static const int EXTENSION = 40;

static const int TERRAIN_RESOLUTION = 2;

const int map_size = EXTENSION * TERRAIN_RESOLUTION;
static const int tFPS = 60;

float light = 1;
GLfloat cam_pos[4] = { 0, 0 ,0, 0 };

static GLfloat matrix[map_size][map_size][3];
GLuint trees, water, mountain, clouds, cockpit;

float velocity = 0.005f;

bool autopilot_on = false;
bool cabin_visible = true;
bool directional_on = false;
int previous_x, previous_y;
static float view[3];

float autopilot_alt = 1;

void initFog() {

	glEnable(GL_FOG);
	GLfloat fog_colour[] = { light,light,light,1.0 };


	glFogfv(GL_FOG_COLOR, fog_colour);
	if (light == 1)
		glFogf(GL_FOG_DENSITY, 0.1);
	else
		glFogf(GL_FOG_DENSITY, 0.2);

}

void initLight() {

	glClearColor(0, 0, 0, 1);

	GLfloat Al0[] = { light,light,light,1.0 };
	GLfloat Dl0[] = { light,light,light,1.0 };
	GLfloat Sl0[] = { light,light,light,1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, Al0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Dl0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Sl0);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
}

void initTextures() {
	glGenTextures(1, &trees);
	glBindTexture(GL_TEXTURE_2D, trees);
	loadImageFile((char*)"trees.jpg");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	glGenTextures(1, &water);
	glBindTexture(GL_TEXTURE_2D, water);
	loadImageFile((char*)"water.jpg");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &mountain);
	glBindTexture(GL_TEXTURE_2D, mountain);
	loadImageFile((char*)"mountain.jpg");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &clouds);
	glBindTexture(GL_TEXTURE_2D, clouds);
	loadImageFile((char*)"clouds.jpg");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &cockpit);
	glBindTexture(GL_TEXTURE_2D, cockpit);
	loadImageFile((char*)"cockpit.png");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


float getTerrAlt(float x = cam_pos[0], float y = cam_pos[1]) {
	return matrix[map_size / 2 + int(x)][map_size / 2 + int(y)][2];
}

void FPS()
{

	static int before = glutGet(GLUT_ELAPSED_TIME);
	static int frames = 0;
	int now, timePassed;

	frames++;

	now = glutGet(GLUT_ELAPSED_TIME);
	timePassed = (now - before) / 1000;

	if (timePassed >= 1) {
		stringstream title;
		title << "FPS= " << frames;
		glutSetWindowTitle(title.str().c_str());
		before = now;
		frames = 0;
	}
}

void onClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		previous_x = x;
		previous_y = y;
	}

}

void onDrag(int x, int y)
{
	static const float pixel2deg = 0.3f;

	int inc_x = (x - previous_x) * pixel2deg;
	int inc_y = (y - previous_y) * pixel2deg;

	if (abs(view[0] + inc_x) < 90)
		view[0] += inc_x;
	if (abs(view[1] + inc_y) < 90)
		view[1] += inc_y;

	previous_x = x;
	previous_y = y;

	glutPostRedisplay();
}

void autoPilot() {


	int depth = 20;
	float max = 0;
	float x, y;
	for (int i = 0; i < depth; i++) {
		x = cam_pos[0] + sin((90 - cam_pos[3]) * 3.14159 / 180) * velocity * i;
		y = cam_pos[1] - cos((90 - cam_pos[3]) * 3.14159 / 180) * velocity * i;
		float height = getTerrAlt(x, y);
		if (height > max) {
			max = height;
		}
	}
	cam_pos[2] = autopilot_alt + max + 0.1f;

}


void init()
{
	cout << "Start " << endl;
	cout << "GL version " << glGetString(GL_VERSION) << endl;


	glClearColor(0, 0, 0, 1); // bg color

	initLight();

	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	//glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);


	initTextures();


	srand(time(0));


	int anomaly_count = round(EXTENSION / 10);
	int** anomalies;


	anomalies = new int* [anomaly_count];

	for (int i = 0; i < anomaly_count; i++) {
		anomalies[i] = new int[2];
		int x = rand() % (map_size - 19) + 10;
		int y = rand() % (map_size - 19) + 10;

		anomalies[i][0] = x;
		anomalies[i][1] = y;

		matrix[x][y][2] = float(rand() % 6) + 1;
	}
	float sum_dist = 0;
	float height = 0;
	for (int j = 0; j < map_size; j++) {


		for (int i = 0; i < map_size; i++) {

			int count = 0;

			for (int z = 0; z < anomaly_count; z++) {
				float d = sqrt((i - anomalies[z][0]) * (i - anomalies[z][0]) +
					(j - anomalies[z][1]) * (j - anomalies[z][1]))
					/ sqrt(map_size * map_size + map_size * map_size);


				if (d < 0.2) {
					sum_dist += d;
					height += matrix[anomalies[z][0]][anomalies[z][1]][2] * (1 - d / 0.2);
					count++;
				}


			}
			if (count)
				sum_dist /= count;


			matrix[i][j][0] = (i - map_size / 2 + 0.5) / TERRAIN_RESOLUTION;
			matrix[i][j][1] = (j - map_size / 2 + 0.5) / TERRAIN_RESOLUTION;
			matrix[i][j][2] = float(rand() % 100) / 500 + pow((1 - sum_dist), 30) * height;

			count = 0;
			sum_dist = 0;
			height = 0;

		}
	}

	for (int i = 0; i < anomaly_count; i++) {
		delete[] anomalies[i];
	}
	delete[] anomalies;

#pragma endregion 

	cam_pos[2] = matrix[map_size / 2][map_size / 2][2] + 1;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	initFog();

}


void directional() {

	glClearColor(0, 0, 0, 1);

	GLfloat Al0[] = { 1,1,1,1.0 };
	GLfloat Dl0[] = { 1.0,1.0,0.0,1.0 };
	GLfloat Sl0[] = { 1.0,1.0,0.0,1.0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, Al0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Dl0);
	glLightfv(GL_LIGHT1, GL_SPECULAR, Sl0);


}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	initLight();
	directional();
	if (directional_on) {
		glEnable(GL_LIGHT1);
		GLfloat position[] = { cam_pos[0],cam_pos[1],cam_pos[2],0 };
		glLightfv(GL_LIGHT1, GL_POSITION, position);

		GLfloat dir_central[] = { cam_pos[0] + 5, cam_pos[1], 0 };
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir_central);
	}
	else
		glDisable(GL_LIGHT1);




	glRotatef(view[1], 1, 0, 0);
	glRotatef(view[0], 0, 1, 0);
	glRotatef(cam_pos[3], 0, 1, 0);
	gluLookAt(cam_pos[0], cam_pos[1], cam_pos[2], cam_pos[0] + EXTENSION,
		cam_pos[1], cam_pos[2], 0, 0, 1);

	initLight();
	GLfloat position[] = { 0,0,1,0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	if (autopilot_on)
		autoPilot();


	glPushMatrix();



	glShadeModel(GL_FLAT);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glBindTexture(GL_TEXTURE_2D, clouds);
	glutSolidCube(EXTENSION);
	glPopAttrib();


#pragma region Terrain_generation
	GLfloat v0[3];
	GLfloat v1[3];
	GLfloat v2[3];
	GLfloat v3[3];

	bool flag = false;
	int i;
	int j;


	for (i = 0; i < map_size; i++) {
		for (j = 0; j < map_size; j++) {

			glPushMatrix();

			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);

			memcpy(v0, matrix[i][j], sizeof(v2));
			memcpy(v1, matrix[i][j + 1], sizeof(v3));
			memcpy(v2, matrix[i + 1][j + 1], sizeof(v2));
			memcpy(v3, matrix[i + 1][j], sizeof(v3));


			float avg = (std::max)({ v0[2], v1[2], v2[2], v3[2] });

			if (avg < 0.15) {
				glBindTexture(GL_TEXTURE_2D, water);
			}
			else if (avg > 1.0) {
				glBindTexture(GL_TEXTURE_2D, mountain);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, trees);
			}


			quad(v0, v1, v2, v3);

			glPopMatrix();

		}
	}
#pragma endregion



	glPopMatrix();


	if (cabin_visible) {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-0.25, 0.25, -0.25, 0.25, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, cockpit);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		float planoS[] = { 1,0,0,0.5 };
		float planoT[] = { 0,1,0,-0.5 };
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGenfv(GL_S, GL_OBJECT_PLANE, planoS);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGenfv(GL_T, GL_OBJECT_PLANE, planoT);
		glDisable(GL_LIGHTING);

		glRotatef(-view[1] + 3, 1, 0, 0);
		glRotatef(-view[0], 0, 1, 0);
		glutSolidSphere(0.5, 20, 20);

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
	FPS();

	if (getTerrAlt() > cam_pos[2]) {
		MessageBox(NULL, L"You crashed!", L"Game over", MB_OK);
		exit(0);
	}
	else if (cam_pos[0] > EXTENSION / 2 || cam_pos[1] > EXTENSION / 2) {
		MessageBox(NULL, L"You left the game area!", L"Game over", MB_OK);
		exit(0);
	}

	initFog();


	glutSwapBuffers();
}


void reshape(GLint w, GLint h)
{
	float aspectRatio = float(w) / h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, aspectRatio, 0.1, 100);
}


void onIdle()
{
	static int before = glutGet(GLUT_ELAPSED_TIME);
	int now = glutGet(GLUT_ELAPSED_TIME);
	cam_pos[0] += sin((90 - cam_pos[3]) * 3.14159 / 180) * velocity;
	cam_pos[1] -= cos((90 - cam_pos[3]) * 3.14159 / 180) * velocity;
	before = now;

	glutPostRedisplay();

}

void onKey(unsigned char tecla, int x, int y) {

	switch (tolower(tecla)) {
	case 'a':
		if (velocity < 0.015f);
		velocity += 0.001f;
		break;
	case 'z':
		if (velocity > 0.005f)
			velocity -= 0.001f;
		break;
	case 'q':
		autopilot_alt = cam_pos[2] - getTerrAlt();
		autopilot_on = !autopilot_on;
		break;
	case 'l':
		if (light == 1)
			light = 0.2f;
		else
			light = 1;
		break;
	case 'c':
		cabin_visible = !cabin_visible;
		break;
	case 'f':
		directional_on = !directional_on;
		break;
	case 27:
		exit(0);
	}


	glutPostRedisplay();
}

void onArrow(int tecla, int x, int y) {
	switch (tecla) {
	case GLUT_KEY_LEFT:
		cam_pos[3] -= 1;
		break;
	case GLUT_KEY_RIGHT:
		cam_pos[3] += 1;
		break;
	case GLUT_KEY_DOWN:
		cam_pos[2] -= 0.1;
		break;
	case GLUT_KEY_UP:
		cam_pos[2] += 0.1;
		break;
	}
	glutPostRedisplay();
}



void onTimer(int tiempo)
{
	glutTimerFunc(tiempo, onTimer, tiempo);

	onIdle();
}


int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Symulator lotu");
	init();


	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKey);
	glutSpecialFunc(onArrow);
	glutMotionFunc(onDrag);
	glutMouseFunc(onClick);
	glutTimerFunc(1000 / tFPS, onTimer, 1000 / tFPS);
	glutMainLoop();
}