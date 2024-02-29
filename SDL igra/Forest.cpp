#include <iostream>

#include "Forest.h"
#include "Util.h"   


Forest::Forest(SDL_Renderer* renderer, float timeToBurn, Color treeColor, Color fireColor, Color excavationColor) {
    this->treeColor = treeColor;
    FillTrees(renderer, timeToBurn, treeColor, fireColor, excavationColor);
}

void Forest::FillTrees(SDL_Renderer* renderer, float timeToBurn, Color treeColor, Color fireColor, Color excavationColor) {
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            trees[row][column] = new Tree(treeColor.red, treeColor.green, treeColor.blue,
                renderer, column * tileSize, row * tileSize,
                tileSize, tileSize, timeToBurn, Util::FPS,
                treeColor, fireColor, excavationColor);
        }
    }
}

void Forest::UpdateTrees() {
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            trees[row][column]->Update();
        }
    }
}

void Forest::RenderTrees() {
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            trees[row][column]->Render();
        }
    }
}

void Forest::UpdateTreesInDestruction() {
    std::vector<Tree*>::iterator it = treesInDestruction.begin();
    std::vector<Tree*> treesToAdd;

    while (it != treesInDestruction.end()) {
        if ((*it)->GetIsDestroyed()) {
            int column = (*it)->GetSpreadX() / 64;
            int row = (*it)->GetSpreadY() / 64;

            if (column >= 0 && row >= 0 && column <= columns * tileSize && row <= rows * tileSize && CanBeDestroyed(row, column)) {
                trees[row][column]->StartBurning();
                treesToAdd.push_back(trees[row][column]);
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

    for (Tree* tree : treesToAdd) {
        treesInDestruction.push_back(tree);
    }
}

void Forest::CalculateDestroyedTreesPercentage() {
    destroyedTreesPercentage = (float)destroyedTrees / totalTrees * 100;
    if (destroyedTreesPercentage >= 70) std::cout << "Game over!" << std::endl;
}

void Forest::Render() {
    RenderTrees();
}

void Forest::Update() {
    UpdateTrees();
    UpdateTreesInDestruction();
}

bool Forest::CanBeDestroyed(int row, int column) {
    if (row < 0 || row >= rows || column < 0 || column >= columns) return false;
    if (trees[row][column]->GetIsDestroyed() || trees[row][column]->GetIsInDestruction()) return false;

    return true;
}

void Forest::StartBurning(int row, int column) {
    trees[row][column]->StartBurning();
    treesInDestruction.push_back(trees[row][column]);
}

void Forest::RandomStartDestroying(bool burn) {
    int row = rand() % rows;
    int column = rand() % columns;

    if (CanBeDestroyed(row, column)) {
        if (burn) StartBurning(row, column);
        else StartExcavating(row, column);
    }
    else {
        RandomStartDestroying(burn);
    }
}

void Forest::StartExcavating(int row, int column) {
    trees[row][column]->StartExcavating();
    treesInDestruction.push_back(trees[row][column]);
}

void Forest::RemoveBurningTree(Tree* tree) {
    std::vector<Tree*>::iterator it = std::find(treesInDestruction.begin(), treesInDestruction.end(), tree);

    if (it != treesInDestruction.end()) {
        treesInDestruction.erase(it);
    }
}

Tree* Forest::GetCollidedTree(int x, int y) {
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

std::vector<Tree*> Forest::GetBurningTrees() const {
    return treesInDestruction;
}

void Forest::Reset(float timeToBurn) {
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            trees[row][column]->Reset(timeToBurn, treeColor.red, treeColor.green, treeColor.blue);
        }
    }
}

float Forest::GetDestroyedTreesPercentage() {
    return destroyedTreesPercentage;
}
