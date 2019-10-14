
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL\glew.h>
#include <GL\freeglut.h>

#define WINDOW_TITLE_PREFIX "Something"

int
CurrentWidth = 800,
CurrentHeight = 600,
WindowHandle = 0;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);

int main(int argc, char* argv[])
{
	Initialize(argc, argv);

	glutMainLoop();

	//exit(EXIT_SUCCESS);
	return 0;
}

void Initialize(int argc, char* argv[])
{
	InitWindow(argc, argv);

	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void InitWindow(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);

	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

	if (WindowHandle < 1) {
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
		);
		exit(EXIT_FAILURE);
	}

	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
}

void ResizeFunction(int Width, int Height)
{
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutSwapBuffers();
	glutPostRedisplay();
}
//#inclide "GL\"
//
//
//int main(int argc, char* argv[])
//{
//	SDL_Init(SDL_INIT_VIDEO);
//
//	SDL_Window* window = SDL_CreateWindow
//	("An SDL2 window", // window's title
//		10, 25, // coordinates on the screen, in pixels, of the window's upper left corner
//		640, 480, // window's length and height in pixels  
//		SDL_WINDOW_OPENGL);
//
//	SDL_Delay(3000); // window lasts 3 seconds
//	SDL_DestroyWindow(window);
//	SDL_Quit();
//	return 0;
//}