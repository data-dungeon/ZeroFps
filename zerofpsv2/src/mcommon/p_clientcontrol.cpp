#include "p_clientcontrol.h" 

P_ClientControl::P_ClientControl()
{
	strcpy(m_acName,"P_ClientControl");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));


	cout<<"client Control created"<<endl;
}

void P_ClientControl::Update()
{
	//cout<<"client control here"<<endl;

}


Property* Create_P_ClientControl()
{
	return new P_ClientControl;
}
