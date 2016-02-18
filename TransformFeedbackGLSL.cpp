//============================================================================
// Name        : GLSLTransformFeedback.cpp
// Author      : Oleg Pavlyniuk
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <stdlib.h>
#include <time.h>

using namespace std;

#include <GL/glew.h>
#include <GL/glxew.h>

#include "/usr/include/GL/glx.h"
#include <GL/freeglut.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#define BUFFER_OFFSET(offset) ((void*)(offset))
#define MEMBER_OFFSET(s,m) ((char*)NULL + (offsetof(s,m)))

#define NUM_PARTICLE 100000

int wWidth = 1980;
int wHeight = 1020;
int wHandle = 0;

float angle = 0.03f;

glm::ivec2 mousePosition;
glm::quat qRotation;

glm::vec3 initCameraPosition;
glm::quat qInitCameraRotation;

glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelViewMatrix;
glm::mat4 projectionMatrix;
glm::mat4 modelViewProjectionMatrix;

glm::mat4 MouseRotation;
glm::mat4 MouseScale;
float xangle(0.0);
float yangle(0.0);
float temp_xangle(0.0);
float temp_yangle(0.0);
int xpos;
int ypos;
bool press_button;


struct ModelVertex
{
	glm::vec3 mvPos;
	glm::vec3 mvVel;
	glm::vec3 mvColor;
};

ModelVertex aVertices[NUM_PARTICLE];
glm::vec3 vertexArray[NUM_PARTICLE];
glm::vec3 velocityArray[NUM_PARTICLE];

int counter = 0;
glm::vec3 attractor;

//ModelVertex aVertices[8] = {
//		{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)},
//		{glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f)},
//		{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
//		{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f)},
//		{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
//		{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f)},
//		{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
//		{glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 0.0f)},
//};

GLuint aIndices[36] = {
		0, 1, 2, 2, 3, 0,
		7, 4, 5, 5, 6, 7,
		6, 5, 2, 2, 1, 6,
		7, 0, 3, 3, 4, 7,
		7, 6, 1, 1, 0, 7,
		3, 2, 5, 5, 4, 3
};

GLuint vaoCube[2];
GLuint vertexBuf[2];
GLuint velocityBuf[2];

GLuint shaderProgram = 0;
GLint uniformMVP = -1;
GLint uniformAttractor = -1;

void IdleGL()
{
	counter = (counter + 1) % 2;
	angle = angle + 0.001f;
	if (angle > 360.0f)
	{
		angle = 0.0f;
	}

	attractor.x = 1.5f * sinf(angle);
	attractor.z = 0.0f;
	attractor.y = 1.5f * cosf(angle);

	glm::mat4 identity;

	modelViewProjectionMatrix =  projectionMatrix * viewMatrix * MouseRotation * MouseScale * modelMatrix;
	glutPostRedisplay();
}

void DisplayGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
	glUniform3fv(uniformAttractor, 1, glm::value_ptr(attractor));

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vertexBuf[(counter + 1) % 2]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velocityBuf[(counter + 1) % 2]);

	glBindVertexArray(vaoCube[counter]);
	glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, NUM_PARTICLE);
	glBindVertexArray(0);
	glEndTransformFeedback();

	glUseProgram(0);

	glutSwapBuffers();
}

void ReshapeGL(int w, int h)
{
	if (h == 0)
	{
		h = 1.0f;
	}

	wWidth = w;
	wHeight = h;

	glViewport(0, 0, w, h);

	float ratio = (float) wWidth / wHeight;
	float k = 0.055f;
	float left = -k * ratio;
	float right = k * ratio;
	float bottom = -k;
	float top = k;
	float near = 0.1f;
	float far = 100.0f;

	projectionMatrix = glm::frustum(left, right, bottom, top, near, far);
	modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
//	modelViewProjectionMatrix = modelViewMatrix * projectionMatrix;

	glutPostRedisplay();
}



