#include <bangtal>
#include <iostream>

using namespace bangtal;
using namespace std;


enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};

enum class Turn {
	PLAYER,		// black
	COMPUTER	// white
};

static Turn turn;
ObjectPtr board[8][8];
State boardState[8][8];
TimerPtr timer;



void setState(int y, int x, State state) {
	string image;
	switch (state) {
	case State::BLANK:
		image = "images/blank.png";	break;
	case State::BLACK:
		image = "images/black.png";	break;
	case State::WHITE:
		image = "images/white.png"; break;
	case State::POSSIBLE:
		image = (turn == Turn::PLAYER) ? "images/black possible.png" : "images/white possible.png";	break;
	}
	board[y][x]->setImage(image);
	boardState[y][x] = state;
}



int checkAround(int y, int x, int dy, int dx, int func) {

	State self = (turn == Turn::PLAYER) ? State::BLACK : State::WHITE;
	State other = (turn == Turn::PLAYER) ? State::WHITE : State::BLACK;

	bool isPossible = false;
	for (y += dy, x += dx; y >= 0 && y < 8 && x >= 0 && x < 8; y += dy, x += dx) {
		if (boardState[y][x] == other) {
			isPossible = true;
		}
		else if (boardState[y][x] == self) {
			switch (func) {
			case 0: // check possible
				return isPossible;
				break;
			case 1: // reverse
				for (y -= dy, x -= dx; y >= 0 && y < 8 && x >= 0 && x < 8; y -= dy, x -= dx) {
					if (boardState[y][x] == other)
						setState(y, x, self);
					else
						return false;
				}
				break;
			case 2: // com operation
				int count = 0;
				for (y -= dy, x -= dx; y >= 0 && y < 8 && x >= 0 && x < 8; y -= dy, x -= dx) {
					if (boardState[y][x] == other)
						count++;
					else
						return count;
				}
				break;
			} // switch (func)			
		}
		else {
			return false;
		}
	}

	return false;

}



bool checkPossible(int y, int x) {

	if (boardState[y][x] == State::BLACK || boardState[y][x] == State::WHITE) return false;
	setState(y, x, State::BLANK);


	int deltha[8][2] = { {-1, -1},
						{-1,  0},
						{-1,  1},
						{ 0, -1},
						{ 0,  1},
						{ 1, -1},
						{ 1,  0},
						{ 1,  1} };

	bool isTherePossible = false;
	for (auto d : deltha) {
		if (checkAround(y, x, d[0], d[1], 0)) isTherePossible = true;
	}

	return isTherePossible;
}


bool setPossible() {

	bool isTherePossible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(y, x)) {
				setState(y, x, State::POSSIBLE);
				isTherePossible = true;
			}
		}
	}
	return isTherePossible;
}



void reverse(int y, int x) {

	int deltha[8][2] = { {-1, -1},
						{-1,  0},
						{-1,  1},
						{ 0, -1},
						{ 0,  1},
						{ 1, -1},
						{ 1,  0},
						{ 1,  1} };

	for (auto d : deltha) {
		checkAround(y, x, d[0], d[1], 1);
	}
}



void comOperation() {

	int max = 0;
	int my = 0, mx = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (boardState[y][x] == State::POSSIBLE) {
				int deltha[8][2] = { {-1, -1},
						{-1,  0},
						{-1,  1},
						{ 0, -1},
						{ 0,  1},
						{ 1, -1},
						{ 1,  0},
						{ 1,  1} };

				for (auto d : deltha) {
					int temp = checkAround(y, x, d[0], d[1], 2);
					if (max < temp) {
						max = temp;
						my = y;
						mx = x;
					}
				}
			}
		}
	}


	setState(my, mx, State::WHITE);
	reverse(my, mx);
}



int countScore(State state) {

	int count = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (boardState[y][x] == state) count++;
		}
	}

	return count;
}



void showScore(State state, ObjectPtr score1, ObjectPtr score2) {

	int score = countScore(state);

	string filename1, filename2;
	if (score / 10 != 0)
		filename1 = "images/L" + to_string(score / 10) + ".png";
	else
		filename1 = "images/balnk.png";

	filename2 = "images/L" + to_string(score % 10) + ".png";

	score1->setImage(filename1);
	score2->setImage(filename2);

}



void turnOperation(ObjectPtr Bscore1, ObjectPtr Bscore2, ObjectPtr Wscore1, ObjectPtr Wscore2) {
	showScore(State::BLACK, Bscore1, Bscore2);
	showScore(State::WHITE, Wscore1, Wscore2);

	if (!setPossible()) {
		turn = (turn == Turn::PLAYER) ? Turn::COMPUTER : Turn::PLAYER;

		showMessage("돌을 놓을 자리가 없습니다. 상대편 순서로 넘어갑니다.");


		if (!setPossible()) {
			int Bscore = countScore(State::BLACK);
			int Wscore = countScore(State::WHITE);
			string str = "게임 종료 ";
			if (Bscore > Wscore)		str += "BLACK 승리!";
			else if (Bscore < Wscore)	str += "WHITE 승리!";
			else						str += "무승부";
			showMessage(str);
		}
		else if (turn == Turn::COMPUTER) {
			timer->set(1.0f);
			timer->start();
		}
	}
}



int main() {

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);


	auto background = Scene::create("  ", "images/background.png");

	auto Bscore1 = Object::create("images/blank.png", background, 750, 280);
	auto Bscore2 = Object::create("images/blank.png", background, 830, 280);
	auto Wscore1 = Object::create("images/blank.png", background, 1070, 280);
	auto Wscore2 = Object::create("images/blank.png", background, 1150, 280);


	// computer
	timer = Timer::create(1.0f);
	timer->setOnTimerCallback([&](TimerPtr timer)->bool {
		comOperation();

		turn = Turn::PLAYER;
		turnOperation(Bscore1, Bscore2, Wscore1, Wscore2);

		return true;
		});


	// player
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("images/blank.png", background, (x * 80) + 40, 600 - (y * 80));
			boardState[y][x] = State::BLANK;

			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr object, int, int, MouseAction) -> bool {

				if (turn == Turn::PLAYER && boardState[y][x] == State::POSSIBLE) {
					setState(y, x, State::BLACK);
					reverse(y, x);
					turn = Turn::COMPUTER;
					turnOperation(Bscore1, Bscore2, Wscore1, Wscore2);

					timer->set(1.0f);
					timer->start();

				}

				return true;
				});
		}
	}



	// set game
	setState(3, 3, State::WHITE);
	setState(3, 4, State::BLACK);
	setState(4, 3, State::BLACK);
	setState(4, 4, State::WHITE);

	turn = Turn::PLAYER;


	setPossible();


	startGame(background);


	return 0;
}