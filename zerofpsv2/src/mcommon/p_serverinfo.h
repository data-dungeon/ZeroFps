#ifndef _P_SERVERINFO_H_
#define _P_SERVERINFO_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"

using namespace std;

class MCOMMON_API P_ServerInfo: public Property {
	private:
		ZeroFps* m_pkFps;


	public:
		string m_sServerName;				
		int	m_iNrOfPlayers;
		
		
		void CloneOf(Property* pkProperty) { }
		P_ServerInfo();
		
		void Update();
};

MCOMMON_API Property* Create_P_ServerInfo();

#endif
