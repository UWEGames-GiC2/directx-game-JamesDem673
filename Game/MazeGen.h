#ifndef _MAZEGEN_H_
#define _MAZEGEN_H_
#include "CMOGO.h"
#include <iostream>
#include <algorithm>
#include <random>

#define ROW 19
#define COLUMN 19

class MazeGen
{

public:
    MazeGen();
    ~MazeGen();

private:

    //sets up definitions of the Maze Generation
    const int WALL = 0;
    const int EMPTY = 1;
    const int START = 2;
    const int END = 3;

    const int DX[4] = { 0, 0, 1, -1 };
    const int DZ[4] = { 1, -1, 0, 0 };

	void dfs(int x, int z, int maze[ROW][COLUMN]);

public:
	void createMaze();
};
#endif