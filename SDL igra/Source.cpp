#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <string>
#include <fstream>

#include "Color.h"
#include "Level.h"
#include "FrameManager.h"
#include "Util.h"
#include "GameObject.h"	
#include "Tree.h"
#include "Ally.h"
#include "Player.h"
#include "Forest.h"
#include "Enemy.h"
#include "Clock.h"
#include "Spawner.h"
#include "ScoreCounter.h"
#include "Text.h"
#include "Score.h"

using namespace std;

int PADDING_TOP = 10;

Color forestGreen{ 55, 178, 77 };
Color gray{ 134, 142, 150 };
Color orange{ 251, 139, 35};

Color red{ 201, 42, 42 };
Color pink{ 166, 30, 77 };

SDL_Color textColor{ 255, 255, 255, 0 };

TTF_Font* font;

const char* pyroSmallPathRight = "Assets/pyromaniac-right.png";
const char* pyroSmallPathLeft = "Assets/pyromaniac-left.png";

const char* pyroBigPathRight = "Assets/pyromaniac-big-right.png";
const char* pyroBigPathLeft = "Assets/pyromaniac-big-left.png";

const char* firefigherPathRight = "Assets/firefighter-right.png";
const char* firefighterPathLeft = "Assets/firefighter-left.png";

const char* nativePathRight = "Assets/native-right.png";
const char* nativePathLeft = "Assets/native-left.png";

const char* robotRegularPath = "Assets/Font/Roboto-Regular.ttf";

Player* player;
vector<GameObject*> playerSpawnSquares(5);
vector<Ally*> allies(3);
Forest* forest;
Spawner* spawner;
vector<Enemy*> enemies(3);
Clock* gameClock;
ScoreCounter* scoreCounter;

Text* timeText, *scoreText, *pauseText;

char testName[21] = "Jože";



class Game
{
	bool isRunning = false;
	SDL_Window* window = NULL;
	SDL_Renderer* gameRenderer = NULL;
	Level levels[2];

	SDL_Rect Message_rect;
	TTF_Font* Roboto;
	SDL_Color White;
	SDL_Texture* Message;

	SDL_Surface* surfaceMessage;

	bool isPlaying = false;
	bool isPaused = false;

	int escapeDelay = 20;
	int escapeDelayCounter = 0;
	bool canEscape = true;

	void HideSpawnSquares() {
		for (int i = 0; i < playerSpawnSquares.size(); i++) {
			playerSpawnSquares[i]->Hide();
		}

		gameClock->StartCounting();
		isPlaying = true;
	}

	void ShowSpawnSquares() {
		for (int i = 0; i < playerSpawnSquares.size(); i++) {
			playerSpawnSquares[i]->Show();
		}
	}

	void UpdateSpawnSquares() {
		if (playerSpawnSquares[0]->GetIsVisible()) {
			for (int i = 0; i < playerSpawnSquares.size(); i++) {
				playerSpawnSquares[i]->Update();
			}
		}
	}

	void HandleAllyMovement(vector<Tree*> treesInDestruction, Ally* ally) {
		if (ally->GetIsVisible()) {
			for (Tree* tree : treesInDestruction) {
				if (ally->IsTreeInRange(tree, 300)) {
					ally->SetTarget(tree->GetX(), tree->GetY(), tree->getWidth(), tree->getHeight());
					return;
				}
			}
		}

		for (Enemy* enemy : enemies) {
			if (enemy != nullptr) {
				if (enemy->GetIsVisible() && !enemy->GetIsBig() &&
					ally->IsInRange(enemy->GetX(), enemy->GetY(), 300)) {
					ally->SetTarget(enemy->GetX(), enemy->GetY(), enemy->getWidth(), enemy->getHeight());
				}
			}
		}
	}

	void RenderAlly(Ally* ally) {
		if (ally->GetIsVisible()) {
			ally->Render();
		}
	}
	
	void RenderAllies() {
		if (allies[0]->GetIsVisible()) {
			for (int i = 0; i < allies.size(); i++) {
				RenderAlly(allies[i]);
			}
		}
	}

	void HandleAllyCollision(vector<Tree*> treesInDestruction, Ally* ally) {
		ally->HandleTreeCollision(treesInDestruction);
	}

	void HandleAllies(vector<Tree*> treesInDestruction) {
		if (allies[0]->GetIsVisible()) {
			for (int i = 0; i < allies.size(); i++) {
				allies[i]->Update();

				HandleAllyCollision(treesInDestruction, allies[i]);
				HandleAllyMovement(treesInDestruction, allies[i]);
				allies[i]->Move();
			}
		}
	}	
	
