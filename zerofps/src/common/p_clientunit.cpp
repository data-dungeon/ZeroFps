#include "p_clientunit.h"

P_ClientUnit::P_ClientUnit()
{
	strcpy(m_acName,"P_ClientUnit");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_ALL;

	m_cTeam = 		0;
	m_cHealth = 	255;
	m_cWeapon = 	0;
	m_cArmor = 		0;
	m_cPropultion =0;
	m_kName = 		"NoName";
	m_bSelected =	false;
}

void P_ClientUnit::Save(ZFMemPackage* pkPackage)
{

}

void P_ClientUnit::Load(ZFMemPackage* pkPackage)
{

}

COMMON_API Property* Create_P_ClientUnit()
{
	return new P_ClientUnit();

}
