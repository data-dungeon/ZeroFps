#include "playerobject.h"
#include "../zerofps/engine/cssphere.h"
#include "../zerofps/engine/csbox.h"

PlayerObject::PlayerObject(HeightMap *pkMap,Input *pkInput)
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	AddProperty(new PlayerControlProperty(/* pkInput ,pkMap */));
	
	GetName()="Player";
	
	AddProperty("ContainerProperty");
	AddProperty("StatusProperty");	
	AddProperty("MassDriverProperty");	
	AddProperty("MachineGunProperty");		
	AddProperty("AutoParentProperty");	
	AddProperty("CameraProperty");	

	AddProperty("PhysicProperty");
	PhysicProperty* pp=static_cast<PhysicProperty*>(GetProperty("PhysicProperty"));
	pp->m_bStride=true;
	pp->m_bGlide=false;	

	//	pp->SetColShape(new CSBox(Vector3(.8,1.8,.8)));	
	//	AddProperty("MadProperty");
	//	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
	//	madp->SetBasePtr(m_pkFps->GetMADPtr("../data/mad/player.mad"));

	/*	
		AddProperty("LightProperty");
		LightProperty* pkJumpLight = dynamic_cast<LightProperty*>(GetProperty("LightProperty"));
		pkJumpLight->SetDiffuse(Vector4(.1,.1,.1,0));
		pkJumpLight->SetQuadratic_Atten(0.01);
	*/	
}

/*

void PlayerObject::HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue)
{
	if(typeid(*pkObject)==typeid(HeightMapObject)){
//		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkObject);
	
		GetPos()=kPos+Vector3(0,.8,0) ;//GetPos()+(GetPos()-kPos);
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
*/



