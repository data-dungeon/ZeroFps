#include "lightupdateproperty.h" 	
#include "../../basic/zfsystem.h"
  	
P_LightUpdate::P_LightUpdate()
{
	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light")); 	

	strcpy(m_acName,"P_LightUpdate");
	bNetwork = true;

	m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_CLIENT;
}

void P_LightUpdate::Update()
{
//	if(m_pkObject->GetUpdateStatus() & UPDATE_LIGHT)
		m_pkLight->Update(m_pkObject->GetWorldPosV());
}

Property* Create_LightUpdateProperty()
{
	return new P_LightUpdate;
}
