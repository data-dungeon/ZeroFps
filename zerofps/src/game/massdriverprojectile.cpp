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
}

void MassDriverProjectile::Update()
{
	if(m_pkFps->GetTicks() - m_fCreateTime >3)
		m_pkObjectMan->Delete(m_pkObject);		
}

void MassDriverProjectile::Touch(Object* pkObject)
{
//	m_pkObject->GetVel().Set(0,0,0);
	
//	m_pkObject->DeleteProperty("PhysicProperty");
	
	
/*	StatusProperty* sp=static_cast<StatusProperty*>(pkObject->GetProperty("StatusProperty"));
	if(sp!=NULL)
	{
		sp->Damage(50);
	}
	
	m_pkObjectMan->Delete(m_pkObject);	*/

}


Property* Create_MassDriverProjectile()
{
	return new MassDriverProjectile;

}


