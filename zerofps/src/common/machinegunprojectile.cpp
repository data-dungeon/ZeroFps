#include "machinegunprojectile.h"
#include "statusproperty.h"

MachineGunProjectile::MachineGunProjectile()
{
	strcpy(m_acName,"MachineGunProjectile");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	
	
	m_fCreateTime=m_pkFps->GetTicks();
	
	m_iMode=0;
}

void MachineGunProjectile::Update()
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

void MachineGunProjectile::Touch(Collision* pkCol)
{
	Object* pkObject;
	
	if(pkCol->m_pkPP1->GetObject() == (Object*)this)
		pkObject=pkCol->m_pkPP2->GetObject();
	else
		pkObject=pkCol->m_pkPP1->GetObject();	
	
	
	if(m_iMode==0)
	{	
	
		StatusProperty* sp=static_cast<StatusProperty*>(pkObject->GetProperty("StatusProperty"));
		if(sp!=NULL)
		{
			sp->Damage(10);
		}
		
		m_iMode=1;	
		BillBoardRenderProperty* cr=static_cast<BillBoardRenderProperty*>(m_pkObject->AddProperty("BillBoardRenderProperty"));
		cr->SetTexture("../data/textures/massdriver_hit.tga");
		cr->SetScale(.3);
	

		m_fCreateTime=m_pkFps->GetTicks();
	}
}


COMMON_API Property* Create_MachineGunProjectile()
{
	return new MachineGunProjectile;

}


