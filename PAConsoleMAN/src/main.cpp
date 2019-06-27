#include <iostream>
using namespace std;

#include "engine\Engine.h"
#include "Player.h"
#include "Ghost.h"
#include "Maze.h"

int initialPlayerRow = 23;
int initialPlayerCol = 14;
int initialGhostCoords[4][2] = { {14, 12},
								 {14, 13},
								 {14, 14},
								 {14, 15} };
float elapsedTimeOverall = 0.0f;
int elapsedTimeSeconds = 0;
float timeForUnlimitedPower = -1;
int unlimitedPowerPoints = 200;
int pelletsToWin = 244;

Player player(initialPlayerRow, initialPlayerCol);
Ghost* ghosts[4] = { new Ghost(initialGhostCoords[0][0], initialGhostCoords[0][1]),
new Ghost(initialGhostCoords[1][0], initialGhostCoords[1][1]),
new Ghost(initialGhostCoords[2][0], initialGhostCoords[2][1]),
new Ghost(initialGhostCoords[3][0], initialGhostCoords[3][1]) };
char playerDirection;


class Game : public olcConsoleGameEngine
{
public:
	Game()
	{

	}

private:
	struct sNode
	{
		bool bObstacle = false;			// Is the node an obstruction?
		bool bVisited = false;			// Have we searched this node before?
		float fGlobalGoal;				// Distance to goal so far
		float fLocalGoal;				// Distance to goal if we took the alternative route
		int x;							// Nodes position in 2D space
		int y;
		vector<sNode*> vecNeighbours;	// Connections to neighbours
		sNode* parent;					// Node connecting to this node that offers shortest parent
	};

	sNode* nodes = nullptr;
	int nMapWidth = mazeCols;
	int nMapHeight = mazeRows;

	sNode* nodeStart = nullptr;
	sNode* nodeEnd = nullptr;


protected:
	virtual bool OnUserCreate()
	{
		for (int i = 0; i < 31; i++)
		{
			for (int j = 0; j < 28; j++)
			{
				if (maze[i][j] == 1)
				{
					Draw(j, i, 0x2588, 0x0001); //Draw wall
				}
				else if (maze[i][j] == 2)
				{
					Draw(j, i, 0x25e6, 0x000e); //Draw pellet
				}
				else if (maze[i][j] == 3)
				{
					Draw(j, i, 0x25cf, 0x000e); //Draw coin
				}
				else if (maze[i][j] == 4)
				{
					//Draw the ghosts
					Draw(j, i, 0x2660, 0x000c);
					j++;
					Draw(j, i, 0x2660, 0x000d);
					j++;
					Draw(j, i, 0x2660, 0x000b);
					j++;
					Draw(j, i, 0x2660, 0x000e);
				}
				else if (maze[i][j] == 5)
				{
					Draw(j, i, 0x2588, 0x0009); //Draw the gate
				}
				else if (maze[i][j] == 6)
				{
					Draw(j, i, 0x263b, 0x0006); //Draw pacman
				}
			}
		}

		nodes = new sNode[nMapWidth * nMapHeight];
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				nodes[y * nMapWidth + x].x = x; // ...because we give each node its own coordinates
				nodes[y * nMapWidth + x].y = y;
				nodes[y * nMapWidth + x].bObstacle = false;
				if (maze[y][x] == 1)
				{
					nodes[y * nMapWidth + x].bObstacle = true;
				}
				nodes[y * nMapWidth + x].parent = nullptr;
				nodes[y * nMapWidth + x].bVisited = false;
			}

