#ifndef _P_ENVIROMENT_H_
#define _P_ENVIROMENT_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

#include "../zerofpsv2/engine_systems/propertys/psystemproperty.h"
#include "../zerofpsv2/engine_systems/propertys/lightproperty.h"

using namespace std;

class MCOMMON_API P_Enviroment: public Property {
	private:
		ZeroFps* 		m_pkFps;
		ObjectManager* m_pkObjectMan;
		
		bool				m_bEnabled;
		
		string			m_StrCurrentEnviroment;
		
	public:

		void CloneOf(Property* pkProperty) { }
		P_Enviroment();
		
		void Update();
		void ZoneChange(int iCurrent,int iNew);

		void SetEnviroment(char* csEnviroment);
		void ResetEnviroment();

		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

		void SetEnable(bool bNew) {m_bEnabled = bNew;};
};

MCOMMON_API Property* Create_P_Enviroment();

#endif
