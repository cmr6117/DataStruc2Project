#pragma once

#include "MyEntity.h"

namespace Simplex
{
	class CellNode
	{
	public:
		CellNode(vector3 cellMin, vector3 cellMax, float cellSize);
		~CellNode();

        void HandleCollisions();

		//Add entity to EntitiesInside list
		void AddLocalEntity(MyEntity* newEntity);

		//Remove entity from EntitiesInside list, return type is for RefreshEntities
		std::vector<MyEntity*>::iterator RemoveLocalEntity(uint a_uIndex);

		//Refresh EntitiesInside list to see if any have moved out
		void RefreshEntities();

        std::vector<MyEntity*> EntitiesInside;
    private:
        float m_fRadius;
        vector3 m_v3GlobalCenter;
	};
}
