#include "gravityproperty.h"

GravityProperty::GravityProperty() {
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	
	m_fGravity=10;
	strcpy(m_acName,"GravityProperty");	

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
}

void GravityProperty::Update() {

/*	m_pkObject->GetVel().y-=m_fGravity * m_pkFps->GetFrameTime(); */
	if(m_pkObject->GetVel().y>-12) 
		m_pkObject->GetVel().y-=m_fGravity * m_pkFps->GetFrameTime();	
	
	m_pkObject->GetPos().y+=m_pkObject->GetVel().y * m_pkFps->GetFrameTime();
}





