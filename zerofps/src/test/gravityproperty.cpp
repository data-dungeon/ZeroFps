#include "gravityproperty.h"

GravityProperty::GravityProperty(ZeroFps *pkFps) {
	m_pkFps=pkFps;
	
	m_fGravity=0.00005;
}

void GravityProperty::Update() {
	m_pkObject->GetVel().y-=m_fGravity*m_pkFps->GetFrameTime();	

}





