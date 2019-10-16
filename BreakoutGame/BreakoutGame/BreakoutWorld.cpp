#include "BreakoutWorld.h"
#include "AssetLoader.h"

#include "TextRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SystemContainers.h"
#include <GLFW\glfw3.h>


BreakoutWorld::BreakoutWorld(float _height, float _width)
	:height(_height), width(_width), keysProcessed(), keys(), gameState(Menu)
{
}


BreakoutWorld::~BreakoutWorld()
{
}

void BreakoutWorld::LoadAssets()
{

	AssetLoader::LoadShader("Resource/shaders/sprite.vert", "Resource/shaders/sprite.frag", nullptr, "sprite");
	
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->width), static_cast<GLfloat>(this->height), 0.0f, -1.0f, 1.0f);
	AssetLoader::GetShader("sprite")->Use().SetInteger("image", 0);
	AssetLoader::GetShader("sprite")->SetMatrix4("projection", projection);
	
	//load texture
	AssetLoader::LoadTexture("Resource/textures/background.jpg", GL_FALSE, "background");
	AssetLoader::LoadTexture("Resource/textures/paddle.png", true, "player");
	AssetLoader::LoadTexture("Resource/textures/block.png", true, "block");
	AssetLoader::LoadTexture("Resource/textures/ball.png", true, "ball");

	//load level
	AssetLoader::LoadLevel("Resource/levels/four.lvl", this->width, this->height/2, "level");
	//load game assets

	//load shaders

	spriteRenderer = new SpriteRenderer(AssetLoader::GetShader("sprite"));
}

void BreakoutWorld::LoadLevels()
{
	std::vector<std::vector<GLuint>> levelData = AssetLoader::GetLevel("level");
	if (levelData.size() <= 0) return;

	GLfloat brick_width, brick_height;
	brick_width = this->width / levelData[0].size();
	brick_height = (this->height/2) / levelData.size();

	for (int j = 0; j < levelData.size(); j++)
	{
		for (int i = 0; i < levelData[0].size(); i++)
		{	
			glm::vec2 position(brick_width * i, brick_height * j);
			glm::vec2 size(brick_width, brick_height);

			auto brick = createEntity();
			brick.addComponent<TransformComponent>(position);
			brick.addComponent<CollisionComponent>(size);

			if (levelData[j][i] == 1)
			{
				brick.addComponent<RenderingComponent>(glm::vec3(0.5,0.4,0.8), AssetLoader::GetTexture("block"), size);
			}
			else
			{
				brick.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("block"), size);
				brick.addComponent<DestructibleComponent>();
			}
		}
	}

}

void BreakoutWorld::Init()
{
	World::Init();

	LoadAssets();

	//create background.
	auto background = createEntity();
	background.addComponent<TransformComponent>();
	background.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("background"), glm::vec2(this->width, this->height));
	
	glm::vec2 playerPosition(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y * 1.5);
	//create player.
	auto player = createEntity();
	player.tag("player");
	player.addComponent<TransformComponent>(playerPosition);
	player.addComponent<CollisionComponent>(PLAYER_SIZE);
	player.addComponent<MovementComponent>(glm::vec2(0, 0), PLAYER_VELOCITY);
	player.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("player"), PLAYER_SIZE);
	player.addComponent<InputComponent>();

	//create level.
	LoadLevels();

	//create ball.
	auto ball = createEntity();
	ball.tag("ball");
	ball.addComponent<TransformComponent>(playerPosition + glm::vec2(PLAYER_SIZE.x/2 - BALL_RADIUS, -BALL_RADIUS *4.0));
	ball.addComponent<CollisionComponent>(glm::vec2(25,25));//
	ball.addComponent<MovementComponent>(INITIAL_BALL_VELOCITY, PLAYER_VELOCITY);//
	ball.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("ball"), glm::vec2(25, 25));
	ball.addComponent<BorderBounceComponent>();
	

	this->getSystemManager().addSystem<CollisionSystem>();
	this->getSystemManager().addSystem<DamageSystem>();
	this->getSystemManager().addSystem<BorderBounceSystem>();
	this->getSystemManager().addSystem<MovementSystem>();
	this->getSystemManager().addSystem<RenderingSystem>();
	this->getSystemManager().addSystem<InputMovementSystem>();
}

void BreakoutWorld::Update(float dt)
{
	World::Update(dt);
}

void BreakoutWorld::Render()
{
	World::Render();
}


void BreakoutWorld::ProcessInputKeyDown(GLfloat dt)
{
	/*if (this->gameState == Menu)
	{
		if (this->keys[GLFW_KEY_ENTER] && !this->keysProcessed[GLFW_KEY_ENTER])
		{
			this->gameState = Active;
			this->keysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}
	}
	if (this->gameState == EndGame)
	{
		if (this->keys[GLFW_KEY_ENTER])
		{
			this->keysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		
			this->gameState = Menu;
		}
	}
	if (this->gameState == Active)*/
	{
		if (this->keys[GLFW_KEY_A] || this->keys[GLFW_KEY_LEFT])
		{
			getEventManager().emitEvent<InputEvent>(LeftDown);
		}
		if (this->keys[GLFW_KEY_D] || this->keys[GLFW_KEY_RIGHT])
		{
			getEventManager().emitEvent<InputEvent>(RightDown);
		}
		
	}
}

void BreakoutWorld::ProcessInputKeyUp()
{
	getEventManager().emitEvent<InputEvent>(KeyUp);
}