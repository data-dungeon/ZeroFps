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

	m_pkColSphere=new CSSphere(0);
	m_pkColSphere->SetPPPointer(this);

}

PhysicProperty::~PhysicProperty()
{
	delete m_pkColSphere;
	delete m_pkColObject;
}

void PhysicProperty::Update()
{
	if(static_cast<CSSphere*>(m_pkColSphere)->m_fRadius==0)
	{
		static_cast<CSSphere*>(m_pkColSphere)->m_fRadius=GetBoundingRadius();
	}
}

float PhysicProperty::GetBoundingRadius()
{
	MadProperty* mp = static_cast<MadProperty*>(m_pkObject->GetProperty("MadProperty"));
	if(mp!=NULL)
	{
//		cout<<"Got radius from madproperty"<<endl;
		return mp->pkCore->GetRadius();	
	}
	
	ModelProperty* mop = static_cast<ModelProperty*>(m_pkObject->GetProperty("ModelProperty"));
	if(mop!=NULL)
	{
//		cout<<"Got radius from Modelproperty"<<endl;	
		return mop->m_fRadius;	
	}
	
	return 0.1;
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






