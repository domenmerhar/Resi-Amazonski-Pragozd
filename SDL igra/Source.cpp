#include <SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

using namespace std;

class Util {
public:
	static const int windowWidth = 800;
	static const int windowHeight= 600;

	static bool WithinBoundsX(int x,  int width) {
		return x >= 0 && x <= windowWidth - width * 2;
	}

	static bool WithinBoundsY(int y, int height) {
		return y >= 0 && y <= windowHeight - height;
	}

	static int ResetX(int x, int width) {
		if (x < 0) {
			return 0;
		}
		else {
			return windowWidth - width * 2;
		}
	}

	static int ResetY(int y, int height) {
		if (y < 0) {
			return 0;
		}
		else {
			return windowHeight - height;
		}
	}

	static int GetRandomX(int width) {
		return rand() % (windowWidth - width * 2);
	}

	static int GetRandomY(int height) {
		return rand() % (windowHeight - height);
	}
};

class GameObject {
protected:
	int x, y, movementSpeed, width, height;
	bool visible;

	SDL_Texture* texture;
	SDL_Rect sourceRectangle, destinationRectangle;
	SDL_Renderer* renderer;
	SDL_Surface* tempSurface;

	void UpdateSourceRectangle() {
		sourceRectangle.h = height;
		sourceRectangle.w = width;
		sourceRectangle.x = 0;
		sourceRectangle.y = 0;
	}

	void UpdateDestinationRectangle() {
		if (Util::WithinBoundsX(x, width)) destinationRectangle.x = x;
		else
			x = destinationRectangle.x = Util::ResetX(x, width);

		if (Util::WithinBoundsY(y, height * 2)) destinationRectangle.y = y;
		else y = destinationRectangle.y = Util::ResetY(y, height * 2);

		destinationRectangle.w = sourceRectangle.w * 2;
		destinationRectangle.h = sourceRectangle.h * 2;
	}
public:
	void Update() {
		if (visible) {
			UpdateSourceRectangle();
			UpdateDestinationRectangle();
		}
	}

	void Render() {
        if(visible) SDL_RenderCopy(renderer, texture, &sourceRectangle, &destinationRectangle);
	}

	void Init(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible) {
		this->width = width;
		this->height = height;

		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);


		this->renderer = renderer;
		this->x = x;
		this->y = y;
		this->movementSpeed = movementSpeed;

		UpdateSourceRectangle();
		this->visible = visible;
	}

	GameObject() {
		Init(255, 255, 255, NULL, 0, 0, 0, 32, 32, true);
	}

	GameObject(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible) {
		Init(red, green, blue, renderer, x, y, movementSpeed, width, height, visible);
	}

	void Hide() {
		visible = false;
	}

	void Show() {
		visible = true;
	}	

	bool GetIsVisible() {
		return visible;
	}

	int GetX() {
		return x;
	}

	int GetY() {
		return y;
	}

	void SetPosition(int x, int y) {
		this->x = x;
		this->y = y;
	}

	int getHeight() {
		return height;
	}

	int getWidth() {
		return width;
	}
};

class Player : public GameObject {
public:
	using GameObject::GameObject;

	void HandleInput() {
		if (visible) {
			const float diagonalSpeedMultiplier = 0.7071;

			SDL_PumpEvents();

			const Uint8* keys = SDL_GetKeyboardState(NULL);

			float dx = 0, dy = 0;

			if (keys[SDL_SCANCODE_UP]) {
				dy -= 1;
			}
			if (keys[SDL_SCANCODE_DOWN]) {
				dy += 1;
			}
			if (keys[SDL_SCANCODE_LEFT]) {
				dx -= 1;
			}
			if (keys[SDL_SCANCODE_RIGHT]) {
				dx += 1;
			}

			if (dx != 0 && dy != 0) {
				dx = dx * diagonalSpeedMultiplier;
				dy = dy * diagonalSpeedMultiplier;
			}

			x += dx * movementSpeed;
			y += dy * movementSpeed;
		}
	}
};

class Ally : public GameObject {
	int spawnX, spawnY;
	int targetX, targetY;


	int r;

	int frameCounter = 180;

	void GenerateRandomTarget() {
		targetX = Util::GetRandomX(width);
		targetY = Util::GetRandomY(height);
	}

public:
	void Init(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height) {
		this->width = width;
		this->height = height;

		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);


		this->renderer = renderer;
		spawnX = x = Util::GetRandomX(width);
		spawnY = y = Util::GetRandomY(height);
		this->movementSpeed = movementSpeed;

		UpdateSourceRectangle();
		visible = true;