void init()
{
//	glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(NULL));

	for (unsigned int i = 0; i < NUM_PARTICLE; i++)
	{
		float a1 = ((float) rand() / (float) RAND_MAX) * 3.0f - 1.5f;
		float a2 = ((float) rand() / (float) RAND_MAX) * 3.0f - 1.5f;
		float a3 = ((float) rand() / (float) RAND_MAX) * 3.0f - 1.5f;

//		float b1 = ((float) rand() / (float) RAND_MAX) * 2.0f - 1.0f;
//		float b2 = ((float) rand() / (float) RAND_MAX) * 2.0f - 1.0f;
//		float b3 = ((float) rand() / (float) RAND_MAX) * 2.0f - 1.0f;
//
//		float c1 = ((float) rand() / (float) RAND_MAX) * 2.0f - 1.0f;
//		float c2 = ((float) rand() / (float) RAND_MAX) * 2.0f - 1.0f;
//		float c3 = ((float) rand() / (float) RAND_MAX) * 2.0f - 1.0f;

		float red = ((float) rand() / (float) RAND_MAX);
		float green = ((float) rand() / (float) RAND_MAX);
		float blue = ((float) rand() / (float) RAND_MAX);

//		aVertices[i].mvPos = glm::vec3((a1 + a2 + a3) / 3.0f, (b1 + b2 + b3) / 3.0f, (c1 + c2 + c3) / 3.0f);
//		aVertices[i].mvPos = glm::vec3((a1 + a2 + a3) / 3.0f, b1, (c1 + c2 + c3) / 3.0f);
		aVertices[i].mvVel = glm::vec3(0.0f);
		aVertices[i].mvColor = glm::vec3(red, green, blue);

		float r = ((float) rand() / (float) RAND_MAX) * 12.0f;
		float alpha = ((float) rand() / (float) RAND_MAX) * 360.0f;
		float z = 4.5f*((float) rand() / (float) RAND_MAX)-3.0f;

		vertexArray[i] = glm::vec3(a1, a2, a3);
		//vertexArray[i] = glm::vec3(r * sinf(alpha), r * cosf(alpha), z);

		//velocityArray[i] = glm::vec3(0.0f);
		//velocityArray[i] = 0.05f * glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(vertexArray[i]));

//		aVertices[i].mvColor = glm::vec3(1.0f, 1.0f, 1.0f);
	}

}



void Mouse(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON)&&(state == GLUT_DOWN)&&(press_button == false))
	{
		press_button = true;
		xpos = x;
		ypos = y;
		temp_xangle = xangle;
		temp_yangle = yangle;
		return;
	}
	if ((button == GLUT_LEFT_BUTTON)&&(state == GLUT_UP)&&(press_button == true))
	{
		press_button = false;
		xangle = temp_xangle;
		yangle = temp_yangle;
		return;
	}

	if (button == 4)
	{
	    if (state == GLUT_UP) return;
    }
	else
	{
	    MouseScale = glm::scale(MouseScale, glm::vec3(1.01, 1.01, 1.01));
	}
	 if (button == 3)
	{
	    if (state == GLUT_UP) return;
	}
	 else
	 {
	     MouseScale = glm::scale(MouseScale, glm::vec3(0.85, 0.85, 0.85));
	 }
}

void MouseFunc(int x, int y)
{
	temp_xangle = xangle + 0.002f * (float)(- xpos + x);
	temp_yangle = yangle + 0.002f * (float)(ypos - y);
	MouseRotation = glm::rotate(glm::mat4(1.0f),  temp_yangle, glm::vec3(-1, 0, 0)) * glm::rotate(glm::mat4(1.0f),  temp_xangle, glm::vec3(0, 1, 0));
}

void InitGL(int argc, char* argv[])
{
	std::cout << "Initialize OpenGL..." << std::endl;

	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);

	//Create an OpenGL 3.3 context
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(wWidth, wHeight);
	wHandle = glutCreateWindow("Rossler attractor");

	init();

	glutIdleFunc(IdleGL);
	glutReshapeFunc(ReshapeGL);
	glutDisplayFunc(DisplayGL);
	glutMotionFunc(MouseFunc);
	glutMouseFunc(Mouse);
	press_button = false;


	std::cout << "Initialize OpenGL Success!" << std::endl;
}

