#pragma once

#include <vector>
#include "Forest.h" 
#include "Enemy.h" 

class Spawner {
    float timeToSpawnFire;
    int framesToSpawnFire;
    int clockToSpawnFire;

    float timeToRespawnEnemy;
    int framesToRespawnEnemy;
    int clockToRespawnEnemy;

    Forest* forest;
    std::vector<Enemy*> enemies;

public:
    Spawner(float timeToSpawnFire, float timeToRespawnEnemy, Forest* forest, std::vector<Enemy*>& enemies);
    void Reset(float timeToSpawnFire, float timeToRespawnEnemy);
    void Update();
};
