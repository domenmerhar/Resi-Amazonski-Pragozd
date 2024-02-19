#include <SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <list>

using namespace std;

struct Color {
	int red;
	int green;
	int blue;
};

Color forestGreen{ 55, 178, 77 };

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
		frameStart = SDL_GetTicks();
	};

	void EndFrame() {
		int frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}

	int getFPS() {
		return FPS;
	}
};

class Util {
public:
	static const int windowWidth = 832;
	static const int windowHeight= 640;

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
		return rand() % (windowHeight - height * 2);
	}

	bool CheckCollision(SDL_Rect rect1, SDL_Rect rect2) {
		int left1 = rect1.x;
		int right1 = rect1.x + rect1.w;
		int top1 = rect1.y;
		int bottom1 = rect1.y + rect1.h;

		int left2 = rect2.x;
		int right2 = rect2.x + rect2.w;
		int top2 = rect2.y;
		int bottom2 = rect2.y + rect2.h;

		if (bottom1 <= top2 || top1 >= bottom2 || right1 <= left2 || left1 >= right2) {
			return false;
		}
		else {
			return true;
		}
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

	SDL_Rect GetBoundingBox() {
		SDL_Rect boundingBox = { x, y, width, height };
		return boundingBox;
	}
};

class Tree{
	int x, y, width, height;

	int framesToBurn;
	bool isBurning;
	float timeToBurn;
	bool isBurnt;
	bool canBeBurnt;

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
		destinationRectangle.x = x;
		destinationRectangle.y = y;

		destinationRectangle.w = sourceRectangle.w * 2;
		destinationRectangle.h = sourceRectangle.h * 2;
	}

	void Burn() {
		framesToBurn--;

		if (framesToBurn <= 0) {
			isBurning = false;
			//SetColor(88, 57, 39);
			
			tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
			SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, 88, 57, 39));
			texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
			SDL_FreeSurface(tempSurface);
			
			isBurnt = true;
		}
	}
	
	void SetColor(int red, int green, int blue) {
		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
	}
	
public:
	void Update() {
		UpdateSourceRectangle();
		UpdateDestinationRectangle();
			
		if(canBeBurnt && isBurning) Burn();
	}

	void Render() {
		SDL_RenderCopy(renderer, texture, &sourceRectangle, &destinationRectangle);
	}

	void Init(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int width, int height, float timeToBurn, int FPS) {
		this->width = width;
		this->height = height;

		tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);

		//SetColor(red, green, blue);

		this->renderer = renderer;
		this->x = x;
		this->y = y;

		UpdateSourceRectangle();
		this->timeToBurn = timeToBurn;
		ResetFramesToBurn();

		isBurnt = false;
		isBurning = canBeBurnt = false;
	}

	Tree() {
		Init(255, 255, 255, NULL, 0, 0, 32, 32,  5, 60);
	}

	Tree(int red, int green, int blue, SDL_Renderer* renderer, int x, int y,  int width, int height, float timeToBurn, int FPS) {
		Init(red, green, blue, renderer, x, y, width, height, timeToBurn, FPS);
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

	SDL_Rect GetBoundingBox() {
		SDL_Rect boundingBox = { x, y, width, height };
		return boundingBox;
	}

	void ResetFramesToBurn() {
		framesToBurn = timeToBurn * 60;
	}

	void Extinguish() {
		canBeBurnt = isBurning = false;
		
		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, forestGreen.red, forestGreen.green, forestGreen.blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
	}

	bool GetIsBurnt() {
		return isBurnt;
	}

	bool GetIsBurning() {
		return isBurning;
	}

	void StartBurning() {
		isBurning = true;
		canBeBurnt = true;
		ResetFramesToBurn();

		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, 255, 0, 0));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
	}
};

class Ally : public GameObject {
	int spawnX, spawnY;
	int targetX, targetY;

	int r;

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
		float dx = targetX - x;
		float dy = targetY - y;

		float magnitude = sqrt(dx * dx + dy * dy);


