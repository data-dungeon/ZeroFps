#include "p_unitsystem.h"

P_UnitSystem::P_UnitSystem()
{
	strcpy(m_acName,"P_UnitSystem");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	cout<<"unit system manager created"<<endl;

	m_iPlayers = 4;
	
	SetupSystems();

	//add base systems to all players
	for(int i =0;i<m_iPlayers;i++)
		m_kPlayerSystems.push_back(m_kBaseSystems);
}

void P_UnitSystem::SetupSystems()
{
	//setup weapons
	
	Weapon Gun80mm;
	Gun80mm.sName = 			"Gun80mm";
	Gun80mm.iType = 			WT_DIRECT;
	Gun80mm.iDamage = 		30;
	Gun80mm.afModifiers[0] =0.5;
	Gun80mm.afModifiers[1] =0.8;	
	Gun80mm.afModifiers[2] =1.0;	
	Gun80mm.afModifiers[3] =1.0;	
	Gun80mm.fRange = 			6;
	Gun80mm.fFireRate =		2;
	Gun80mm.iHit = 			0;
											
	m_kBaseSystems.kWeapons.push_back(Gun80mm);

	//setup armors

	Armor BodyArmor;
		BodyArmor.sName =				"BodyArmor";
		BodyArmor.fModifier = 		1;

	Armor LightArmor;
		LightArmor.sName =			"LightArmor";
		LightArmor.fModifier = 		0.5;
	
	Armor HeavyArmor;
		HeavyArmor.sName =			"HeavyArmor";
		HeavyArmor.fModifier = 		0.3;
	
	Armor BuildingArmor;
		BuildingArmor.sName =		"BuildingArmor";
		BuildingArmor.fModifier = 	0.6;
	
	m_kBaseSystems.kArmors.push_back(BodyArmor);	
	m_kBaseSystems.kArmors.push_back(LightArmor);		
	m_kBaseSystems.kArmors.push_back(HeavyArmor);		
	m_kBaseSystems.kArmors.push_back(BuildingArmor);		
}

Weapon* P_UnitSystem::GetWeaponPointer(int iPlayer,int iWeapon)
{
	if(iPlayer >= m_iPlayers)
		return NULL;
		
	if(iWeapon >= m_kPlayerSystems[iPlayer].kWeapons.size())
		return NULL;

	return &(m_kPlayerSystems[iPlayer].kWeapons[iWeapon]);
}

COMMON_API Property* Create_P_UnitSystem()
{
	return new P_UnitSystem();

}







