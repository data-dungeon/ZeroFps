#ifndef _LEVELMANAGER_H_
#define _LEVELMANAGER_H_

#include "engine_x.h"
#include "../basic/basic.pkg"
#include "objectmanager.h"
#include "collisionmanager.h"
#include "objects.pkg"
#include "zerofps.h"
#include <string>
#include "../render/render.pkg"
#include "heightmapobject.h"
//#include "worldinfoproperty.h"

using namespace std;

//class WorldInfoProperty;

class ENGINE_API LevelManager : public ZFObject {
	private:
		ObjectManager* m_pkObjectMan;
		ZeroFps* m_pkZeroFps;
		CollisionManager* m_pkCollisionMan;
		ZFBasicFS* m_pkBasicFS;
		Console* m_pkConsole;
		Render* m_pkRender;
		
		HeightMap* m_pkMap;
		HeightMapObject* m_pkHeightMapObject;
		
		bool m_bVisibleZones;
		float m_fZoneRadius;
		
		string m_kMapBaseDir;
		WorldInfoProperty m_kWIP;


	public:
		LevelManager();
		void CreateZones(float fRadius,bool bVisible);
		bool LoadLevel(const char* acName);
		bool SaveLevel(const char* acName);
		void CreateNew(int iSize);
		void Clear();
		void CreateEmptyLevel(int iSize);
		HeightMap* GetHeightMap(){return m_pkMap;};

		Object* AddWorldInfoObject();			//is called before saving
		bool RemoveWorldInfoObject();
		bool ExtractWorldInfoObject();	//is called after loading
		void SetupWorld();
		
		void Fog(Vector3 kColor,float fStart,float fStop);
		void Water(bool bWater);
		
		void SetVisibleZones(bool bVisible) {m_bVisibleZones=bVisible;};
};


#endif