		if (magnitude > 0) {
			dx /= magnitude;
			dy /= magnitude;

			x += round(dx * movementSpeed);
			y += round(dy * movementSpeed);
		}
		else {
			GenerateRandomTarget();
		}
	}
	
	void Hide() {
		visible = false;
		x = spawnX;
		y = spawnY;
	}

	bool IsTreeInRange(Tree* tree, int range) {
		if (!tree->GetIsBurning())return false;

		int treeX = tree->GetX() + tree->getWidth() / 2;
		int treeY = tree->GetY() + tree->getHeight() / 2;

		int distanceSquared = (x - treeX) * (x - treeX) + (y - treeY) * (y - treeY);
		int rangeSquared = range * range;

		return distanceSquared <= rangeSquared;
	}

	void SetTarget(Tree* tree) {
		targetX = tree->GetX() + tree->getWidth() / 2;
		targetY = tree->GetY() + tree->getHeight() / 2;
	}

	void HandleTreeCollision(vector<Tree*> burningTrees) {
		for (Tree* tree : burningTrees) {
			SDL_Rect allyBoundingBox = GetBoundingBox();
		}

		for (Tree* tree : burningTrees) {
			if (visible && tree != nullptr) {
				SDL_Rect allyBoundingBox = GetBoundingBox();
				SDL_Rect treeBoundingBox = tree->GetBoundingBox();

				if (tree->GetIsBurning() && SDL_HasIntersection(&allyBoundingBox, &treeBoundingBox)) {
					tree->Extinguish();
					GenerateRandomTarget();
					break;
				}
			}
		}
	}
};

class Forest{
	const int rows = 10;
	const int columns = 13;
	const int tileSize = 64;
	float timeToBurn;

	const int totalTrees = rows * columns;
	int burntTrees = 0;
	float burntTreePercentage = 0;

	Tree *trees[10][13];
	vector<Tree*> burningTrees;


	void FillTrees(SDL_Renderer* renderer, float timeToBurn) {
		for (int row = 0; row < rows; row++) {
			for (int column = 0; column < columns; column++) {
				trees[row][column] = new Tree(forestGreen.red, forestGreen.green, forestGreen.blue,
					renderer, column * tileSize, row * tileSize,
					tileSize, tileSize, timeToBurn, 60);
			}
		}
	}

	void UpdateTrees() {
		for (int row = 0; row < rows; row++) {
			for (int column = 0; column < columns; column++) {
				trees[row][column]->Update();
			}
		}
	}
	
	void RenderTrees() {
		for (int row = 0; row < rows; row++) {
			for (int column = 0; column < columns; column++) {
				trees[row][column]->Render();
			}
		}
	}		

