#pragma once

struct Level {
	float timeToBurn,
		timeToSpawnDestruction,
		timeToSpawnEnemy;

	int time,
		playerSpeed,
		allySpeed,
		enemySpeed;
};
