/* Richard Svensson

	Client & Server property that contains map information

*/


#ifndef _P_SERVERINFO_H_
#define _P_SERVERINFO_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"


using namespace std;

class COMMON_API P_ServerInfo: public Property {
	private:
	
	public:
		struct ServerInfo
		{
			char m_acServerName[64];
			unsigned char m_acColor[4];
			int m_aiPlayerResourses[4];
		
		} m_kSInfo; 

	
		P_ServerInfo();
		void CloneOf(Property* pkProperty) { }

		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

};

COMMON_API Property* Create_P_ServerInfo();


#endif
