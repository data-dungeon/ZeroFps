#include "collisionshape.h"


void CollisionShape::SetPPPointer(PhysicProperty* pkPP)
{
	m_pkPP=pkPP;
	m_pkPhysEngine=static_cast<PhysicsEngine*>(g_ZFObjSys.GetObjectPtr("PhysicsEngine"));
}

 
 

