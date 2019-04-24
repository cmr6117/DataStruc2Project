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

        std::vector<MyEntity*> EntitiesInside;
    private:
        float m_fRadius;
        vector3 m_v3GlobalCenter;
	};
}
