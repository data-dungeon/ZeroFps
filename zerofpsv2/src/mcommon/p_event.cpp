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
	if(m_pkScriptResHandle)
		delete m_pkScriptResHandle;

	m_pkScriptResHandle = new ZFResourceHandle;
	if(!m_pkScriptResHandle->SetRes(m_pkObject->GetType()))
		printf("Failed to load event script %s\n", m_pkObject->GetType().c_str());
	
	ZFScript* pkScriptRes = (ZFScript*)m_pkScriptResHandle->GetResourcePtr();

//	cout<<"ba"<<endl;
//	cout<<"SCRIPT:"<<(char*)m_pkObject->GetType().c_str()<<endl;
	if(!m_pkScriptSys->Run(pkScriptRes))
		return false;
		
//	cout<<"bla"<<endl;
	if(!m_pkScriptSys->Call(pkScriptRes, (char*)acEvent, 0, 0))
		return false;

//	cout<<"hohoa"<<endl;
	return true;
}

Property* Create_P_Event()
{
	return new P_Event;
}
