#include "treeidleproperty.h"


TreeIdleProperty::TreeIdleProperty()
{
	strcpy(m_acName,"TreeIdleProperty");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;	
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_fWind=1;
	m_fRand= (rand()%2000)/1000.0;

}

void TreeIdleProperty::Update()
{
	float t=m_pkFps->GetTicks();
	
	m_pkObject->GetRot().x+=sin(t+m_fRand)*m_fWind;

}

vector<PropertyValues> TreeIdleProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	kReturn[0].kValueName="m_fWind";
	kReturn[0].iValueType=VALUETYPE_FLOAT;
	kReturn[0].pkValue=(void*)&m_fWind;
	
	return kReturn;
};


void TreeIdleProperty::Save(ZFMemPackage* pkPackage)
{
	pkPackage->Write((void*)&m_fWind,4);


}

void TreeIdleProperty::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read((void*)&m_fWind,4);

}

Property* Create_TreeIdleProperty()
{
	return new TreeIdleProperty;

}





