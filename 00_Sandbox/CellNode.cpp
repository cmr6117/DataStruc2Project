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

vector3 Simplex::CellNode::GetCenter() { return m_v3GlobalCenter; }
float Simplex::CellNode::GetRadius() { return m_fRadius; }

void Simplex::CellNode::HandleCollisions()
{
	// if there's 1 or fewer entities in this cell, there's nothing to collide with
	if (EntitiesInside.size() < 2)
		return;


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

	RefreshEntities();
}

void Simplex::CellNode::AddLocalEntity(MyEntity * newEntity)
{
	EntitiesInside.push_back(newEntity);
}

std::vector<MyEntity*>::iterator Simplex::CellNode::RemoveLocalEntity(uint a_uIndex)
{
	return EntitiesInside.erase(EntitiesInside.begin() + a_uIndex);
}

//I think this is causing some slow up
void Simplex::CellNode::RefreshEntities()
{
	//If there are no entities in cell, no need to refresh
	if (EntitiesInside.size() == 0)
		return;

	//iterator variable to be able to remove entities and still loop through without error
	std::vector<MyEntity*>::iterator iter;

	//On cell class, loop through local entities to see if they've since left the bounds of the cell
	for (iter = EntitiesInside.begin(); iter != EntitiesInside.end(); ) 
	{
		//get index as an int
		//NOTE: std::distance is different than glm::distance, which is a distance function
		//std::distance is to get the distance between iterators, so you can determine the int of the index you're on
		int index = std::distance(EntitiesInside.begin(), iter);

		//If the object is a fence, it will never need reassignment so skip over calculating
		if (EntitiesInside[index]->GetUniqueID().find("fence") != std::string::npos)
		{
			++iter;
		}
		else
		{
			//determine center of entity
			vector3 entityCenter = (EntitiesInside[index]->GetRigidBody())->GetCenterGlobal();

			//if entity is outside of the radius, it has left the cell
			if (glm::distance(m_v3GlobalCenter, entityCenter) > m_fRadius)
			{
				EntitiesInside[index]->needReassign = true;
				iter = RemoveLocalEntity(index); //will return next valid iterator
			}
			else //else, continue looping through
			{
				++iter;
			}
		}
	}
}


