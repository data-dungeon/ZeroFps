/* Richard Svensson

	Client side property for units in zerorts

*/


#ifndef _P_SERVERUNIT_H_
#define _P_SERVERUNIT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <map>
#include <iostream>
#include <string>
#include "common_x.h"
#include "externalcommand.h"
#include "unitcommand.h"

#include "p_serverinfo.h"

using namespace std;

class P_ServerInfo;

class COMMON_API P_SuplyGiver: public Property {
	private:
		P_ServerInfo*	m_pkServerInfo;	
		P_ServerUnit*	m_pkServerUnit;
		
		ZeroFps*			m_pkFps;
		
		float				m_fTime;
		
//		vector<PropertyValues> GetPropertyValues();				
		
	public:
		int	m_iAmount;
				
		P_SuplyGiver();

		void CloneOf(Property* pkProperty) { }
		void Update();

//		void Save(ZFMemPackage* pkPackage);
//		void Load(ZFMemPackage* pkPackage);
};

COMMON_API Property* Create_P_SuplyGiver();


#endif
