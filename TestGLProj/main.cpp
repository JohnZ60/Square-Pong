#include <GL/glew.h>
#include <GL/freeglut.h>

//glm library
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "Shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Shader shader; // loads our vertex and fragment shaders
Model *cylinder; //a cylinder 
Model *plane; //a plane
Model* p1Model; //player 1 model
Model* p2Model;
Model* ball;
Model* ball2;
Model* testp1;
Model* testp2;
glm::mat4 projection; // projection matrix
glm::mat4 view; // where the camera is looking
glm::mat4 model; // where the model (i.e., the myModel) is located wrt the camera
glm::mat4 p1;
glm::mat4 p2;
float p1Position = 0.0f;
float p2Position = 0.0f;
float ballrotate = 0.0f;
float ballvertical = 0.0f;
float ballhorizontal = 0.0f;
const float yfactor = 5.2f;
const float xfactor = 2.0f;
float spinangle = 0.0f;
glm::mat4 movement = glm::mat4();
bool right = true;
bool down = false;
bool rightspin = false;
bool leftspin = false;
bool keyb[256];

bool firstmotion = true;
bool fps = false;
/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
	GLenum error;
	while (( error = glGetError() ) != GL_NO_ERROR) {
		std::cerr << "GL error " << error << " detected in " << functionName << std::endl;
	}
}

void initShader(void)
{
	shader.InitializeFromFile("shaders/phong.vert", "shaders/phong.frag");
	shader.AddAttribute("vertexPosition");
	shader.AddAttribute("vertexNormal");

	checkError ("initShader");
}

void initRendering(void)
{
	glClearColor (0.117f, 0.565f, 1.0f, 0.0f); // Dodger Blue
	checkError ("initRendering");
}

void init(void) 
{	
	// Perspective projection matrix.
	projection = glm::perspective(45.0f, 800.0f/600.0f, 1.0f, 1000.0f);

	
	// Load identity matrix into model matrix (no initial translation or rotation)
	

	initShader ();
	initRendering ();
}

/* This prints in the console when you start the program*/
void dumpInfo(void)
{
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
	checkError ("dumpInfo");
}

