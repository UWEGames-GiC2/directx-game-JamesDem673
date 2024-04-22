#include "pch.h"
#include "Monster.h"
#include <dinput.h>
#include "GameData.h"
#include <math.h>
#include <iostream>
#include <stack>
#include <set>
#include <iostream>
#include <vector>

Monster::Monster(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale) :CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	GameObject::Tick(nullptr); //update my world_transform
}

Monster::~Monster()
{
	//tidy up anything I've created
}


void Monster::Tick(GameData* _GD)
{
	_GD->ePosx = GetPos().x;
	_GD->ePosz = GetPos().z;

	trackPlayer(_GD->pPosx, _GD->pPosz, _GD);
    CMOGO::Tick(_GD);
}

void Monster::trackPlayer(float playerx, float playerz, GameData* _GD)
{
	float adjactentLength;
	float oppositeLength;

	adjactentLength = playerx - m_pos.x;
	oppositeLength = playerz - m_pos.z;

	float angleLookAt = atan2(adjactentLength, oppositeLength);
	m_yaw = angleLookAt;
}


// sets up search function
void Monster::searchFunction(GameData* m_GD, int grid[ROW][COLUMN])
{
    /* Description of the Grid-
     1--> The cell is not blocked
     0--> The cell is blocked    */

    int playerX = m_GD->pPosx;
    int playerZ = m_GD->pPosz;

    int MonsterX = m_GD->ePosx;
    int MonsterZ = m_GD->ePosz;

    playerX = std::round(playerX / 15);
    playerZ = std::round(playerZ / 15);

    MonsterX = std::round(MonsterX / 15);
    MonsterZ = std::round(MonsterZ / 15);

    //format (z,x)
    Pair src = std::make_pair(MonsterZ, MonsterX);
    Pair dest = std::make_pair(playerZ, playerX);

    aStarSearch(grid, src, dest);

    if (!Path.empty())
    {
        Path.pop();

        std::pair<int, int> p = Path.top();
        SetPos(Vector3((p.second * 15) + 7.5, GetPos().y, (p.first * 15) + 7.5));
    }
}

bool Monster::isValid(int row, int col)
{
    return (row >= 0) && (row < ROW) && (col >= 0)
        && (col < COLUMN);
}

