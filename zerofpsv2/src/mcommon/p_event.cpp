#include "p_event.h" 

P_Event::P_Event()
{
	strcpy(m_acName,"P_Event");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkScript=static_cast<ZFScript*>(g_ZFObjSys.GetObjectPtr("ZFScript"));

	
	m_bHaveRunInit=false;
}

void P_Event::Update()
{
	if(!m_bHaveRunInit)	
		m_bHaveRunInit = SendEvent("Init");

}


bool P_Event::SendEvent(const char* acEvent)
{
//	cout<<"ba"<<endl;
//	cout<<"SCRIPT:"<<(char*)m_pkObject->GetType().c_str()<<endl;
	if(!m_pkScript->RunScript((char*)m_pkObject->GetType().c_str()))
		return false;
		
//	cout<<"bla"<<endl;
	if(!m_pkScript->CallScript((char*)acEvent, 0, 0))
		return false;

//	cout<<"hohoa"<<endl;
	return true;
}

Property* Create_P_Event()
{
	return new P_Event;
}
