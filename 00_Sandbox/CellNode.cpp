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
