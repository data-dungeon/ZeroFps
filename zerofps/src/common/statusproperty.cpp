#include "statusproperty.h"

StatusProperty::StatusProperty()
{
	strcpy(m_acName,"StatusProperty");	
	
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	

	m_fMaxArmor=100;
	m_fMaxHealth=100;
	m_fArmor=100;
	m_fHealth=100;
	m_fArmorModifier=1;
	m_fHealthModifier=5;	
}

void StatusProperty::Damage(float fDamage)
{
	if(m_fArmor <= 0)
		m_fHealth -= fDamage * m_fHealthModifier;
	else {	
		m_fArmor -= fDamage * m_fArmorModifier;		
		
		if(m_fArmor<0)
			m_fHealth+=(m_fArmor/m_fArmorModifier) * m_fHealthModifier;
	}

	cout<<"armor "<<m_fArmor<<endl;
	cout<<"health "<<m_fHealth<<endl;	

}


Property* Create_StatusProperty()
{
	return new StatusProperty;
}








