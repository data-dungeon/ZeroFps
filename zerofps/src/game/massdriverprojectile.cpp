#include "massdriverprojectile.h"

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
	m_pkObjectMan->Delete(m_pkObject);
//	m_pkObject->GetObjectType()=OBJECT_TYPE_STATIC;
	
//	PhysicProperty* pp = dynamic_cast<PhysicProperty*>(m_pkObject->GetProperty("PhysicProperty"));
//	pp->SetColShape(new CSSphere(0.2));		
//	static_cast<CSSphere*>(pp->GetColSphere())->m_fRadius=0.5;
//	pp->m_bGravity=false;
//	pp->m_bFloat=false;	
}


Property* Create_MassDriverProjectile()
{
	return new MassDriverProjectile;

}


