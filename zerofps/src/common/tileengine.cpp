#include "tileengine.h"
#include "p_serverunit.h"

TileEngine::TileEngine()
{

}

void TileEngine::CreateMap()
{
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkLevelMan=static_cast<LevelManager*>(g_ZFObjSys.GetObjectPtr("LevelManager"));
	m_pkMap=m_pkLevelMan->GetHeightMap();

	
	
	m_iSizeX = m_pkMap->m_iHmSize -1;
	m_iSizeY = m_pkMap->m_iHmSize -1;	
	
	cout<<"Creating a tile map of "<<m_iSizeX-1<<" x "<<m_iSizeY-1<<" tiles"<<endl;

	m_kTiles.reserve(m_iSizeX * m_iSizeY);
	
	Tile bla;
	for(int i=0;i< (m_iSizeX * m_iSizeY) ;i++)
	{
		m_kTiles.push_back(bla);
	}
	
	Generate();
}

void TileEngine::Generate()
{
	GenerateHM(0,0,m_iSizeX,m_iSizeY);
	GenerateUnits();
}

void TileEngine::GenerateHM(int ix,int iy,int iw,int ih)
{
	cout<<"generating tile info from heightmap"<<endl;

	Vector3 yref = Vector3(0,1,0);

	if(ix >= 0 &&
		iy >= 0 && 
		ix+iw <= m_iSizeX &&
		iy+ih <= m_iSizeY)
	{

		for(int y = iy;y<(iy+ih);y++)
		{
			for(int x = ix;x<(ix+iw);x++)
			{
				int hmsize = m_pkMap->m_iHmSize;
		
				//get avrage angle in tile
				int index1 = (y*hmsize) + x;
				int index2 = (y*hmsize) + x+1;
				int index3 = ((y+1)*hmsize) + x;
				int index4 = ((y+1)*hmsize) + x+1;
				
				float angle1 = yref.Angle(m_pkMap->GetHMVertex()[index1].normal);
				float angle2 = yref.Angle(m_pkMap->GetHMVertex()[index2].normal);				
				float angle3 = yref.Angle(m_pkMap->GetHMVertex()[index3].normal);				
				float angle4 = yref.Angle(m_pkMap->GetHMVertex()[index4].normal);				
				
				float avrage = degtorad * ((angle1 + angle2 + angle3 + angle4)/4.0);
				
				Tile* t = GetTile(x,y);
				if(!t)
					cout<<"Error while generating tile info from heightmap, im gona crash now i think,,,sorry =("<<endl;
				
				t->fAngle = avrage;
				
				//get type
				int type = m_pkMap->GetMostVisibleTexture(x + HEIGHTMAP_SCALE/2 ,y + HEIGHTMAP_SCALE/2  ); 
				
				t->iTerrainType = type;
				
				//cout<<"angle is "<<avrage<<endl;
				//cout<<"Type:    "<<type<<endl;
			}
		}
	}
	else
		cout<<"Error outside tile range"<<endl;
}

Tile* TileEngine::GetTile(int x,int y)
{
	if(x >= 0 &&
		y >= 0 &&
		x <  m_iSizeX &&
		y <  m_iSizeY)
		return &m_kTiles[(y*m_iSizeX) +x];
	else
	{
		cout<<"get tile out of index"<<endl;
		return NULL;
	}
}

void TileEngine::AddUnit(int x,int y,int iID)
{
	if(!UnitInTile(x,y,iID))
	{
		Tile* t = GetTile(x,y);
		
		if(t)
			t->kUnits.push_back(iID);
		else
			cout<<"Invalid tile when adding unit"<<endl;
	}
}

void TileEngine::RemoveUnit(int x,int y,int iID)
{
	Tile* t = GetTile(x,y);
	
	if(t)
		t->kUnits.remove(iID);
	else
		cout<<"Invalid tile when removing unit"<<endl;

}

bool TileEngine::UnitInTile(int x,int y,int iID)
{
	Tile* t = GetTile(x,y);
	
	if(t)
	{
		for(list<int>::iterator it = t->kUnits.begin();it != t->kUnits.end();it++)		
			if((*it) == iID)
				return true;
	}
	
	
	return false;
}

void TileEngine::GenerateUnits()
{
	cout<<"generating tile unit info"<<endl;
	vector<Object*> kObjects;		
	kObjects.clear();
	
	//clear all units
	ClearUnits();

	//get all objects
	m_pkObjectMan->GetAllObjects(&kObjects);
	
	cout<<"nr of objects:"<<kObjects.size()<<endl;
	
	for(int i=0;i<kObjects.size();i++)
	{
		P_ServerUnit* su = (P_ServerUnit*)kObjects[i]->GetProperty("P_ServerUnit");		
			
		if(su)
		{
			Point pos = GetSqrFromPos(kObjects[i]->GetPos());	
			
			AddUnit(pos.x,pos.y,kObjects[i]->iNetWorkID);
		}
	}
}



Point TileEngine::GetSqrFromPos(Vector3 pos)
{
	int iSquareX = m_pkMap->m_iHmSize/2+floor(pos.x / HEIGHTMAP_SCALE);
	int iSquareY = m_pkMap->m_iHmSize/2+floor(pos.z / HEIGHTMAP_SCALE);

	return Point(iSquareX,iSquareY);
}

Vector3 TileEngine::GetPosFromSqr(Point square)
{
	float x = -(m_pkMap->m_iHmSize/2)*HEIGHTMAP_SCALE + square.x*HEIGHTMAP_SCALE;
	float z = -(m_pkMap->m_iHmSize/2)*HEIGHTMAP_SCALE + square.y*HEIGHTMAP_SCALE;

	x -= HEIGHTMAP_SCALE/2;	// Translate to center 
	z -= HEIGHTMAP_SCALE/2;	// of square.*/

	float y = m_pkMap->Height(x,z);

	return Vector3(x,y,z);
}

void TileEngine::ClearUnits()
{
	for(int y=0;y< m_iSizeY;y++)
	{
		for(int x=0;x< m_iSizeX;x++)
		{
			Tile* t = GetTile(x,y);

			if(t)
				if(t->kUnits.size() >0)
					t->kUnits.clear();
		}
	}
}



