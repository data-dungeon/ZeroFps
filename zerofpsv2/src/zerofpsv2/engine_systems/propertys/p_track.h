#ifndef _ENGINE_PROXYPROPERTY_H_
#define _ENGINE_PROXYPROPERTY_H_

#include "../../engine/property.h"
#include <iostream>
#include <limits.h>
#include "../../basic/zfsystem.h"
#include "../../engine/zerofps.h"
#include "../../engine/entitymanager.h"
#include <string.h>
#include "../engine_systems_x.h"
#include <set>


using namespace std;


/// Property to track objects to activate zones.
class ENGINE_SYSTEMS_API P_Track : public Property {
	private:
		EntityManager* m_pkOBjM;
		ZeroFps*			m_pkFps;
		float				m_fNextMove;
		
	public:
		int				m_iConnectID;		// Client that should recv updates activated by this tracker.
		bool				m_bClosestZone;
		int				m_iZoneNum;

		set<int>			m_iActiveZones;
		set<int>			m_iNewActiveZones;		
		set<int>			m_iUnloadZones;				//sent to client

		void Update();
		void Init();
		void SetClient(int iId);
		
		P_Track();
		~P_Track();
		void CloneOf(Property* pkProperty) { }
};

Property* Create_TrackProperty();



#endif




