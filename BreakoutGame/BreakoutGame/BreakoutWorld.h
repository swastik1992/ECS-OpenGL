#pragma once
#include "World.h"
#include "SpriteRenderer.h"

const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const GLfloat BALL_RADIUS = 12.5f;

enum GameState
{
	Active = 0,
	Menu = 1,
	EndGame = 2
};

class BreakoutWorld :
	public World
{
public:
	BreakoutWorld(float _height, float _width);
	~BreakoutWorld();

	virtual void Init();

	virtual void Update(float dt);

	virtual void Render();

	void ProcessInputKeyDown(GLfloat dt);
	void ProcessInputKeyUp();

	SpriteRenderer* GetSpriteRenderer() { return spriteRenderer; }

	GameState gameState;
	GLboolean keys[1024];
	GLboolean keysProcessed[1024];

	float height;
	float width;

protected:

	void LoadAssets();

	void LoadLevels();

	void SetUpPlayer();

	

	SpriteRenderer* spriteRenderer;
};

