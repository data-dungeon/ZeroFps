#include "tileengine.h"

TileEngine::TileEngine()
{

}

void TileEngine::CreateMap()
{
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkLevelMan=static_cast<LevelManager*>(g_ZFObjSys.GetObjectPtr("LevelManager"));
	m_pkMap=m_pkLevelMan->GetHeightMap();

	
	
	m_iSizeX = m_pkMap->m_iHmSize;
	m_iSizeY = m_pkMap->m_iHmSize;	
	
	cout<<"Creating a tile map of "<<m_iSizeX<<" x "<<m_iSizeY<<" tiles"<<endl;

	m_kTiles.reserve(m_iSizeX * m_iSizeY);
	
	Generate();
}

void TileEngine::Generate()
{
	GenerateHM(0,0,m_iSizeX,m_iSizeY);
}

void TileEngine::GenerateHM(int ix,int iy,int iw,int ih)
{
	cout<<"generating tile info from heightmap"<<endl;

	Vector3 yref = Vector3(0,1,0);

	if(ix >= 0 &&
		iy >= 0 && 
		ix+iw <= m_iSizeX-1 &&
		iy+ih <= m_iSizeY-1)
	{

		for(int y = iy;y<(iy+ih);y++)
		{
			for(int x = ix;x<(ix+iw);x++)
			{
				//get avrage angle in tile
				int index1 = (y*m_iSizeX) + x;
				int index2 = (y*m_iSizeX) + x+1;
				int index3 = ((y+1)*m_iSizeX) + x;
				int index4 = ((y+1)*m_iSizeX) + x+1;
				
				float angle1 = yref.Angle(m_pkMap->GetHMVertex()[index1].normal);
				float angle2 = yref.Angle(m_pkMap->GetHMVertex()[index2].normal);				
				float angle3 = yref.Angle(m_pkMap->GetHMVertex()[index3].normal);				
				float angle4 = yref.Angle(m_pkMap->GetHMVertex()[index4].normal);				
				
				float avrage = degtorad * ((angle1 + angle2 + angle3 + angle4)/4.0);
				
				m_kTiles[index1].fAngle = avrage;
				
				//get type
				int type = m_pkMap->GetMostVisibleTexture(x + HEIGHTMAP_SCALE/2 ,y + HEIGHTMAP_SCALE/2  ); 
				m_kTiles[index1].iTerrainType = type;
				
				cout<<"angle is "<<avrage<<endl;
				cout<<"Type:    "<<type<<endl;
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
		return NULL;
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






