#ifndef  _TILEENGINE_H_
#define  _TILEENGINE_H_

/*
	Dvoid
	Tile engine 
*/

#include <list>
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include "p_serverunit.h"
#include "common_x.h"

using namespace std;

class COMMON_API Tile
{
	public:
		float 		fAngle;
		int			iTerrainType;
		list<int>	kUnits;		
};

class COMMON_API TileEngine 
{
	private:
		HeightMap*		m_pkMap;
		LevelManager*	m_pkLevelMan;
		ObjectManager* m_pkObjectMan;
		Render*	m_pkRender;		
		
		vector<Tile>	m_kTiles;
		
		int				m_iSizeX,m_iSizeY;

	
		void AddUnit(int x,int y,int iID);
		void AddUnit(int x,int y,P_ServerUnit* kSu);		
	
		void RemoveUnit(int x,int y,int iID);
		void RemoveUnit(int x,int y,P_ServerUnit* kSu);				
	
		bool UnitInTile(int x,int y,int iID);
	
	public:
		static TileEngine*		m_pkInstance;		
		
		TileEngine();
		static TileEngine* GetInstance();
		
		void CreateMap();
		Tile* GetTile(int x,int y);		
		void Generate();
		void GenerateHM(int x,int y,int w,int h);
		
		void GenerateUnits();		
		void AddUnit(Vector3 kPos,P_ServerUnit* kSu);
		void RemoveUnit(Vector3 kPos,P_ServerUnit* kSu);		
		void ClearUnits();
		
		vector<Tile>* GetTilesPointer() {return &m_kTiles;};
		
		//convertions
		Point GetSqrFromPos(Vector3 pos);				
		Vector3 GetPosFromSqr(Point square);

		void Draw();
};


#endif




