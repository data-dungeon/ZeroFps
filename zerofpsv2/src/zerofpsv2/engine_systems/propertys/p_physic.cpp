#include "p_physic.h"
#include "p_primitives3d.h"
#include "p_hmrp2.h"
#include <typeinfo>

#include "../physicsengine/cssphere.h"
#include "../physicsengine/csbox.h"
#include "../physicsengine/csmech.h"

P_Physic::P_Physic()
{
	strcpy(m_acName,"P_Physic");
	
	m_iType=PROPERTY_TYPE_PHYSIC;
	m_iSide=PROPERTY_SIDE_SERVER;	
	
	bNetwork  = false;
	
	m_bGravity=false;
	m_bFloat=false;	
	m_bSolid=false;	
	m_bGlide=true;
	m_bStride=false;
	m_bStatic=false;
	
	m_bStrideHeight=0.7;

	m_pkColSphere=NULL;
	m_pkColObject=NULL;

	m_fColShape=0;
	m_bDummyValue=true;

	m_pkColSphere=new CSSphere(0);
	m_pkColSphere->SetPPPointer(this);

}

P_Physic::~P_Physic()
{
	delete m_pkColSphere;
	delete m_pkColObject;
}

void P_Physic::Update()
{
	if(static_cast<CSSphere*>(m_pkColSphere)->m_fRadius==0)
	{
		static_cast<CSSphere*>(m_pkColSphere)->m_fRadius=GetBoundingRadius();
	}
}

float P_Physic::GetBoundingRadius()
{
	P_Mad* mp = static_cast<P_Mad*>(m_pkObject->GetProperty("P_Mad"));
	if(mp!=NULL)
	{
		//cout<<"got radius from mad"<<endl;
		return mp->GetRadius();	
	}
	
	P_Primitives3D* mop = static_cast<P_Primitives3D*>(m_pkObject->GetProperty("P_Primitives3D"));
	if(mop!=NULL)
	{
		return mop->m_fRadius;	
	}
	
	P_HMRP2* hp = static_cast<P_HMRP2*>(m_pkObject->GetProperty("P_HMRP2"));
	if(hp!=NULL)
	{
		float k=hp->GetHeightMap()->GetSize()/2;		
		return sqrt((k*k)+(k*k));
	}
	
	return 1;
}

void P_Physic::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write((void*)&m_bGravity,4,1);
	pkPackage->Write((void*)&m_bFloat,4,1);	
	pkPackage->Write((void*)&m_bSolid,4,1);		
	pkPackage->Write((void*)&m_bGlide,4,1);			
	pkPackage->Write((void*)&m_bStride,4,1);				
	pkPackage->Write((void*)&m_bStrideHeight,4,1);
	pkPackage->Write((void*)&(static_cast<CSSphere*>(GetColSphere())->m_fRadius),4,1);	


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
	else if(typeid(*m_pkColObject) == typeid(CSMech))
	{
		type=3;		
	}
		
		
	pkPackage->Write((void*)&type,4,1);		
	
	

	switch(type)
	{
		case 1:
			//pkPackage->Write((void*)&(static_cast<CSSphere*>(m_pkColObject)->m_fRadius),4,1);						
			break;
		case 2:
			//pkPackage->Write((void*)&(static_cast<CSBox*>(m_pkColObject)->m_kScale),12,1);						
			break;
		case 3:
			//pkPackage->Write((void*)&(static_cast<CSMech*>(m_pkColObject)->m_iModelID),4,1);										
			//pkPackage->Write((void*)&(static_cast<CSMech*>(m_pkColObject)->m_fScale),4,1);													
//			pkPackage->Write((void*)&(static_cast<CSBox*>(m_pkColObject)->m_kScale),12);						
			break;
			
	}	
	
}

void P_Physic::Load(ZFIoInterface* pkPackage)
{
	pkPackage->Read((void*)&m_bGravity,4,1);
	pkPackage->Read((void*)&m_bFloat,4,1);
	pkPackage->Read((void*)&m_bSolid,4,1);	
	pkPackage->Read((void*)&m_bGlide,4,1);		
	pkPackage->Read((void*)&m_bStride,4,1);				
	pkPackage->Read((void*)&m_bStrideHeight,4,1);	
	pkPackage->Read((void*)&(static_cast<CSSphere*>(GetColSphere())->m_fRadius),4,1);		


	int type;
	pkPackage->Read((void*)&type,4,1);
	
	
	SetColShape(type);
	
}


