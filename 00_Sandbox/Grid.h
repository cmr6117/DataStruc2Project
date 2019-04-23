#pragma once

#include "AppClass.h"
#include "CellNode.h"

namespace Simplex
{
	class Grid
	{
	public:
		Grid(float worldRadius);
		~Grid();

		//grid functions
		void Init(float worldRadius);

		//grid variables
		bool Active;
		float CellSize;
		float WorldRadius;
		static const int NUM_CELLS = 12;
		std::vector<CellNode> CellList;
	};
}
