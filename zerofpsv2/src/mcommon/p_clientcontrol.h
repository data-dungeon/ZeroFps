#ifndef _P_CLIENTCONTROL_H_
#define _P_CLIENTCONTROL_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"

using namespace std;

class MCOMMON_API P_ClientControl: public Property {
	private:
		ZeroFps* m_pkFps;


	public:
		void CloneOf(Property* pkProperty) { }
		P_ClientControl();
		
		void Update();
};

MCOMMON_API Property* Create_P_ClientControl();

#endif
