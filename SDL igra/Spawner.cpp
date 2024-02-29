#include "Spawner.h"
#include <cstdlib> // Include for rand() function

Spawner::Spawner(float timeToSpawnFire, float timeToRespawnEnemy, Forest* forest, std::vector<Enemy*>& enemies)
    : timeToSpawnFire(timeToSpawnFire), timeToRespawnEnemy(timeToRespawnEnemy),
    forest(forest), enemies(enemies),
    framesToSpawnFire(timeToSpawnFire* Util::FPS), framesToRespawnEnemy(timeToRespawnEnemy* Util::FPS),
    clockToSpawnFire(0), clockToRespawnEnemy(0) {}

void Spawner::Reset(float timeToSpawnFire, float timeToRespawnEnemy) {
    this->timeToSpawnFire = timeToSpawnFire;
    this->framesToSpawnFire = timeToSpawnFire * Util::FPS;
    this->clockToSpawnFire = 0;

    this->timeToRespawnEnemy = timeToRespawnEnemy;
    this->framesToRespawnEnemy = timeToRespawnEnemy * Util::FPS;
    this->clockToRespawnEnemy = 0;
}

void Spawner::Update() {
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
