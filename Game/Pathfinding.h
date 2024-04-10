#ifndef _PATHFINDING_H
#define _PATHFINDING_H_

#include <bits.h>
#include "GameData.h"

#define ROW 21
#define COLUMN 21

// Shortcut for int pair
typedef std::pair<int, int> Pair;

// Shortcut for int, pair type pair
typedef std::pair<double, std::pair<int, int> > pPair;

//structure for tile of the maze grid
struct cell
{
	int parent_i, parent_j;
	double f, g, h;
};


class Pathfinding
{

public:
	void aStarSearch(int grid[][COLUMN], Pair src, Pair dest);
	void searchFunction(GameData* _GD, int grid[ROW][COLUMN]);



private:
	bool isValid(int row, int col);
	bool isUnBlocked(int grid[][COLUMN], int row, int column);
	bool isDestination(int row, int coumn, Pair dest);
	double calculateHValue(int row, int col, Pair dest);
	void tracePath(cell cellDetails[][COLUMN], Pair dest);
};

#endif

