// pathfindbuilder.cpp: implementation of the PathFindBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "pathfindbuilder.h"
#include "heightmap.h"
#include "pathfind.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PathFindBuilder::PathFindBuilder(HeightMap* pkHeightMap, PathFind** m_ppkPathFind) 
	: ZFObject("PathFindBuilder")
{
	m_pkHeightMap = pkHeightMap;
	m_pkPathFind = *m_ppkPathFind;
	m_piCostMap = NULL;
}

PathFindBuilder::~PathFindBuilder()
{
	if(m_piCostMap)
		delete[] m_piCostMap;
}

void PathFindBuilder::Build()
{
	int iMapSize = m_pkHeightMap->GetSize();
	m_piCostMap = new int[iMapSize*iMapSize];
	m_pkPathFind = new PathFind(m_piCostMap, iMapSize, BLOCKED);
}

Point PathFindBuilder::GetMapTile(Vector3 pos)
{
	return Point((int)pos.x, (int)pos.y);
}