	void RenderEnemies() {
		if (enemies[0]->GetIsVisible()) {
			for (int i = 0; i < enemies.size(); i++) {
				enemies[i]->Render();
			}
		}
	}

	void HandleEnemies(){
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies[i]->GetIsVisible()) {
				enemies[i]->Update();
				enemies[i]->HandleCollision(allies, enemies, player, scoreCounter, testName);
				enemies[i]->Move();
			}
		}
	}

	void ResetGame(Level levelToSet) {
		gameClock->Reset(levelToSet.time);
		forest->Reset(levelToSet.timeToBurn);
		spawner->Reset(levelToSet.timeToSpawnDestruction, levelToSet.timeToSpawnEnemy);
		player->Reset(levelToSet.playerSpeed, false);

		for (int i = 0; i < allies.size(); i++) {
			allies[i]->Reset(levelToSet.allySpeed);
		}

		for (int i = 0; i < enemies.size(); i++) {
			enemies[i]->Reset(levelToSet.enemySpeed);
		}

		ShowSpawnSquares();
	}

	void HandleLevels() {
		if (!player->GetIsVisible() && gameClock->GetIsCounting()) {
			ResetGame(levels[0]);
			Util::SaveScore(scoreCounter->GetScore(), testName);
			scoreCounter->ResetScore();
		}

		if (gameClock->GetTimeRemaining() <= 0) {
			if (forest->GetDestroyedTreesPercentage() >= 70) {
				gameClock->StopCounting();
				cout << "Game over!" << endl;
				Util::SaveScore(scoreCounter->GetScore(), testName);
				scoreCounter->ResetScore();
				ResetGame(levels[0]);
			}
			else {
				ResetGame(levels[1]);
			}
		}
	}

	void HandleEventsSpawnSquares(int x, int y) {
		if (playerSpawnSquares[0]->GetIsVisible()) {
			for (int i = 0; i < playerSpawnSquares.size(); i++) {
				if (
					playerSpawnSquares[i]->GetX() - 100 <= x && x <= playerSpawnSquares[i]->GetX() + 100 &&
					playerSpawnSquares[i]->GetY() - 100 <= y && y <= playerSpawnSquares[i]->GetY() + 100
					) {
					if (i == 4) {
						int x = Util::GetRandomX(player->getWidth());
						int y = Util::GetRandomY(player->getHeight());

						player->SetPosition(x, y);
					}
					else
					{
						player->SetPosition(
							playerSpawnSquares[i]->GetX() + playerSpawnSquares[i]->getHeight() / 2,
							playerSpawnSquares[i]->GetY() + playerSpawnSquares[i]->getHeight() / 2
						);

					}

					player->Show();
					HideSpawnSquares();
					break;
				}
			}
		}
	}

	void UpdateCurrentScore() {
		const char* currentScore =
			Util::IntToCharPointer(scoreCounter->GetScore());
		scoreText->ChangeText(currentScore);
		delete currentScore;
	}
	
	void UpdateCurrentTime() {
		const char* currentTime =
			Util::IntToCharPointer(gameClock->GetTimeRemaining());
		timeText->ChangeText(currentTime);
		delete currentTime;
	}

	void HandlePause() {
		SDL_PumpEvents();
		const Uint8* keys = SDL_GetKeyboardState(NULL);

		escapeDelayCounter++;

		if (escapeDelayCounter >= escapeDelay) {
			canEscape = true;
			escapeDelayCounter = 0;
		}

		if (!canEscape || !keys[SDL_SCANCODE_ESCAPE]) return;

		if (isPlaying) {
			gameClock->StopCounting();
			isPlaying =  false;
			isPaused = true;
			canEscape = false;
		}
		else if (!isPlaying) {
			gameClock->StartCounting();
			isPlaying = isPaused = true;
			isPaused = false;
			canEscape = false;
		}
	}