	void UpdateBurningTrees() {
		vector<Tree*>::iterator it = burningTrees.begin();

		while (it != burningTrees.end()) {
			if ((*it)->GetIsBurnt()) {
				it = burningTrees.erase(it);
				burntTrees++;
				CalculateBurntTreePercentage();
			}
			else if (!(*it)->GetIsBurning()) {
				it = burningTrees.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void CalculateBurntTreePercentage() {
		burntTreePercentage = (float)burntTrees / totalTrees * 100;
		if(burntTreePercentage >= 70) cout << "Game over!" << endl;
	}
public:
	Forest(SDL_Renderer* renderer, float timeToBurn) {
		FillTrees(renderer, timeToBurn);
	}

	void Render() {
		RenderTrees();
	}

	void Update() {
		UpdateTrees();
		UpdateBurningTrees();
	}

	bool CanBurn(int row, int column) {
		if (row < 0 || row >= rows || column < 0 || column >= columns) return false;
		if (trees[row][column]->GetIsBurnt() || trees[row][column]->GetIsBurning()) return false;

		return true;
	}

	void StartBurning(int row, int column) {
		trees[row][column]->StartBurning();
		burningTrees.push_back(trees[row][column]);
	}

	void RandomStartBurning() {
		int row = rand() % rows;
		int column = rand() % columns;

		if (CanBurn(row, column)) {
			StartBurning(row, column);
		}
		else {
			RandomStartBurning();
		}
	}
	
	void RemoveBurningTree(Tree* tree) {
		vector<Tree*>::iterator it = find(burningTrees.begin(), burningTrees.end(), tree);

		if (it != burningTrees.end()) {
			burningTrees.erase(it);
		}
	}

	Tree* GetCollidedTree(int x, int y) {
		for (int row = 0; row < rows; row++) {
			for (int column = 0; column < columns; column++) {
				if (trees[row][column]->GetBoundingBox().x <= x && x <= trees[row][column]->GetBoundingBox().x + tileSize &&
					trees[row][column]->GetBoundingBox().y <= y && y <= trees[row][column]->GetBoundingBox().y + tileSize) {
					return trees[row][column];
				}
			}
		}
		return nullptr;
	}

	vector<Tree*> GetBurningTrees() const {
		return burningTrees;
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

	void HandleTreeCollision(vector<Tree*> burningTrees) {
		for (Tree* tree : burningTrees) {
			if (visible && tree != nullptr) {
				SDL_Rect playerBoundingBox = GetBoundingBox();
				SDL_Rect treeBoundingBox = tree->GetBoundingBox();

				if (!tree->GetIsBurnt() && SDL_HasIntersection(&playerBoundingBox, &treeBoundingBox)) {
					tree->Extinguish();
				}
			}
		}
	}

	void Update() {
		if (visible) {
			UpdateSourceRectangle();
			UpdateDestinationRectangle();
		}
	}
};

class Spawner {
	float timeToSpawnFire;
	int framesToSpawnFire;
	int clockToSpawnFire;

	Forest * forest;

public:
	Spawner(float timeToSpawnFire, Forest *forest) {
		this->timeToSpawnFire = timeToSpawnFire;
		framesToSpawnFire = timeToSpawnFire * 60;
		clockToSpawnFire = 0;

		this->forest = forest;
	}

	void Update() {
		clockToSpawnFire++;
		if (clockToSpawnFire >= framesToSpawnFire) {
			clockToSpawnFire = 0;

			forest->RandomStartBurning();
		}
	}
};

Player* player;
vector<GameObject*> playerSpawnSquares(5);
vector<Ally*> allies(3);
Forest* forest;
Spawner* spawner;

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
				//allies[i]->HandleTreeCollision(trees[0]);		
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
			
	void CheckAllyBehavior(vector<Tree *> burningTrees) {
		for (int i = 0; i < allies.size(); i++) {
			if (allies[i]->GetIsVisible()) {
				for (Tree* tree : burningTrees) {
					if (allies[i]->IsTreeInRange(tree, 300)) {
						allies[i]->SetTarget(tree);
					}
				}
			}
		}
	}

	void HandleAlliesCollision(vector<Tree*> burningTrees) {
		for (int ally = 0; ally < 3; ally++) {
			allies[ally]->HandleTreeCollision(burningTrees);
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

		player = new Player(255, 255, 255, gameRenderer, 0, 0, 5, 32, 32, false);
		playerSpawnSquares[0] = new GameObject(200, 200, 200, gameRenderer, 0, 0, 0, 64, 64, true);
		playerSpawnSquares[1] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth - 64, 0, 0, 64, 64, true);
		playerSpawnSquares[2] = new GameObject(200, 200, 200, gameRenderer, 0, Util::windowHeight - 64, 0, 64, 64, true);
		playerSpawnSquares[3] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth - 64, Util::windowHeight - 64, 0, 64, 64, true);
		playerSpawnSquares[4] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth / 2 - 64, Util::windowHeight  / 2- 64, 0, 64, 64, true);

		allies[0] = new Ally(0, 0, 255, gameRenderer, 1, 32, 32);
		allies[1] = new Ally(0, 0, 255, gameRenderer, 1, 32, 32);
		allies[2] = new Ally(0, 0, 255, gameRenderer, 1, 32, 32);

		forest = new Forest(gameRenderer, 0.5f);

		spawner = new Spawner(3, forest);

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
					if (playerSpawnSquares[0]->GetIsVisible()) {
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
								player->Show();
								HideSpawnSquares();
								break;
							}
						}
					}
				}
			}
		}
	};

	void Update() {

		player->Update();
		player->HandleTreeCollision(forest->GetBurningTrees());

		forest->Update();
		
		CheckAllyBehavior(forest->GetBurningTrees());
		HandleAlliesCollision(forest->GetBurningTrees());
		UpdateAllies();

		spawner->Update();

		UpdateSpawnSquares();
	};

	void Render() {
		SDL_RenderClear(gameRenderer);

		forest->Render();   

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

int main(int argc, char* argv[]) {
	srand(time(NULL));

	bool isFullscreen = false;

	FrameManager frameManager;

	Game *game = new Game();
	game->Init("Resi Amazonski pragozd", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Util::windowWidth, Util::windowHeight, isFullscreen);	

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


//Burning trees
//Pass to Player and Allies collision detection
//Loop over all trees and check for collision
//If collision, extinguish tree, break loop
//Delete from array / vector / list / whatever