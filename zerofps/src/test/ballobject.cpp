#include "ballobject.h"

BallObject::BallObject(HeightMap *pkMap,ZeroFps *pkFps) {
	SetPropertyFactory(&pkFps->m_kPropertyFactory);

	AddProperty(new CollisionProperty(&m_kPos,new float(.5)));
	AddProperty(new GravityProperty());

	AddProperty("MadProperty");
	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
	madp->SetBase(pkFps->GetMADPtr("sphere.mad"));
//	madp->SetScale(0.02);

//	AddProperty("ModelProperty");

	m_pkFps=pkFps;
	onGround=false;
}

void BallObject::HandleCollision(Object* pkOther,Vector3 kPos,bool bContinue){
	
	//if a ball colides with the player destroy it
	if(typeid(*pkOther)==typeid(HeightMapObject)){
		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkOther);
	
	GetPos()=kPos+Vector3(0,.5,0);
	m_kVel.y=0;
	onGround=true;
		
	} else if(typeid(*pkOther)==typeid(PlayerBallObject)){
		PlayerBallObject *kO=dynamic_cast<PlayerBallObject*>(pkOther);
		Remove();	
		
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

