#include "p_dmmission.h" 
#include "si_dm.h"
#include "p_dmgameinfo.h"

P_DMMission::P_DMMission()
{
	strcpy(m_acName,"P_DMMission");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = true;

	m_pkScriptSys = static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_fMissionDoneCheckTime=0;
	m_fMissionFailedCheckTime=0;

	m_pkCurrentMission = NULL;
}

P_DMMission::~P_DMMission()
{


}

void P_DMMission::Init()
{
	m_pkObjectMan = 
		static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));

	ZFVFileSystem* pkFileSys = reinterpret_cast<ZFVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

	vector<string> t;
	vector<string> final;
	pkFileSys->ListDir(&t, "data/script/missions/");
	for(unsigned int i=0; i<t.size(); i++)
	{
		if(t[i].find(".lua") != string::npos)
		{
			bool bDubble = false;
			for(int j=0; j<final.size(); j++)
			{
				if(final[j] == t[i] && j != i)
				{
					bDubble = true;
					break;
				}
			}

			if(bDubble == false)
				final.push_back(t[i]); 
		}
	}

	for(int i=0; i<final.size(); i++)
	{
		DMMissionInfo* pkMission = new DMMissionInfo;
		pkMission->m_pkScriptResHandle = new ZFResourceHandle;

		string strPath = string("data/script/missions/") + final[i];

		bool test = pkMission->m_pkScriptResHandle->SetRes(strPath);

		if(test == false)
		{
			char szErrorMssage[512];
			sprintf(szErrorMssage, "Failed to load lua script %s, for P_DMMission\n", 
				strPath.c_str());

			ZFAssert(test, szErrorMssage);
		}

		pkMission->m_strScript = strPath;

		GetMissionInfoFromScript(&pkMission);

		m_vkMissions.push_back(pkMission);
	}
}


vector<PropertyValues> P_DMMission::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
	//	
	//kReturn[0].kValueName = "name";
	//kReturn[0].iValueType = VALUETYPE_STRING;
	//kReturn[0].pkValue    = &pkMissionm_strName;		

	//kReturn[1].kValueName = "difficulty";
	//kReturn[1].iValueType = VALUETYPE_INT;
	//kReturn[1].pkValue    = &m_iDifficulty;	

	//kReturn[2].kValueName = "missionscript";
	//kReturn[2].iValueType = VALUETYPE_STRING;
	//kReturn[2].pkValue    = &m_strMissionScript;	

	return kReturn;
}

void P_DMMission::Save(ZFIoInterface* pkPackage)
{	
	//char temp[256];
	//
	//strcpy(temp,m_strName.c_str());
	//pkPackage->Write(temp,256,1);

	//pkPackage->Write(&m_iDifficulty,sizeof(m_iDifficulty),1);

	//strcpy(temp,m_strMissionScript.c_str());
	//pkPackage->Write(temp,256,1);
}

void P_DMMission::Load(ZFIoInterface* pkPackage)
{
	//char temp[256];
	//
	//pkPackage->Read(temp,256,1);
	//m_strName = temp;

	//pkPackage->Read(&m_iDifficulty,sizeof(m_iDifficulty),1);	

	//pkPackage->Read(temp,256,1);
	//m_strMissionScript = temp;
}

void P_DMMission::Update()
{
	if(m_pkCurrentMission == NULL)
		return;

	if(m_pkCurrentMission->m_pkScriptResHandle != NULL)
	{
		//
		// Kolla om uppdraget har slutförts 1 gång var 3:e sekund
		// 

		float fTimeCheck = m_pkZeroFps->m_pkObjectMan->GetGameTime();

		if(fTimeCheck - m_fMissionDoneCheckTime > 3.0f)
		{
			m_pkScriptSys->Call(m_pkCurrentMission->m_pkScriptResHandle, 
				"IsMissionDone", 0, 0);
			m_fMissionDoneCheckTime = fTimeCheck;

			bool bSuccess;
			int iSuccess = m_pkScriptSys->GetGlobalInt(
				((ZFScript*)m_pkCurrentMission->m_pkScriptResHandle->GetResourcePtr())->m_pkLuaState, 
				"Success", &bSuccess);
		}

		//
		// Kolla om uppdraget har misslyckats 1 gång var 3:e sekund
		// 

		if(fTimeCheck - m_fMissionFailedCheckTime > 3.0f)
		{
			m_pkScriptSys->Call(m_pkCurrentMission->m_pkScriptResHandle, 
				"IsMissionFailed", 0, 0);
			m_fMissionFailedCheckTime = fTimeCheck;
		}
	}
}

