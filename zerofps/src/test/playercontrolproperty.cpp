#include "playercontrolproperty.h"

PlayerControlProperty::PlayerControlProperty(Input *pkInput,ZeroFps *pkFps,HeightMap *pkMap) {
	m_pkMap=pkMap;
	m_pkFps=pkFps;
	m_pkInput=pkInput;	
	strcpy(m_acName,"PlayerControlProperty");
	
	ner=0;
};


void PlayerControlProperty::Update() {
//	m_pkObject->GetPos().x+=0.1;
//	m_pkObject->GetPos().z+=0.1;	
	float speed=0.01;
	
	if(m_pkInput->Pressed(KEY_D)){
		m_pkObject->GetPos().x+=speed*m_pkFps->GetFrameTime();
	}
	if(m_pkInput->Pressed(KEY_A)){
		m_pkObject->GetPos().x-=speed*m_pkFps->GetFrameTime();
	}
	if(m_pkInput->Pressed(KEY_W)){
		m_pkObject->GetPos().z-=speed*m_pkFps->GetFrameTime();
	}
	if(m_pkInput->Pressed(KEY_S)){
		m_pkObject->GetPos().z+=speed*m_pkFps->GetFrameTime();
	}
	
	if(m_pkInput->Pressed(KEY_Z)){
		m_pkObject->GetRot().y-=0.1*m_pkFps->GetFrameTime();
	}
	if(m_pkInput->Pressed(KEY_C)){
		m_pkObject->GetRot().y+=0.1*m_pkFps->GetFrameTime();
	}
	
	
	
	if(m_pkInput->Pressed(KEY_Q)){
		ner+=0.13;
	}
	
	
	ner-=0.08;
	
	m_pkObject->GetPos().y+=ner;


//	if(m_pkObject->GetPos().x>100)
//		m_pkObject->Remove();

};

void PlayerControlProperty::Bounce(Vector3 kPos){
	ner=abs((ner/1.2));
//	m_pkObject->GetPos().y+=0.25;

	m_pkObject->GetPos()=kPos;
}






