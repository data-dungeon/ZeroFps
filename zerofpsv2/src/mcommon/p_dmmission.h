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

class P_DMGameInfo;

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
		void OnMissionSuccess();

		vector<DMMissionInfo*> m_vkMissions;
		DMMissionInfo* m_pkCurrentMission;

		EntityManager* m_pkEntityManager;
		P_DMGameInfo* m_pkGameInfoEntity;
		
	public:
		
		P_DMMission();
		~P_DMMission();

		
		void Init();
		void Update();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

		bool SetCurrentMission(string strMissionScript);
		void GetPossibleMissions(float fReputation, vector<DMMissionInfo>& vkInfo);
		bool RemoveMission(string strMissionScript);
		bool CancelCurrent();

		DMMissionInfo* GetCurrentMission() { return m_pkCurrentMission; }

		ZFScriptSystem* m_pkScriptSys;
		ZeroFps* m_pkZeroFps;
};

MCOMMON_API Property* Create_P_DMMission();

#endif // _P_DMMISSION_H_
