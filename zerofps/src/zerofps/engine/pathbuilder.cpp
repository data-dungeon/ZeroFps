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

				// Kolla terrängtyp
				float dx=x, dy=y;
				int texture = m_pkHeightMap->GetMostVisibleTexture(dx, dy); 

				// Kolla vinkeln mot XZ planet
				float fx = -(iMapSize/2)*HEIGHTMAP_SCALE + x*HEIGHTMAP_SCALE;
				float fy = -(iMapSize/2)*HEIGHTMAP_SCALE + y*HEIGHTMAP_SCALE;
				Vector3 sqrnorm = m_pkHeightMap->Tilt(fx,fy);
				Vector3 ground_plane = Vector3(0,1,0);
				float angle = RadToDeg(sqrnorm.Angle(ground_plane));

				if(pkVert->height <= 2.0f || angle > 40.0f)
					texture = 4;

				int iIndex = y*iMapSize+x;
				m_piTerrain[iIndex] = texture;					
			}
	}

	m_piCostMap = new int[iMapSize*iMapSize];

	for(y=0; y<iMapSize; y++)
		for(x=0; x<iMapSize; x++)
		{
			int iIndex = y*iMapSize+x;
			switch(m_piTerrain[iIndex])
			{
			case 0:
				m_piCostMap[iIndex] = pkObjectTypeCost[0]; // gräs
				break;
			case 1:
				m_piCostMap[iIndex] = pkObjectTypeCost[1]; // öken
				break;
			case 2:
				m_piCostMap[iIndex] = pkObjectTypeCost[2]; // sten
				break;
			case 3:
				m_piCostMap[iIndex] = pkObjectTypeCost[3]; // väg?
				break;
			case 4:
				m_piCostMap[iIndex] = pkObjectTypeCost[4]; // water
				break;
			default:
				printf("Bad terran type found, terrain not intialized!\n");
				break;
			}
		}

	*m_ppkPathFind = new PathFind(m_piCostMap, iMapSize, /*BLOCKED*/999);
}

Point PathBuilder::GetMapTile(Vector3 pos)
{
	return Point((int)pos.x, (int)pos.y);
}
