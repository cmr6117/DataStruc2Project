#include "Grid.h"
using namespace Simplex;



Grid::Grid(float worldRadius)
{
	Init(worldRadius);
}


Grid::~Grid()
{
}

void Simplex::Grid::Init(float worldRadius)
{
	WorldRadius = worldRadius;
	CellSize = (worldRadius * 2) / NUM_CELLS;

	//create the grid
	vector3 startPos = vector3(-WorldRadius, 0.0f, -WorldRadius);
	for (uint i = 0; i < NUM_CELLS; i++)
	{
		vector3 rowPos = vector3(startPos.x+(CellSize*i), 0.0f, startPos.z);
		for (uint j = 0; j < NUM_CELLS; j++)
		{
			vector3 cellMin = vector3(rowPos.x, 0.0f, rowPos.z + (CellSize*j));
			vector3 cellMax = vector3(cellMin.x + CellSize, 0.0f, cellMin.z + CellSize);
			//create the cell sudocode (until cell node is actually created
			CellNode *buffer = new CellNode(cellMin, cellMax, CellSize);
			CellList.push_back(buffer);
		}
	}
}

void Simplex::Grid::Update()
{
    // handle collisions within each cell
    for (uint i = 0; i < CellList.size(); i++)
    {
        CellNode* current = CellList[i];

        current->HandleCollisions();
    }
}
