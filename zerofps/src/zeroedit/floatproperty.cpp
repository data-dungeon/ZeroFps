#include "floatproperty.h"

FloatProperty::FloatProperty() {
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
//	m_pkFps=pkFps;
	
	m_fGravity=-13;
	strcpy(m_acName,"FloatProperty");	
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
}

void FloatProperty::Update() {
/*
	if(m_pkObject->GetPos().y<0) {
		//if upspeed becomes to great dont add more speed
		if(m_pkObject->GetVel().y<4) 
			m_pkObject->GetVel().y-=m_fGravity * m_pkFps->GetFrameTime();
		m_pkObject->GetPos().y+=m_pkObject->GetVel().y * m_pkFps->GetFrameTime();
		
		m_pkObject->GetVel().y*=0.99;
	}
*/	
}

void FloatProperty::Touch(CollisionData* Data)
{
//	m_pkObject->GetPos()=Data->m_kPos;
//	m_pkObject->GetVel()=Vector3(0,0,0);
//	m_pkObject->GetAcc()=Data->m_kAcc;
	cout<<"BALLE"<<endl;
}



Property* Create_FloatProperty()
{
	return new FloatProperty();
}







