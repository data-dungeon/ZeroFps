#include "tileengine.h"

TileEngine* TileEngine::m_pkInstance = new TileEngine;		 

TileEngine* TileEngine::GetInstance()
{
	return m_pkInstance;
}


TileEngine::TileEngine()
{
	m_iSizeX = -1;
	m_iSizeY = -1;	
}

void TileEngine::CreateMap()
{
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkLevelMan=static_cast<LevelManager*>(g_ZFObjSys.GetObjectPtr("LevelManager"));
	m_pkMap=m_pkLevelMan->GetHeightMap();

	
	
	m_iSizeX = m_pkMap->m_iHmSize -1;
	m_iSizeY = m_pkMap->m_iHmSize -1;	
	
	cout<<"Creating a tile map of "<<m_iSizeX<<" x "<<m_iSizeY<<" tiles"<<endl;

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
		cout<<"get tile out of index "<< x << " " << y << endl;
		return NULL;
	}
}

void TileEngine::AddUnit(int x,int y,int iID)
{
	if(m_iSizeX != -1)
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
	else
		cout<<"Trying to add unit before tileengine has been started"<<endl;
}

void TileEngine::AddUnit(int x,int y,P_ServerUnit* kSu)
{
	if(m_iSizeX != -1)
	{	
		int w = kSu->m_kInfo.m_Info2.m_cWidth;
		int h = kSu->m_kInfo.m_Info2.m_cHeight;
			
		float sh = h/2.0;
		float sw = w/2.0;			
	
		for(int yy = int(-sh);yy<sh;yy++)
			for(int xx = int(-sw);xx<sw;xx++)
			{			
				AddUnit(x + xx,y + yy,kSu->GetObject()->iNetWorkID);			
			}
	}
	else
		cout<<"Trying to add unit before tileengine has been started"<<endl;
}

void TileEngine::RemoveUnit(int x,int y,int iID)
{
	Tile* t = GetTile(x,y);
	
	if(t)
		t->kUnits.remove(iID);
	else
		cout<<"Invalid tile when removing unit"<<endl;

}

void TileEngine::RemoveUnit(int x,int y,P_ServerUnit* kSu)
{
	if(m_iSizeX != -1)
	{	
		int w = kSu->m_kInfo.m_Info2.m_cWidth;
		int h = kSu->m_kInfo.m_Info2.m_cHeight;
			
		float sh = h/2.0;
		float sw = w/2.0;			
	
		for(int yy = int(-sh);yy<sh;yy++)
			for(int xx = int(-sw);xx<sw;xx++)
			{			
				RemoveUnit(x + xx,y + yy,kSu->GetObject()->iNetWorkID);			
			}
	}
	else
		cout<<"Trying to add unit before tileengine has been started"<<endl;
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
			AddUnit(kObjects[i]->GetPos(),su);
		}
	}
}



Point TileEngine::GetSqrFromPos(Vector3 pos)
{
	int iSquareX = int((m_pkMap->m_iHmSize/2.0)+floor(pos.x / HEIGHTMAP_SCALE));
	int iSquareY = int((m_pkMap->m_iHmSize/2.0)+floor(pos.z / HEIGHTMAP_SCALE));

	return Point(iSquareX,iSquareY);
}

Vector3 TileEngine::GetPosFromSqr(Point square)
{
	float x = -(m_pkMap->m_iHmSize/2.0)*HEIGHTMAP_SCALE + square.x*HEIGHTMAP_SCALE;
	float z = -(m_pkMap->m_iHmSize/2.0)*HEIGHTMAP_SCALE + square.y*HEIGHTMAP_SCALE;

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

void TileEngine::AddUnit(Vector3 kPos,P_ServerUnit* kSu)
{
	Point pos = GetSqrFromPos(kPos);
	
	//make sure its removed from its old tile
	RemoveUnit(kSu->m_kTile.x,kSu->m_kTile.y,kSu);
	kSu->m_kTile = pos;
	
	//add it to the new tile
	AddUnit(pos.x,pos.y,kSu);
	
}

void TileEngine::RemoveUnit(Vector3 kPos,P_ServerUnit* kSu)
{
	Point pos = GetSqrFromPos(kPos);
	RemoveUnit(pos.x,pos.y,kSu);
}


void TileEngine::Draw()
{
	for(int y = 0;y<m_iSizeY;y++)
	{
		for(int x = 0;x<m_iSizeX;x++)
		{
			if(!GetTile(x,y)->kUnits.empty())
			{
				Vector3 pos;
				pos.Set(x - ceil(m_iSizeX/2.0) + 0.5,4,y - ceil(m_iSizeY/2.0) + 0.5);
				pos *= HEIGHTMAP_SCALE;

				Vector3 p = m_pkMap->GetPosFromSqr(Point(x,y));
				pos.y = m_pkMap->Height(p.x,p.z) + 0.1f;
				
				m_pkRender->Quad(pos,Vector3(-90,0,0),Vector3(2,2,2),1);
			}	
		}
	}
}

