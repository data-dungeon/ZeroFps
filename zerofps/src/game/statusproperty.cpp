#include "statusproperty.h"

StatusProperty::StatusProperty()
{
	strcpy(m_acName,"StatusProperty");	
	
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	

	m_fArmor=100;
	m_fHealth=100;
	m_fArmorModifier=1;
	m_fHealthModifier=50;	
}

void StatusProperty::Damage(float fDamage)
{
	float rest = m_fArmor - fDamage*m_fArmorModifier;
	m_fArmor -= fDamage * m_fArmorModifier;	
	m_fHealth -= rest * m_fArmorModifier;
}


Property* Create_StatusProperty()
{
	return new StatusProperty;
}








