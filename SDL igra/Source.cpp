#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <string>

using namespace std;

struct Color {
	int red;
	int green;
	int blue;
};

Color forestGreen{ 55, 178, 77 };
Color gray{ 134, 142, 150 };
Color orange{ 251, 139, 35};

Color red{ 201, 42, 42 };
Color pink{ 166, 30, 77 };

const char* pyroSmallPath = "Assets/pyromaniac.png";
const char* pyroBigPath = "Assets/pyromaniac-big.png";
const char* firefigherPath = "Assets/firefighter.png";
const char* nativePath = "Assets/native.png";

struct Level {
	float timeToBurn,
		  timeToSpawnDestruction,
		  timeToSpawnEnemy;
	
	int time,
		playerSpeed,
		allySpeed,
		enemySpeed;
};

class TextureManager {
public:
		static SDL_Texture* LoadTexture(const char* fileName, SDL_Renderer* renderer) {
		SDL_Surface* tempSurface = SDL_LoadBMP(fileName);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);

		return texture;
	}
};

class FrameManager {
	Uint32 frameStart = NULL;
	int FPS;
	int frameDelay;

public:
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
};

class Util {
public:
	static const int windowWidth = 832;
	static const int windowHeight= 640;
	static const int FPS = 60;

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
};

class GameObject {
protected:
	int x, y, movementSpeed, width, height;
	bool visible;

	SDL_Texture* texture;
	SDL_Rect sourceRectangle, destinationRectangle;
	SDL_Renderer* renderer;

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

		Reset(movementSpeed, visible);
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

	void Reset(int movementSpeed, bool isVisible) {
		this->movementSpeed = movementSpeed;
		visible = isVisible;

		UpdateSourceRectangle();
	}

	GameObject() {
		Init(255, 255, 255, NULL, 0, 0, 0, 32, 32, true);
	}

	GameObject(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible) {
		Init(red, green, blue, renderer, x, y, movementSpeed, width, height, visible);
	}

	GameObject(const char* imagePath, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible) {
		this->width = width;
		this->height = height;

		SDL_Surface* tempSurface = IMG_Load(imagePath);
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);


		this->renderer = renderer;
		this->x = x;
		this->y = y;

		Reset(movementSpeed, visible);
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

	int spreadX, spreadY;

	int framesToDestruction;
	float timeToBurn;
	bool canSpread;

	float timeToExcavate;

	bool inDestruction;
	bool isDestroyed;
	bool canBeDestroyed;

	SDL_Texture* texture;
	SDL_Rect sourceRectangle, destinationRectangle;
	SDL_Renderer* renderer;
	SDL_Surface* tempSurface;

	Color treeColor, fireColor, excavationColor;

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
		framesToDestruction--;

		if (framesToDestruction <= 100 && canSpread) {
			canSpread = false;
			if (true) {
				int combination = rand() % 9;

				switch (combination) {
				case 0:
					spreadX = x + 64;
					spreadY = y + 64;
					break;
				case 1:
					spreadX = x + 64;
					spreadY = y + 0;
					break;
				case 2:
					spreadX = x + 64;
					spreadY = y - 64;
					break;
				case 3:
					spreadX = x + 0;
					spreadY = y + 64;
					break;
				case 4:
					spreadX = -1;
					spreadY = -1;
					break;
				case 5:
					spreadX = x + 0;
					spreadY = y - 64;
					break;
				case 6:
					spreadX = x - 64;
					spreadY = y + 64;
					break;
				case 7:
					spreadX = x - 64;
					spreadY = y + 0;
					break;
				case 8:
					spreadX = x - 64;
					spreadY = y - 64;
					break;
				}
			}
		}

		if (framesToDestruction <= 0) {
			inDestruction = false;
			//SetColor(88, 57, 39);
			
			tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
			SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, 88, 57, 39));
			texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
			SDL_FreeSurface(tempSurface);

			isDestroyed = true;
		}
	}
	
	void SetColor(int red, int green, int blue) {
		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
	}

	void ResetFramesToBurn() {
		framesToDestruction = timeToBurn * Util::FPS;
	}

	void ResetFramesToExcavate() {
		framesToDestruction = timeToExcavate * Util::FPS;
	}
	
