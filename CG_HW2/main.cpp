#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include <glew.h>
#include <glut.h>
#include <shader.h>
#include "glm/glm.h"
extern "C"
{
#include "glm_helper.h"
}

struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;

void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
GLuint loadTexture(char* name, GLfloat width, GLfloat height);

namespace
{
	char *obj_file_dir = "../Resources/bunny.obj";
	//char *obj_file_dir = "../Resources/Ball.obj";
	char *main_tex_dir = "../Resources/Stone.ppm";
	char *noise_tex_dir = "../Resources/Noise.ppm";
	char *ramp_tex_dir = "../Resources/Ramp.ppm";

	GLfloat light_rad = 0.05;//radius of the light bulb
	float eyet = 0.0;//theta in degree
	float eyep = 90.0;//phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;
	int mousex = 0;
	int mousey = 0;
}

const float speed = 0.003; // camera / light / ball moving speed
const float rotation_speed = 0.005; // ball rotating speed

GLuint mainTextureID, noiseTextureID, rampTextureID;

GLMmodel *model;

float eyex = 0.0;
float eyey = 0.0;
float eyez = 5.6;

GLfloat light_pos[] = { 1.1, 1.0, 1.3 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };

GLuint program, program_dis, program_ram;
GLuint vboName;

int num;
int modelSwitch = 0;
GLfloat ViewPosition[3] = { eyex, eyey, eyez };
GLfloat dissoveFactor = 0.0;
float R = 1, G = 1, B = 1;	// dissolving edge color
bool dissove = false;

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("CG_HW2_0756616");
	glutReshapeWindow(512, 512);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	printf("-----Phong Shading Model-----\n");	//first model hint word

	glutMainLoop();

	glmDelete(model);
	return 0;
}

