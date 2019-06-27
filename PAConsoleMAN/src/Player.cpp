#include "Player.h"

Player::Player(int r, int c)
{
	row = r;
	col = c;
	direction = 'l';
}

Player::Player()
{

}

Player::~Player()
{

}

void Player::changeDirection(char d)
{
	direction = d;
}

void Player::move(float distance)
{
	switch (direction)
	{
	case 'u':
		row -= distance; 
		break;
	case 'd': 
		row += distance; 
		break;
	case 'l': 
		col -= distance; 
		break;
	case 'r': 
		col += distance; 
		break;
	default:
		break;
	}
}
