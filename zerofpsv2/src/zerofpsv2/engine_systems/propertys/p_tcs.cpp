#include "p_tcs.h"
#include "p_mad.h"


P_Tcs::P_Tcs()
{
	strcpy(m_acName,"P_Tcs");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;	
		
	bNetwork  = false;

	m_pkTcs	= static_cast<Tcs*>(g_ZFObjSys.GetObjectPtr("Tcs"));

	m_bHavePolygonData=	false;
	m_bPolygonTest=		false;
	m_fRadius=				0.5;	
	m_bGravity=				false;
	
	m_fScale=				1;
	m_pkFaces =				NULL;
	m_pkVertex =			NULL;	
	m_pkNormal =			NULL;	
	m_iModelID=				0;

	m_iGroup=				0;
	
	ResetGroupFlags();
	
}

P_Tcs::~P_Tcs()
{
	Disable();

}


void P_Tcs::Init()
{
	//cout<<"Tiny Collisions system property Added"<<endl;	
	Enable();
}

void P_Tcs::Enable()
{
	m_pkTcs->AddBody(this);
}
void P_Tcs::Disable()
{
	m_pkTcs->RemoveBody(this);
}

void P_Tcs::Update()
{	
	if(m_bPolygonTest)
	{
		if(!m_bHavePolygonData)
		{
			m_bHavePolygonData = SetupMeshData();
			m_fRadius = GetBoundingRadius();
		}
	}
	else			//if no polygon test, check if we shuld at least get radius from mesh
	{
		if(m_fRadius == -1)
		{	
			m_fRadius = GetBoundingRadius();
		}
	}
}


void P_Tcs::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write((void*)&m_bPolygonTest,sizeof(m_bPolygonTest),1);
	pkPackage->Write((void*)&m_fRadius,sizeof(m_fRadius),1);	
	pkPackage->Write((void*)&m_iGroup,sizeof(m_iGroup),1);	
	pkPackage->Write((void*)&m_akTestGroups,sizeof(m_akTestGroups),1);		
	pkPackage->Write((void*)&m_iModelID,sizeof(m_iModelID),1);		
	pkPackage->Write((void*)&m_bGravity,sizeof(m_bGravity),1);			
}

void P_Tcs::Load(ZFIoInterface* pkPackage)
{
	pkPackage->Read((void*)&m_bPolygonTest,sizeof(m_bPolygonTest),1);
	pkPackage->Read((void*)&m_fRadius,sizeof(m_fRadius),1);	
	pkPackage->Read((void*)&m_iGroup,sizeof(m_iGroup),1);	
	pkPackage->Read((void*)&m_akTestGroups,sizeof(m_akTestGroups),1);		
	pkPackage->Read((void*)&m_iModelID,sizeof(m_iModelID),1);		
	pkPackage->Read((void*)&m_bGravity,sizeof(m_bGravity),1);			
}

vector<PropertyValues> P_Tcs::GetPropertyValues()
{
	vector<PropertyValues> kReturn(6);

	int dummy;

	kReturn[0].kValueName="polygontest";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bPolygonTest;

	kReturn[1].kValueName="groupflag";
	kReturn[1].iValueType=VALUETYPE_INT;
	kReturn[1].pkValue=(void*)&dummy;

	kReturn[2].kValueName="radius";
	kReturn[2].iValueType=VALUETYPE_FLOAT;
	kReturn[2].pkValue=(void*)&m_fRadius;

	kReturn[3].kValueName="modelid";
	kReturn[3].iValueType=VALUETYPE_INT;
	kReturn[3].pkValue=(void*)&m_iModelID;
	
	kReturn[4].kValueName="gravity";
	kReturn[4].iValueType=VALUETYPE_BOOL;
	kReturn[4].pkValue=(void*)&m_bGravity;
	
	kReturn[5].kValueName="group";
	kReturn[5].iValueType=VALUETYPE_INT;
	kReturn[5].pkValue=(void*)&m_iGroup;
	
	return kReturn;
}

bool P_Tcs::HandleSetValue( string kValueName, string kValue )
{
	if( strcmp(kValueName.c_str(), "groupflag") == 0 ) 
	{
		int flag = atoi(kValue.c_str());
		//cout << "unsetting flag:" << flag << endl;

		SetTestGroupFlag(flag,false);
		
		return true;
	}

	return false;
}

float P_Tcs::GetBoundingRadius()
{
	P_Mad* mp = static_cast<P_Mad*>(m_pkObject->GetProperty("P_Mad"));
	if(mp)
	{
		//cout<<"got radius from mad"<<endl;
		return mp->GetRadius();	
	}
	
	return -1;
}

bool P_Tcs::SetupMeshData()
{
	//cout<<"setting up mesh:"<<endl;
	//look for mad property
	P_Mad* pkMP = static_cast<P_Mad*>(m_pkObject->GetProperty("P_Mad"));	
	if(pkMP != NULL)
	{
		//cout<<"found mad property"<<endl;
		//look for core pointer in mad property
		//m_pkCore = pkMP->pkCore;	
		Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMP->kMadHandle.GetResourcePtr()); 

		if(pkCore != NULL)
		{
			//cout<<"found core"<<endl;
			//look for mech pointer in core
			Mad_CoreMesh* pkCoreMech = pkCore->GetMeshByID(m_iModelID);					
			if(pkCoreMech != NULL)		
			{
				//cout<<"found mech"<<endl;
				
				m_pkFaces = pkCoreMech->GetFacesPointer();
				m_pkVertex = (*pkCoreMech->GetVertexFramePointer())[0].GetVertexPointer();
				m_pkNormal = (*pkCoreMech->GetVertexFramePointer())[0].GetNormalPointer();
				
				m_fScale = pkMP->m_fScale;
				
				//found the mech return true
				return true;
			}
		}	
	}
	
	//cout<<"error mech not found"<<endl;
	return false;
}

Property* Create_P_Tcs()
{
	return new P_Tcs;
}



