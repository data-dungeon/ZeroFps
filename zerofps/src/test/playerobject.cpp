#include "playerobject.h"



PlayerObject::PlayerObject(HeightMap *pkMap,Input *pkInput,ZeroFps *pkFps)
{
	AddProperty(new CollisionProperty(&m_kPos,new float(1.0)));
	AddProperty(new PlayerControlProperty(pkInput,pkFps,pkMap));
	AddProperty(new GravityProperty(pkFps));
	AddProperty(new FloatProperty(pkFps));	
//	AddProperty(new ModelProperty());
	
	onGround=false;
	m_pkFps=pkFps;
}

void PlayerObject::HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue)
{
	if(typeid(*pkObject)==typeid(HeightMapObject)){
//		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkObject);
	
		GetPos()=kPos+Vector3(0,1,0) ;//GetPos()+(GetPos()-kPos);
		m_kVel.y=0;
		onGround=true;
	} else if(typeid(*pkObject)==typeid(BallObject)){
		
		GetPos()=Vector3(GetPos().x,kPos.y+1,GetPos().z);//+Vector3(0,1,0);
		m_kVel.y=0;
		onGround=true;
	
	} else if(bContinue){
		pkObject->HandleCollision(this,kPos,false);
	}
	

}

void PlayerObject::ObjectUpdate() {
//	m_kPos+=m_kVel*m_pkFps->GetFrameTime();	


}




