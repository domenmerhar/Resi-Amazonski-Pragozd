#include "ScoreCounter.h"
#include <iostream>

using namespace std;

ScoreCounter::ScoreCounter() {
	score = 0;
}

void ScoreCounter::AddScore(int scoreToAdd) {
	score += scoreToAdd;
}

void ScoreCounter::SaveScore() {
	// Save score to file
}

void ScoreCounter::ResetScore() {
	score = 0;
}

int ScoreCounter::GetScore() {
	return score;
}