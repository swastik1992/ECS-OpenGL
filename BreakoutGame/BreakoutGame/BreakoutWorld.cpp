#include "BreakoutWorld.h"
#include "AssetLoader.h"
#include "EntityCollisionSystem.h"
#include "TextRenderer.h"
#include "SystemContainers.h"
#include "GameComponents.h"
#include "MovementSystem.h"

BreakoutWorld::BreakoutWorld(GLint _height, GLint _width)
	:height(_height), width(_width), keysProcessed(), keys(), gameState(GameState::Menu)
{
}


BreakoutWorld::~BreakoutWorld()
{
} 

void BreakoutWorld::LoadAssets()
{
	AssetLoader::LoadShader("Resource/shaders/sprite.vert", "Resource/shaders/sprite.frag", nullptr, "sprite");
	AssetLoader::LoadShader("Resource/shaders/post_processing.vert", "Resource/shaders/post_processing.frag", nullptr, "postprocessing");
	
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->width), static_cast<GLfloat>(this->height), 0.0f, -1.0f, 1.0f);
	AssetLoader::GetShader("sprite")->Use().SetInteger("image", 0);
	AssetLoader::GetShader("sprite")->SetMatrix4("projection", projection);
	
	//load texture
	AssetLoader::LoadTexture("Resource/textures/background.jpg", GL_FALSE, "background");
	AssetLoader::LoadTexture("Resource/textures/paddle2.png", true, "player");
	AssetLoader::LoadTexture("Resource/textures/block.png", true, "block");
	AssetLoader::LoadTexture("Resource/textures/ball.png", true, "ball");
	AssetLoader::LoadTexture("Resource/textures/line.png", true, "line");
	AssetLoader::LoadTexture("Resource/textures/rock1.png", true, "rock");

	//load level
	AssetLoader::LoadLevel("Resource/levels/five.lvl", this->width, this->height/2, "level_show");
	AssetLoader::LoadLevel("Resource/levels/four.lvl", this->width, this->height / 2, "level_game");

	//load game assets

	//load shaders

	spriteRenderer = new SpriteRenderer(AssetLoader::GetShader("sprite"));
	postProcessing = new PostProcessing(AssetLoader::GetShader("postprocessing"), this->width, this->height);
	textRenderer = new TextRenderer(this->width, this->height);
	soundEngine = irrklang::createIrrKlangDevice();
	textRenderer->Load("Resource/fonts/SEASRN__.TTF", 24);

	soundEngine->play2D("Resource/audio/410574__yummie__game-background-music-loop-short.mp3", true);
}

void BreakoutWorld::LoadLevels(std::string levelName)
{
	std::vector<std::vector<GLuint>> levelData = AssetLoader::GetLevel(levelName);
	if (levelData.size() <= 0) return;

	GLfloat brick_width, brick_height;
	brick_width = (GLint)(this->width / levelData[0].size());
	brick_height = (GLint)((this->height/2) / levelData.size());

	for (unsigned int j = 0; j < levelData.size(); j++)
	{
		for (unsigned int i = 0; i < levelData[0].size(); i++)
		{	
			glm::vec2 position(brick_width * i, brick_height * j);
			glm::vec2 size(brick_width, brick_height);

			auto brick = createEntity();
			brick.addComponent<TransformComponent>(position);
			brick.addComponent<CollisionComponent>(size);
			brick.group("level");
			glm::vec3 color(1);

			if (levelData[j][i] == 1)
			{
				brick.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("block"), size);
				continue;
			}
			else if (levelData[j][i] == 2)//defualt
			{
				color = glm::vec3(0, 1, 0);
			}
			else if (levelData[j][i] == 3)//red
			{
				color = glm::vec3(1, 0, 0);
			}
			else if (levelData[j][i] == 4)//blue
			{
				color = glm::vec3(1, 1, 1);
			}
			else if (levelData[j][i] == 5)//blue
			{
				color = glm::vec3(0, 0, 1);
			}
			else if (levelData[j][i] == 6)//yellow
			{
				color = glm::vec3(1, 1, 0.8);
			}
			brick.addComponent<RenderingComponent>(color, AssetLoader::GetTexture("block"), size);
			brick.addComponent<DestructibleComponent>();
		}
	}

	
}

void BreakoutWorld::UnLoadLevel()
{
	std::vector<Entity> entities = getEntityManager().getEntityGroup("level");
	for (Entity& e : entities)
		e.kill();
}