void P_Physic::SetColShape(CollisionShape* pkCs)
{
	m_pkColObject = pkCs;
	m_pkColObject->SetPPPointer(this);	
}

void P_Physic::SetColShape(int iType)
{
	if(m_fColShape != iType)
	{
		if(m_pkColObject)
		{			
			delete m_pkColObject;
			
			m_fColShape=0;
			m_pkColObject=NULL;
		}
		
		//set new type		
		m_fColShape= iType;
		
		switch(iType)
		{
			case 1:			
				SetColShape(new CSSphere(0));	
				//pkPackage->Read((void*)&(static_cast<CSSphere*>(m_pkColObject)->m_fRadius),4,1);
				break;
			case 2:
				SetColShape(new CSBox(Vector3(1,1,1)));	
				//pkPackage->Read((void*)&(static_cast<CSBox*>(m_pkColObject)->m_kScale),12,1);
				break;
			case 3:
				SetColShape(new CSMech());	
				((CSSphere*)m_pkColSphere)->m_fRadius=0;
				//pkPackage->Read((void*)&(static_cast<CSMech*>(m_pkColObject)->m_iModelID),4,1);										
				//pkPackage->Read((void*)&(static_cast<CSMech*>(m_pkColObject)->m_fScale),4,1);																			
				break;		
		}				
	}
}

vector<PropertyValues> P_Physic::GetPropertyValues()
{
	vector<PropertyValues> kReturn(10);

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

	kReturn[4].kValueName="m_bStride";
	kReturn[4].iValueType=VALUETYPE_BOOL;
	kReturn[4].pkValue=(void*)&m_bStride;

	kReturn[5].kValueName="m_bStrideHeight";
	kReturn[5].iValueType=VALUETYPE_FLOAT;
	kReturn[5].pkValue=(void*)&m_bStrideHeight;


	kReturn[6].kValueName="m_fRadius";
	kReturn[6].iValueType=VALUETYPE_FLOAT;
	kReturn[6].pkValue = (void*)&static_cast<CSSphere*>(GetColSphere())->m_fRadius;	

	kReturn[7].kValueName="m_fColShape";
	kReturn[7].iValueType=VALUETYPE_FLOAT;
	kReturn[7].pkValue=(void*)&m_fColShape;
	
	switch((int)m_fColShape)
	{
		case 0:
			kReturn[8].kValueName="NOCOLSHAPE";
			kReturn[8].iValueType=VALUETYPE_BOOL;
			kReturn[8].pkValue=(void*)&m_bDummyValue;
			break;
		
		case 1:{
			kReturn[8].kValueName="co-sphere-radius";
			kReturn[8].iValueType=VALUETYPE_FLOAT;
			kReturn[8].pkValue = (void*)&static_cast<CSSphere*>(m_pkColObject)->m_fRadius;
	
			break;
		}
	
		case 2:{
			kReturn[8].kValueName="co-box-scale";
			kReturn[8].iValueType=VALUETYPE_VECTOR3;
			kReturn[8].pkValue = (void*)&static_cast<CSBox*>(m_pkColObject)->m_kScale;								
			break;
		}
		
		case 3:
			kReturn[8].kValueName="co-mech-id";
			kReturn[8].iValueType=VALUETYPE_INT;
			kReturn[8].pkValue=(void*)&static_cast<CSMech*>(m_pkColObject)->m_iModelID;								
			
			kReturn[9].kValueName="co-mech-scale";
			kReturn[9].iValueType=VALUETYPE_FLOAT;
			kReturn[9].pkValue=(void*)&static_cast<CSMech*>(m_pkColObject)->m_fScale;								
			
			break;
	
	}
		


	return kReturn;
};

bool P_Physic::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_fColShape") == 0) 
	{
			if(kValue=="0"){
				if(m_pkColObject!=NULL){
					delete m_pkColObject;
					m_pkColObject=NULL;
				}
				
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
			
			if(kValue=="3")
			{
				if(m_pkColObject!=NULL)
					delete m_pkColObject;								
				SetColShape(new CSMech());	
				m_fColShape=3;				
			}
			
			return true;
	}	
	
	return false;
}


Property* Create_PhysicProperty()
{
	return new P_Physic;
}






