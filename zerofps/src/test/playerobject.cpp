#include "playerobject.h"



PlayerObject::PlayerObject(HeightMap *pkMap,Input *pkInput)
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	AddProperty(new CollisionProperty(&m_kPos,new float(1.0)));
	AddProperty(new PlayerControlProperty(pkInput,pkMap));
	AddProperty(new GravityProperty());
	AddProperty(new FloatProperty());	
//	AddProperty(new ModelProperty());
	
	onGround=false;
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




