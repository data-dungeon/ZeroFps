#include "playercontrolproperty.h"

PlayerControlProperty::PlayerControlProperty(Input *pkInput) {
	m_pkInput=pkInput;
	strcpy(m_acName,"PlayerControlProperty");
};


void PlayerControlProperty::Update() {
//	m_pkObject->GetPos().x+=0.1;
//	m_pkObject->GetPos().z+=0.1;	
	float speed=0.000001;
	
	if(m_pkInput->Pressed(KEY_D)){
		m_pkObject->GetPos().x+=speed*SDL_GetTicks();
	}
	if(m_pkInput->Pressed(KEY_A)){
		m_pkObject->GetPos().x-=speed*SDL_GetTicks();
	}
	if(m_pkInput->Pressed(KEY_W)){
		m_pkObject->GetPos().z-=speed*SDL_GetTicks();
	}
	if(m_pkInput->Pressed(KEY_S)){
		m_pkObject->GetPos().z+=speed*SDL_GetTicks();
	}


/*	
		pkFps->CamPos().x+=cos((pkFps->CamRot().y)/degtorad) *pkFps->GetFrameTime()*speed;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y)/degtorad) *pkFps->GetFrameTime()*speed;				
	}
	if(pkInput->Pressed(KEY_A)){
		pkFps->CamPos().x+=cos((pkFps->CamRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y+180)/degtorad)*pkFps->GetFrameTime()*speed;				
	}
	
	if(pkInput->Pressed(KEY_W))	{
			pkFps->CamPos().x+=cos((pkFps->CamRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
			pkFps->CamPos().z+=sin((pkFps->CamRot().y-90)/degtorad)*pkFps->GetFrameTime()*speed;			
	}					
	if(pkInput->Pressed(KEY_S))	{
		pkFps->CamPos().x+=cos((pkFps->CamRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;			
		pkFps->CamPos().z+=sin((pkFps->CamRot().y-90-180)/degtorad)*pkFps->GetFrameTime()*speed;
	}		

	if(pkInput->Pressed(KEY_E))
		pkFps->CamPos().y+=2*pkFps->GetFrameTime()*speed;			
	if(pkInput->Pressed(KEY_Q))
		pkFps->CamPos().y-=2*pkFps->GetFrameTime()*speed;

	
	
//	m_pkObject->GetPos().y=m_pkMap->Height(m_pkObject->GetPos().x,m_pkObject->GetPos().z);


//	Vector3 normal=m_pkMap->Tilt(m_pkObject->GetPos().x,m_pkObject->GetPos().z);
//	m_pkRender->Line(m_pkObject->GetPos(),m_pkObject->GetPos()+normal);
*/	

};






