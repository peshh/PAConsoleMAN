#include "Ghost.h"
#include "Calculus.h"

Ghost::Ghost(int r, int c)
{
	row = r;
	col = c;
	targetRow = exitRow;
	targetCol = c;
}

Ghost::Ghost()
{

}

Ghost::~Ghost()
{

}

void Ghost::move(float distance)
{
	if ((int)row > targetRow)
	{
		row -= distance;
	}
	else if ((int)row < targetRow)
	{
		row += distance;
	}
	else if ((int)col > targetCol)
	{
		col -= distance;
	}
	else if ((int)col < targetCol)
	{
		col += distance;
	}
}

void Ghost::findPath(int r, int c, int** maze)
{
	int pathMaze[31][28];

	for (int rows = 0; rows < 31; rows++)
	{
		for (int cols = 0; cols < 28; cols++)
		{
			if (maze[rows][cols] == 1 || maze[rows][cols] == 5)
			{
				pathMaze[rows][cols] = -1;
			}
			else
			{
				pathMaze[rows][cols] = 31 * 28;
			}
		}
	}


}

void Ghost::getTargets(deque<deque<int>> newTargets)
{
	targets.clear();
	targets.swap(newTargets);
	changeTarget();
}

void Ghost::changeTarget()
{
	if (!targets.empty())
	{
		deque<int> target = targets.front();
		targets.pop_front();

		targetRow = target.front();
		targetCol = target.back();
	}
}

void Ghost::exitHome()
{
	targetRow = exitRow;
	targetCol = 13;
}

