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
		
		vector<PropertyValues> GetPropertyValues();
		
	public:
		int				m_iConnectID;		// Client that should recv updates activated by this tracker.
		bool				m_bClosestZone;
		int				m_iCurrentTrackedZone;			//current zone
		bool				m_bOneZoneOnly;

		set<int>			m_kActiveZones;
		set<int>			m_kNewActiveZones;		
		set<int>			m_kUnloadZones;				//sent to client

		void Update();
		void Init();
		void SetClient(int iId);
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		
		P_Track();
		~P_Track();
};

Property* Create_TrackProperty();



#endif




