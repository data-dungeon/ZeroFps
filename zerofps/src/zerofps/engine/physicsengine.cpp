#include "physicsengine.h"

PhysicsEngine::PhysicsEngine() 
: ZFObject("PhysicsEngine")
{
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));		

	
	m_bUpdate=true;
}


void PhysicsEngine::Update()
{
	if(!m_bUpdate)
		return;

	
	m_fFrameTime=m_pkZeroFps->GetFrameTime();



	list<Property*> kPropertys;		
	m_pkObjectMan->GetWorldObject()->GetAllPropertys(&kPropertys,PROPERTY_TYPE_PHYSIC,PROPERTY_SIDE_SERVER);
	
	
	for(list<Property*>::iterator it=kPropertys.begin();it!=kPropertys.end();it++) {	
		PhysicProperty* PP = static_cast<PhysicProperty*>(*it);
		
		if(PP->m_bMove)
			MoveObject(PP->GetObject());
	}

	
	kPropertys.clear();
}

Vector3 PhysicsEngine::GetNewPos(Object* pkObject,float fTime)
{
	Vector3 Vel = pkObject->GetVel() + pkObject->GetAcc() * fTime;
	return pkObject->GetPos() + Vel * fTime;;
}

void PhysicsEngine::MoveObject(Object* pkObject)
{
	pkObject->GetVel() += pkObject->GetAcc() * m_fFrameTime;
	pkObject->GetPos() += pkObject->GetVel() * m_fFrameTime;;
}























