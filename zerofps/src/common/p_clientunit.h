/* Richard Svensson

	Client side property for units in zerorts

*/


#ifndef _P_CLIENTUNIT_H_
#define _P_CLIENTUNIT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"
#include "p_serverunit.h"

using namespace std;


class COMMON_API P_ClientUnit: public Property {
	private:
		ZeroFps*				m_pkFps;
		
		bool					m_bCurrentSelectionRenderState;
	
	
	public:
		UnitInfo				m_kInfo;	
		bool					m_bSelected;
	
	
		P_ClientUnit();
		void CloneOf(Property* pkProperty) { }
		void Update();

		void EnableSelectionRender();
		void DisableSelectionRender();		

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);

		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);
};

COMMON_API Property* Create_P_ClientUnit();


#endif






