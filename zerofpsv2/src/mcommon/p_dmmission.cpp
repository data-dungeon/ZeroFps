#include "p_dmmission.h" 
#include "si_dm.h"
#include "p_dmgameinfo.h"
#include "p_dmcharacter.h" 
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/gui/zguiwindow.h"

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
	m_pkGameInfoEntity = NULL;
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

	int antal = t.size();

	for(unsigned int i=0; i<antal; i++)
	{
		if(t[i].find(".lua") != string::npos)
		{
			bool bDubble = false;
			for(unsigned int j=0; j<final.size(); j++)
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

	for(unsigned int i=0; i<final.size(); i++)
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
	if(m_pkGameInfoEntity == NULL)
	{
		Entity* pkGlobal = m_pkObjectMan->GetGlobalObject();

		vector<Entity*> kObjects;
		pkGlobal->GetAllEntitys(&kObjects);

		for(unsigned int i=0;i<kObjects.size();i++)
			if(kObjects[i]->GetProperty("P_DMGameInfo"))
			{
				m_pkGameInfoEntity = (P_DMGameInfo*) 
					kObjects[i]->GetProperty("P_DMGameInfo");
				break;
			}
	}

	if(m_pkGameInfoEntity == NULL)
		return;

	if(m_pkCurrentMission == NULL)
		return;

	if(m_pkCurrentMission->m_pkScriptResHandle != NULL)
	{
		//
		// Kolla om uppdraget har slutförts 1 gång var 3:e sekund
		// 

		float fTimeCheck = m_pkZeroFps->m_pkObjectMan->GetSimTime();

		if(fTimeCheck - m_fMissionDoneCheckTime > 3.0f)
		{
			m_pkScriptSys->Call(m_pkCurrentMission->m_pkScriptResHandle, 
				"IsMissionDone", 0, 0);
			m_fMissionDoneCheckTime = fTimeCheck;

			double dSuccess;
			m_pkScriptSys->GetGlobal(
				((ZFScript*)m_pkCurrentMission->m_pkScriptResHandle->GetResourcePtr())->m_pkLuaState, 
				"MissionInfo", "success", dSuccess);

			if(dSuccess > 0.9f) // lyckats med uppdraget?
			{
				OnMissionSuccess();
				
				RemoveMission(m_pkCurrentMission->m_strScript);
				m_pkCurrentMission = NULL;

				return ; // returnera (viktigt)
			}

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
			if(m_pkCurrentMission == (*it))
				m_pkCurrentMission = NULL;

			delete (*it)->m_pkScriptResHandle;
			delete (*it);

			m_vkMissions.erase(it);

			bSuccess = true;

			break;
		}
	}

	return bSuccess;
}

bool P_DMMission::CancelCurrent()
{
	if(m_pkCurrentMission)
	{
		m_pkScriptSys->Call(m_pkCurrentMission->m_pkScriptResHandle, 
			"OnMissionFailed", 0, 0);
	}

	return RemoveMission(m_pkCurrentMission->m_strScript);
}

bool P_DMMission::SetCurrentMission(string strMissionScript)
{
	bool bSuccess = false;

	for(unsigned int i=0; i<m_vkMissions.size(); i++)
	{
		if(m_vkMissions[i]->m_strScript == strMissionScript)
		{
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

	//
	// Anropa den Lua funktion som har till uppgift att initiera uppdraget.
	//
	m_pkScriptSys->Call(m_pkCurrentMission->m_pkScriptResHandle, 
		"OnMissionStart", 0, 0);

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
	double dChash;

	(*ppInfo)->m_pkScriptResHandle = (*ppInfo)->m_pkScriptResHandle;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionInfo", "name", szName);
	(*ppInfo)->m_strName = szName;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionInfo", "difficulty", dDifficulty);
	(*ppInfo)->m_iLevel = (int) dDifficulty;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionInfo", "xp", dXP);
	(*ppInfo)->m_iXP = (int) dXP;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionInfo", "cash", dChash);
	(*ppInfo)->m_iCash = (int) dChash;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionText", "short", szInfoTextShort);	
	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionText", "long", szInfoTextLong);
	(*ppInfo)->m_strInfoTextShort = szInfoTextShort;
	(*ppInfo)->m_strInfoTextLong = m_pkScriptSys->FormatMultiLineTextFromLua(szInfoTextLong);

	return true;
}

Property* Create_P_DMMission()	
{
	return new P_DMMission;
}

void P_DMMission::GetPossibleMissions(float fReputation, vector<DMMissionInfo>& vkInfo)
{
	for(unsigned int i=0; i<m_vkMissions.size(); i++)
	{
		if(m_vkMissions[i]->m_iLevel <= fReputation)
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


void P_DMMission::OnMissionSuccess()
{
	int ExtraCash = m_pkCurrentMission->m_iCash;
	int ExtraXP = m_pkCurrentMission->m_iXP;

	// Öka gruppens pengar
	m_pkGameInfoEntity->m_iMoney += ExtraCash;

	// Öka gruppens reputatation, som möjligör fler svårare uppdrag.
	m_pkGameInfoEntity->m_fReputation += (float) ExtraXP / 1000.0f;
	
	unsigned int iAntal=0, i;
	vector<Entity*> kObjects;	
	m_pkObjectMan->GetAllObjects(&kObjects);

	// Kolla hur många agenter som finns.
	for(i=0;i<kObjects.size();i++)
		if((P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter"))
			if ( ((P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter"))->m_iTeam == 0 )
				iAntal++;

	int iXPEachMember = ExtraXP/iAntal;

	// Öka alla agenters XP
	for(i=0;i<kObjects.size();i++)
		if((P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter"))
			if ( ((P_DMCharacter*)kObjects[i]->GetProperty("P_DMCharacter"))->m_iTeam == 0 )
			{
				((P_DMCharacter*)kObjects[i]->GetProperty(
					"P_DMCharacter"))->AddXP(iXPEachMember);
			}

	char text[150];
	sprintf(text, "Mission %s success!\nRaising cash for (%i) agents, with %i and xp with %i\n%s%f",
		m_pkCurrentMission->m_strName.c_str(), 
		iAntal,
		m_pkCurrentMission->m_iCash,
		m_pkCurrentMission->m_iXP,
		"The groups reputation is now:",
		m_pkGameInfoEntity->m_fReputation);


	ZGuiResourceManager* pkGuiResMan = (ZGuiResourceManager*) 
		static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));

	pkGuiResMan->Wnd("MissionInfoWnd")->Show();
	pkGuiResMan->Wnd("MissionInfoLabel")->SetText(text); 
}




















