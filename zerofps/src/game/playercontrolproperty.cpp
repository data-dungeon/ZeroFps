#include "playercontrolproperty.h"

PlayerControlProperty::PlayerControlProperty(Input *pkInput,HeightMap *pkMap) {
	m_pkMap=pkMap;
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));	
	m_pkCollisionMan = static_cast<CollisionManager*>(g_ZFObjSys.GetObjectPtr("CollisionManager"));	
	//m_pkFps=pkFps;
	m_pkInput=pkInput;	
	strcpy(m_acName,"PlayerControlProperty");

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	walk=0;
	walking=false;
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
	m_pkObject->GetObjectType()=OBJECT_TYPE_PLAYER;
	
	float speed=3;
	walking=false;
	
	//cant move fast while in air
//	if(dynamic_cast<PlayerObject*>(m_pkObject)->onGround==false)
//		speed*=0.5;
	
	if(m_pkInput->Pressed(KEY_X)){
		speed*=0.5;
	}
	
	if(m_pkInput->Pressed(KEY_D)){
		walking=true;		
		m_pkObject->GetPos().x+=cos((m_pkObject->GetRot().y)/degtorad)*m_pkFps->GetFrameTime()*speed;			
		m_pkObject->GetPos().z+=sin((m_pkObject->GetRot().y)/degtorad)*m_pkFps->GetFrameTime()*speed;			
	}
	if(m_pkInput->Pressed(KEY_A)){
		walking=true;		
		m_pkObject->GetPos().x+=cos((m_pkObject->GetRot().y+180)/degtorad)*m_pkFps->GetFrameTime()*speed;			
		m_pkObject->GetPos().z+=sin((m_pkObject->GetRot().y+180)/degtorad)*m_pkFps->GetFrameTime()*speed;			
	}
	if(m_pkInput->Pressed(KEY_W)){
		walking=true;
		m_pkObject->GetPos().x+=cos((m_pkObject->GetRot().y-90)/degtorad)*m_pkFps->GetFrameTime()*speed;			
		m_pkObject->GetPos().z+=sin((m_pkObject->GetRot().y-90)/degtorad)*m_pkFps->GetFrameTime()*speed;			
	}
	if(m_pkInput->Pressed(KEY_S)){
		walking=true;
		m_pkObject->GetPos().x+=cos((m_pkObject->GetRot().y-270)/degtorad)*m_pkFps->GetFrameTime()*speed;			
		m_pkObject->GetPos().z+=sin((m_pkObject->GetRot().y-270)/degtorad)*m_pkFps->GetFrameTime()*speed;			
	}
	if(m_pkInput->Pressed(MOUSERIGHT) ){
		if(dynamic_cast<PlayerObject*>(m_pkObject)->onGround){
			m_pkObject->GetVel().y+=5;// *m_pkFps->GetFrameTime();;						
		}
	}
	
	
	//camera tilting when walking
	if(walk>6.28)
		walk=0;
	if(walk<0)
		walk=6.28;
	if(walking)
		walk+=m_pkFps->GetFrameTime()*5;
	else
		walk*=0.99;
	m_pkObject->GetRot().z = (sin(walk))*2.5;	
	
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






