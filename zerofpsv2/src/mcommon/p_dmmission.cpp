#include "p_dmmission.h" 
#include "si_dm.h"

P_DMMission::P_DMMission()
{
	strcpy(m_acName,"P_DMMission");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	m_strName = "Unnamed mission";
	m_strMissionScript = "";
	m_iDifficulty = 0;
	
	bNetwork = true;

	m_pkScriptSys = static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkScriptResHandle = NULL;
	m_fMissionDoneCheckTime=0;
	m_fMissionFailedCheckTime=0;
}

P_DMMission::~P_DMMission()
{


}

void P_DMMission::Init()
{
	cout<< "New mission created:" << m_strName.c_str() << " : "
		<< m_strMissionScript << endl;
}


vector<PropertyValues> P_DMMission::GetPropertyValues()
{
	vector<PropertyValues> kReturn(3);
		
	kReturn[0].kValueName = "name";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = &m_strName;		

	kReturn[1].kValueName = "difficulty";
	kReturn[1].iValueType = VALUETYPE_INT;
	kReturn[1].pkValue    = &m_iDifficulty;	

	kReturn[2].kValueName = "missionscript";
	kReturn[2].iValueType = VALUETYPE_STRING;
	kReturn[2].pkValue    = &m_strMissionScript;	

	return kReturn;
}

void P_DMMission::Save(ZFIoInterface* pkPackage)
{	
	char temp[256];
	
	strcpy(temp,m_strName.c_str());
	pkPackage->Write(temp,256,1);

	pkPackage->Write(&m_iDifficulty,sizeof(m_iDifficulty),1);

	strcpy(temp,m_strMissionScript.c_str());
	pkPackage->Write(temp,256,1);
}

void P_DMMission::Load(ZFIoInterface* pkPackage)
{
	char temp[256];
	
	pkPackage->Read(temp,256,1);
	m_strName = temp;

	pkPackage->Read(&m_iDifficulty,sizeof(m_iDifficulty),1);	

	pkPackage->Read(temp,256,1);
	m_strMissionScript = temp;
}

void P_DMMission::Update()
{
	if(m_pkScriptResHandle != NULL)
	{
		//
		// Kolla om uppdraget har slutförts 1 gång var 3:e sekund
		// 

		float fTimeCheck = m_pkZeroFps->m_pkObjectMan->GetGameTime();

		if(fTimeCheck - m_fMissionDoneCheckTime > 3.0f)
		{
			m_pkScriptSys->Call(m_pkScriptResHandle, "IsMissionDone", 0, 0);
			m_fMissionDoneCheckTime = fTimeCheck;

			if(DMLua::g_iMissionStatus == 1)
			{
				printf("\n---------------------------------\n");
				printf("Mission \"%s\" sucess!\n", m_strMissionScript.c_str());
				printf("\n---------------------------------\n");

				m_pkScriptSys->Call(m_pkScriptResHandle, "OnMissionSuccess", 0, 0);
			}
		}

		//
		// Kolla om uppdraget har misslyckats 1 gång var 3:e sekund
		// 

		if(fTimeCheck - m_fMissionFailedCheckTime > 3.0f)
		{
			m_pkScriptSys->Call(m_pkScriptResHandle, "IsMissionFailed", 0, 0);
			m_fMissionFailedCheckTime = fTimeCheck;

			if(DMLua::g_iMissionStatus == -1)
			{
				printf("\n---------------------------------\n");
				printf("Mission \"%s\" failed!\n", m_strMissionScript.c_str());
				printf("\n---------------------------------\n");

				//m_pkScriptSys->Call(m_pkScriptResHandle, "OnMissionSuccess", 0, 0);
			}
		}
	}
}

bool P_DMMission::SetCurrentMission(string strMissionScript)
{
	m_strMissionScript = strMissionScript;
	
	if(m_pkScriptResHandle)
	{
		delete m_pkScriptResHandle;
		m_pkScriptResHandle = NULL;
	}

	m_pkScriptResHandle = new ZFResourceHandle;
	if(!m_pkScriptResHandle->SetRes(m_strMissionScript))
	{
		printf("Failed to load lua script: %s, for P_DMMission\n", 
			m_strMissionScript.c_str());
		return false;
	}

	printf("\n---------------------------------\n");
	printf("Starting mission \"%s\"\n", strMissionScript.c_str());
	printf("\n---------------------------------\n");

	return GetMissionInfoFromScript();
}

bool P_DMMission::GetMissionInfoFromScript()
{
	ZFScript* pkScript = (ZFScript*) m_pkScriptResHandle->GetResourcePtr();

	char szName[128];
	double dDifficulty;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionInfo", "name", szName);
	m_strName = szName;

	m_pkScriptSys->GetGlobal(pkScript->m_pkLuaState, "MissionInfo", "difficulty", dDifficulty);
	m_iDifficulty = dDifficulty;

	return true;
}

Property* Create_P_DMMission()
{
	return new P_DMMission;
}