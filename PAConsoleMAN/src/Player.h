#pragma once
class Player
{
public:
	float row, col;
	char direction;
	int points = 0;

	Player();
	Player(int r, int c);
	~Player();

	void move(float distance);
	void changeDirection(char d);

private:


};

