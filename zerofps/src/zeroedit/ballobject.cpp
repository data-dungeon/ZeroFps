#include "ballobject.h"
#include "../zerofps/engine/cssphere.h"
#include "../game/teleportproperty.h"

BallObject::BallObject() {
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
 
//	AddProperty(new CollisionProperty(&m_kPos,new float(1.0)));
//	AddProperty(new GravityProperty());
//	AddProperty(new FloatProperty());	

/*
	AddProperty("MadProperty");
	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
//	madp->SetBase(m_pkFps->GetMADPtr("../data/mad/tree.mad"));
	madp->SetBase("../data/mad/tree.mad");
	madp->SetScale(0.5);
*/
	
	m_iObjectType=OBJECT_TYPE_STATIC;

	AddProperty("ModelProperty");
	AddProperty("LightProperty");
	AddProperty("PhysicProperty");
	AddProperty("TeleportProperty");
	TeleportProperty* tp = dynamic_cast<TeleportProperty*>(GetProperty("TeleportProperty"));
	tp->SetToPos(Vector3(-7,8,394));	
	
	PhysicProperty* pp = dynamic_cast<PhysicProperty*>(GetProperty("PhysicProperty"));
//	pp->SetColShape(new CSSphere(0.2));		
//	static_cast<CSSphere*>(pp->GetColSphere())->m_fRadius=0.5;
	pp->m_bGravity=false;
	pp->m_bFloat=false;


//	m_kAcc.Set(0,0,0);

	onGround = false;
}

void BallObject::HandleCollision(Object* pkOther,Vector3 kPos,bool bContinue){
	
	//if a ball colides with the player destroy it
	if(typeid(*pkOther)==typeid(HeightMapObject)){
		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkOther);
	
		GetPos()=kPos+Vector3(0,.5,0);
		m_kVel.y=0;
		onGround=true;
//		GetStatic()=true;
		
	} else if(bContinue){
		pkOther->HandleCollision(this,kPos,false);
	}

}

