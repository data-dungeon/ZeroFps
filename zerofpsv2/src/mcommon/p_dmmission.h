#ifndef _P_DMMISSION_H_
#define _P_DMMISSION_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

struct MCOMMON_API DMMissionInfo
{
	int m_iLevel;
	int m_iXP;
	int m_iCash;
	string m_strName;
	string m_strScript;
	string m_strInfoTextShort;
	string m_strInfoTextLong;
	ZFResourceHandle* m_pkScriptResHandle;
};

class MCOMMON_API P_DMMission: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();

		float m_fMissionDoneCheckTime;
		float m_fMissionFailedCheckTime;
		
		bool GetMissionInfoFromScript(DMMissionInfo** ppInfo);
		const char* GetMissionText(bool bLong);

		vector<DMMissionInfo*> m_vkMissions;
		DMMissionInfo* m_pkCurrentMission;
		
	public:
		
		P_DMMission();
		~P_DMMission();

		void CloneOf(Property* pkProperty) { }
		
		void Init();
		void Update();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);

		bool SetCurrentMission(string strMissionScript);
		void GetPossibleMissions(int iLevel);

		//string m_strName;
		//string m_strMissionScript;
		//int m_iDifficulty;
		//int m_iXP;

		ZFScriptSystem* m_pkScriptSys;
		//ZFResourceHandle* m_pkScriptResHandle;
		ZeroFps* m_pkZeroFps;
};

MCOMMON_API Property* Create_P_DMMission();

#endif // _P_DMMISSION_H_
