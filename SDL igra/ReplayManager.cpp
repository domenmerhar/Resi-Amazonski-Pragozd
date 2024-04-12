#include "ReplayManager.h"
#include "Position.h"

#include <iostream>
#include <fstream>

using namespace std;

void ReplayManager::SavePosition(struct Position positionToSave) {
	if (inputFile.is_open()) inputFile.close();

	outputFile.open(pathTmp, ios::binary | ios::app);

	if (!outputFile.is_open()) {
		cout << "ReplayManager: File not found!" << endl;
		return;
	}

	outputFile.write((char*)&positionToSave, sizeof(positionToSave));
	outputFile.close();
}

void ReplayManager::ClearFile() {
	outputFile.open(pathTmp, ios::binary | ios::trunc);
	outputFile.close();
}

struct Position ReplayManager::GetPosition() {
	if (outputFile.is_open()) outputFile.close();

	struct Position position;
	position.x = -1;
	position.y = -1;

	if (!inputFile.is_open()) inputFile.open(pathTmp, ios::binary);

	if (!inputFile.is_open()) {
		cout << "ReplayManager: File not open!" << endl;
		return position;
	}

	if (!inputFile.read((char*)&position, sizeof(position)))
		cout << "ReplayManager: can't read file!" << endl;

	return position;
}

void ReplayManager::SetIsReplaying(bool toSet) {
	isReplaying = toSet;
}

bool ReplayManager::GetIsReplaying() {
	return isReplaying;
}