#ifndef _P_EVENT_H_
#define _P_EVENT_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"

using namespace std;

class MCOMMON_API P_Event: public Property {
	private:
		ZeroFps* m_pkFps;
		ZFScriptSystem* m_pkScript;

		bool m_bHaveRunInit;

	public:
		void CloneOf(Property* pkProperty) { }
		P_Event();
		
		void Update();				
		bool SendEvent(const char* acEvent);
};

MCOMMON_API Property* Create_P_Event();

#endif