/*This gets called when the OpenGL is asked to display. This is where all the main rendering calls go*/
void display(void)
{

	if (keyb['d']) {
		if (p1Position <= 7.0f)
			p1Position+=.03f;
	}
	if (keyb['a']) {
		if (p1Position >= -7.0f)
			p1Position-=.03f;
	}
	if (keyb['j']) {
		if (p2Position >= -7.0f)
			p2Position-=.03f;
	}

	if (keyb['l']) {
		if (p2Position <= 7.0f)
			p2Position+=.03f;
	}


	
	if (right) {
		ballhorizontal += .01f;
		if (ballhorizontal >= 7.0f) {
			right = false;
		}
	}
	else{
		ballhorizontal -= .01f;
		if (ballhorizontal <= -7.0f) {
			right = true;
		}
	}



	if (down) {
		ballvertical -= .008f;
	}
	else {
		ballvertical += .008f;
	}

	if (rightspin) {
		ballrotate += 20.0f;
	}

	if (leftspin) {
		ballrotate -= 20.0f;
	}


	//glm::rot
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	p1 = glm::lookAt(glm::vec3(0.0f, -6.0f, 20.0f), glm::vec3(0.0f, -6.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.f)); 
	p2 = glm::lookAt(glm::vec3(0.0f, 6.0f, 20.0f), glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.f));

	//model =  glm::rotate(angle+=.1, 0.0f,0.0f, 1.0f);

	//check collision FOR PLAYER 1 ONLY
		// player bounding line
	glm::vec2 player1LeftCorner(p1Position - xfactor, yfactor);
	glm::vec2 player1RightCorner(p1Position + xfactor, yfactor);

	//ball bounding box
	glm::vec2 ballBottomLeft(ballhorizontal - 0.5f, ballvertical - 0.5f);
	glm::vec2 ballBottomRight(ballhorizontal + 0.5f, ballvertical - 0.5f);
	glm::vec2 ballTopLeft(ballhorizontal - 0.5f, ballvertical + 0.5f);
	glm::vec2 ballTopRight(ballhorizontal + 0.5f, ballvertical + 0.5f);

	if ((ballTopRight.x > player1LeftCorner.x && ballTopRight.x < player1RightCorner.x) && (ballTopLeft.x > player1LeftCorner.x && ballTopLeft.x < player1RightCorner.x)) {
		if ((ballTopLeft.y >= player1LeftCorner.y || ballTopRight.y >= player1LeftCorner.y) && (ballBottomLeft.y <= player1LeftCorner.y || ballBottomRight.y <= player1LeftCorner.y)) {
			down = true;
		}
	}
	else if (ballTopRight.x > player1LeftCorner.x && ballTopRight.x < player1RightCorner.x) {
		if ((ballTopLeft.y >= player1LeftCorner.y || ballTopRight.y >= player1LeftCorner.y) && (ballBottomLeft.y <= player1LeftCorner.y || ballBottomRight.y <= player1LeftCorner.y)) {
			down = true;
			leftspin = false;
			rightspin = true;
		}
	}
	else if ((ballTopLeft.x > player1LeftCorner.x && ballTopLeft.x < player1RightCorner.x)) {
		if ((ballTopLeft.y >= player1LeftCorner.y || ballTopRight.y >= player1LeftCorner.y) && (ballBottomLeft.y <= player1LeftCorner.y || ballBottomRight.y <= player1LeftCorner.y)) {
			down = true;
			rightspin = false;
			leftspin = true;
		}
	}



	//check collision FOR PLAYER 2 ONLY
	glm::vec2 player2LeftCorner(p2Position - xfactor, -yfactor);
	glm::vec2 player2RightCorner(p2Position + xfactor, -yfactor);

	if ((ballBottomRight.x > player2LeftCorner.x && ballBottomRight.x < player2RightCorner.x) && (ballBottomLeft.x > player2LeftCorner.x && ballBottomLeft.x < player2RightCorner.x)) {
		if ((ballBottomLeft.y <= player2LeftCorner.y || ballBottomRight.y <= player2LeftCorner.y) && (ballTopRight.y >= player2LeftCorner.y || ballTopLeft.y >= player2LeftCorner.y)) {
			down = false;
		}
	}
	else if (ballBottomRight.x > player2LeftCorner.x && ballBottomRight.x < player2RightCorner.x) {
		if ((ballBottomLeft.y <= player2LeftCorner.y || ballBottomRight.y <= player2LeftCorner.y) && (ballTopRight.y >= player2LeftCorner.y || ballTopLeft.y >= player2LeftCorner.y)) {
			down = false;
			rightspin = false;
			leftspin = true;
		}
	}
	else if (ballBottomLeft.x > player2LeftCorner.x && ballBottomLeft.x < player2RightCorner.x) {
		if ((ballBottomLeft.y <= player2LeftCorner.y || ballBottomRight.y <= player2LeftCorner.y) && (ballTopRight.y >= player2LeftCorner.y || ballTopLeft.y >= player2LeftCorner.y)) {
			down = false;
			leftspin = false;
			rightspin = true;
		}
	}


	
	// Render current active model.


	
	//checkCollisionPlayer1();
	

	//plane->render(view * glm::rotate(90.0f, 1.0f, 0.0f, 0.0f)* glm::translate(0.0f, 0.0f, 0.0f)*glm::scale(8.0f,1.0f,8.0f), projection);

	ball->render(view* glm::translate(ballhorizontal, ballvertical, 0.0f) * glm::rotate(90.0f, 1.0f, 0.0f, 0.0f)*glm::rotate(ballrotate, 0.0f, 1.0f, 0.0f)*glm::scale(0.4f, 0.4f, 0.4f), projection);
	p1Model->render(p1*glm::translate(p1Position, 0.0f, 0.0f) *glm::rotate(90.0f, 1.0f, 0.0f, 0.0f) * glm::scale(2.0f, 0.8f, 0.8f), projection);
	p2Model->render(p2 * glm::rotate(90.0f, 1.0f, 0.0f, 0.0f) * glm::translate(p2Position, 0.0f, 0.0f) * glm::scale(2.0f, 0.8f, 0.8f), projection);


	
	glutSwapBuffers(); // Swap the buffers.
	checkError ("display");
}

