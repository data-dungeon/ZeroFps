#ifndef _ENGINE_LEVELMANAGER_H_
#define _ENGINE_LEVELMANAGER_H_

/*#include "engine_x.h"
#include "objectmanager.h"
#include "../engine_systems/propertys/worldinfoproperty.h"

#include "zerofps.h"
#include <string>

using namespace std;


class ENGINE_API LevelManager : public ZFSubSystem {
	private:
		ObjectManager* 	m_pkObjectMan;
		ZeroFps* 			m_pkZeroFps;
		ZFBasicFS* 			m_pkBasicFS;
		Console* 			m_pkConsole;
		Render* 				m_pkRender;
//		CmdSystem* 			m_pkCmd;
//		ZFIni* 				m_pkIni;
		Light*				m_pkLight;
		
		HeightMap* 			m_pkMap;
		Object*				m_pkHeightMapObject;
		
		LightSource*		m_bSun;
		LightSource*		m_bMoon;	
		
		int 					m_iShowDecorations;
		int					m_iDecorationStep;
		bool 					m_bVisibleZones;
		float 				m_fZoneRadius;
		
		int 					m_iZpr;			//zones per radius
		float 				m_fZoneDistance;
		
		string 				m_kMapBaseDir;
		string				m_kCurrentMapDir;
		WorldInfoProperty m_kWIP;

//		list<Object*> 		m_kTrackedObjects;		
//		vector<Object*> 	m_kZones;
//		bool					m_bDrawZones;

		Object*				CreateHeightMapObject(HeightMap* m_pkMap);
		

	public:
		LevelManager();
//		void CreateZones();
		bool LoadLevel(const char* acName);
		bool LoadLevelHmapOnly(const char* acFile);
		bool SaveLevel(const char* acName);
		void CreateNew(int iSize);
		void Clear();
		void ClearObjects();
		void CreateEmptyLevel(int iSize);
		HeightMap* GetHeightMap(){return m_pkMap;};

		Object* AddWorldInfoObject();			//is called before saving
		bool RemoveWorldInfoObject();
		bool ExtractWorldInfoObject();	//is called after loading
		void SetupWorld();
		
		void SetVisibleZones(bool bVisible) {m_bVisibleZones=bVisible;};		
		float& GetZoneRadius() {return m_fZoneRadius;};
		
		int& GetDecorationStep() {return m_iDecorationStep;};
		int* GetDecorationStepPointer() {return &m_iDecorationStep;};
		
		void Fog(Vector3 kColor,float fStart,float fStop);
		void Water(bool bWater);
		void SkyBox(const char* acHor,const char* acTop,Vector3 kRotate);
		
		void SetMoonColor(Vector3 kColor);		
		void SetSunColor(Vector3 kColor);		
		void SetAmbientColor(Vector3 kColor);
		void ChangeLandscapeFillMode(PolygonMode eMode);
		
//		void AddTracker(Object* kObject);
//		void RemoveTracker(Object* kObject);
//		int GetNrOfTrackedObjects();
//		list<Object*>* GetTrackerList();
//		void ClearTrackers();

//		void UpdateZones();
//		void EnableZone(int xp,int zp,Vector3 &kPos);
//		Object* GetClosestZone(Vector3 &kPos);

//		void DrawZones();

		const string GetMapBaseDir()			{ return m_kMapBaseDir; }

		const string GetLogFileFullName();
		const string GetCurrentMapDir()		{ return m_kCurrentMapDir; }		

		bool StartUp();
		bool ShutDown();
		bool IsValid();
};
*/

#endif






