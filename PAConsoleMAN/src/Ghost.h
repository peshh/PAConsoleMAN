#pragma once
#include <utility>
#include <iostream>
#include <deque>

using namespace std;
using std::deque;

class Ghost
{
public:
	const int exitRow = 11;
	float row, col;
	int  targetRow, targetCol;
	deque<deque<int>> targets;
	string mode = "exit";
	bool isDead = false;

	Ghost();
	Ghost(int r, int c);
	~Ghost();

	void move(float distance);
	void findPath(int r, int c, int **maze);
	void getTargets(deque<deque<int>> newTargets);
	void changeTarget();
	void exitHome();


private:


};

