#include "lightupdateproperty.h" 	
#include "../../basic/zfsystem.h"
  	
LightUpdateProperty::LightUpdateProperty()
{
	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light")); 	
	
	strcpy(m_acName,"LightUpdateProperty");
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
}

void LightUpdateProperty::Update()
{
//	if(m_pkObject->GetUpdateStatus() & UPDATE_LIGHT)
		m_pkLight->Update(m_pkObject->GetWorldPosV());

}



Property* Create_LightUpdateProperty()
{
	return new LightUpdateProperty;
}
