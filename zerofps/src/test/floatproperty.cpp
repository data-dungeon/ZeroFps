#include "floatproperty.h"

FloatProperty::FloatProperty(ZeroFps *pkFps) {
	m_pkFps=pkFps;
	
	m_fGravity=-0.00002;
	strcpy(m_acName,"GravityProperty");	
}

void FloatProperty::Update() {
	if(m_pkObject->GetPos().y<0) {
		m_pkObject->GetVel().y-=m_fGravity * m_pkFps->GetFrameTime();
		m_pkObject->GetPos().y+=m_pkObject->GetVel().y * m_pkFps->GetFrameTime();
	}
}




