#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Shader.h"
#include "Texture2D.h"
#include "SpriteRenderer.h"
#include <iostream>
class PostProcessing
{
public:
	PostProcessing(Shader* _shader, GLint _width, GLint _height);
	~PostProcessing();

	Shader* postProcessingShader;
	Texture2D texture;
	GLint width;
	GLint height;

	void Render(GLfloat deltaTime);

	void BeginRender();

	void EndRender();

	GLboolean Shake;
	GLboolean Blur;
private:

	void Init();

	GLuint MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
	GLuint RBO; // RBO is used for multisampled color buffer
	GLuint VAO;
};