bool P_DMMission::RemoveMission(string strMission)
{
	bool bSuccess = false;

	vector<DMMissionInfo*>::iterator it = m_vkMissions.begin();
	for( ; it != m_vkMissions.end(); it++)
	{
		if((*it)->m_strScript == strMission)
		{
			delete (*it)->m_pkScriptResHandle;
			delete (*it);

			m_vkMissions.erase(it);
			m_pkCurrentMission = NULL;
			bSuccess = true;

			break;
		}
	}

	return bSuccess;
}

bool P_DMMission::SetCurrentMission(string strMissionScript)
{
	bool bSuccess = false;

	for(int i=0; i<m_vkMissions.size(); i++)
	{
		if(m_vkMissions[i]->m_strScript == strMissionScript)
		{
			if(m_pkCurrentMission != NULL)
			{
				int ExtraCash = m_pkCurrentMission->m_iCash;
				int ExtraXP = m_pkCurrentMission->m_iXP;

				if(!RemoveMission(m_pkCurrentMission->m_strScript))
				{	
					printf("Failed to remove mission!\n");
				}
			}

			m_pkCurrentMission = m_vkMissions[i];
			bSuccess = true;
			break;
		}
	}

	if(bSuccess == false)
	{
		return false;
	}

	printf("\n---------------------------------\n");
	printf("Starting mission \"%s\"\n", strMissionScript.c_str());
	printf("\n---------------------------------\n");

	return bSuccess;
}

bool P_DMMission::GetMissionInfoFromScript(DMMissionInfo** ppInfo)
{
	ZFScript* pkScript = (ZFScript*) (*ppInfo)->m_pkScriptResHandle->GetResourcePtr();

	char szInfoTextShort[256];
	char szInfoTextLong[1024];
	char szName[128];
	double dDifficulty;
	double dXP;

	(*ppInfo)->m_pkScriptResHandle = (*ppInfo)->m_pkScriptResHandle;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionInfo", "name", szName);
	(*ppInfo)->m_strName = szName;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionInfo", "difficulty", dDifficulty);
	(*ppInfo)->m_iLevel = (int) dDifficulty;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionInfo", "xp", dXP);
	(*ppInfo)->m_iXP = (int) dXP;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionText", "short", szInfoTextShort);	
	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionText", "long", szInfoTextLong);
	(*ppInfo)->m_strInfoTextShort = szInfoTextShort;
	(*ppInfo)->m_strInfoTextLong = szInfoTextLong;

	return true;
}

Property* Create_P_DMMission()
{
	return new P_DMMission;
}

void P_DMMission::GetPossibleMissions(int iLevel, vector<DMMissionInfo>& vkInfo)
{
	for(int i=0; i<m_vkMissions.size(); i++)
	{
		if(m_vkMissions[i]->m_iLevel == iLevel)
		{
			DMMissionInfo kInfo;
			kInfo.m_iLevel = m_vkMissions[i]->m_iLevel;
			kInfo.m_iCash = m_vkMissions[i]->m_iCash;
			kInfo.m_iXP = m_vkMissions[i]->m_iXP;
			kInfo.m_pkScriptResHandle = m_vkMissions[i]->m_pkScriptResHandle;
			kInfo.m_strInfoTextLong = m_vkMissions[i]->m_strInfoTextLong;
			kInfo.m_strInfoTextShort = m_vkMissions[i]->m_strInfoTextShort;
			kInfo.m_strName = m_vkMissions[i]->m_strName;
			kInfo.m_strScript = m_vkMissions[i]->m_strScript;

			vkInfo.push_back( kInfo );
		}
	}
}