#include "massdriverprojectile.h"
#include "statusproperty.h"

MassDriverProjectile::MassDriverProjectile()
{
	strcpy(m_acName,"MassDriverProjectile");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	
	
	m_fCreateTime=m_pkFps->GetTicks();
	
	m_iMode=0;
}

void MassDriverProjectile::Update()
{
	
	if(m_iMode==0)
	{		
		if(m_pkFps->GetTicks() - m_fCreateTime >3)
			m_pkObjectMan->Delete(m_pkObject);		
	}
	else
	{
		m_pkObject->DeleteProperty("PhysicProperty");
		if(m_pkFps->GetTicks() - m_fCreateTime >.5)
			m_pkObjectMan->Delete(m_pkObject);			
	}
}

void MassDriverProjectile::Touch(Object* pkObject)
{
	if(m_iMode==0)
	{	
	
		StatusProperty* sp=static_cast<StatusProperty*>(pkObject->GetProperty("StatusProperty"));
		if(sp!=NULL)
		{
			sp->Damage(200);
		}
		
		m_iMode=1;	
		CrossRenderProperty* cr=static_cast<CrossRenderProperty*>(m_pkObject->AddProperty("CrossRenderProperty"));
		cr->SetTexture("../data/textures/massdriver_hit.tga");
		cr->SetScale(Vector3(.3,.3,.3));

		m_fCreateTime=m_pkFps->GetTicks();
	}
}


Property* Create_MassDriverProjectile()
{
	return new MassDriverProjectile;

}


