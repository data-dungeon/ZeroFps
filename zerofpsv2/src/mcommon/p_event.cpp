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

}

void P_Event::Update()
{
	if(!m_bHaveRunInit)	
		m_bHaveRunInit = SendEvent("Init");

}


bool P_Event::SendEvent(const char* acEvent)
{
	if(m_pkObject->GetObjectScript())
	{
		//ZFScript* pkScriptRes = (ZFScript*)m_pkObject->GetObjectScript()->GetResourcePtr();

		if(!m_pkScriptSys->Call(m_pkObject->GetObjectScript(), (char*)acEvent, 0, 0))
			return false;

		return true;
	}
}

Property* Create_P_Event()
{
	return new P_Event;
}