//check for collision between ball and players
void checkCollisionPlayer1() {
	// player bounding line
	glm::vec2 player1LeftCorner(p1Position - xfactor, yfactor);
	glm::vec2 player1RightCorner(p1Position + xfactor, yfactor);

	//ball bounding box
	glm::vec2 ballBottomLeft(ballhorizontal - 0.5f, ballvertical - 0.5f);
	glm::vec2 ballBottomRight(ballhorizontal + 0.5f, ballvertical - 0.5f);
	glm::vec2 ballTopLeft(ballhorizontal - 0.5f, ballvertical + 0.5f);
	glm::vec2 ballTopRight(ballhorizontal + 0.5f, ballvertical + 0.5f);

	if ((ballTopRight.x > player1LeftCorner.x && ballTopRight.x < player1RightCorner.x) && (ballTopLeft.x > player1LeftCorner.x && ballTopLeft.x < player1RightCorner.x)) {
		if ((ballTopLeft.y >= player1LeftCorner.y || ballTopRight.y >= player1LeftCorner.y) && (ballBottomLeft.y <= player1LeftCorner.y || ballBottomRight.y <= player1LeftCorner.y)) {
			down = true;
		}
	}
	else if (ballTopRight.x > player1LeftCorner.x && ballTopRight.x < player1RightCorner.x) {
		if ((ballTopLeft.y >= player1LeftCorner.y || ballTopRight.y >= player1LeftCorner.y) && (ballBottomLeft.y <= player1LeftCorner.y || ballBottomRight.y <= player1LeftCorner.y)) {
			down = true;
			leftspin = false;
			rightspin = true;
		}
	}
	else if ((ballTopLeft.x > player1LeftCorner.x && ballTopLeft.x < player1RightCorner.x)) {
		if ((ballTopLeft.y >= player1LeftCorner.y || ballTopRight.y >= player1LeftCorner.y) && (ballBottomLeft.y <= player1LeftCorner.y || ballBottomRight.y <= player1LeftCorner.y)) {
			down = true;
			rightspin = false;
			leftspin = true;
		}
	}


}

void checkCollisionPlayer2() {

}


/*This gets called when nothing is happening (OFTEN)*/
void idle()
{
	glutPostRedisplay(); // create a display event. Display calls as fast as CPU will allow when put in the idle function
}

/*Called when the window is resized*/
void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	checkError ("reshape");
}

/*Called when a normal key is pressed*/
void keyboard(unsigned char key, int x, int y)
{
	/*
	switch (key) {
	case 27: // this is an ascii value
		exit(0);
		break;
	case 'd':
		if (p1Position <= 7.0f)
		p1Position++;
		break;
	case 'a':
		if (p1Position >= -7.0f)
			p1Position--;
		break;

	case 'j':
		if (p2Position >= -7.0f)
			p2Position--;
		break;
	case 'l':
		if (p2Position <= 7.0f)
			p2Position++;
		break;
	}

	*/

	keyb[key] = true;


}

void keyRelease(unsigned char key, int x, int y) {
	keyb[key] = false;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE| GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	glewInit();
	dumpInfo ();
	init ();
	glutDisplayFunc(display); 
	glutIdleFunc(idle); 
	glutReshapeFunc(reshape);
	glutIgnoreKeyRepeat(0);
	glutKeyboardFunc (keyboard);
	glutKeyboardUpFunc(keyRelease);
	glEnable(GL_DEPTH_TEST);


	

	plane = new Model(&shader, "models/plane.obj");

	p1Model = new Model(&shader, "models/plane.obj");
	p2Model = new Model(&shader, "models/plane.obj");
	testp1 = new Model(&shader, "models/plane.obj");
	testp2 = new Model(&shader, "models/plane.obj");

	ball = new Model(&shader, "models/plane.obj");
	ball2 = new Model(&shader, "models/plane.obj");

	glutMainLoop();

	return 0;
}