		GenerateRandomTarget();
	}

	Ally(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height) {
		Init(red, green, blue, renderer, movementSpeed, width, height);
		r = red;
	}

	void Move() {
		frameCounter--;

		if (frameCounter <= 0) {
			GenerateRandomTarget();
			frameCounter = 180;
		}

		float dx = targetX - x;
		float dy = targetY - y;

		float magnitude = sqrt(dx * dx + dy * dy);


		if (magnitude > 0) {
			dx /= magnitude;
			dy /= magnitude;

			x += round(dx * movementSpeed);
			y += round(dy * movementSpeed);
		}
	}
	
	void Hide() {
		visible = false;
		x = spawnX;
		y = spawnY;
	}
};

Player* player;
vector<GameObject*> playerSpawnSquares(5);
vector<Ally*> allies(3);

class Game
{
	bool isRunning = false;
	SDL_Window* window = NULL;
	SDL_Renderer* gameRenderer = NULL;

	void HideSpawnSquares() {
		for (int i = 0; i < playerSpawnSquares.size(); i++) {
			playerSpawnSquares[i]->Hide();
		}
	}

	void RenderAllies() {
		if (allies[0]->GetIsVisible()) {
			for (int i = 0; i < allies.size(); i++) {
				allies[i]->Render();
			}
		}
	}

	void UpdateAllies() {
		if (allies[0]->GetIsVisible()) {
			for (int i = 0; i < allies.size(); i++) {
				allies[i]->Update();
			}
		}
	}	

	void UpdateSpawnSquares() {
		if (playerSpawnSquares[0]->GetIsVisible()) {
			for (int i = 0; i < playerSpawnSquares.size(); i++) {
				playerSpawnSquares[i]->Update();
			}
		}
	}
		
public:
	void Init(const char* title, int x, int y, int width, int height, bool fullscreen) {
		int flags = 0;

		if (fullscreen) {
			flags = SDL_WINDOW_FULLSCREEN;
		}

		if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
			cout <<"Subsystems Initialized!..." << endl;

			window = SDL_CreateWindow(title, x, y, width, height, fullscreen);

			if (window) {
				cout << "Window created!" << endl;
			}

			gameRenderer = SDL_CreateRenderer(window, -1, 0);

			if(gameRenderer) {
				SDL_SetRenderDrawColor(gameRenderer, 55, 178, 77, 255);
				cout << "Renderer created!" << endl;
			}

			isRunning = true;
		}

		player = new Player(255, 255, 255, gameRenderer, 0, 0, 5, 32, 32, true);
		playerSpawnSquares[0] = new GameObject(200, 200, 200, gameRenderer, 0, 0, 0, 64, 64, true);
		playerSpawnSquares[1] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth - 64, 0, 0, 64, 64, true);
		playerSpawnSquares[2] = new GameObject(200, 200, 200, gameRenderer, 0, Util::windowHeight - 64, 0, 64, 64, true);
		playerSpawnSquares[3] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth - 64, Util::windowHeight - 64, 0, 64, 64, true);
		playerSpawnSquares[4] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth / 2 - 64, Util::windowHeight  / 2- 64, 0, 64, 64, true);

		allies[0] = new Ally(255, 0, 0, gameRenderer, 1, 32, 32);
		allies[1] = new Ally(0, 255, 0, gameRenderer, 1, 32, 32);
		allies[2] = new Ally(0, 0, 255, gameRenderer, 1, 32, 32);

		for (int i = 0; i < allies.size(); i++) {
			allies[i]->Show();
		}
	}

	void HandleEvents() {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				isRunning = false;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					int x = event.button.x;
					int y = event.button.y;

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
							HideSpawnSquares();
							break;
						}
					}
				}
			}
		}
	};

	void Update() {
		player->Update();

		UpdateAllies();

		UpdateSpawnSquares();
	};

	void Render() {
		SDL_RenderClear(gameRenderer);

		RenderAllies();

		player->Render();
		for (int i = 0; i < playerSpawnSquares.size(); i++) {
			playerSpawnSquares[i]->Render();
		}

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

class FrameManager {
	Uint32 frameStart = NULL;
	int FPS;
	int frameDelay;

public:
	FrameManager() {
		FPS = 60;
		frameDelay = 1000 / FPS;
	};

	FrameManager(int FPS) {
		this->FPS = FPS;
		frameDelay = 1000 / FPS;
	};

	void StartFrame() {
		frameStart = SDL_GetTicks(); // how much time since the program started
	};

	void EndFrame() {
		int frameTime = SDL_GetTicks() - frameStart; // how much time it took to render the frame

		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
};

int main(int argc, char* argv[]) {
	srand(time(NULL));

	bool isFullscreen = false;

	FrameManager frameManager;

	Game *game = new Game();
	game->Init("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Util::windowWidth, Util::windowHeight, isFullscreen);	

	while (game->IsRunning()) {

		frameManager.StartFrame();

		player->HandleInput();
		for (int i = 0; i < allies.size(); i++) {
			allies[i]->Move();
		}

		game->HandleEvents();
		game->Update();
		game->Render();

		frameManager.EndFrame();
	}

	game->Clean();

	return 0;
}