#include "p_event.h" 

P_Event::P_Event()
{
	strcpy(m_acName,"P_Event");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkScriptSys=static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));

	m_pkScriptResHandle = NULL;
	
	m_bHaveRunInit=false;
	m_bRun1SUpdate=true;

	m_fTimer = m_pkFps->GetGameTime();
}

P_Event::~P_Event()
{
	SendEvent("Destroy");	
}

void P_Event::Update()
{
	if(!m_bHaveRunInit)
	{
		m_bHaveRunInit = true;		
		SendEvent("Init");
	}

	if(m_bRun1SUpdate)
		if(m_pkFps->GetGameTime() - m_fTimer > 1.0)
		{
			SendEvent("Update1S");
			
			m_fTimer = m_pkFps->GetGameTime();
		}
}


bool P_Event::SendEvent(const char* acEvent)
{
	if(m_pkObject->GetObjectScript())
	{
		//set self id before calling the funktion
		MistLandLua::g_iCurrentObjectID = m_pkObject->iNetWorkID;
		
		if(!m_pkScriptSys->Call(m_pkObject->GetObjectScript(), (char*)acEvent, 0, 0))
		{
			//cout<<"error calling "<<acEvent<<endl;
			return false;
		}
		
		return true;
	}

	return false;
}

Property* Create_P_Event()
{
	return new P_Event;
}