		// Create connections - in this case nodes are on a regular grid
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				if (y > 0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + (x + 0)]);
				if (y < nMapHeight - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + (x + 0)]);
				if (x > 0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x - 1)]);
				if (x < nMapWidth - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 0) * nMapWidth + (x + 1)]);
			}

		// Manually positio the start and end markers so they are not nullptr
		nodeStart = &nodes[initialGhostCoords[0][1] * nMapWidth + initialGhostCoords[0][0]];
		nodeEnd = &nodes[(int)player.col * nMapWidth + (int)player.row];

		return 1;
	}

	bool playerCanChangeDirection(char d)
	{
		int row = (int)player.row;
		int col = (int)player.col;

		if (d == 'u' && maze[row - 1][col] != 1)
		{
			return 1;
		}
		else if (d == 'd' && maze[row + 1][col] != 1 && maze[row + 1][col] != 5)
		{
			return 1;
		}
		else if (d == 'l' && maze[row][col - 1] != 1)
		{
			return 1;
		}
		else if (d == 'r' && maze[row][col + 1] != 1)
		{
			return 1;
		}

		return 0;
	}
	void playerChangeDirection()
	{
		if (m_keys[VK_LEFT].bHeld && playerCanChangeDirection('l'))
		{
			player.direction = 'l';
		}
		if (m_keys[VK_RIGHT].bHeld && playerCanChangeDirection('r'))
		{
			player.direction = 'r';
		}
		if (m_keys[VK_UP].bHeld && playerCanChangeDirection('u'))
		{
			player.direction = 'u';
		}
		if (m_keys[VK_DOWN].bHeld && playerCanChangeDirection('d'))
		{
			player.direction = 'd';
		}
	}
	bool playerCanMove()
	{
		return playerCanChangeDirection(player.direction);
	}
	void incrementScore(int r, int c)
	{
		if (maze[r][c] == 2)
		{
			player.points += 10;
		}
		else if (maze[r][c] == 3)
		{
			player.points += 50;
		}
		DrawScore();
	}
	void DrawScore()
	{
		int mod = 10;
		int playerPoints = player.points;
		for (int i = 39; i > 32; i--)
		{
			Draw(i, 14, 48 + playerPoints % mod, 0x000f);
			playerPoints /= 10;
		}
	}
	void movePlayer(int r, int c, float fElapsedTime)
	{
		if (r == 14 && c == 0 && player.direction == 'l')
		{
			player.col = 27;
		}
		else if (r == 14 && c == 27 && player.direction == 'r')
		{
			player.col = 0;
		}
		else if (playerCanMove())
		{
			player.move(7.5f * fElapsedTime);
		}
	}

	bool Solve_AStar()
	{
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				nodes[y * nMapWidth + x].bVisited = false;
				nodes[y * nMapWidth + x].fGlobalGoal = INFINITY;
				nodes[y * nMapWidth + x].fLocalGoal = INFINITY;
				nodes[y * nMapWidth + x].parent = nullptr;
			}

		auto distance = [](sNode* a, sNode* b)
		{
			return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
		};

		auto heuristic = [distance](sNode* a, sNode* b)
		{
			return distance(a, b);
		};

		sNode* nodeCurrent = nodeStart;
		nodeStart->fLocalGoal = 0.0f;
		nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);

		list<sNode*> listNotTestedNodes;
		listNotTestedNodes.push_back(nodeStart);

		while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)
		{
			listNotTestedNodes.sort([](const sNode* lhs, const sNode* rhs) { return lhs->fGlobalGoal < rhs->fGlobalGoal; });

			while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
				listNotTestedNodes.pop_front();

			if (listNotTestedNodes.empty())
				break;

			nodeCurrent = listNotTestedNodes.front();
			nodeCurrent->bVisited = true;

			for (auto nodeNeighbour : nodeCurrent->vecNeighbours)
			{
				if (!nodeNeighbour->bVisited && nodeNeighbour->bObstacle == 0)
					listNotTestedNodes.push_back(nodeNeighbour);

				float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbour);

				if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal)
				{
					nodeNeighbour->parent = nodeCurrent;
					nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;

					nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
				}
			}
		}

		return true;
	}

	void moveGhost(float fElapsedTime, Ghost* ghost)
	{
		ghost->move(7.0f * fElapsedTime);
	}
	void drawAfterGhost(int i, int j)
	{
		//Draw whatever the ghost is stepping on
		if (maze[i][j] == 0 || maze[i][j] == 4)
		{
			Draw(j, i, ' ', 0x000f);
		}
		if (maze[i][j] == 1)
		{
			Draw(j, i, 0x2588, 0x0001);
		}
		else if (maze[i][j] == 2)
		{
			Draw(j, i, 0x25e6, 0x000e);
		}
		else if (maze[i][j] == 3)
		{
			Draw(j, i, 0x25cf, 0x000e);
		}
		else if (maze[i][j] == 5)
		{
			Draw(j, i, 0x2588, 0x0009);
		}
	}



	virtual bool OnUserUpdate(float fElapsedTime)
	{
		elapsedTimeOverall += fElapsedTime;
		int pRow = (int)player.row;
		int pCol = (int)player.col;

		playerChangeDirection();
		incrementScore(pRow, pCol);

		if (maze[pRow][pCol] == 3)
		{
			timeForUnlimitedPower = elapsedTimeOverall + 5;
			for (int i = 0; i < 4; i++)
			{
				ghosts[i]->mode = "run";
			}
		}

		if (maze[pRow][pCol] == 2 || maze[pRow][pCol] == 3)
		{
			pelletsToWin--;
		}
		maze[pRow][pCol] = 0; //If a pellet or a power token is on the slot, the player eats it
		Draw(player.col, player.row, ' ', 0x000f);
		movePlayer(pRow, pCol, fElapsedTime);
		Draw(player.col, player.row, 0x263b, 0x0006);

		//Looping through the ghosts
		for (int i = 0; i < 4; i++)
		{
			Ghost* ghost = ghosts[i];

			int gRow = (int)ghost->row;
			int gCol = (int)ghost->col;

			//If the ghost goes back to the starting point, it revives
			if (gRow == initialGhostCoords[i][0] && gCol == initialGhostCoords[i][1])
			{
				ghost->isDead = false;
			}

			//Changing ghost behaviours
			if (gRow >= 12 && gRow <= 16 &&
				gCol >= 10 && gCol <= 17)
			{
				ghost->mode = "exit";
			}
			else if (ghost->isDead)
			{
				ghost->mode = "return";
			}
			else if (!ghost->isDead && timeForUnlimitedPower < elapsedTimeOverall)
			{
				ghost->mode = "chase";
			}

			//Changing ghost path depending on behaviour
			if (ghost->mode == "exit")
			{
				nodeStart = &nodes[gRow * nMapWidth + gCol];
				nodeEnd = &nodes[(initialGhostCoords[i][0] + 3) * nMapWidth + initialGhostCoords[i][1]];
			}
			else if (ghost->mode == "chase")
			{
				nodeStart = &nodes[gRow * nMapWidth + gCol];
				nodeEnd = &nodes[pRow * nMapWidth + pCol];
			}
			else if (ghost->mode == "return")
			{
				nodeStart = &nodes[gRow * nMapWidth + gCol];
				nodeEnd = &nodes[initialGhostCoords[i][0] * nMapWidth + initialGhostCoords[i][1]];
			}
			else if (ghost->mode == "run")
			{
				nodeStart = &nodes[gRow * nMapWidth + gCol];
				nodeEnd = &nodes[(mazeRows - 1 - pRow) * nMapWidth + (mazeCols - 1 - pCol)];
			}


			//Getting new path for the ghost
			if ((int)(elapsedTimeOverall - i) % 4 == 0 && (int)elapsedTimeOverall != elapsedTimeSeconds)
			{
				elapsedTimeSeconds++;
			Solve_AStar();
				deque<deque<int>> targets;
				sNode node = *nodeEnd;
				while (true)
				{
					deque<int> target = { node.y, node.x };
					targets.push_front(target);
					if (node.parent != nullptr)
					{
						node = *node.parent;
					}
					else
					{
						break;
					}
				}
				ghost->getTargets(targets);
			}

			if (gRow == ghost->targetRow && gCol == ghost->targetCol)
			{
				//Normalize the ghost coordinates
				/*ghost->row = gRow;
				ghost->col = gCol;*/

				//Target the next tile in the path
				ghost->changeTarget();
			}

			drawAfterGhost(ghost->row, ghost->col);
			moveGhost(fElapsedTime, ghost);

			//Draw the ghost in it's current state
			if (ghost->mode == "return")
			{
				Draw(ghost->col, ghost->row, 34, 0x000f);
			}
			else if (ghost->mode == "run")
			{
				Draw(ghost->col, ghost->row, 0x2660, 0x000f);
			}
			else
			{
				switch (i)
				{
				case 0:
					Draw(ghost->col, ghost->row, 0x2660, 0x000c);
					break;
				case 1:
					Draw(ghost->col, ghost->row, 0x2660, 0x000d);
					break;
				case 2:
					Draw(ghost->col, ghost->row, 0x2660, 0x000b);
					break;
				case 3:
					Draw(ghost->col, ghost->row, 0x2660, 0x000e);
					break;
				}
			}
		}

		if (timeForUnlimitedPower >= elapsedTimeOverall)
		{
			//Player eating the running ghosts
			for (int i = 0; i < 4; i++)
			{
				if (ghosts[i]->mode == "run"
					&& (int)ghosts[i]->row == (int)player.row
					&& (int)ghosts[i]->col == (int)player.col)
				{
					ghosts[i]->isDead = true;
					ghosts[i]->mode = "return";
					player.points += unlimitedPowerPoints;
					unlimitedPowerPoints *= 2;
				}
			}
		}
		else
		{
			unlimitedPowerPoints = 200;

			//Player dying
			for (int i = 0; i < 4; i++)
			{
				if ((int)ghosts[i]->row == (int)player.row
					&& (int)ghosts[i]->col == (int)player.col)
				{
					Fill(0, 0, 40, 31, ' ', 0x000f);
					Draw(12, 14, 'G', 0x000f);
					Draw(13, 14, 'A', 0x000f);
					Draw(14, 14, 'M', 0x000f);
					Draw(15, 14, 'E', 0x000f);
					Draw(16, 14, ' ', 0x000f);
					Draw(17, 14, 'O', 0x000f);
					Draw(18, 14, 'V', 0x000f);
					Draw(19, 14, 'E', 0x000f);
					Draw(20, 14, 'R', 0x000f);
					return false;
				}
			}
		}

		//Player winning
		if (pelletsToWin == 0)
		{
			Fill(0, 0, 40, 31, ' ', 0x000f);
			Draw(12, 14, 'Y', 0x000f);
			Draw(13, 14, 'O', 0x000f);
			Draw(14, 14, 'U', 0x000f);
			Draw(15, 14, ' ', 0x000f);
			Draw(16, 14, 'W', 0x000f);
			Draw(17, 14, 'I', 0x000f);
			Draw(18, 14, 'N', 0x000f);
			DrawScore();
			return false;
		}

		return true;
	}
};

int main()
{
	Game game;
	game.ConstructConsole(40, 31, 24, 24);
	game.Start();
	char ch;
	cin >> ch;
}