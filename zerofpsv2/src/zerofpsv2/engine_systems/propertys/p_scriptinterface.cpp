#include "p_scriptinterface.h" 
#include "../script_interfaces/si_objectmanager.h"

P_ScriptInterface::P_ScriptInterface()
{
	strcpy(m_acName,"P_ScriptInterface");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkScriptSys=static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));
	
	m_bHaveRunInit=false;
	m_bFirstRun=true;
	m_fHeartRate=-1;

	m_fTimer = m_pkFps->m_pkObjectMan->GetGameTime();
	
	
	//cout<<"P_ScriptInterface created"<<endl;
}

P_ScriptInterface::~P_ScriptInterface()
{
	CallFunction("Destroy");	
}

void P_ScriptInterface::Update()
{
	if(m_bFirstRun)
	{
		m_bFirstRun=false;
		
		CallFunction("FirstRun");
	}

	if(!m_bHaveRunInit)
	{
		m_bHaveRunInit = true;		
		CallFunction("Init");
	}

	if(m_fHeartRate != -1)
		if(m_pkFps->m_pkObjectMan->GetGameTime() - m_fTimer > m_fHeartRate)
		{
			CallFunction("HeartBeat");
			
			m_fTimer = m_pkFps->m_pkObjectMan->GetGameTime();
		}
}


bool P_ScriptInterface::CallFunction(const char* acFunction,vector<ARG_DATA>* pkParams)
{
	//set self id before calling the funktion
	ObjectManagerLua::g_iCurrentObjectID = m_pkObject->iNetWorkID;	
		
		
	if(pkParams)
		return m_pkScriptSys->Call(m_pkObject->GetObjectScript(), (char*)acFunction,*pkParams);
	else
		return m_pkScriptSys->Call(m_pkObject->GetObjectScript(), (char*)acFunction,0,0);	

}

void P_ScriptInterface::SetHeartRate(float blub) 
{
	m_fHeartRate = blub;
	
	m_fTimer += rand() % (int)m_fHeartRate;
}


void P_ScriptInterface::Save(ZFIoInterface* pkPackage)
{
   pkPackage->Write ( (void*)&m_bFirstRun, sizeof(m_bFirstRun), 1 );
   pkPackage->Write ( (void*)&m_fHeartRate, sizeof(m_fHeartRate), 1 );
}

void P_ScriptInterface::Load(ZFIoInterface* pkPackage)
{
   pkPackage->Read ( (void*)&m_bFirstRun, sizeof(m_bFirstRun), 1 );
   pkPackage->Read ( (void*)&m_fHeartRate, sizeof(m_fHeartRate), 1 );
	SetHeartRate(m_fHeartRate);
}


Property* Create_P_ScriptInterface()
{
	return new P_ScriptInterface;
}







