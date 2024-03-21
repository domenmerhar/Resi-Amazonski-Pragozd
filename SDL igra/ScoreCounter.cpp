#include "ScoreCounter.h"
#include <iostream>

using namespace std;

ScoreCounter::ScoreCounter() {
	score = 0;
}

void ScoreCounter::AddScore(int scoreToAdd) {
	score += scoreToAdd;
	cout << score << endl;
}

void ScoreCounter::SaveScore() {
	// Save score to file
}

void ScoreCounter::ResetScore() {
	score = 0;
}