bool Monster::isUnBlocked(int grid[][COLUMN], int row, int column)
{
    if (grid[row][column] == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Monster::isDestination(int row, int column, Pair dest)
{
    if (row == dest.first && column == dest.second)
        return (true);
    else
        return (false);
}

double Monster::calculateHValue(int row, int column, Pair dest)
{
    return ((double)sqrt(
        (row - dest.first) * (row - dest.first)
        + (column - dest.second) * (column - dest.second)));
}

void Monster::tracePath(cell cellDetails[][COLUMN], Pair dest)
{
    int row = dest.first;
    int column = dest.second;

    while (!(cellDetails[row][column].parent_i == row
        && cellDetails[row][column].parent_j == column))
    {
        Path.push(std::make_pair(row, column));
        int temp_row = cellDetails[row][column].parent_i;
        int temp_column = cellDetails[row][column].parent_j;
        row = temp_row;
        column = temp_column;
    }

    Path.push(std::make_pair(row, column));

}

void Monster::aStarSearch(int grid[][COLUMN], Pair src, Pair dest)
{
    // Checks to see if source and destination are valid locations
    if (isValid(src.first, src.second) == false)
    {
        printf("Source is invalid\n");
        return;
    }
    if (isValid(dest.first, dest.second) == false)
    {
        printf("Destination is invalid\n");
        return;
    }

    // Checks to make sure no obstacles are on the source or destination tiles
    if (isUnBlocked(grid, src.first, src.second) == false)
    {
        printf("Source is blocked\n");
        return;
    }
    if (isUnBlocked(grid, dest.first, dest.second) == false)
    {
        printf("Destination is blocked\n");
        return;
    }

    //Checks to see if source is at destination
    if (isDestination(src.first, src.second, dest) == true)
    {
        printf("We are already at the destination\n");
        return;
    }

    //Creates a closed list as a 2d bool array
    bool closedList[ROW][COLUMN];
    memset(closedList, false, sizeof(closedList));

    //Creates a 2D array to hold detail of a cell
    cell cellDetails[ROW][COLUMN];

    int i, j;

    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COLUMN; j++)
        {
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;

            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
        }
    }

    // Initialising parameters of starting node
    i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    // Creates open list formatted <f, <i, j>>; f = g + h; i, j are rows and columns of cell
    // 0 <= i <= ROW-1 & 0 <= j <= COL-1
    std::set<pPair> openList;

    // Add starting cell with f as 0
    Pair tempPair = std::make_pair(i, j);
    pPair tempPPair = std::make_pair(0.0, tempPair);

    openList.insert(tempPPair);


    //bool is dest is found
    bool foundDest = false;
    std::vector<int> nextStep;


    while (!openList.empty())
    {
        pPair p = *openList.begin();
        openList.erase(openList.begin());

        i = p.second.first;
        j = p.second.second;
        closedList[i][j] = true;

        /*
        Generating all the 4 successor of this cell

                  N
                  |
                  |
            W----Cell----E
                  |
                  |
                  S

        Cell-->Popped Cell (i, j)
        N -->  North       (i-1, j)
        S -->  South       (i+1, j)
        E -->  East        (i, j+1)
        W -->  West        (i, j-1) */

        //To store the 'g' 'h' and 'f' of the 4 successors
        double gNew, hNew, fNew;

        //----------- 1st Successor (North) ------------

        // Only process this cell if this is a valid one
        if (isValid(i - 1, j) == true) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i - 1, j, dest) == true) {
                // Set the Parent of the destination cell
                cellDetails[i - 1][j].parent_i = i;
                cellDetails[i - 1][j].parent_j = j;
                tracePath(cellDetails, dest);
                foundDest = true;
                return;
            }
            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            else if (closedList[i - 1][j] == false
                && isUnBlocked(grid, i - 1, j)
                == true) {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = calculateHValue(i - 1, j, dest);
                fNew = gNew + hNew;

                // If it isn�t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i - 1][j].f == FLT_MAX
                    || cellDetails[i - 1][j].f > fNew) {
                    openList.insert(std::make_pair(
                        fNew, std::make_pair(i - 1, j)));

                    // Update the details of this cell
                    cellDetails[i - 1][j].f = fNew;
                    cellDetails[i - 1][j].g = gNew;
                    cellDetails[i - 1][j].h = hNew;
                    cellDetails[i - 1][j].parent_i = i;
                    cellDetails[i - 1][j].parent_j = j;
                }
            }
        }

        //----------- 2nd Successor (South) ------------

        // Only process this cell if this is a valid one
        if (isValid(i + 1, j) == true) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i + 1, j, dest) == true) {
                // Set the Parent of the destination cell
                cellDetails[i + 1][j].parent_i = i;
                cellDetails[i + 1][j].parent_j = j;
                tracePath(cellDetails, dest);
                foundDest = true;
                return;
            }
            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            else if (closedList[i + 1][j] == false
                && isUnBlocked(grid, i + 1, j)
                == true) {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = calculateHValue(i + 1, j, dest);
                fNew = gNew + hNew;

                // If it isn�t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i + 1][j].f == FLT_MAX
                    || cellDetails[i + 1][j].f > fNew) {
                    openList.insert(make_pair(
                        fNew, std::make_pair(i + 1, j)));
                    // Update the details of this cell
                    cellDetails[i + 1][j].f = fNew;
                    cellDetails[i + 1][j].g = gNew;
                    cellDetails[i + 1][j].h = hNew;
                    cellDetails[i + 1][j].parent_i = i;
                    cellDetails[i + 1][j].parent_j = j;
                }
            }
        }

        //----------- 3rd Successor (East) ------------

        // Only process this cell if this is a valid one
        if (isValid(i, j + 1) == true) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i, j + 1, dest) == true) {
                // Set the Parent of the destination cell
                cellDetails[i][j + 1].parent_i = i;
                cellDetails[i][j + 1].parent_j = j;
                tracePath(cellDetails, dest);
                foundDest = true;
                return;
            }

            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            else if (closedList[i][j + 1] == false
                && isUnBlocked(grid, i, j + 1)
                == true) {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = calculateHValue(i, j + 1, dest);
                fNew = gNew + hNew;

                // If it isn�t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i][j + 1].f == FLT_MAX
                    || cellDetails[i][j + 1].f > fNew) {
                    openList.insert(std::make_pair(
                        fNew, std::make_pair(i, j + 1)));

                    // Update the details of this cell
                    cellDetails[i][j + 1].f = fNew;
                    cellDetails[i][j + 1].g = gNew;
                    cellDetails[i][j + 1].h = hNew;
                    cellDetails[i][j + 1].parent_i = i;
                    cellDetails[i][j + 1].parent_j = j;
                }
            }
        }

        //----------- 4th Successor (West) ------------

        // Only process this cell if this is a valid one
        if (isValid(i, j - 1) == true) {
            // If the destination cell is the same as the
            // current successor
            if (isDestination(i, j - 1, dest) == true) {
                // Set the Parent of the destination cell
                cellDetails[i][j - 1].parent_i = i;
                cellDetails[i][j - 1].parent_j = j;
                tracePath(cellDetails, dest);
                foundDest = true;
                return;
            }

            // If the successor is already on the closed
            // list or if it is blocked, then ignore it.
            // Else do the following
            else if (closedList[i][j - 1] == false
                && isUnBlocked(grid, i, j - 1)
                == true) {
                gNew = cellDetails[i][j].g + 1.0;
                hNew = calculateHValue(i, j - 1, dest);
                fNew = gNew + hNew;

                // If it isn�t on the open list, add it to
                // the open list. Make the current square
                // the parent of this square. Record the
                // f, g, and h costs of the square cell
                //                OR
                // If it is on the open list already, check
                // to see if this path to that square is
                // better, using 'f' cost as the measure.
                if (cellDetails[i][j - 1].f == FLT_MAX
                    || cellDetails[i][j - 1].f > fNew) {
                    openList.insert(std::make_pair(
                        fNew, std::make_pair(i, j - 1)));

                    // Update the details of this cell
                    cellDetails[i][j - 1].f = fNew;
                    cellDetails[i][j - 1].g = gNew;
                    cellDetails[i][j - 1].h = hNew;
                    cellDetails[i][j - 1].parent_i = i;
                    cellDetails[i][j - 1].parent_j = j;
                }
            }
        }

    }

    // When the destination cell is not found and the open
    // list is empty, then we conclude that we failed to
    // reach the destination cell. This may happen when the
    // there is no way to destination cell (due to
    // blockages)
    if (foundDest == false)
    {
        printf("Failed to find the Destination Cell\n");
    }
    else
    {
        std::cout << openList.size();
    }

}