void InitGLEW()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "There was a problem initializing GLEW. Exiting..." << std::endl;
		exit(-1);
	}
//	glSwapIntervalMESA(0);
}

GLuint LoadShader(GLenum shaderType, const char* shaderFile)
{
	std::ifstream ifs;
	ifs.open(shaderFile, std::ifstream::in);

	if (!ifs)
	{
		std::cerr << "Cannot open shader file: \"" << shaderFile << "\"" << std::endl;
		return 0;
	}

	std::string source(std::istreambuf_iterator<char>(ifs), (std::istreambuf_iterator<char>()));
	ifs.close();

	GLuint shader = glCreateShader(shaderType);

	const GLchar* sources[] = {source.c_str()};
	glShaderSource(shader, 1, sources, NULL);
	glCompileShader(shader);
//	std::cout << shader << std::endl;

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus != GL_TRUE)
	{
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* infoLog = new GLchar[logLength];
		glGetShaderInfoLog(shader, logLength, NULL, infoLog);

		std::cerr << infoLog << std::endl;
		delete infoLog;
		return 0;
	}

	return shader;
}

GLuint CreateShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

	if (linkStatus != GL_TRUE)
	{
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* infoLog = new GLchar[logLength];
		glGetProgramInfoLog(program, logLength, NULL, infoLog);

		std::cerr << infoLog << std::endl;

		delete infoLog;
		return 0;
	}

	return program;
}

int main(int argc, char** argv) {

	initCameraPosition = glm::vec3(0.6f, 4.4f, 4.0f);
	viewMatrix = glm::lookAt(initCameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));

	modelViewMatrix = viewMatrix * modelMatrix;

	InitGL(argc, argv);
	InitGLEW();

	std::cout << "FLOAt_MAX = " << numeric_limits<float>::max() << std::endl;

	const char* strVersion = (const char*) glGetString(GL_VERSION);
	std::cout << strVersion << std::endl;

	GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, "shaderAttractor.vert");
	GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, "shader.frag");

	shaderProgram = CreateShaderProgram(vertexShader, fragmentShader);
	assert(shaderProgram != 0);

	glUseProgram(shaderProgram);
	const GLchar* varyings[] = {"location", "velocity"};
	glTransformFeedbackVaryings(shaderProgram, 2, varyings, GL_SEPARATE_ATTRIBS);
	glLinkProgram(shaderProgram);
	glUseProgram(0);

	GLint positionAttribID = glGetAttribLocation(shaderProgram, "in_position");
	GLint velocityAttribID = glGetAttribLocation(shaderProgram, "in_velocity");
	//GLint colorAttribID = glGetAttribLocation(shaderProgram, "in_color");
	uniformMVP = glGetUniformLocation(shaderProgram, "MVP");
	uniformAttractor = glGetUniformLocation(shaderProgram, "attractor");

	std::cout << "uniformMVP = " << uniformMVP << std::endl;
	std::cout << "uniformAttractor = " << uniformAttractor << std::endl;
	std::cout << "positionAtribID = " << positionAttribID << std::endl;
	std::cout << "velocityAttribID = " << velocityAttribID << std::endl;
//	std::cout << "colorAtribID = " << colorAttribID << std::endl;

	for (int i = 0; i < 2; i++)
	{
		glGenVertexArrays(1, &vaoCube[i]);
		glBindVertexArray(vaoCube[i]);

		glGenBuffers(1, &vertexBuf[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuf[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3), (void*) NULL);
		glEnableVertexAttribArray(positionAttribID);

		glGenBuffers(1, &velocityBuf[i]);
		glBindBuffer(GL_ARRAY_BUFFER, velocityBuf[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(velocityArray), velocityArray, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(glm::vec3), (void*) NULL);
		glEnableVertexAttribArray(velocityAttribID);

		glBindVertexArray(0);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(positionAttribID);
	glDisableVertexAttribArray(velocityAttribID);

	glutMainLoop();

	return 0;
}
