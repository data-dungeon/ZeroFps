#ifndef _P_EVENT_H_
#define _P_EVENT_H_

#include "../../engine/property.h"
#include "../../engine/zerofps.h"
#include "../../script/zfscript.h"
#include <iostream>
#include <string.h>


using namespace std;

class ENGINE_SYSTEMS_API P_ScriptInterface: public Property {
	private:
		ZeroFps* 			m_pkFps;
		ZFScriptSystem* 	m_pkScriptSys;

		bool		m_bFirstRun;
		bool		m_bHaveRunInit;
		float 	m_fHeartRate;
		float 	m_fTimer;

	public:
		void CloneOf(Property* pkProperty) { }
		P_ScriptInterface();
		~P_ScriptInterface();
		
		void Update();				
		bool CallFunction(const char* acFunction,vector<ARG_DATA>* pkParams = NULL);
//		bool SendEvent(const char* acEvent);
//		bool SendObjectClickEvent(const char* acType,int iCallerObject );		
//		bool SendGroudClickEvent(const char* acType,Vector3 kPos,int iCallerObject);
//		void Touch(int iId);					//executet when collision occurs

      void Save(ZFIoInterface* pkPackage);
      void Load(ZFIoInterface* pkPackage);

		void SetHeartRate(float blub);
		
};

Property* Create_P_ScriptInterface();

#endif