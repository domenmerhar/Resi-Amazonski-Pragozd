#pragma once

class ScoreCounter {
	int score;
public:
	ScoreCounter();

	void AddScore(int scoreToAdd);

	void SaveScore();

	void ResetScore();

	void RenderScore();
	
	int GetScore();
};