void BreakoutWorld::Init()
{
	LoadAssets();

	//create background.
	auto background = createEntity();
	background.addComponent<TransformComponent>();
	background.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("background"), glm::vec2(this->width, this->height));
	
	glm::vec2 playerPosition(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);

	//create player.
	auto player = createEntity();
	player.tag("player");
	player.addComponent<TransformComponent>(playerPosition);
	player.addComponent<MovementComponent>(glm::vec2(0, 0), PLAYER_VELOCITY);
	player.addComponent<InputComponent>();
	//player.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("player"), PLAYER_SIZE);
	//player.addComponent<CollisionComponent>(PLAYER_SIZE, Box);

	//create level.
	LoadLevels("level_show");

	//create ball.
	auto ball = createEntity();
	ball.tag("ball");
	ball.addComponent<TransformComponent>(playerPosition + glm::vec2(PLAYER_SIZE.x/2 - BALL_RADIUS, -BALL_RADIUS *2.250));
	ball.addComponent<CollisionComponent>(BALL_RADIUS, Circle);//
	ball.addComponent<MovementComponent>(INITIAL_BALL_VELOCITY, 1);//
	ball.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("ball"), glm::vec2(25, 25));
	ball.addComponent<BorderBounceComponent>();
	
	//border
	auto border = createEntity();
	border.tag("border");
	border.addComponent<TransformComponent>(glm::vec2(0,height * 0.95));
	border.addComponent<CollisionComponent>(glm::vec2(width, 10), Box);
	border.addComponent<MovementComponent>(glm::vec2(0, 0), PLAYER_VELOCITY);
	border.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("line"), glm::vec2(width, 12));

	getEventBus().Subscribe(this, &BreakoutWorld::OnPlayerDied);
	getEventBus().Subscribe(this, &BreakoutWorld::OnLevelEnded);

	postProcessing->Blur = GL_TRUE;

	this->getSystemManager().addSystem<EntityCollisionSystem>();
	this->getSystemManager().addSystem<BorderBounceSystem>();
	this->getSystemManager().addSystem<DamageSystem>();
	this->getSystemManager().addSystem<MovementSystem>();
	this->getSystemManager().addSystem<RenderingSystem>();
	this->getSystemManager().addSystem<InputMovementSystem>();
	this->getSystemManager().addSystem<PostProcessingSystem>();
	this->getSystemManager().addSystem<GamsStateSystem>();

	World::Init();
}

void BreakoutWorld::Update(float dt)
{
	World::Update(dt);

	if(gameState == GameState::InGame)
		timeSinceInGame += dt;
}

void BreakoutWorld::OnPlayerDied(PlayerDiedEvent* event)
{
	this->lives--;

	if (lives <= 0)
	{
		this->gameState = EndGame;
		PrepareNonGame();
	}
	else
	{
		ResetBall();
	}
}

void BreakoutWorld::OnLevelEnded(LevelEnd* event)
{
	//if only one level.
	if (gameState == GameState::InGame)
	{
		this->gameState = EndGame;
		PrepareNonGame();
	}
	//else next load next level.
}

void BreakoutWorld::Render()
{
	postProcessing->BeginRender();

	World::Render();

	postProcessing->EndRender();
	postProcessing->Render(glfwGetTime());

	GLint timeSG = timeSinceInGame;
	GLint hh = timeSG /3600;
	timeSG = (GLint)timeSG % 3600;
	GLint mm = timeSG /60;
	timeSG = (GLint)timeSG % 60;
	GLint ss = timeSG;

	std::string time = std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss);

	if (gameState == GameState::Menu)
	{
		textRenderer->RenderText("PRESS ENTER TO START", 190.0f, this->height / 2 - 50, 1.5f);
	}
	else if (gameState == GameState::EndGame)
	{
		textRenderer->RenderText("GAME OVER", this->width * 0.25, this->height * 0.2, 2.50f);
		textRenderer->RenderText("SUMMARY", this->width * 0.45, this->height * 0.35, 0.8f);
		textRenderer->RenderText("TIME SPENT", this->width * 0.22, this->height * 0.44, 1.0f);
		textRenderer->RenderText(time, this->width * 0.72, this->height * 0.44, 1.0f);
		textRenderer->RenderText("LIFE REMAINING", this->width * 0.22, this->height * 0.51, 1.0f);
		textRenderer->RenderText("3", this->width * 0.72, this->height * 0.51, 1.0f);
		textRenderer->RenderText("PRESS ENTER TO RETRY OR ESC TO QUIT", this->width * 0.1, this->height * 0.71, 1.250f);
	}
	else
	{
		textRenderer->RenderText("Lives : " + std::to_string(this->lives), this->width * 0.025, this->height * 0.95, 0.80f);
		textRenderer->RenderText("TIME  : " + time, this->width * 0.82, this->height * 0.95, 0.80f);
	}
}

