#pragma once

#include "MyEntity.h"

namespace Simplex
{
	class CellNode
	{
		float m_fRadius;
		vector3 m_v3GlobalCenter;

	public:
		std::vector<MyEntity*> EntitiesInside; //Tracks all local entities

		//Constructor
		CellNode(vector3 cellMin, vector3 cellMax, float cellSize);

		//Destructor
		~CellNode();

		//return center of cell as vector3
		vector3 GetCenter();

		//return radius of cell
		float GetRadius();

		//Handles collisions between entities
        void HandleCollisions(); 

		//Add entity to EntitiesInside list
		void Simplex::CellNode::AddLocalEntity(MyEntity* newEntity);

		//Remove entity from EntitiesInside list, return type is for RefreshEntities
		std::vector<MyEntity*>::iterator RemoveLocalEntity(uint a_uIndex);

		//Remove entity from EntitiesInside list by uniqueID
		void RemoveLocalEntity(String uniqueID);

		//Refresh EntitiesInside list to see if any have moved out
		//void RefreshEntities();
	};
}