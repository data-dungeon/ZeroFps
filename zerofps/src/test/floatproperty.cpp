#include "floatproperty.h"

FloatProperty::FloatProperty() {
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
//	m_pkFps=pkFps;
	
	m_fGravity=-0.00002;
	strcpy(m_acName,"GravityProperty");	
	

}

void FloatProperty::Update() {
	if(m_pkObject->GetPos().y<0) {
		if(m_pkObject->GetVel().y<0.01) 
			m_pkObject->GetVel().y-=m_fGravity * m_pkFps->GetFrameTime();
		m_pkObject->GetPos().y+=m_pkObject->GetVel().y * m_pkFps->GetFrameTime();
		
		m_pkObject->GetVel()*0.5;	
	}
}




