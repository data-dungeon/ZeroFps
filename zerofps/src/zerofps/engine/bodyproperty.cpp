#include "bodyproperty.h"

#include "object.h"

BodyProperty::BodyProperty()
{
	strcpy(m_acName,"BodyProperty");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;	
	
	m_pkPhysics_Engine=static_cast<Physics_Engine*>(g_ZFObjSys.GetObjectPtr("Physics_Engine"));		
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	
	m_bHaveSetBody = false;
	
}

BodyProperty::~BodyProperty()
{
	Disable();

}


void BodyProperty::Init()
{
	if(!Enable())
		cout<<"Body did not get enabled cos it collides with another body"<<endl;

}

void BodyProperty::Update()
{

	if(!m_bHaveSetBody)
	{
		m_bHaveSetBody=true;
		SetBodyToObject();
	}
	

	if(m_pkZeroFps->m_bRunWorldSim)
		SetObjectToBody();
	else
		SetBodyToObject();
}

bool BodyProperty::Enable()
{
	return m_pkPhysics_Engine->AddBody(&m_kBody);
}

bool BodyProperty::Disable()
{
	return m_pkPhysics_Engine->RemoveBody(&m_kBody);
}

void BodyProperty::SetBodyToObject()
{
	m_kBody.SetPos(m_pkObject->GetPos());
}

void BodyProperty::SetObjectToBody()
{
	m_pkObject->SetPos(m_kBody.GetPos());
}

void BodyProperty::SetBodyPos(Vector3 kPos)
{
	m_kBody.SetPos(kPos);
}

vector<PropertyValues> BodyProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(6);
		
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
	
	

	return kReturn;
}


Property* Create_BodyProperty()
{
	return new BodyProperty;
}






