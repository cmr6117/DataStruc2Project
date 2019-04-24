#include "CellNode.h"
using namespace Simplex;

CellNode::CellNode(vector3 cellMin, vector3 cellMax, float cellSize)
{
    m_fRadius = cellSize / 2.f;
    m_v3GlobalCenter = cellMin + ((cellMax - cellMin) / 2.f);
}


CellNode::~CellNode()
{
}

void Simplex::CellNode::HandleCollisions()
{
    for (uint i = 0; i < EntitiesInside.size(); i++) 
    {
        MyEntity* current = EntitiesInside[i];

        for (uint j = 0; j < EntitiesInside.size(); j++)
        {
            // ignore self
            if (i == j)
                continue;

            MyEntity* other = EntitiesInside[j];

            if (current->IsColliding(other))
            {
                current->ResolveCollision(other);
            }
        }
    }
}
