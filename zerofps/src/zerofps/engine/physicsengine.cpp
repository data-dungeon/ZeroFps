#include "physicsengine.h"

PhysicsEngine::PhysicsEngine() 
: ZFObject("PhysicsEngine")
{
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));		


}


void PhysicsEngine::Update()
{
	list<Property*> kPropertys;	
	
	m_pkObjectMan->GetWorldObject()->GetAllPropertys(&kPropertys,PROPERTY_TYPE_PHYSIC,PROPERTY_SIDE_SERVER);
	
	
	//HANDLE ALL COLLISSIONS HERE =)
	
	
	
	//TO HERE =)
	
	kPropertys.clear();
}

















