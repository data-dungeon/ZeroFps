#include "proxyproperty.h"

ProxyProperty::ProxyProperty() 
{
	strcpy(m_acName,"ProxyProperty");		
	m_fRadius=200;
	m_iAktion=UPDATECHILDS;
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
}

void ProxyProperty::Update() 
{
	float fDistance= abs((m_pkFps->GetCam()->GetPos() - m_pkObject->GetPos()).Length());

	if(fDistance < m_fRadius){		
		//if camera is inside the proximity
		switch(m_iAktion) {
			case UPDATECHILDS:
				m_pkObject->GetUpdateChilds()=true;
				break;				
		}
	} else {
		//if the camera is outside the proximity
		switch(m_iAktion) {
			case UPDATECHILDS:
				m_pkObject->GetUpdateChilds()=false;
				break;				
		}		
	}
}

Property* Create_ProxyProperty()
{
	return new ProxyProperty;
}
















