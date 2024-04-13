#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <string>
#include <cstring>
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
#include "Position.h"
#include "ReplayManager.h"

using namespace std;

int PADDING_TOP = 10;
int MARGIN_LEFT = 350;
int TIME_PER_LEVEL = 60;

Color forestGreen{ 55, 178, 77 };
Color gray{ 134, 142, 150 };
Color orange{ 251, 139, 35};

Color red{ 201, 42, 42 };
Color pink{ 166, 30, 77 };

SDL_Color textColor{ 255, 255, 255, 0};
SDL_Color greyTextColor{ 134, 142, 150, 0 };

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

Text* timeText, *scoreText, *pauseText, *backToMenuText, *replayText;

Text* menuPlay, *menuLeaderboard, *menuExit, *menuTitleText;

Text *leaderboardBackText;
vector<Text*> leaderboardTexts(5);

Text* inputLabel, *inputText, *inputConfirm;

ReplayManager *replayManager;

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

	short int locationNumber = 0;
	// 0 - main menu
	// 1 - input
	// 2 - game
	// 3 - leaderboard

	string inputName = " ";
	const char* inputNameChar = inputName.c_str();

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

	void HandleEnemies() {
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies[i]->GetIsVisible()) {
				enemies[i]->Update();
				enemies[i]->HandleCollision(allies, enemies, player, scoreCounter, inputNameChar);
				enemies[i]->Move();
			}
		}
	}

	void ResetGame(Level levelToSet) {
		gameClock->Reset(levelToSet.time);
		forest->Reset(levelToSet.timeToBurn);
		spawner->Reset(levelToSet.timeToSpawnDestruction, levelToSet.timeToSpawnEnemy);
		player->Reset(levelToSet.playerSpeed, false);
		replayManager->ClearFile();

		isPlaying = false;

		for (int i = 0; i < allies.size(); i++) {
			allies[i]->Reset(levelToSet.allySpeed);
		}

		for (int i = 0; i < enemies.size(); i++) {
			enemies[i]->Reset(levelToSet.enemySpeed);
		}

		ShowSpawnSquares();
	}

	void HandleLevels() {
		if (!player->GetIsVisible() && gameClock->GetIsCounting() || forest->GetDestroyedTreesPercentage() >= 70) {
			ResetGame(levels[0]);
			Util::SaveScore(scoreCounter->GetScore(), inputNameChar);
			scoreCounter->ResetScore();
			gameClock->StopCounting();
		} 
		else if (gameClock->GetTimeRemaining() <= 0) {
			if (forest->GetDestroyedTreesPercentage() >= 70) {
				gameClock->StopCounting();
				Util::SaveScore(scoreCounter->GetScore(), inputNameChar);
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
		const Uint8* keys = SDL_GetKeyboardState(NULL);

		escapeDelayCounter++;

		if (escapeDelayCounter >= escapeDelay) {
			canEscape = true;
			escapeDelayCounter = 0;
		}

		if (!canEscape || !keys[SDL_SCANCODE_ESCAPE]) return;

		if (isPlaying) {
			gameClock->StopCounting();
			isPlaying = false;
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

	void HandlePauseButton(int x, int y) {
		if (backToMenuText->IsColliding(x, y)) {
			menuPlay->ChangeText("Nadaljuj igro");
			locationNumber = 0;
		}
	}

	void HandleReplay() {

		const Uint8* keys = SDL_GetKeyboardState(NULL);

		if (replayManager->GetIsReplaying() || isPaused || !isPlaying || !keys[SDL_SCANCODE_R]) return;
		replayManager->SetIsReplaying(true);
	}

	void UpdateGameplay() {
		HandleLevels();
		UpdateSpawnSquares();

		HandleReplay();
		if (replayManager->GetIsReplaying()) {
			struct Position position = replayManager->GetPosition();
			if (position.x == -1 && position.y == -1) {
				replayManager->SetIsReplaying(false);
				return;
			}

			player->SetPosition(position.x, position.y);
			player->Update();
			return;
		}

		HandlePause();
		if (!isPlaying) return;

		if (player->GetIsVisible()) scoreCounter->AddScore(1);

		UpdateCurrentScore();

		UpdateCurrentTime();

		player->Update();
		player->HandleTreeCollision(forest->GetBurningTrees());

		forest->Update();

		HandleAllies(forest->GetBurningTrees());
		HandleEnemies();

		spawner->Update();
		gameClock->Update();
		if (!isPaused && !replayManager->GetIsReplaying()) replayManager->SavePosition({ player->GetX(), player->GetY() });
	}

	void RenderGameplay() {
		forest->Render();

		RenderEnemies();
		RenderAllies();

		player->Render();
		for (int i = 0; i < playerSpawnSquares.size(); i++) {
			playerSpawnSquares[i]->Render();
		}

		timeText->Render();
		scoreText->Render();

		if (isPaused) {
			pauseText->Render();
			backToMenuText->Render();
		}
		if (replayManager->GetIsReplaying()) replayText->Render();
	}

	void RenderMainMenu() {
		menuTitleText->Render();
		menuPlay->Render();
		menuLeaderboard->Render();
		menuExit->Render();
	}

	void InitMainMenu() {
		menuTitleText = new Text("RESI AMAZONSKI PRAGOZD", 265, 100, gameRenderer, robotRegularPath, true, textColor);
		menuPlay = new Text("Zacni igro", MARGIN_LEFT, 240, gameRenderer, robotRegularPath, true, textColor);
		menuLeaderboard = new Text("Lestvica", MARGIN_LEFT, 300, gameRenderer, robotRegularPath, true, textColor);
		menuExit = new Text("Izhod", MARGIN_LEFT, 360, gameRenderer, robotRegularPath, true, textColor);
	}

	void HandleMainMenu(int x, int y) {
		if (menuPlay->IsColliding(x, y)) locationNumber = isPlaying && isPaused ? 2 : 1;
		else if (menuLeaderboard->IsColliding(x, y)) locationNumber = 3;
		else if (menuExit->IsColliding(x, y)) {
			Util::SaveScore(scoreCounter->GetScore(), inputNameChar);
			isRunning = false;
		}
	}

	void InitLeaderboard() {
		leaderboardBackText = new Text("Nazaj", PADDING_TOP, PADDING_TOP, gameRenderer, robotRegularPath, true, textColor);

		ifstream original("Assets/Score/scores.bin", ios::binary);

		if (!original.is_open()) {
			leaderboardTexts[0] = new Text("Ni rezultatov", MARGIN_LEFT, 180, gameRenderer, robotRegularPath, true, textColor);

			for (int i = 1; i < leaderboardTexts.size(); i++) {
				leaderboardTexts[i] = nullptr;
			}
			return;
		}

		int i = 0;
		struct Score curr;

		while (original.read((char*)&curr, sizeof(curr)))
		{
			const char* scoreString = Util::IntToCharPointer(curr.score);

			leaderboardTexts[i] = new Text(strcat(strcat(curr.name, " - "), scoreString), MARGIN_LEFT, 180 + i * 50, gameRenderer, robotRegularPath, true, textColor);
			i++;
		}

		for (; i < leaderboardTexts.size(); i++) {
			leaderboardTexts[i] = nullptr;
		}

		original.close();
	}

	void RenderLeaderBoard() {
		leaderboardBackText->Render();
		for (int i = 0; i < leaderboardTexts.size(); i++) {
			if (leaderboardTexts[i] != nullptr) leaderboardTexts[i]->Render();
		}
	}

	void HandleLeaderboardBack(int x, int y) {
		if (leaderboardBackText->IsColliding(x, y)) locationNumber = 0;
	}

	void InitInputSceen() {
		inputLabel = new Text("Vnesi ime", MARGIN_LEFT, 180, gameRenderer, robotRegularPath, true, textColor);
		inputText = new Text(inputName.c_str(), MARGIN_LEFT, 240, gameRenderer, robotRegularPath, true, textColor);
		inputConfirm = new Text("Potrdi", MARGIN_LEFT, 300, gameRenderer, robotRegularPath, true, textColor);
	}

	void RenderInputScreen() {
		leaderboardBackText->Render();
		inputLabel->Render();
		inputText->Render();
		inputConfirm->Render();
	}

	void HandleInputConfirm(int x, int y) {
		if (inputConfirm->IsColliding(x, y)) {
			locationNumber = 2;
		}
	}

	void HandleInputSceenName(SDL_Event e) {

		if (e.type == SDL_KEYDOWN) {
			char toAppend = NULL;

			if(inputName.size() >= 20) return;

			if (e.key.keysym.sym == SDLK_BACKSPACE && inputName.size() > 0) {
				inputName.pop_back();
				inputNameChar = inputName.c_str();
				inputText->ChangeText(inputNameChar);
				return; 
			}

			switch (e.key.keysym.sym) {
			case SDLK_a:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'A' : 'a';
				break;
			case SDLK_b:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'B' : 'b';
				break;
			case SDLK_c:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'C' : 'c';
				break;
			case SDLK_d:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'D' : 'd';
				break;
			case SDLK_e:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'E' : 'e';
				break;
			case SDLK_f:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'F' : 'f';
				break;
			case SDLK_g:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'G' : 'g';
				break;
			case SDLK_h:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'H' : 'h';
				break;
			case SDLK_i:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'I' : 'i';
				break;
			case SDLK_j:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'J' : 'j';
				break;
			case SDLK_k:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'K' : 'k';
				break;
			case SDLK_l:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'L' : 'l';
				break;
			case SDLK_m:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'M' : 'm';
				break;
			case SDLK_n:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'N' : 'n';
				break;
			case SDLK_o:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'O' : 'o';
				break;
			case SDLK_p:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'P' : 'p';
				break;
			case SDLK_q:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'Q' : 'q';
				break;
			case SDLK_r:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'R' : 'r';
				break;
			case SDLK_s:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'S' : 's';
				break;
			case SDLK_t:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'T' : 't';
				break;
			case SDLK_u:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'U' : 'u';
				break;
			case SDLK_v:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'V' : 'v';
				break;
			case SDLK_w:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'W' : 'w';
				break;
			case SDLK_x:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'X' : 'x';
				break;
			case SDLK_y:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'Y' : 'y';
				break;
			case SDLK_z:
				toAppend = (SDL_GetModState() & KMOD_SHIFT) ? 'Z' : 'z';
				break;
			}
			
			if (toAppend == NULL) return;

			if (inputName == " ") inputName = toAppend;
			else inputName.push_back(toAppend);

			inputNameChar = inputName.c_str();

			inputText->ChangeText(inputNameChar);
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

		pauseText = new Text("PREMOR", MARGIN_LEFT, height / 2 - 40, gameRenderer, robotRegularPath, true, textColor);
		backToMenuText = new Text("Meni", MARGIN_LEFT, height / 2 + 10, gameRenderer, robotRegularPath, true, textColor);

		replayText = new Text("PONAVLJANJE", MARGIN_LEFT, height / 2 - 40, gameRenderer, robotRegularPath, true, textColor);

		scoreCounter = new ScoreCounter();

		levels[0] = { 1, 1, 15, TIME_PER_LEVEL, 5, 1, 1 };
		levels[1] = { 2, 0.5, 10, TIME_PER_LEVEL, 5, 1, 2 };

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
		replayManager = new ReplayManager();
		replayManager->ClearFile();

		InitMainMenu();
		InitLeaderboard();
		InitInputSceen();   

		for (int i = 0; i < allies.size(); i++) {
			allies[i]->Show();
		}

		gameClock = new Clock(levels[0].time, Util::FPS);
	}

	void HandleEvents() {
		if(locationNumber == 2 && isPlaying && !isPaused) player->HandleInput();

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT && isRunning) {
				isRunning = false;

				if(scoreCounter->GetScore() > 0) Util::SaveScore(scoreCounter->GetScore(), inputNameChar);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					if (locationNumber == 0) HandleMainMenu(event.button.x, event.button.y);
					else if (locationNumber == 1) HandleInputConfirm(event.button.x, event.button.y);
					else if (locationNumber == 2 && playerSpawnSquares[0]->GetIsVisible()) HandleEventsSpawnSquares(event.button.x, event.button.y);
					else if (locationNumber == 2 && isPaused) HandlePauseButton(event.button.x, event.button.y);
					else if (locationNumber == 3 || locationNumber == 1) HandleLeaderboardBack(event.button.x, event.button.y);
				}
			}
			else if (locationNumber == 1) HandleInputSceenName(event);

		}
	};

	void Update() {
		switch (locationNumber) {
			// 0 - main menu
			// 1 - input
			// 2 - game
			// 3 - leaderboard
			case 0:
				break;
			case 1:
				break;
			case 2:
				UpdateGameplay();
				break;
			case 3:
				break;
		}
		};

	void Render() {
		SDL_RenderClear(gameRenderer);
		
		switch (locationNumber) {
			// 0 - main menu
			// 1 - input
			// 2 - game
			// 3 - leaderboard
			case 0:
				RenderMainMenu();
				break;
			case 1:
				RenderInputScreen();
				break;
			case 2:
				RenderGameplay();
				break;
			case 3:
				RenderLeaderBoard();
		}

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

	while (game->IsRunning()) {
		frameManager.StartFrame();

		game->HandleEvents();
		game->Update();
		game->Render();


		frameManager.EndFrame();
	}

	game->Clean();

	return 0;
}
