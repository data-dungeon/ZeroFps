#include "ballobject.h"
 
BallObject::BallObject() {
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
  
	AddProperty("MadProperty");
	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
	madp->SetBasePtr(m_pkFps->GetMADPtr("../data/mad/tree.mad"));
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

bool BallObject::Save(void *pkData,int iSize) {
	struct data{
		Vector3 kPos;
		Vector3 kRot;
	};

//	pkData=new data;
	
//	pkData->kPos=m_kPos

	return true;
}

