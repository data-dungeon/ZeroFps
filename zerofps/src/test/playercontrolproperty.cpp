#include "playercontrolproperty.h"

PlayerControlProperty::PlayerControlProperty(Input *pkInput,HeightMap *pkMap) {
	m_pkMap=pkMap;
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));	
	m_pkCollisionMan = static_cast<CollisionManager*>(g_ZFObjSys.GetObjectPtr("CollisionManager"));	
	//m_pkFps=pkFps;
	m_pkInput=pkInput;	
	strcpy(m_acName,"PlayerControlProperty");
};

/*
class	ClientMoveCommand
{
public:
	Vector3 kTranslate;
	Vector3 kRotate;
	int		iJump;
};
*/

void PlayerControlProperty::Update() {
	float speed=6;
	
	//cant move fast while in air
//	if(dynamic_cast<PlayerObject*>(m_pkObject)->onGround==false)
//		speed*=0.5;
	

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

/*	if(m_pkInput->Pressed(KEY_Z)){
		m_pkObject->GetRot().y-=0.1*m_pkFps->GetFrameTime();
	}
	if(m_pkInput->Pressed(KEY_C)){
		m_pkObject->GetRot().y+=0.1*m_pkFps->GetFrameTime();
	}*/

	if(m_pkInput->Pressed(MOUSERIGHT) ){
		if(dynamic_cast<PlayerObject*>(m_pkObject)->onGround){
			m_pkObject->GetVel().y+=5;// *m_pkFps->GetFrameTime();;						
		}
	}
	if(m_pkInput->Pressed(KEY_Q)){
		Object *test = new BunnyObject();
		test->GetPos()=m_pkObject->GetPos();
		m_pkObjectMan->Add(test);
		m_pkCollisionMan->Add(test);	
	}
	
	if(m_pkInput->Pressed(KEY_E))
	{
		float height=m_pkMap->GetVert(int(m_pkObject->GetPos().x),int(m_pkObject->GetPos().z))->height;
		for(int xp=-1;xp<2;xp++){
			for(int yp=-1;yp<2;yp++){
				m_pkMap->GetVert(int(m_pkObject->GetPos().x+xp),int(m_pkObject->GetPos().z+yp))->texture=2;
				m_pkMap->GetVert(int(m_pkObject->GetPos().x+xp),int(m_pkObject->GetPos().z+yp))->color=Vector3(.6,.45,0.3);		
				m_pkMap->GetVert(int(m_pkObject->GetPos().x+xp),int(m_pkObject->GetPos().z+yp))->height=height;				
			}
		}
	}
	
	
	//Get mouse x,y		
	int x,z;		
	m_pkInput->RelMouseXY(x,z);

	//rotate the camera		
	m_pkObject->GetRot().x+=z/5.0;
	m_pkObject->GetRot().y+=x/5.0;
	
	if(m_pkObject->GetRot().x>90)
		m_pkObject->GetRot().x=90;
	
	if(m_pkObject->GetRot().x<-90)
		m_pkObject->GetRot().x=-90;
	
	
	dynamic_cast<PlayerObject*>(m_pkObject)->onGround=false;
};

void PlayerControlProperty::Bounce(Vector3 kPos){
//	ner=abs((ner*0.7));
//	m_pkObject->GetPos().y+=0.25;

	m_pkObject->GetPos()=kPos;
}






