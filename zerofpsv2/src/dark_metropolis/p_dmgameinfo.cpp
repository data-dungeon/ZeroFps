#include "p_dmgameinfo.h" 

P_DMGameInfo::P_DMGameInfo()
{
	strcpy(m_acName,"P_DMGameInfo");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
//	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
//	m_pkEntityMan=static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
//	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			

	bNetwork = true;
	
}

P_DMGameInfo::~P_DMGameInfo()
{


}

void P_DMGameInfo::Init()
{
	cout<<"game info created"<<endl;

}


vector<PropertyValues> P_DMGameInfo::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		

	return kReturn;
}


Property* Create_P_DMGameInfo()
{
	return new P_DMGameInfo;
}