public:
	void Init(const char* title, int x, int y, int width, int height, bool fullscreen) {
		int flags = 0;
		
		if (fullscreen) {
			flags = SDL_WINDOW_FULLSCREEN;
		}

		if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

			window = SDL_CreateWindow(title, x, y, width, height, fullscreen);

			if (window) {
			}

			gameRenderer = SDL_CreateRenderer(window, -1, 0);

			if(gameRenderer) {
				SDL_SetRenderDrawColor(gameRenderer, 55, 178, 77, 255);
				cout << "Renderer created!" << endl;
			}

			isRunning = true;
		}

		timeText = new Text("60", 400, PADDING_TOP, gameRenderer, robotRegularPath, true, textColor);
		scoreText = new Text("000", 10, PADDING_TOP, gameRenderer, robotRegularPath, true, textColor);
		pauseText = new Text("PAUSE", width / 2 - 40, height / 2 - 40, gameRenderer, robotRegularPath, true, textColor);

		scoreCounter = new ScoreCounter();

		levels[0] = { 1, 1, 15, 60, 5, 1, 1 };
		levels[1] = { 2, 0.5, 10, 60, 5, 1, 2 };

		player = new Player(firefigherPathRight, firefighterPathLeft, gameRenderer, 0, 0, levels[0].playerSpeed, 32, 32, false, scoreCounter);
		playerSpawnSquares[0] = new GameObject(200, 200, 200, gameRenderer, 0, 0, 0, 64, 64, true);
		playerSpawnSquares[1] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth - 64, 0, 0, 64, 64, true);
		playerSpawnSquares[2] = new GameObject(200, 200, 200, gameRenderer, 0, Util::windowHeight - 64, 0, 64, 64, true);
		playerSpawnSquares[3] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth - 64, Util::windowHeight - 64, 0, 64, 64, true);
		playerSpawnSquares[4] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth / 2 - 64, Util::windowHeight  / 2- 64, 0, 64, 64, true);

		allies[0] = new Ally(nativePathLeft, nativePathRight, gameRenderer, levels[0].allySpeed, 32, 32);
		allies[1] = new Ally(nativePathLeft, nativePathRight, gameRenderer, levels[0].allySpeed, 32, 32);
		allies[2] = new Ally(nativePathLeft, nativePathRight, gameRenderer, levels[0].allySpeed, 32, 32);

		forest = new Forest(gameRenderer, levels[0].timeToBurn, forestGreen, orange, gray);

		enemies[0] = new Enemy(gameRenderer, levels[0].enemySpeed, 32, 32, forest, pyroSmallPathLeft, pyroSmallPathRight, pyroBigPathLeft, pyroBigPathRight);
		enemies[1] = new Enemy(gameRenderer, levels[0].enemySpeed, 32, 32, forest, pyroSmallPathLeft, pyroSmallPathRight, pyroBigPathLeft, pyroBigPathRight);
		enemies[2] = new Enemy(gameRenderer, levels[0].enemySpeed, 32, 32, forest, pyroSmallPathLeft, pyroSmallPathRight, pyroBigPathLeft, pyroBigPathRight);

		spawner = new Spawner(levels[0].timeToSpawnDestruction, levels[0].timeToSpawnEnemy, forest, enemies);

		for (int i = 0; i < allies.size(); i++) {
			allies[i]->Show();
		}

		gameClock = new Clock(levels[0].time, Util::FPS);
	}

	void HandleEvents() {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT && isRunning) {
				isRunning = false;

				if(scoreCounter->GetScore() > 0) Util::SaveScore(scoreCounter->GetScore(), testName);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					HandleEventsSpawnSquares(event.button.x, event.button.y);
				}
			}
		}
	};

	void Update() {
		HandleLevels();
		UpdateSpawnSquares();

		HandlePause();
		if (!isPlaying) return;

		if(player->GetIsVisible()) scoreCounter->AddScore(1);

		UpdateCurrentScore();

		UpdateCurrentTime();

		player->Update();
		player->HandleTreeCollision(forest->GetBurningTrees());

		forest->Update();
		
		HandleAllies(forest->GetBurningTrees());
		HandleEnemies();

		spawner->Update();
		gameClock->Update();

		if (forest->GetDestroyedTreesPercentage() >= 70) ResetGame(levels[0]);
	};

	void Render() {
		SDL_RenderClear(gameRenderer);

		forest->Render();

		RenderEnemies();

		RenderAllies();

		player->Render();
		for (int i = 0; i < playerSpawnSquares.size(); i++) {
			playerSpawnSquares[i]->Render();
		}
		
		timeText->Render();
		scoreText->Render();
		if(isPaused) pauseText->Render();

		SDL_RenderCopy(gameRenderer, Message, NULL, &Message_rect);
		SDL_RenderPresent(gameRenderer);
	};

	void Clean() {
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(gameRenderer);
		SDL_Quit();
		cout << "Game cleaned!" << endl;
	};

	bool IsRunning() { 
		return isRunning;
	}
};

int main(int argc, char* argv[]) {
	srand(time(NULL));
	TTF_Init();
	font = TTF_OpenFont("Assets/Font/Roboto-Regular.ttf", 24);

	bool isFullscreen = false;

	FrameManager frameManager(Util::FPS);

	Game *game = new Game();
	game->Init("Resi Amazonski pragozd", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Util::windowWidth, Util::windowHeight, isFullscreen);	
	Util::PrintScore();

	while (game->IsRunning()) {
		frameManager.StartFrame();

		player->HandleInput();

		game->HandleEvents();
		game->Update();
		game->Render();


		frameManager.EndFrame();
	}

	game->Clean();

	return 0;
}