void init(void)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);
	model = glmReadOBJ(obj_file_dir);

	mainTextureID = loadTexture(main_tex_dir, 512, 256);
	noiseTextureID = loadTexture(noise_tex_dir, 360, 360);
	rampTextureID = loadTexture(ramp_tex_dir, 256, 256);

	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(model);

	num = model->numtriangles;
	//宣告要使用的模型Mesh
	Vertex *obj = new Vertex[num * 3];

	//	position[3] normal[3] texcoord[2]
	int cn = 0;	// obj counter
	for (int j = 0;j < num;j++)	// triangle number
		for (int k = 0;k < 3;k++)	// 3 vertices
		{
			//position
			obj[cn].position[0] = model->vertices[model->triangles[j].vindices[k] * 3 + 0];
			obj[cn].position[1] = model->vertices[model->triangles[j].vindices[k] * 3 + 1];
			obj[cn].position[2] = model->vertices[model->triangles[j].vindices[k] * 3 + 2];

			//normal
			obj[cn].normal[0] = model->normals[model->triangles[j].nindices[k] * 3 + 0];
			obj[cn].normal[1] = model->normals[model->triangles[j].nindices[k] * 3 + 1];
			obj[cn].normal[2] = model->normals[model->triangles[j].nindices[k] * 3 + 2];

			//texcoord
			obj[cn].texcoord[0] = model->texcoords[model->triangles[j].tindices[k] * 2 + 0];
			obj[cn].texcoord[1] = model->texcoords[model->triangles[j].tindices[k] * 2 + 1];

			cn++;
		}

	program = createProgram(createShader("Shaders/bump.vert", "vertex"), createShader("Shaders/bump.frag", "fragment"));
	program_dis = createProgram(createShader("Shaders/bump_dis.vert", "vertex"), createShader("Shaders/bump_dis.frag", "fragment"));
	program_ram = createProgram(createShader("Shaders/bump_ram.vert", "vertex"), createShader("Shaders/bump_ram.frag", "fragment"));

	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num * 3 * 8, obj, GL_STATIC_DRAW);	//傳遞數值
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	//編號、數量、間隔(1=8)、位移
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* Shader program affect in this block. */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat P[16], MV[16], N[9], M[16];
	GLint loc;

	//please try not to modify the following block of code(you can but you are not supposed to)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez,
		eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180),
		eyey + sin(eyet*M_PI / 180),
		eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
		0.0, 1.0, 0.0);
	draw_light_bulb();
	glPushMatrix();
		glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
		glRotatef(ball_rot[0], 1, 0, 0);
		glRotatef(ball_rot[1], 0, 1, 0);
		glRotatef(ball_rot[2], 0, 0, 1);
		//glmDraw(model, GLM_TEXTURE);
		glGetFloatv(GL_MODELVIEW_MATRIX, MV);
	glPopMatrix();
	// please try not to modify the previous block of code

	glGetFloatv(GL_PROJECTION_MATRIX, P);
	//Model Matrix
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
		glRotatef(ball_rot[0], 1, 0, 0);
		glRotatef(ball_rot[1], 0, 1, 0);
		glRotatef(ball_rot[2], 0, 0, 1);
		glGetFloatv(GL_MODELVIEW_MATRIX, M);
	glPopMatrix();

	GLfloat mat_amb[3], mat_dif[3], mat_spe[3], lig_amb[3], lig_dif[3], lig_spe[3];

	glEnable(GL_TEXTURE_2D);

	switch (modelSwitch % 3)
	{
	case 0:	//Phong Shading
	{
		glUseProgram(program);

		loc = glGetUniformLocation(program, "P");
		glUniformMatrix4fv(loc, 1, GL_FALSE, P);
		loc = glGetUniformLocation(program, "MV");
		glUniformMatrix4fv(loc, 1, GL_FALSE, MV);
		loc = glGetUniformLocation(program, "M");
		glUniformMatrix4fv(loc, 1, GL_FALSE, M);

		for (int i = 0;i < 3;i++)
		{
			mat_amb[i] = 1.0f;
			mat_dif[i] = 1.0f;
			mat_spe[i] = 1.0f;
			lig_amb[i] = 0.2f;
			lig_dif[i] = 0.8f;
			lig_spe[i] = 0.5f;
		}

		//Material
		loc = glGetUniformLocation(program, "mat_amb");
		glUniform3fv(loc, 1, mat_amb);
		loc = glGetUniformLocation(program, "mat_dif");
		glUniform3fv(loc, 1, mat_dif);
		loc = glGetUniformLocation(program, "mat_spe");
		glUniform3fv(loc, 1, mat_spe);
		loc = glGetUniformLocation(program, "mat_shi");
		glUniform1f(loc, 100);

		//Light
		loc = glGetUniformLocation(program, "lig_pos");
		glUniform3fv(loc, 1, light_pos);
		loc = glGetUniformLocation(program, "lig_amb");
		glUniform3fv(loc, 1, lig_amb);
		loc = glGetUniformLocation(program, "lig_dif");
		glUniform3fv(loc, 1, lig_dif);
		loc = glGetUniformLocation(program, "lig_spe");
		glUniform3fv(loc, 1, lig_spe);

		loc = glGetUniformLocation(program, "ViewPosition");
		glUniform3fv(loc, 1, ViewPosition);

		//Texture
		loc = glGetUniformLocation(program, "Tex2D");
		glActiveTexture(GL_TEXTURE0 + 0); //GL_TEXTUREi = GL_TEXTURE0 + i
		glBindTexture(GL_TEXTURE_2D, mainTextureID);
		glUniform1i(loc, 0);

		break;
	}
	case 1:	//Dissolving
	{
		glUseProgram(program_dis);

		loc = glGetUniformLocation(program_dis, "P");
		glUniformMatrix4fv(loc, 1, GL_FALSE, P);
		loc = glGetUniformLocation(program_dis, "MV");
		glUniformMatrix4fv(loc, 1, GL_FALSE, MV);

		//Texture
		loc = glGetUniformLocation(program_dis, "Tex2D");
		glActiveTexture(GL_TEXTURE0 + 0); //GL_TEXTUREi = GL_TEXTURE0 + i
		glBindTexture(GL_TEXTURE_2D, mainTextureID);
		glUniform1i(loc, 0);

		loc = glGetUniformLocation(program_dis, "Tex2D_noise");
		glActiveTexture(GL_TEXTURE0 + 1); //GL_TEXTUREi = GL_TEXTURE0 + i
		glBindTexture(GL_TEXTURE_2D, noiseTextureID);
		glUniform1i(loc, 1);

		loc = glGetUniformLocation(program_dis, "dissolveFactor");
		glUniform1f(loc, dissoveFactor);

		if (dissoveFactor > 1)
			dissove = true;
		else if (dissoveFactor < 0)
			dissove = false;

		if (dissove == true)
			dissoveFactor = dissoveFactor - 0.0003;
		else
			dissoveFactor = dissoveFactor + 0.0003;

		//color
		loc = glGetUniformLocation(program_dis, "R");
		glUniform1f(loc, R);
		loc = glGetUniformLocation(program_dis, "G");
		glUniform1f(loc, G);
		loc = glGetUniformLocation(program_dis, "B");
		glUniform1f(loc, B);


		break;
	}
	case 2:	//Ramp
	{
		glUseProgram(program_ram);

		loc = glGetUniformLocation(program_ram, "P");
		glUniformMatrix4fv(loc, 1, GL_FALSE, P);
		loc = glGetUniformLocation(program_ram, "MV");
		glUniformMatrix4fv(loc, 1, GL_FALSE, MV);
		loc = glGetUniformLocation(program_ram, "M");
		glUniformMatrix4fv(loc, 1, GL_FALSE, M);

		for (int i = 0;i < 3;i++)
		{
			mat_dif[i] = 1.0f;
			lig_dif[i] = 0.8f;
		}

		//Material
		loc = glGetUniformLocation(program_ram, "mat_dif");
		glUniform3fv(loc, 1, mat_dif);

		//Light
		loc = glGetUniformLocation(program_ram, "lig_pos");
		glUniform3fv(loc, 1, light_pos);
		loc = glGetUniformLocation(program_ram, "lig_dif");
		glUniform3fv(loc, 1, lig_dif);

		//Texture
		loc = glGetUniformLocation(program_ram, "Tex2D");
		glActiveTexture(GL_TEXTURE0 + 0); //GL_TEXTUREi = GL_TEXTURE0 + i
		glBindTexture(GL_TEXTURE_2D, mainTextureID);
		glUniform1i(loc, 0);

		loc = glGetUniformLocation(program_ram, "Tex2D_ramp");
		glActiveTexture(GL_TEXTURE0 + 2); //GL_TEXTUREi = GL_TEXTURE0 + i
		glBindTexture(GL_TEXTURE_2D, rampTextureID);
		glUniform1i(loc, 2);

		break;
	}
	}
	
	glDrawArrays(GL_TRIANGLES, 0, num * 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	
	for (int i = 0;i < 3;i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, i);
	}

	glDisable(GL_TEXTURE_2D);
	glUseProgram(NULL);

	glutSwapBuffers();
	camera_light_ball_move();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	//ESC
		break;
	}
	case 'b'://toggle mode
	{
		//you may need to do somting here
		modelSwitch++;
		switch (modelSwitch % 3)
		{
		case 0:	//Phong Shading
		{
			printf("-----Phong Shading Model-----\n");
			break;
		}
		case 1:	//Dissolving
		{
			printf("-----Dissolving Model-----\n");
			break;
		}
		case 2:	//Ramp
		{
			printf("-----Ramp Model-----\n");
			break;
		}
		}
		break;
	}
	// move cameara
	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	// move light
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	// move bunny 
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	// move bunny
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos[0] = 1.1;
		light_pos[1] = 1.0;
		light_pos[2] = 1.3;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0.0;
		eyey = 0.0;
		eyez = 5.6;
		eyet = 0;
		eyep = 90;
		break;
	}

	//change dissolving color
	case 'n':
	{
		if (R < 0)
			R++;
		R -= 0.1;
		break;
	}
	case 'm':
	{
		if (G < 0)
			G++;
		G -= 0.1;
		break;
	}
	case ',':
	{
		if (B < 0)
			B++;
		B -= 0.1;
		break;
	}
	default:
	{
		break;
	}
	}
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	if (mleft)
	{
		eyep -= (x - mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz = 0;
	if (left || right || forward || backward || up || down)
	{
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy * cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx * sin(eyep*M_PI / 180);
		eyey += dy * sin(eyet*M_PI / 180);
		eyez += dy * (-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx * cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if (lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy * cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx * sin(eyep*M_PI / 180);
		light_pos[1] += dy * sin(eyet*M_PI / 180);
		light_pos[2] += dy * (-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx * cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if (ldown)
			light_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy * cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx * sin(eyep*M_PI / 180);
		ball_pos[1] += dy * sin(eyet*M_PI / 180);
		ball_pos[2] += dy * (-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx * cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if (bx || by || bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void draw_light_bulb()
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
	glColor3f(0.4, 0.5, 0);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	gluSphere(quad, light_rad, 40, 20);
	glPopMatrix();
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	{
		right = false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}

void idle(void)
{
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}
