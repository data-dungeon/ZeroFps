#include "floatproperty.h"

FloatProperty::FloatProperty(ZeroFps *pkFps,HeightMap *pkMap) {
	m_pkFps=pkFps;
	m_pkMap=pkMap;
	
	m_fGravity=-0.00002;
	strcpy(m_acName,"GravityProperty");	
}

void FloatProperty::Update() {
	m_pkObject->GetVel().y-=m_fGravity * m_pkFps->GetFrameTime();
	m_pkObject->GetPos().y+=m_pkObject->GetVel().y * m_pkFps->GetFrameTime();
}




