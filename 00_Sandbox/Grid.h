#pragma once

#include "AppClass.h"
#include "CellNode.h"

namespace Simplex
{
	class Grid
	{
	MeshManager* m_pMeshMngr = nullptr; //for displaying the Rigid Body

	public:
		//grid variables
		bool Active;
		float CellSize;
		float WorldRadius;
		static const int NUM_CELLS = 5;
		std::vector<CellNode*> CellList;

		Grid(float worldRadius);
		~Grid();

		//grid functions
		void Init(float worldRadius);
        void Update();
		void DisplayGrid();

	};
}

