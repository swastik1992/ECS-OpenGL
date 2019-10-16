#pragma once

#include <string>
#include <map>
#include <vector>

#include <GL/glew.h>

#include "Texture2D.h"
#include "Shader.h"



class AssetLoader
{
public:
	// Resource storage
	static std::map<std::string, Shader*>    Shaders;
	static std::map<std::string, Texture2D*> Textures;
	static std::map<std::string, std::vector<std::vector<GLuint>>> Levels;

	// Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
	static Shader *   LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, const std::string & name);
	// Retrieves a stored sader
	static Shader *   GetShader(const std::string & name);
	// Loads (and generates) a texture from file
	static Texture2D * LoadTexture(const GLchar *file, GLboolean alpha, const std::string & name);
	// Retrieves a stored texture
	static Texture2D * GetTexture(const std::string & name);
	//Get Map.
	static std::vector<std::vector<GLuint>> LoadLevel(const GLchar *file, GLfloat width, GLfloat height, const std::string& name);
	static std::vector<std::vector<GLuint>> GetLevel(const std::string & name);

	// Properly de-allocates all loaded resources
	static void        Clear();
private:
	// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
	AssetLoader() { }
	// Loads and generates a shader from file
	static Shader *    loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
	// Loads a single texture from file
	static Texture2D * loadTextureFromFile(const GLchar *file, GLboolean alpha);
};