public:
	void Update() {
		UpdateSourceRectangle();
		UpdateDestinationRectangle();
			
		if(canBeDestroyed && inDestruction) Burn();
	}

	void Render() {
		SDL_RenderCopy(renderer, texture, &sourceRectangle, &destinationRectangle);
	}

	void Init(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int width, int height, float timeToBurn, int FPS,
		Color treeColor, Color fireColor, Color excavationColor) {
		this->width = width;
		this->height = height;

		this->treeColor = treeColor;
		this->fireColor = fireColor;
		this->excavationColor = excavationColor;

		tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, treeColor.red, treeColor.green, treeColor.blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);

		//SetColor(red, green, blue);

		this->renderer = renderer;
		this->x = x;
		this->y = y;

		UpdateSourceRectangle();

		this->timeToBurn = timeToBurn;
		timeToExcavate = timeToBurn * 2;

		ResetFramesToBurn();

		isDestroyed = false;
		inDestruction = canBeDestroyed = false;

		spreadX = -1;
		spreadY = -1;
		canSpread = true;
	}

	void Reset(float timeToBurn, int red, int green, int blue) {
		tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, treeColor.red, treeColor.green, treeColor.blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);

		this->timeToBurn = timeToBurn;
		timeToExcavate = timeToBurn * 2;

		isDestroyed = false;
		inDestruction = canBeDestroyed = false;

		spreadX = -1;
		spreadY = -1;
		canSpread = true;
	}

	Tree() {
		Init(255, 255, 255, NULL, 0, 0, 32, 32,  5, Util::FPS, forestGreen, orange, gray);
	}

	Tree(int red, int green, int blue, SDL_Renderer* renderer, int x, int y,  int width, int height, float timeToBurn, int FPS,
		Color treeColor, Color fireColor, Color excavationColor) {
		Init(red, green, blue, renderer, x, y, width, height, timeToBurn, FPS, treeColor, fireColor, excavationColor);
	}

	int GetX() {
		return x;
	}

	int GetY() {
		return y;
	}
	
	int GetSpreadX() {
		return spreadX;
	}

	int GetSpreadY() {
		return spreadY;
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

	void Extinguish() {
		canBeDestroyed = inDestruction = false;
		
		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, forestGreen.red, forestGreen.green, forestGreen.blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
	}

	bool GetIsDestroyed() {
		return isDestroyed;
	}

	bool GetIsInDestruction() {
		return inDestruction;
	};

	void StartBurning() {
		inDestruction = true;
		canBeDestroyed = true;
		ResetFramesToBurn();

		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		if (tempSurface == NULL) {
			SDL_Log("SDL_CreateRGBSurface failed: %s", SDL_GetError());
			return;
		}
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, fireColor.red, fireColor.green, fireColor.blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
	}

	void StartExcavating() {
		inDestruction = true;
		canBeDestroyed = true;
		ResetFramesToExcavate();

		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, excavationColor.red, excavationColor.green, excavationColor.blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
	}
};

class Ally : public GameObject {
	int spawnX, spawnY;
	int targetX, targetY;

	void GenerateRandomTarget() {
		targetX = Util::GetRandomX(width);
		targetY = Util::GetRandomY(height);
	}

	void Init(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height) {
		this->width = width;
		this->height = height;

		SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
		SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);


		this->renderer = renderer;

		Reset(movementSpeed);
	}

