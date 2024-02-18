#include <SDL.h>
#include <iostream>

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
};

class GameObject {
protected:
	int x, y, movementSpeed, width, height;

	SDL_Texture* texture;
	SDL_Rect sourceRectangle, destinationRectangle;
	SDL_Renderer* renderer;
	SDL_Surface* tempSurface;

public:
	GameObject(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height) {
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
	}

	void UpdateSourceRectangle() {
		sourceRectangle.h = height;
		sourceRectangle.w = width;
		sourceRectangle.x = 0;
		sourceRectangle.y = 0;
	}

	void Update() {
		/*x += movementSpeed;
		y += movementSpeed;*/

		UpdateSourceRectangle();

		if (Util::WithinBoundsX(x, width)) destinationRectangle.x = x;
		else 
			x = destinationRectangle.x = Util::ResetX(x, width);

		if(Util::WithinBoundsY(y, height * 2)) destinationRectangle.y = y;
		else y = destinationRectangle.y = Util::ResetY(y, height * 2);

		//cout << "x: " << x << " y: " << y << endl;

		destinationRectangle.w = sourceRectangle.w * 2;
		destinationRectangle.h = sourceRectangle.h * 2;
	}

	void Render() {
        SDL_RenderCopy(renderer, texture, &sourceRectangle, &destinationRectangle);
	}

};

class Player : public GameObject {
public:
	using GameObject::GameObject;

	void HandleInput() {
		const float diagonalSpeedMultiplier = 0.7071; // Approximately 1 / sqrt(2)

		SDL_PumpEvents(); // Update the internal key state

		const Uint8* keys = SDL_GetKeyboardState(NULL);

		float dx = 0, dy = 0;

		// Determine the movement direction based on key states
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

		// If diagonal movement detected, adjust dx and dy
		if (dx != 0 && dy != 0) {
			dx = dx * diagonalSpeedMultiplier;
			dy = dy * diagonalSpeedMultiplier;
		}

		// Update the position
		x +=  dx * movementSpeed;
		y += dy * movementSpeed;
	}
};


Player* player;

class Game
{
	bool isRunning = false;
	SDL_Window* window = NULL;
	SDL_Renderer* gameRenderer;
		
public:
	Game(const char* title, int x, int y, int width, int height, bool fullscreen) {
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

		player = new Player(255, 255, 255, gameRenderer, 0, 0, 5, 32, 32);
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

	bool isFullscreen = false;

	FrameManager frameManager;

	Game *game = new Game("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Util::windowWidth, Util::windowHeight, isFullscreen);

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