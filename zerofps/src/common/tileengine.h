#ifndef  _TILEENGINE_H_
#define  _TILEENGINE_H_

/*
	Dvoid
	Tile engine 
*/

#include <list>
#include "../zerofps/engine/engine.pkg"

using namespace std;

class Tile
{
	public:
		float 		fAngle;
		int			iTerrainType;
		list<int>	kUnits;
};


class TileEngine 
{
	private:
		HeightMap*		m_pkMap;
		LevelManager*	m_pkLevelMan;
		ObjectManager* m_pkObjectMan;
		
		vector<Tile>	m_kTiles;
		
		int				m_iSizeX,m_iSizeY;

	public:
		TileEngine();
		
		void CreateMap();
		
		Tile* GetTile(int x,int y);
		
		void Generate();
		void GenerateHM(int x,int y,int w,int h);
		
		void AddUnit(int x,int y,int iID);
		void RemoveUnit(int x,int y,int iID);
		bool UnitInTile(int x,int y,int iID);
		
		
};

#endif




