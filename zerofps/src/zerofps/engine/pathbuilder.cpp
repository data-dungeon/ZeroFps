// pathbuilder.cpp: implementation of the PathBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "pathbuilder.h"
#include "heightmap.h"
#include "pathfind.h"

int* PathBuilder::m_piTerrain = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PathBuilder::PathBuilder(HeightMap* pkHeightMap, PathFind** ppkPathFind) 
	: ZFObject("PathBuilder")
{
	m_pkHeightMap = pkHeightMap;
	m_ppkPathFind = ppkPathFind;
	m_piTerrain = NULL;
	m_piCostMap = NULL;

}

PathBuilder::~PathBuilder()
{
/*	if(m_piCostMap)
		delete[] m_piCostMap;*/
}

void PathBuilder::Build(int pkObjectTypeCost[5])
{
	int iMapSize = m_pkHeightMap->m_iHmSize;
	int y,x;

	if(m_piTerrain == NULL)
	{
		m_piTerrain = new int[iMapSize*iMapSize];

		for(y=0; y<iMapSize; y++)
			for(x=0; x<iMapSize; x++)
			{
				HM_vert* pkVert = m_pkHeightMap->GetVert(x,y);

				int texture = 0; 
				if(pkVert->height <= 2.0f)
					texture = 1;

				int iIndex = y*iMapSize+x;
				switch(texture)
				{
				case 0: // slättland
					m_piTerrain[iIndex] = GRASS;
					break;
				case 1: // vatten
					m_piTerrain[iIndex] = WATER;
					break;
				case 2: // träsk
					m_piTerrain[iIndex] = SWAMP;
					break;
				case 3: // väg
					m_piTerrain[iIndex] = ROAD;
					break;
				case 4: // skog
					m_piTerrain[iIndex] = WOOD;
					break;
				}
			}
	}

	m_piCostMap = new int[iMapSize*iMapSize];

	for(y=0; y<iMapSize; y++)
		for(x=0; x<iMapSize; x++)
		{
			int iIndex = y*iMapSize+x;
			switch(m_piTerrain[iIndex])
			{
			case GRASS:
				m_piCostMap[iIndex] = pkObjectTypeCost[GRASS];
				break;
			case WOOD:
				m_piCostMap[iIndex] = pkObjectTypeCost[WOOD];
				break;
			case SWAMP:
				m_piCostMap[iIndex] = pkObjectTypeCost[SWAMP];
				break;
			case WATER:
				m_piCostMap[iIndex] = pkObjectTypeCost[WATER];
				break;
			case ROAD:
				m_piCostMap[iIndex] = pkObjectTypeCost[ROAD];
				break;
			}
		}

	*m_ppkPathFind = new PathFind(m_piCostMap, iMapSize, BLOCKED);
}

Point PathBuilder::GetMapTile(Vector3 pos)
{
	return Point((int)pos.x, (int)pos.y);
}
