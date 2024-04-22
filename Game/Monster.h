#ifndef _MONSTER_H_
#define _MONSTER_H_
#include "CMOGO.h"
#include <bits.h>
#include "GameData.h"
#include <stack>

#define ROW 21
#define COLUMN 21

typedef std::pair<int, int> Pair;
typedef std::pair<double, std::pair<int, int> > pPair;

//structure for tile of the maze grid
struct cell
{
	int parent_i, parent_j;
	double f, g, h;
};

//=================================================================
//Base Monster Class (Controls movement)
//=================================================================

class Monster : public CMOGO
{

public:
	Monster(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale);
	~Monster();

	virtual void Tick(GameData* _GD) override;
	void trackPlayer(float playerx, float playerz, GameData* _GD);
	void aStarSearch(int grid[][COLUMN], Pair src, Pair dest);
	void searchFunction(GameData* _GD, int grid[ROW][COLUMN]);

	int s1 = 0;
	int s2 = 0;
	std::stack<Pair> Path;


private:
	float speed = 3.0f;
	
	bool isValid(int row, int col);
	bool isUnBlocked(int grid[][COLUMN], int row, int column);
	bool isDestination(int row, int coumn, Pair dest);
	double calculateHValue(int row, int col, Pair dest);
	void tracePath(cell cellDetails[][COLUMN], Pair dest);
};

#endif

