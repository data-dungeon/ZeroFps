#ifndef _P_EVENT_H_
#define _P_EVENT_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/engine_systems/propertys/p_physic.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"

#include "si_mistland.h"

using namespace std;

class MCOMMON_API P_Event: public Property {
	private:
		ZeroFps* m_pkFps;
		ZFScriptSystem* m_pkScriptSys;
		ZFResourceHandle* m_pkScriptResHandle;

		bool m_bHaveRunInit;
		bool m_bRun1SUpdate;

		float m_fTimer;

	public:
		void CloneOf(Property* pkProperty) { }
		P_Event();
		~P_Event();
		
		//void ZoneChange(int iCurrent,int iNew);
		
		void Update();				
		bool SendEvent(const char* acEvent);
		bool SendObjectClickEvent(const char* acType,int iCallerObject );		
		bool SendGroudClickEvent(const char* acType,Vector3 kPos,int iCallerObject);
		
		void Touch(int iId);					//executet when collision occurs
};

MCOMMON_API Property* Create_P_Event();

#endif