public:
	void Reset(int movementSpeed) {
		spawnX = x = Util::GetRandomX(width);
		spawnY = y = Util::GetRandomY(height);
		this->movementSpeed = movementSpeed;

		UpdateSourceRectangle();
		visible = true;

		GenerateRandomTarget();
	}

	Ally(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height) {
		Init(red, green, blue, renderer, movementSpeed, width, height);
	}

	Ally(const char* imagePath, SDL_Renderer* renderer, int movementSpeed, int width, int height) {
		this->width = width;
		this->height = height;

		SDL_Surface* tempSurface = IMG_Load(imagePath);
		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);

		this->renderer = renderer;

		Reset(movementSpeed);
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

	bool IsInRange(int x, int y, int range) {
		int distanceSquared = (this->x - x) * (this->x - x) + (this->y - y) * (this->y - y);
		int rangeSquared = range * range;

		return distanceSquared <= rangeSquared;
	}

	bool IsTreeInRange(Tree* tree, int range) {
		if (!tree->GetIsInDestruction()) return false;

		int treeX = tree->GetX() + tree->getWidth() / 2;
		int treeY = tree->GetY() + tree->getHeight() / 2;

		IsInRange(treeX, treeY, range);
	}

	void SetTarget(int x, int y, int width, int height) {
		targetX = x + width / 2;
		targetY = y + height / 2;
	}

	void HandleTreeCollision(vector<Tree*> treesInDestruction) {
		for (Tree* tree : treesInDestruction) {
			SDL_Rect allyBoundingBox = GetBoundingBox();
		}

		for (Tree* tree : treesInDestruction) {
			if (visible && tree != nullptr) {
				SDL_Rect allyBoundingBox = GetBoundingBox();
				SDL_Rect treeBoundingBox = tree->GetBoundingBox();

				if (tree->GetIsInDestruction() && SDL_HasIntersection(&allyBoundingBox, &treeBoundingBox)) {
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
	int destroyedTrees = 0;
	float destroyedTreesPercentage = 0;

	Tree *trees[10][13];
	vector<Tree*> treesInDestruction;


	void FillTrees(SDL_Renderer* renderer, float timeToBurn, Color treeColor, Color fireColor, Color excavationColor) {
		for (int row = 0; row < rows; row++) {
			for (int column = 0; column < columns; column++) {
				trees[row][column] = new Tree(forestGreen.red, forestGreen.green, forestGreen.blue,
					renderer, column * tileSize, row * tileSize,
					tileSize, tileSize, timeToBurn, Util::FPS,
					treeColor, fireColor, excavationColor);
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

	void UpdateTreesInDestruction() {
		vector<Tree*>::iterator it = treesInDestruction.begin();


		while (it != treesInDestruction.end()) {
			if ((*it)->GetIsDestroyed()) {
				int column = (*it)->GetSpreadX() / 64;
				int row = (*it)->GetSpreadY() / 64;

				cout << "SpreadX: " << column << " SpreadY: " << row << endl;

				if (column >= 0 && row >= 0  && column <= columns * tileSize && row <= rows * tileSize && CanBeDestroyed(row, column)) {
					trees[row][column]->StartBurning();
				}

				it = treesInDestruction.erase(it);
				destroyedTrees++;
				CalculateDestroyedTreesPercentage();
			}
			else if (!(*it)->GetIsInDestruction()) {
				it = treesInDestruction.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void CalculateDestroyedTreesPercentage() {
		destroyedTreesPercentage = (float)destroyedTrees / totalTrees * 100;
		if(destroyedTreesPercentage >= 70) cout << "Game over!" << endl;
	}
public:
	Forest(SDL_Renderer* renderer, float timeToBurn, Color treeColor, Color fireColor, Color excavationColor) {
		FillTrees(renderer, timeToBurn, treeColor, fireColor, excavationColor);
	}

	void Render() {
		RenderTrees();
	}

	void Update() {
		UpdateTrees();
		UpdateTreesInDestruction();
	}

	bool CanBeDestroyed(int row, int column) {
		if (row < 0 || row >= rows || column < 0 || column >= columns) return false;
		if (trees[row][column]->GetIsDestroyed() || trees[row][column]->GetIsInDestruction()) return false;

		return true;
	}

	void StartBurning(int row, int column) {
		trees[row][column]->StartBurning();
		treesInDestruction.push_back(trees[row][column]);
	}

	void RandomStartDestroying(bool burn) {
		int row = rand() % rows;
		int column = rand() % columns;

		if (CanBeDestroyed(row, column)) {
			if(burn) StartBurning(row, column);
			else StartExcavating(row, column);
		}
		else {
			RandomStartDestroying(burn);
		}
	}

	void StartExcavating(int row, int column) {
		trees[row][column]->StartExcavating();
		treesInDestruction.push_back(trees[row][column]);
	}
	
	void RemoveBurningTree(Tree* tree) {
		vector<Tree*>::iterator it = find(treesInDestruction.begin(), treesInDestruction.end(), tree);

		if (it != treesInDestruction.end()) {
			treesInDestruction.erase(it);
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
		return treesInDestruction;
	}

	void Reset(float timeToBurn) {
		for (int row = 0; row < rows; row++) {
			for (int column = 0; column < columns; column++) {
				trees[row][column]->Reset(timeToBurn, forestGreen.red, forestGreen.green, forestGreen.blue);
			}
		}
	}

	float GetDestroyedTreesPercentage() {
		return destroyedTreesPercentage;
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

	void HandleTreeCollision(vector<Tree*> treesInDestruction) {
		for (Tree* tree : treesInDestruction) {
			if (visible && tree != nullptr) {
				SDL_Rect playerBoundingBox = GetBoundingBox();
				SDL_Rect treeBoundingBox = tree->GetBoundingBox();

				if (!tree->GetIsDestroyed() && SDL_HasIntersection(&playerBoundingBox, &treeBoundingBox)) {
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

class Enemy : public GameObject {
	int spawnX, spawnY;
	int targetX, targetY;

	int targetRow, targetColumn;

	bool isBig;

	Forest* forest;

	SDL_Rect boundingBox;

	const char *pathImageSmall, *pathImageBig;

	void GenerateRandomTarget() {
		targetX = Util::GetRandomX(width);
		targetY = Util::GetRandomY(height);

		targetRow = targetY / 64;
		targetColumn = targetX / 64;
	}

public:
	void Reset(int movementSpeed) {
		spawnX = x = Util::GetRandomX(width);
		spawnY = y = Util::GetRandomY(height);
		this->movementSpeed = movementSpeed;

		UpdateSourceRectangle();

		GenerateRandomTarget();
		
		SetBig(false);
		visible = true;

		boundingBox = GetBoundingBox();
	}

	void Init(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height, Forest* forest, const char* pathImageSmall, const char* pathImageBig) {
		this->width = width;
		this->height = height;

		this->pathImageSmall = pathImageSmall;
		this->pathImageBig = pathImageBig;

		this->renderer = renderer;

		SDL_Surface* tempSurface = IMG_Load(pathImageSmall);
		this->texture = SDL_CreateTextureFromSurface(this->renderer, tempSurface);
		SDL_FreeSurface(tempSurface);

		this->forest = forest;

		Reset(movementSpeed);
	}

	Enemy(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height, Forest* forest, const char* pathImageSmall, const char* pathImageBig) {
		Init(red, green, blue, renderer, movementSpeed, width, height, forest, pathImageSmall, pathImageBig);
	}

	void Move() {
		float dx = targetX - x;
		float dy = targetY - y;

		float magnitude = sqrt(dx * dx + dy * dy);

		if (!forest->CanBeDestroyed(targetRow, targetColumn)) GenerateRandomTarget();
		else {

			if (magnitude > 0) {
				dx /= magnitude;
				dy /= magnitude;

				x += round(dx * movementSpeed);
				y += round(dy * movementSpeed);

			}
			else {
				forest->StartBurning(targetRow, targetColumn);
				GenerateRandomTarget();
			}
		}
	}

	void Hide() {
		visible = false;
		x = spawnX;
		y = spawnY;
	}

	void SetTarget(Tree* tree) {
		targetX = tree->GetX() + tree->getWidth() / 2;
		targetY = tree->GetY() + tree->getHeight() / 2;
	}

	void SetBig(bool toSet) {
		if (isBig == toSet) return;

		isBig = toSet;

		if (toSet == true) SDL_Surface* tempSurface = IMG_Load(pathImageSmall);
		else; SDL_Surface* tempSurface = IMG_Load(pathImageBig);

		texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
	}

	bool GetIsBig() {
		return isBig;
	}

	void HandleCollision(vector<Ally*> allies, vector<Enemy*> enemies, Player *player) {
		boundingBox = GetBoundingBox();

		if (!visible) return;

		SDL_Rect boundingBox = GetBoundingBox();
		SDL_Rect playerBoundingBox = player->GetBoundingBox();
		bool invinciblePlayer = false;

		for (Ally* ally : allies) {
			bool playerIsCloseToAlly = false;
			if (ally != nullptr && ally->GetIsVisible()) {
				SDL_Rect allyBoundingBox = ally->GetBoundingBox();

				if ((abs(playerBoundingBox.x - allyBoundingBox.x) < 100 && abs(playerBoundingBox.y - allyBoundingBox.y) < 100) && !playerIsCloseToAlly) { 
					playerIsCloseToAlly = true; 
					invinciblePlayer = true;
				}

				if (SDL_HasIntersection(&allyBoundingBox, &boundingBox)) {
					cout << "Collision detected with ally!" << endl;

					if (!isBig) Hide();  
					else if(isBig) {
						if (SDL_HasIntersection(&playerBoundingBox, &boundingBox)) {
							Hide();
						}
						else if(!playerIsCloseToAlly) {
							ally->Hide();
						}
					}
				}
			}
		}

		for (Enemy* enemy : enemies) {
			if (enemy != nullptr && enemy->GetIsVisible()) {
				SDL_Rect enemyBoundingBox = enemy->GetBoundingBox();
				if (enemy != this) {
					if (SDL_HasIntersection(&enemyBoundingBox, &boundingBox) && isBig == false) {
						enemy->Hide();
						SetBig(true);
					}
				}
			}
		}


		if (!player->GetIsVisible() || invinciblePlayer) return;

		if (isBig &&  SDL_HasIntersection(&playerBoundingBox, &boundingBox)) {
			player->Hide();
		}
		else if (!isBig && SDL_HasIntersection(&playerBoundingBox, &boundingBox)) {
			this->Hide();
			cout << "Bouding box x: " << boundingBox.x << " y: " << boundingBox.y << endl;
		}
	}
};

class Spawner {
	float timeToSpawnFire;
	int framesToSpawnFire;
	int clockToSpawnFire;

	float timeToRespawnEnemy;
	int framesToRespawnEnemy;
	int clockToRespawnEnemy;

	Forest * forest;
	vector<Enemy*> enemies;
public:
	Spawner(float timeToSpawnFire, float timeToRespawnEnemy, Forest *forest, vector<Enemy*> enemies) {
		Reset(timeToSpawnFire, timeToRespawnEnemy);

		this->forest = forest;
		this->enemies = enemies;
	}

	void Reset(float timeToSpawnFire, float timeToRespawnEnemy) {
		this->timeToSpawnFire = timeToSpawnFire;
		framesToSpawnFire = timeToSpawnFire * Util::FPS;
		clockToSpawnFire = 0;

		this->timeToRespawnEnemy = timeToRespawnEnemy;
		framesToRespawnEnemy = timeToRespawnEnemy * Util::FPS;
		clockToRespawnEnemy = 0;
	}

	void Update() {
		clockToSpawnFire++;
		if (clockToSpawnFire >= framesToSpawnFire) {
			clockToSpawnFire = 0;

			bool burnTree = rand() % 2;

			forest->RandomStartDestroying(burnTree);
		}

		clockToRespawnEnemy++;

		if (clockToRespawnEnemy >= framesToRespawnEnemy) {
			clockToRespawnEnemy = 0;

			for (Enemy* enemy : enemies) {
				if (!enemy->GetIsVisible()) {
					enemy->Show();
					break;
				}
			}
		}
	}
};

class Clock {
	int framesToCountdown;
	int framesRemaining;
	int FPS;

	bool canCount;
	bool isCounting;

public:
	Clock(int seconds, int FPS) {
		this->FPS = FPS;
		Reset(seconds);
	}

	void Reset(int seconds) {
		framesToCountdown = seconds * FPS;
		framesRemaining = framesToCountdown;
		canCount = false;
		isCounting = false;
	}

	void Update() {
		if (canCount) {
			framesRemaining--;

			if (framesRemaining < 0) {
				StopCounting();
			}
		}
	}

	bool IsTimeUp() {
		return framesRemaining <= 0;
	}

	int GetTimeRemaining() {
		return framesRemaining / FPS;
	}

	void StartCounting() {
		canCount = true;
		isCounting = true;
	}

	void StopCounting() {
		canCount = false;
	}

	bool GetIsCounting() {
		return isCounting;
	}
};

Player* player;
vector<GameObject*> playerSpawnSquares(5);
vector<Ally*> allies(3);
Forest* forest;
Spawner* spawner;
vector<Enemy*> enemies(3);
Clock* gameClock;

class Game
{
	bool isRunning = false;
	SDL_Window* window = NULL;
	SDL_Renderer* gameRenderer = NULL;
	Level levels[2];

	bool isPlaying = false;

	void HideSpawnSquares() {
		for (int i = 0; i < playerSpawnSquares.size(); i++) {
			playerSpawnSquares[i]->Hide();
		}

		gameClock->StartCounting();
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
				enemies[i]->HandleCollision(allies, enemies, player);
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
		}

		if (gameClock->GetTimeRemaining() <= 0) {
			if (forest->GetDestroyedTreesPercentage() >= 70) {
				gameClock->StopCounting();
				cout << "Game over!" << endl;
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

		levels[0] = { 1, 1, 15, 60, 5, 1, 1 };
		levels[1] = { 2, 0.5, 10, 60, 5, 1, 2 };

		player = new Player(firefigherPath, gameRenderer, 0, 0, levels[0].playerSpeed, 32, 32, false);
		playerSpawnSquares[0] = new GameObject(200, 200, 200, gameRenderer, 0, 0, 0, 64, 64, true);
		playerSpawnSquares[1] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth - 64, 0, 0, 64, 64, true);
		playerSpawnSquares[2] = new GameObject(200, 200, 200, gameRenderer, 0, Util::windowHeight - 64, 0, 64, 64, true);
		playerSpawnSquares[3] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth - 64, Util::windowHeight - 64, 0, 64, 64, true);
		playerSpawnSquares[4] = new GameObject(200, 200, 200, gameRenderer, Util::windowWidth / 2 - 64, Util::windowHeight  / 2- 64, 0, 64, 64, true);

		allies[0] = new Ally(nativePath, gameRenderer, levels[0].allySpeed, 32, 32);
		allies[1] = new Ally(nativePath, gameRenderer, levels[0].allySpeed, 32, 32);
		allies[2] = new Ally(nativePath, gameRenderer, levels[0].allySpeed, 32, 32);

		forest = new Forest(gameRenderer, levels[0].timeToBurn, forestGreen, orange, gray);

		enemies[0] = new Enemy(200, 0, 0, gameRenderer, levels[0].enemySpeed, 32, 32, forest, pyroSmallPath, pyroBigPath);
		enemies[1] = new Enemy(200, 0, 200, gameRenderer, levels[0].enemySpeed, 32, 32, forest, pyroSmallPath, pyroBigPath);
		enemies[2] = new Enemy(200, 0, 200, gameRenderer, levels[0].enemySpeed, 32, 32, forest, pyroSmallPath, pyroBigPath);
		
		spawner = new Spawner(levels[0].timeToSpawnDestruction, levels[0].timeToSpawnEnemy, forest, enemies);

		for (int i = 0; i < allies.size(); i++) {
			allies[i]->Show();
		}

		gameClock = new Clock(levels[0].time, Util::FPS);
		
	}

	void HandleEvents() {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				isRunning = false;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					HandleEventsSpawnSquares(event.button.x, event.button.y);
				}
			}
		}
	};

	void Update() {
		player->Update();
		player->HandleTreeCollision(forest->GetBurningTrees());

		forest->Update();
		
		HandleAllies(forest->GetBurningTrees());

		HandleEnemies();

		spawner->Update();
		gameClock->Update();
		
		HandleLevels();
		cout << gameClock->GetTimeRemaining() << endl;
		
		UpdateSpawnSquares();
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

	FrameManager frameManager(Util::FPS);

	Game *game = new Game();
	game->Init("Resi Amazonski pragozd", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Util::windowWidth, Util::windowHeight, isFullscreen);	

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
