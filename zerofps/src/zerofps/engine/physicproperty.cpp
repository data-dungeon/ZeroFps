#include "physicproperty.h"
#include "cssphere.h"
#include "csbox.h"
#include <typeinfo>

PhysicProperty::PhysicProperty()
{
	strcpy(m_acName,"PhysicProperty");
	
	m_iType=PROPERTY_TYPE_PHYSIC;
	m_iSide=PROPERTY_SIDE_SERVER;	
	
	m_bGravity=true;
	m_bFloat=true;	
	m_bSolid=true;	
	m_bGlide=true;

	m_pkColSphere=NULL;
	m_pkColObject=NULL;

	m_fColShape=0;
	m_bDummyValue=true;

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
		return mp->GetRadius();	
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
	pkPackage->Write((void*)&m_bGlide,4);			
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
	else if(typeid(*m_pkColObject) == typeid(CSBox))
	{
		type=2;		
	}
		
		
	pkPackage->Write((void*)&type,4);		
	

	switch(type)
	{
		case 1:
			pkPackage->Write((void*)&(static_cast<CSSphere*>(m_pkColObject)->m_fRadius),4);						
			break;
		case 2:
			pkPackage->Write((void*)&(static_cast<CSBox*>(m_pkColObject)->m_kScale),12);						
			break;
			
	}	
	
}

void PhysicProperty::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read((void*)&m_bGravity,4);
	pkPackage->Read((void*)&m_bFloat,4);
	pkPackage->Read((void*)&m_bSolid,4);	
	pkPackage->Read((void*)&m_bGlide,4);		
	pkPackage->Read((void*)&(static_cast<CSSphere*>(GetColSphere())->m_fRadius),4);		


	int type;
	pkPackage->Read((void*)&type,4);
	
	m_fColShape=type;
	
	switch(type)
	{
		case 1:
			SetColShape(new CSSphere(0));	
			pkPackage->Read((void*)&(static_cast<CSSphere*>(m_pkColObject)->m_fRadius),4);
			break;
		case 2:
			SetColShape(new CSBox(Vector3(1,1,1)));	
			pkPackage->Read((void*)&(static_cast<CSBox*>(m_pkColObject)->m_kScale),12);
			break;
		
	}		
}


void PhysicProperty::SetColShape(CollisionShape* pkCs)
{
	m_pkColObject = pkCs;
	m_pkColObject->SetPPPointer(this);	
}


vector<PropertyValues> PhysicProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(7);

	kReturn[0].kValueName="m_bGravity";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bGravity;

	kReturn[1].kValueName="m_bFloat";
	kReturn[1].iValueType=VALUETYPE_BOOL;
	kReturn[1].pkValue=(void*)&m_bFloat;

	kReturn[2].kValueName="m_bSolid";
	kReturn[2].iValueType=VALUETYPE_BOOL;
	kReturn[2].pkValue=(void*)&m_bSolid;

	kReturn[3].kValueName="m_bGlide";
	kReturn[3].iValueType=VALUETYPE_BOOL;
	kReturn[3].pkValue=(void*)&m_bGlide;


	kReturn[4].kValueName="m_fRadius";
	kReturn[4].iValueType=VALUETYPE_FLOAT;
	kReturn[4].pkValue = (void*)&static_cast<CSSphere*>(GetColSphere())->m_fRadius;	

/*
	CSSphere* pkColSphere = static_cast<CSSphere*>(GetColSphere());
	float fHata = static_cast<CSSphere*>(GetColSphere())->m_fRadius;

	if(pkColSphere != NULL)
		kReturn[4].pkValue = (void*)&static_cast<CSSphere*>(GetColSphere())->m_fRadius;
	else
		assert(0);
*/

	kReturn[5].kValueName="m_fColShape";
	kReturn[5].iValueType=VALUETYPE_FLOAT;
	kReturn[5].pkValue=(void*)&m_fColShape;
	
	switch((int)m_fColShape)
	{
		case 0:
			kReturn[6].kValueName="NOCOLSHAPE";
			kReturn[6].iValueType=VALUETYPE_BOOL;
			kReturn[6].pkValue=(void*)&m_bDummyValue;
			break;
		
		case 1:{
			kReturn[6].kValueName="co-sphere-radius";
			kReturn[6].iValueType=VALUETYPE_FLOAT;
			kReturn[6].pkValue = (void*)&static_cast<CSSphere*>(m_pkColObject)->m_fRadius;
	
			break;
		}
	
		case 2:{
			kReturn[6].kValueName="co-box-scale";
			kReturn[6].iValueType=VALUETYPE_VECTOR3;
			kReturn[6].pkValue = (void*)&static_cast<CSBox*>(m_pkColObject)->m_kScale;								
			break;
		}
	
	}
		


	return kReturn;
};

bool PhysicProperty::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_fColShape") == 0) 
	{
			if(kValue=="0"){
				if(m_pkColObject!=NULL)
					delete m_pkColObject;
				
				m_fColShape=0;
			}
					
			if(kValue=="1")
			{
				if(m_pkColObject!=NULL)
					delete m_pkColObject;				
				SetColShape(new CSSphere(1));	
				m_fColShape=1;
			}
			
			if(kValue=="2")
			{
				if(m_pkColObject!=NULL)
					delete m_pkColObject;								
				SetColShape(new CSBox(Vector3(1,1,1)));	
				m_fColShape=2;				
			}
			
			return true;
	}	
	
	return false;
}


Property* Create_PhysicProperty()
{
	return new PhysicProperty;
}






