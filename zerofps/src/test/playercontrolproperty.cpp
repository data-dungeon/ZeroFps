#include "playercontrolproperty.h"

PlayerControlProperty::PlayerControlProperty(Input *pkInput,ZeroFps *pkFps,HeightMap *pkMap) {
	m_pkMap=pkMap;
	m_pkFps=pkFps;
	m_pkInput=pkInput;	
	strcpy(m_acName,"PlayerControlProperty");
};


void PlayerControlProperty::Update() {
	float speed=0.006;
	
	if(m_pkInput->Pressed(KEY_D)){
		m_pkObject->GetPos().x+=cos((m_pkObject->GetRot().y)/degtorad)*m_pkFps->GetFrameTime()*speed;			
		m_pkObject->GetPos().z+=sin((m_pkObject->GetRot().y)/degtorad)*m_pkFps->GetFrameTime()*speed;			
	}
	if(m_pkInput->Pressed(KEY_A)){
		m_pkObject->GetPos().x+=cos((m_pkObject->GetRot().y+180)/degtorad)*m_pkFps->GetFrameTime()*speed;			
		m_pkObject->GetPos().z+=sin((m_pkObject->GetRot().y+180)/degtorad)*m_pkFps->GetFrameTime()*speed;			
	}
	if(m_pkInput->Pressed(KEY_W)){
		m_pkObject->GetPos().x+=cos((m_pkObject->GetRot().y-90)/degtorad)*m_pkFps->GetFrameTime()*speed;			
		m_pkObject->GetPos().z+=sin((m_pkObject->GetRot().y-90)/degtorad)*m_pkFps->GetFrameTime()*speed;			
	}
	if(m_pkInput->Pressed(KEY_S)){
		m_pkObject->GetPos().x+=cos((m_pkObject->GetRot().y-270)/degtorad)*m_pkFps->GetFrameTime()*speed;			
		m_pkObject->GetPos().z+=sin((m_pkObject->GetRot().y-270)/degtorad)*m_pkFps->GetFrameTime()*speed;			
	}
	if(m_pkInput->Pressed(KEY_Z)){
		m_pkObject->GetRot().y-=0.1*m_pkFps->GetFrameTime();
	}
	if(m_pkInput->Pressed(KEY_C)){
		m_pkObject->GetRot().y+=0.1*m_pkFps->GetFrameTime();
	}
	if(m_pkInput->Pressed(KEY_Q) || m_pkInput->Pressed(MOUSERIGHT) ){
		if(dynamic_cast<PlayerObject*>(m_pkObject)->onGround){
			m_pkObject->GetVel().y+=0.005;// *m_pkFps->GetFrameTime();;						
		}
	}
	
	
	//Get mouse x,y		
	int x,z;		
	m_pkInput->RelMouseXY(x,z);

	//rotate the camera		
	m_pkObject->GetRot().x+=z/5.0;
	m_pkObject->GetRot().y+=x/5.0;
	
	
	dynamic_cast<PlayerObject*>(m_pkObject)->onGround=false;
};

void PlayerControlProperty::Bounce(Vector3 kPos){
//	ner=abs((ner*0.7));
//	m_pkObject->GetPos().y+=0.25;

	m_pkObject->GetPos()=kPos;
}






