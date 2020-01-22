#pragma once
#include "World.h"
#include "SpriteRenderer.h"
#include "Utils.h"
#include "PostProcessing.h"
#include "TextRenderer.h"
#include "GameEvents.h"
#include "irrKlang.h"

const glm::vec2 PLAYER_SIZE(140, 40);
const GLfloat PLAYER_VELOCITY(500.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -250.0f);
const GLfloat BALL_RADIUS = 6.5f;

//Breakout World class to manage game states, updates every entities and components. 
//This class is the first enry point from OpenGL Game loop and contains the handle to most core functionalities eg: sprite renderer, sound etc.
//Alo manages level load/unload. 

class BreakoutWorld :
	public World
{
public:
	BreakoutWorld(GLint _height, GLint _width);
	~BreakoutWorld();

	virtual void Init();

	virtual void Update(float dt);

	virtual void Render();

	void ProcessInputKey(InputKeyAction action, int key);

	SpriteRenderer* GetSpriteRenderer() { return spriteRenderer; }
	PostProcessing* GetPostProcessing() { return postProcessing; }
	TextRenderer* GetTextRenderer() { return textRenderer; }
	irrklang::ISoundEngine* GetSoundEngine() { return soundEngine; }

	void OnPlayerDied(PlayerDiedEvent* event);
	void OnLevelEnded(LevelEnd* event);

	PlayerDiedEvent evet;

	GameState gameState;
	GLboolean keys[1024];
	GLboolean keysProcessed[1024];

	GLint height;
	GLint width;

protected:

	GLint lives;

	void LoadAssets();
	void LoadLevels(std::string levelName);
	void UnLoadLevel();
	void ResetGame();
	void PrepareNonGame();

	void ResetBall();
	void StartGame();

	TextRenderer* textRenderer;
	SpriteRenderer* spriteRenderer;
	PostProcessing* postProcessing;
	irrklang::ISoundEngine* soundEngine;

	GLfloat timeSinceInGame = 0;
};

