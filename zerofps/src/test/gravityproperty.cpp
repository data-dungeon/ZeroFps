#include "gravityproperty.h"

GravityProperty::GravityProperty(ZeroFps *pkFps) {
	m_pkFps=pkFps;
	
	m_fGravity=0.00001;
	strcpy(m_acName,"GravityProperty");	
}

void GravityProperty::Update() {
	if(m_pkObject->GetVel().y>-0.02) 
		m_pkObject->GetVel().y-=m_fGravity * m_pkFps->GetFrameTime();	
	
	m_pkObject->GetPos().y+=m_pkObject->GetVel().y * m_pkFps->GetFrameTime();
}





