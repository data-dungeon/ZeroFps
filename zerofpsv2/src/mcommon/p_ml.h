#ifndef _P_ML_H_
#define _P_ML_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

class MCOMMON_API P_Ml: public Property {
	private:
		ZeroFps* m_pkFps;
		
		vector<string>	m_kActions;	

	public:

		vector<string> GetActions() { return m_kActions; }

		void CloneOf(Property* pkProperty) { }
		P_Ml();
		
		void Update();
		
		void AddAction(const char* csAction);
		
		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);
};

MCOMMON_API Property* Create_P_Ml();

#endif
