#include <SDL.h>
#include <iostream>

using namespace std;

SDL_Texture* playerTex;
SDL_Rect sourceRectangle, destinationRectangle;

class Game
{
	bool isRunning = false;
	SDL_Window* window;
	SDL_Renderer* renderer;
		
public:
	Game() {};
	~Game() {};

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

			renderer = SDL_CreateRenderer(window, -1, 0);

			if(renderer) {
				SDL_SetRenderDrawColor(renderer, 55, 178, 77, 255);
				cout << "Renderer created!" << endl;
			}

			isRunning = true;
		}

		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, 32, 32, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, 32, 32, 32));
		playerTex = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
	}

	void HandleEvents() {
		SDL_Event event;
		SDL_PollEvent(&event);

		switch (event.type) {
			case SDL_QUIT:
				isRunning = false;
				break;

			default:
				break;
		}
	};

	void Update() {
		destinationRectangle.h = 64;
		destinationRectangle.w = 64;
		destinationRectangle.x+= 5;
		destinationRectangle.y+= 5;
	};

	void Render() {
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, playerTex, NULL, &destinationRectangle);
		SDL_RenderPresent(renderer);
	};

	void Clean() {
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		cout << "Game cleaned!" << endl;
	};

	bool IsRunning() { 
		return isRunning;
	}
};

class GameObject {
	int x, y;
	SDL_Renderer* renderer;
public:
	GameObject(const char* texturesheet, SDL_Renderer* renderer) {
		this->renderer = renderer;
		x = y = 0;
	};

	GameObject(const char* texturesheet, SDL_Renderer* renderer, int x, int y) {
		objTexture = TextureManager::LoadTexture(texturesheet, ren);
		this->renderer = renderer;
		this->x = x;
		this->y = y;
	}
};

class FrameManager {
	Uint32 frameStart;
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
	const int height = 800;
	const int width = 600;

	bool isFullscreen = false;

	FrameManager frameManager;

	Game *game = new Game();
	game->Init("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, height, width, isFullscreen);

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