#include "pch.h"
#include "MazeGen.h"

MazeGen::MazeGen()
{
}

MazeGen::~MazeGen()
{
}



void MazeGen::createMaze()
{
	int maze[ROW][COLUMN]
		= { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{ 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
			{ 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
			{ 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
			{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
			{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
			{ 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
			{ 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
			{ 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0},
			{ 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
			{ 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
			{ 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
			{ 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
			{ 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
			{ 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0},
			{ 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
			{ 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0} };

	//sets all values in maze to 0
	for (int x = 0; x < ROW; x++)
	{
		for (int z = 0; z < COLUMN; z++)
		{
			maze[z][x] = WALL;
		}
	}

	//sets start and end
	maze[0][0] = START;
	maze[29][29] = END;

	dfs(0, 0, maze);

	//print maze
	for (int x = 0; x < ROW; x++)
	{
		for (int z = 0; z < COLUMN; z++)
		{
			std::cout << maze[z][x] << " ";
		}

		std::cout << endl;

	}
}

void MazeGen::dfs(int x, int z, int maze[ROW][COLUMN])
{
	int directions[4] = { 0, 1, 2, 3 };

	//std::shuffle(directions[0], directions[3], random);

	for (int d = 0; d < 4; d++)
	{
		int nx = x + DX[d];
		int nz = z + DZ[d];


		if (nx < 0 || nx >= ROW || nz < 0 || nz >= COLUMN || maze[nx][nz] != WALL)
		{
			continue;
		}

		// Create maze path
		maze[x + DX[d] / 2][z + DZ[d] / 2] = EMPTY;
		maze[nx][nz] = EMPTY;

	}
}