#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture2D.h"

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

enum CollisionShape
{
	Box,
	Circle
};

enum GameState
{
	Menu = 1,
	InGame = 0,
	EndGame = 2
};

enum InputKeyAction
{
	Up,
	Down
};


//typedef std::tuple<GLboolean, Direction, glm::vec2> CollisionParam; // <collision?, what direction?, difference vector center - closest point>