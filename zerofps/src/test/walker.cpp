#include "walker.h"
#include "walkeraiproperty.h"

WalkerObject::WalkerObject() {
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	AddProperty(new CollisionProperty(&m_kPos,new float(0.5)));
	AddProperty(new GravityProperty());
	AddProperty(new FloatProperty());	
	AddProperty(new WalkerAIProperty());

	AddProperty("MadProperty");
	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
	madp->SetBase(m_pkFps->GetMADPtr("../data/mad/golem.mad"));
	madp->SetScale(0.01);
	madp->bFlipFace = true;
	madp->PlayAnimation(0,0);
// 	GetRot().y = (rand()%360);

	onGround = false;
}

WalkerObject::~WalkerObject()
{


}


void WalkerObject::HandleCollision(Object* pkOther,Vector3 kPos,bool bContinue){
	
	//if a ball colides with the player destroy it
	if(typeid(*pkOther)==typeid(HeightMapObject)){
		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkOther);
	
		GetPos()=kPos+Vector3(0,.25,0);
		m_kVel.y=0;
		onGround=true;
		//GetStatic()=true;
		
	} else if(bContinue){
		pkOther->HandleCollision(this,kPos,false);
	}
}


