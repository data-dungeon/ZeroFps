#include "physicproperty.h"
#include "cssphere.h"
//#include "collisionshape.h"

PhysicProperty::PhysicProperty()
{
	strcpy(m_acName,"PhysicProperty");
	
	m_iType=PROPERTY_TYPE_PHYSIC;
	m_iSide=PROPERTY_SIDE_SERVER;	
	
	m_bGravity=true;
	m_bFloat=true;	

	m_pkColSphere=NULL;
	m_pkColObject=NULL;

	m_pkColSphere=new CSSphere(1);
	m_pkColSphere->SetPPPointer(this);

}

PhysicProperty::~PhysicProperty()
{
	delete m_pkColSphere;
	delete m_pkColObject;
}

void PhysicProperty::Save(ZFMemPackage* pkPackage)
{
	pkPackage->Write((void*)&m_bGravity,4);
	pkPackage->Write((void*)&m_bFloat,4);
}

void PhysicProperty::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read((void*)&m_bGravity,4);
	pkPackage->Read((void*)&m_bFloat,4);
}


Property* Create_PhysicProperty()
{
	return new PhysicProperty;
}






