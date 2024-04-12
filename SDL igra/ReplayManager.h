#pragma once

#include "Position.h"
#include <fstream>

using namespace std;

class ReplayManager {
	ofstream outputFile;
	ifstream inputFile;

	const char* pathOriginal = "Assets/Replay/scores.bin";
	const char* pathTmp = "Assets/Replay/scoresTmp.bin";

	bool isReplaying = false;

public:
	void SavePosition(struct Position positionToSave);
	void ClearFile();
	struct Position GetPosition();
	void SetIsReplaying(bool toSet);
	bool GetIsReplaying();
};