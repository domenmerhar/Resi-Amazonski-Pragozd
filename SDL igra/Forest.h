#pragma once

#include <SDL.h>
#include <vector>
#include "Tree.h"
#include "Util.h"
/*
class Forest {
private:
    const int rows = 10;
    const int columns = 13;
    const int tileSize = 64;
    float timeToBurn;

    const int totalTrees = rows * columns;
    int destroyedTrees = 0;
    float destroyedTreesPercentage = 0;

    Tree* trees[10][13];
    std::vector<Tree*> treesInDestruction;

    void FillTrees(SDL_Renderer* renderer, float timeToBurn, Color treeColor, Color fireColor, Color excavationColor);
    void UpdateTrees();
    void RenderTrees();
    void UpdateTreesInDestruction();
    void CalculateDestroyedTreesPercentage();

    bool CanBeDestroyed(int row, int column);
    void StartBurning(int row, int column);
    void StartExcavating(int row, int column);
    void RemoveBurningTree(Tree* tree);

public:
    Forest(SDL_Renderer* renderer, float timeToBurn, Color treeColor, Color fireColor, Color excavationColor);

    void Render();
    void Update();
    void RandomStartDestroying(bool burn);
    Tree* GetCollidedTree(int x, int y);
    std::vector<Tree*> GetBurningTrees() const;
    void Reset(float timeToBurn);
    float GetDestroyedTreesPercentage();
};

*/