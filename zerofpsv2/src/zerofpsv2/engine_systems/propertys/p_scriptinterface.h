#ifndef _P_SI_H_
#define _P_SI_H_

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
		P_ScriptInterface();
		~P_ScriptInterface();
		
		void Update();				
		//bool CallFunction(const char* acFunction,vector<ScriptFuncArg>* pkParams = NULL);
		bool SendObjectClickEvent(const char* acType,int iCallerObject );		
		bool SendGroudClickEvent(const char* acType,Vector3 kPos,int iCallerObject);
		bool SendEvent(const char* acEvent);

		void OnEvent(GameMessage& Msg);

      void Save(ZFIoInterface* pkPackage);
      void Load(ZFIoInterface* pkPackage,int iVersion);

		void SetHeartRate(float blub);
		
		void Touch(int iId);
};

Property* Create_P_ScriptInterface();

#endif


