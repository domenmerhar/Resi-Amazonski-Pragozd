#pragma once

#include <SDL.h>
#include <vector>
#include "Tree.h"
#include "Color.h" 

class Forest {
    const int rows = 10;
    const int columns = 13;
    const int tileSize = 64;
    float timeToBurn;

    const int totalTrees = rows * columns;
    int destroyedTrees = 0;
    float destroyedTreesPercentage = 0;

    Tree* trees[10][13];
    std::vector<Tree*> treesInDestruction;

    Color treeColor;

    void FillTrees(SDL_Renderer* renderer, float timeToBurn, Color treeColor, Color fireColor, Color excavationColor);
    void UpdateTrees();
    void RenderTrees();
    void UpdateTreesInDestruction();
    void CalculateDestroyedTreesPercentage();

public:
    Forest(SDL_Renderer* renderer, float timeToBurn, Color treeColor, Color fireColor, Color excavationColor);
    void Render();
    void Update();
    bool CanBeDestroyed(int row, int column);
    void StartBurning(int row, int column);
    void RandomStartDestroying(bool burn);
    void StartExcavating(int row, int column);
    void RemoveBurningTree(Tree* tree);
    Tree* GetCollidedTree(int x, int y);
    std::vector<Tree*> GetBurningTrees() const;
    void Reset(float timeToBurn);
    float GetDestroyedTreesPercentage();
};
