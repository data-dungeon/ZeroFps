#include "p_body.h"
#include "../../basic/zfsystem.h"
#include "../../engine/object.h"
#include "p_mad.h"
 
P_Body::P_Body()
{
	strcpy(m_acName,"P_Body");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;	
	
	m_pkPhysics_Engine=static_cast<Physics_Engine*>(g_ZFObjSys.GetObjectPtr("Physics_Engine"));		
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	
	m_bHaveSetBody = false;
	m_bHaveSetMesh = false;
	m_iMeshID		= 0;
}

P_Body::~P_Body()
{
	Disable();

}


void P_Body::Init()
{
	if(!Enable())
		cout<<"Body did not get enabled cos it collides with another body"<<endl;

}

void P_Body::Update()
{

	if(!m_bHaveSetBody)
	{
		m_bHaveSetBody=true;
		SetBodyToObject();
	}
	
	if(!m_bHaveSetMesh)
	{
		m_bHaveSetMesh = SetUpMesh();
	}

	if(m_pkZeroFps->m_bRunWorldSim)
		SetObjectToBody();
	else
		SetBodyToObject();
}

bool P_Body::Enable()
{
	return m_pkPhysics_Engine->AddBody(&m_kBody);
}

bool P_Body::Disable()
{
	return m_pkPhysics_Engine->RemoveBody(&m_kBody);
}

void P_Body::SetBodyToObject()
{
	m_kBody.SetPos(m_pkObject->GetWorldPosV());
	m_kBody.SetRot(m_pkObject->GetWorldRotV());	
}

void P_Body::SetObjectToBody()
{
	m_pkObject->SetWorldPosV(m_kBody.GetPos());
	m_pkObject->SetWorldRotV(m_kBody.GetRot());	
}

void P_Body::SetBodyPos(Vector3 kPos)
{
	m_kBody.SetPos(kPos);
}

vector<PropertyValues> P_Body::GetPropertyValues()
{
	vector<PropertyValues> kReturn(7);
		
	kReturn[0].kValueName = "m_bPolygonCheck";
	kReturn[0].iValueType = VALUETYPE_BOOL;
	kReturn[0].pkValue    = (void*)&m_kBody.m_bPolygonCheck;

	kReturn[1].kValueName = "m_bGravity";
	kReturn[1].iValueType = VALUETYPE_BOOL;
	kReturn[1].pkValue    = (void*)&m_kBody.m_bGravity;

	kReturn[2].kValueName = "m_bSolid";
	kReturn[2].iValueType = VALUETYPE_BOOL;
	kReturn[2].pkValue    = (void*)&m_kBody.m_bSolid;
	
	kReturn[3].kValueName = "m_fBounce";
	kReturn[3].iValueType = VALUETYPE_FLOAT;
	kReturn[3].pkValue    = (void*)&m_kBody.m_fBounce;
	
	kReturn[4].kValueName = "m_fMass";
	kReturn[4].iValueType = VALUETYPE_FLOAT;
	kReturn[4].pkValue    = (void*)&m_kBody.m_fMass;
	
	kReturn[5].kValueName = "m_fRadius";
	kReturn[5].iValueType = VALUETYPE_FLOAT;
	kReturn[5].pkValue    = (void*)&m_kBody.m_fRadius;
	
	kReturn[6].kValueName = "m_iMeshID";
	kReturn[6].iValueType = VALUETYPE_INT;
	kReturn[6].pkValue    = (void*)&m_iMeshID;
	

	return kReturn;
}

bool P_Body::SetUpMesh()
{
	P_Mad* pkMP = static_cast<P_Mad*>(m_pkObject->GetProperty("P_Mad"));	
	if(pkMP != NULL)
	{
		Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMP->kMadHandle.GetResourcePtr());
		if(pkCore != NULL)		
		{	
			m_kBody.SetScalep(&pkMP->m_fScale);
			return m_kBody.SetMad(pkCore,m_iMeshID);		
		}
	}
	
	return false;
}

Property* Create_BodyProperty()
{
	return new P_Body;
}





