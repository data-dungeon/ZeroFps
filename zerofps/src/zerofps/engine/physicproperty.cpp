#include "physicproperty.h"
#include "cssphere.h"
//#include "collisionshape.h"

PhysicProperty::PhysicProperty()
{
	strcpy(m_acName,"PhysicProperty");
	
	m_iType=PROPERTY_TYPE_PHYSIC;
	m_iSide=PROPERTY_SIDE_SERVER;	
	

	m_bGravity=true;
	m_bMove=true;



	m_pkColSphere=new CSSphere(1);
	m_pkColSphere->SetPPPointer(this);

	cout<<"set pointer"<<endl;
}

CollisionData* PhysicProperty::Test(PhysicProperty* kOther)
{


	return NULL;
}


Property* Create_PhysicProperty()
{
	return new PhysicProperty;
}






