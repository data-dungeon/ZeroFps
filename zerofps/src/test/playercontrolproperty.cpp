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
	
	if(m_pkInput->Pressed(KEY_Q)){
		m_pkObject->GetPos().y=100;
	}
	
	
	ner-=0.20;
	
	m_pkObject->GetPos().y+=ner;

	if(m_pkObject->GetPos().y<m_pkMap->Height(m_pkObject->GetPos().x,m_pkObject->GetPos().z)){
		ner=(ner/1.2)*-1;
		m_pkObject->GetPos().y=m_pkMap->Height(m_pkObject->GetPos().x,m_pkObject->GetPos().z);
	}

};






