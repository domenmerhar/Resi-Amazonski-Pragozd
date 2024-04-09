#include <string>
#include <fstream>
#include <iostream>

#include "Util.h"
#include "Score.h"

using namespace std;

const int Util::windowWidth = 832;
const int Util::windowHeight = 640;
const int Util::FPS = 60;

bool Util::WithinBoundsX(int x, int width) {
    return x >= 0 && x <= windowWidth - width * 2;
}

bool Util::WithinBoundsY(int y, int height) {
    return y >= 0 && y <= windowHeight - height;
}

int Util::ResetX(int x, int width) {
    if (x < 0) {
        return 0;
    }
    else {
        return windowWidth - width * 2;
    }
}

int Util::ResetY(int y, int height) {
    if (y < 0) {
        return 0;
    }
    else {
        return windowHeight - height;
    }
}

int Util::GetRandomX(int width) {
    return rand() % (windowWidth - width * 2);
}

int Util::GetRandomY(int height) {
    return rand() % (windowHeight - height * 2);
}

const char* Util::IntToCharPointer(int number) {
    string strNumber = to_string(number);
    char* charPointer = new char[strNumber.length() + 1];
    strcpy(charPointer, strNumber.c_str());
    return charPointer;
}

void Util::SaveScore(int score, char name[]) {
	ifstream original("Assets/Score/scores.bin", ios::binary);

	if (!original.is_open()) {
		cout << "ScoreSaver: File not found!" << endl;
		return;
	}

	struct Score scoreToSave;
	scoreToSave.score = score;
	strcpy(scoreToSave.name, name);

	ofstream copy("Assets/Score/tmp.bin", ios::binary);
	struct Score curr;
	bool isSaved = false;

	while (original.read((char*)&curr, sizeof(curr)))
	{
		if (!isSaved && scoreToSave.score > curr.score) {
			copy.write((char*)&scoreToSave, sizeof(scoreToSave));
			isSaved = true;
		}

		copy.write((char*)&curr, sizeof(curr));
	}

	if (!isSaved && scoreToSave.score > curr.score)
		copy.write((char*)&scoreToSave, sizeof(scoreToSave));

	original.close();;
	copy.close();

	remove("Assets/Score/scores.bin");

	if (rename("Assets/Score/tmp.bin", "Assets/Score/scores.bin") != 0) {
		cout << "Error renaming file: " << strerror(errno) << endl;
	}
}

void Util::PrintScore() {
	ifstream file("Assets/Score/scores.bin", ios::binary);

	if (!file.is_open()) {
		cout << "ScoreSaver: File not found!" << endl;
		return;
	}

	struct Score curr;
	while (file.read((char*)&curr, sizeof(curr))) {
		cout << curr.score << " " << curr.name << endl;
	}

	file.close();
}
