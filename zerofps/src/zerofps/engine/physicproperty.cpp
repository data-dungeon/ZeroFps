#include "physicproperty.h"
#include "cssphere.h"
#include <typeinfo>

PhysicProperty::PhysicProperty()
{
	strcpy(m_acName,"PhysicProperty");
	
	m_iType=PROPERTY_TYPE_PHYSIC;
	m_iSide=PROPERTY_SIDE_SERVER;	
	
	m_bGravity=true;
	m_bFloat=true;	
	m_bSolid=true;	


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
		return mp->pkCore->GetRadius();	
	}
	
	ModelProperty* mop = static_cast<ModelProperty*>(m_pkObject->GetProperty("ModelProperty"));
	if(mop!=NULL)
	{
		return mop->m_fRadius;	
	}
	
	HMRP2* hp = static_cast<HMRP2*>(m_pkObject->GetProperty("HMRP2"));
	if(hp!=NULL)
	{
		float k=hp->GetHeightMap()->GetSize()/2;		
		return sqrt((k*k)+(k*k));
	}
	
	return 1;
}

void PhysicProperty::Save(ZFMemPackage* pkPackage)
{
	pkPackage->Write((void*)&m_bGravity,4);
	pkPackage->Write((void*)&m_bFloat,4);	
	pkPackage->Write((void*)&m_bSolid,4);		
	pkPackage->Write((void*)&(static_cast<CSSphere*>(GetColSphere())->m_fRadius),4);	


	int type;
	if(m_pkColObject==NULL)
	{
		type=0;
	}
	else if(typeid(*m_pkColObject) == typeid(CSSphere))
	{
		type=1;		
	}
		
	pkPackage->Write((void*)&type,4);		
	

	switch(type)
	{
		case 1:
			pkPackage->Write((void*)&(static_cast<CSSphere*>(m_pkColObject)->m_fRadius),4);						
			break;
	}	
	
}

void PhysicProperty::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read((void*)&m_bGravity,4);
	pkPackage->Read((void*)&m_bFloat,4);
	pkPackage->Read((void*)&m_bSolid,4);	
	pkPackage->Read((void*)&(static_cast<CSSphere*>(GetColSphere())->m_fRadius),4);		


	int type;
	pkPackage->Read((void*)&type,4);
	
	switch(type)
	{
		case 1:
			SetColShape(new CSSphere(0));	
			pkPackage->Read((void*)&(static_cast<CSSphere*>(m_pkColObject)->m_fRadius),4);
			break;
	}		
}


void PhysicProperty::SetColShape(CollisionShape* pkCs)
{
	m_pkColObject = pkCs;
	m_pkColObject->SetPPPointer(this);	
}


vector<Property::PropertyValues> PhysicProperty::GetPropertyValues()
{
	vector<Property::PropertyValues> kReturn(3);

	kReturn[0].kValueName="m_bGravity";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bGravity;

	kReturn[1].kValueName="m_bFloat";
	kReturn[1].iValueType=VALUETYPE_BOOL;
	kReturn[1].pkValue=(void*)&m_bFloat;

	kReturn[2].kValueName="m_bSolid";
	kReturn[2].iValueType=VALUETYPE_BOOL;
	kReturn[2].pkValue=(void*)&m_bSolid;

	return kReturn;
};


Property* Create_PhysicProperty()
{
	return new PhysicProperty;
}






