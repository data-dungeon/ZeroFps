#include "playerobject.h"



PlayerObject::PlayerObject(HeightMap *pkMap,Input *pkInput,ZeroFps *pkFps)
{
	AddProperty(new CollisionProperty(&m_kPos,new float(1.0)));
//	AddProperty(new CollisionProperty(&m_kPos));	
//	AddProperty(new CollisionProperty(&m_kPos));
	AddProperty(new PlayerControlProperty(pkInput,pkFps,pkMap));
	AddProperty(new GravityProperty(pkFps));
	
	onGround=false;

}

void PlayerObject::HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue)
{
	if(typeid(*pkObject)==typeid(HeightMapObject)){
		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkObject);

/*
		if(GetProperty("PlayerControlProperty")!=NULL) {
			PlayerControlProperty *pc= dynamic_cast<PlayerControlProperty*>(GetProperty("PlayerControlProperty"));			
			pc->ner=0;			
		}
*/		
		GetPos()=kPos;
		m_kVel.y=0;
		onGround=true;
		
	} else if(bContinue){
		pkObject->HandleCollision(this,kPos,false);
	}
	

}