void BreakoutWorld::PrepareNonGame()
{
	postProcessing->Blur = GL_TRUE;

	if (this->getEntityManager().hasTag("player"))
	{
		auto player = this->getEntityManager().getEntityByTag("player");
		if (player.hasComponent<RenderingComponent>())
			player.removeComponent<RenderingComponent>();

		if (player.hasComponent<CollisionComponent>())
			player.removeComponent<CollisionComponent>();

		if (this->getEntityManager().hasTag("border"))
		{
			auto border = this->getEntityManager().getEntityByTag("border");

			if (!border.hasComponent<CollisionComponent>())
				border.addComponent<CollisionComponent>(glm::vec2(width, 10), Box);

			if (!border.hasComponent<RenderingComponent>())
				border.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("line"), glm::vec2(width, 12));
		}

		if (this->getEntityManager().hasTag("ball"))
		{
			auto ball = this->getEntityManager().getEntityByTag("ball");
			if (ball.hasComponent<TransformComponent>())
			{
				glm::vec2 pos = player.getComponent<TransformComponent>().position;
				ball.getComponent<TransformComponent>().position = pos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2.250);
			}
			if (!ball.hasComponent<MovementComponent>())
				ball.addComponent<MovementComponent>(INITIAL_BALL_VELOCITY, 1);
		}
	}

	//reset level.
	UnLoadLevel();

	//load game level.
	LoadLevels("level_show");
}

void BreakoutWorld::ResetGame()
{
	postProcessing->Blur = GL_FALSE;

	if (this->getEntityManager().hasTag("player"))
	{
		auto player = this->getEntityManager().getEntityByTag("player");
		if (!player.hasComponent<RenderingComponent>())
			player.addComponent<RenderingComponent>(glm::vec3(1), AssetLoader::GetTexture("player"), PLAYER_SIZE);

		if (!player.hasComponent<CollisionComponent>())
			player.addComponent<CollisionComponent>(PLAYER_SIZE, Box);

		getSystemManager().addToSystems(player);

		ResetBall();
	}

	if (this->getEntityManager().hasTag("border"))
	{
		auto border = this->getEntityManager().getEntityByTag("border");
		border.removeComponent<CollisionComponent>();
		border.removeComponent<RenderingComponent>();
		getSystemManager().addToSystems(border);
	}

	//reset level.
	UnLoadLevel();

	//load game level.
	LoadLevels("level_game");
	//add any addition variables
	
	this->lives = 3;

	this->timeSinceInGame = 0;
}

void BreakoutWorld::ResetBall()
{
	if (this->getEntityManager().hasTag("player"))
	{
		auto player = this->getEntityManager().getEntityByTag("player");

		if (this->getEntityManager().hasTag("ball"))
		{
			auto ball = this->getEntityManager().getEntityByTag("ball");
			if (ball.hasComponent<MovementComponent>())
				ball.removeComponent<MovementComponent>();
			
			if (ball.hasComponent<TransformComponent>())
			{
				glm::vec2 pos = player.getComponent<TransformComponent>().position;
				ball.getComponent<TransformComponent>().position = pos + glm::vec2(PLAYER_SIZE.x - BALL_RADIUS, -BALL_RADIUS * 2.50);
			}

			ball.addComponent<ParentEntityComponent>(player);
		}
	}
}

void BreakoutWorld::StartGame()
{
	if (this->getEntityManager().hasTag("ball"))
	{
		auto ball = this->getEntityManager().getEntityByTag("ball");
		if (ball.hasComponent<ParentEntityComponent>())
		{
			ball.removeComponent< ParentEntityComponent>();
		}
		if (!ball.hasComponent<MovementComponent>())
			ball.addComponent<MovementComponent>(INITIAL_BALL_VELOCITY, 1);
	}
}

void BreakoutWorld::ProcessInputKey(InputKeyAction action, int key)
{
	if (gameState == GameState::Menu)
	{
		if (action == InputKeyAction::Down)
		{
			if (key == GLFW_KEY_ENTER)
			{
				this->gameState = GameState::InGame;
				ResetGame();
			}
		}
	}
	else if (gameState == GameState::EndGame)
	{
		if (action == InputKeyAction::Down)
		{
			if (key == GLFW_KEY_ENTER)
			{
				this->gameState = GameState::InGame;
				ResetGame();
			}
		}
	}
	else if (gameState == GameState::InGame)
	{
		if (action == InputKeyAction::Down)
		{
			if (key == GLFW_KEY_SPACE)
			{
				StartGame();
			}
		}

		getEventBus().Publish(new InputEvent(action, key));
	}
}