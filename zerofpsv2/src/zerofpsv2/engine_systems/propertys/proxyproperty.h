#ifndef _ENGINE_PROXYPROPERTY_H_
#define _ENGINE_PROXYPROPERTY_H_

#include "../../engine/property.h"
#include <iostream>
#include <limits.h>
#include "../../basic/zfobjectmanger.h"
#include "../../engine/zerofps.h"
#include "../../engine/objectmanager.h"
#include <string.h>
#include "../engine_systems_x.h"
#include <set>
//#include "GL/glut.h"

using namespace std;

enum PROXYPROPERTY_AKTIONS
{
	UPDATESTATUS,
	WAKEUP
};


class ENGINE_SYSTEMS_API ProxyProperty: public Property {
	private:
		ZeroFps* m_pkFps;
//		LevelManager* m_pkLevelMan;
		
		list<Object*>* m_pkTrackers;
		
		float m_fRadius;	
		int m_iAktion;
			
	public:
		ProxyProperty();
		void CloneOf(Property* pkProperty) { }
		void Update();
		
		float GetDistance();
		
		void SetRadius(float fRadius) {m_fRadius= fRadius;};
		float GetRadius() {return m_fRadius;};
	
		void SetAktion(int iAktion) {m_iAktion=iAktion;};
		int GetAktion() {return m_iAktion;};
};

Property* Create_ProxyProperty();


class ENGINE_SYSTEMS_API TrackProperty: public Property {
	private:
		ObjectManager*		m_pkOBjM;
		ZeroFps*				m_pkFps;
		float					m_fNextMove;
		
	public:
		bool					m_bClosestZone;
		int					m_iZoneNum;
		int					m_iLastZoneIndex;	// Index of last zone we was close to.
		set<int>				m_iActiveZones;

		void Update();
		void Init();
		
		TrackProperty();
		~TrackProperty();
		void CloneOf(Property* pkProperty) { }
};

Property* Create_TrackProperty();



#endif




