#include "p_dmhq.h" 

P_DMHQ::P_DMHQ()
{
	strcpy(m_acName,"P_DMHQ");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
//	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
//	m_pkEntityMan=static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
//	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			

	bNetwork = true;
	

	m_strName = "Unnamed HQ";
}

P_DMHQ::~P_DMHQ()
{


}

void P_DMHQ::Init()
{
	cout<< "New HQ created"<<endl;

}


vector<PropertyValues> P_DMHQ::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		

	return kReturn;
}


Property* Create_P_DMHQ()
{
	return new P_DMHQ;
}