#include <SDL.h>
#include <iostream>

using namespace std;

class GameObject {
	int x, y;

	SDL_Texture* texture;
	SDL_Rect sourceRectangle, destinationRectangle;
	SDL_Renderer* renderer;
	SDL_Surface* tempSurface;

public:
	GameObject(int red, int green, int blue, SDL_Renderer* renderer, int x, int y) {
		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, 32, 32, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);


		this->renderer = renderer;
		this->x = x;
		this->y = y;
	}

	void Update() {
		x += 5;
		y += 5;

		sourceRectangle.h = 64;
		sourceRectangle.w = 64;
		sourceRectangle.x = 0;
		sourceRectangle.y = 0;

		destinationRectangle.x = x;
		destinationRectangle.y = y;
		destinationRectangle.w = sourceRectangle.w * 2;
		destinationRectangle.h = sourceRectangle.h * 2;
	}

	void Render() {
        SDL_RenderCopy(renderer, texture, &sourceRectangle, &destinationRectangle);
	}

};

GameObject* player;


class Game
{
	bool isRunning = false;
	SDL_Window* window;
	SDL_Renderer* gameRenderer;
		
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

			gameRenderer = SDL_CreateRenderer(window, -1, 0);

			if(gameRenderer) {
				SDL_SetRenderDrawColor(gameRenderer, 55, 178, 77, 255);
				cout << "Renderer created!" << endl;
			}

			isRunning = true;
		}

		player = new GameObject(255, 255, 255, gameRenderer, 0, 0);
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
		player->Update();
	};

	void Render() {
		SDL_RenderClear(gameRenderer);
		player->Render();
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