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

	/*	
		madp->SetBase(m_pkFps->GetMADPtr("../data/mad/golem.mad"));
		madp->SetScale(0.01);
		madp->bFlipFace = true;
		madp->PlayAnimation(0,0);
	*/

	int iRndMad = rand() % 4;
	int iRndColor = rand() % 4;
	
	switch(iRndMad) {
		case 0:		
			madp->SetBase(m_pkFps->GetMADPtr("../data/mad/cone.mad"));
			madp->SetReplaceTexture("c_red");
			break;
		case 1:		
			madp->SetBase(m_pkFps->GetMADPtr("../data/mad/cube.mad"));
			madp->SetReplaceTexture("c_blue");
			break;
		case 2:		
			madp->SetBase(m_pkFps->GetMADPtr("../data/mad/sphere.mad"));
			madp->SetReplaceTexture("c_yellow");
			break;
		case 3:		
			madp->SetBase(m_pkFps->GetMADPtr("../data/mad/torus.mad"));
			madp->SetReplaceTexture("c_green");
			break;
		}


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


