// pathbuilder.cpp: implementation of the PathBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "pathbuilder.h"
#include "heightmap.h"
#include "pathfind.h"

int* PathBuilder::m_piTerrain = NULL;

map< string, int* > PathBuilder::m_akCostMaps;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PathBuilder::PathBuilder(HeightMap* pkHeightMap, PathFind** ppkPathFind) 
	: ZFObject("PathBuilder")
{
	m_pkHeightMap = pkHeightMap;
	m_ppkPathFind = ppkPathFind;
}

PathBuilder::~PathBuilder()
{

}

void PathBuilder::Build(int pkObjectTypeCost[5])
{
	int iMapSize = m_pkHeightMap->m_iHmSize;
	int y,x;

	if(m_piTerrain == NULL)
	{
		m_piTerrain = new int[iMapSize*iMapSize];

		printf("Building terrain for the first and only time!\n");

		for(y=0; y<iMapSize; y++)
			for(x=0; x<iMapSize; x++)
			{
				HM_vert* pkVert = m_pkHeightMap->GetVert(x,y);

				// Kolla terrängtyp
				float dx=(float)x, dy=(float)y;
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

	int* piCostMap = NULL;
	
	string szKey = GenerateKey(pkObjectTypeCost);
	map<string,int*>::iterator r = m_akCostMaps.find(szKey);

	// Skapa en ny om kostnadsmappen inte redan finns.
	if(r == m_akCostMaps.end())
	{
		piCostMap = new int[iMapSize*iMapSize];

		for(y=0; y<iMapSize; y++)
			for(x=0; x<iMapSize; x++)
			{
				int iIndex = y*iMapSize+x;
				switch(m_piTerrain[iIndex])
				{
				case 0:
					piCostMap[iIndex] = pkObjectTypeCost[0]; // gräs
					break;
				case 1:
					piCostMap[iIndex] = pkObjectTypeCost[1]; // öken
					break;
				case 2:
					piCostMap[iIndex] = pkObjectTypeCost[2]; // sten
					break;
				case 3:
					piCostMap[iIndex] = pkObjectTypeCost[3]; // väg?
					break;
				case 4:
					piCostMap[iIndex] = pkObjectTypeCost[4]; // water
					break;
				default:
					printf("Bad terran type found, terrain not intialized!\n");
					break;
				}
			}

		m_akCostMaps.insert( map<string, int*>::value_type(szKey, piCostMap) );
	}
	else
	{
		printf("Found identical cost map\n");
		piCostMap = r->second;
	}

	*m_ppkPathFind = new PathFind(piCostMap, iMapSize, BLOCKED);
}

string PathBuilder::GenerateKey(int akCosts[])
{
	char szCostMap[20];
	sprintf(szCostMap, "%i%i%i%i%i", akCosts[0],
		akCosts[1], akCosts[2], akCosts[3], akCosts[4]);

	return string(szCostMap);
}

Point PathBuilder::GetSqrFromPos(Vector3 pos)
{
	int iSquareX = m_pkHeightMap->m_iHmSize/2+ceil(pos.x / HEIGHTMAP_SCALE);
	int iSquareY = m_pkHeightMap->m_iHmSize/2+ceil(pos.z / HEIGHTMAP_SCALE);

	return Point(iSquareX,iSquareY);
}

Vector3 PathBuilder::GetPosFromSqr(Point square)
{
	float x = -(m_pkHeightMap->m_iHmSize/2)*HEIGHTMAP_SCALE + square.x*HEIGHTMAP_SCALE;
	float z = -(m_pkHeightMap->m_iHmSize/2)*HEIGHTMAP_SCALE + square.y*HEIGHTMAP_SCALE;

	x -= HEIGHTMAP_SCALE/2;	// Translate to center 
	z -= HEIGHTMAP_SCALE/2;	// of square.*/

	float y = m_pkHeightMap->Height(x,z);

	return Vector3(x,y,z);
}