/* Richard Svensson

	Server property that contains all information about weapons, armor ,propotion etc

*/

#ifndef _P_UNITSYSTEM_H_
#define _P_UNITSYSTEM_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

#include <vector>

using namespace std;

enum COMMON_API WeaponTypes
{
	WT_DIRECT,
	WT_PROJECTILE,
	WT_BALISTIC,
};


class COMMON_API Weapon
{
	public:
	
		string	sName;
		int		iType;

		int		iDamage;
		float		afModifiers[4];
	
		float		fRange;
		float		fFireRate;
	
		int		iProjectile;
		int		iHit;
	
		float		fEffectArea;
};

class COMMON_API Armor
{
	public:
		string	sName;
		
		float		fModifier;
};

class COMMON_API UnitSystems
{
	public:
		vector<Weapon>		kWeapons;
		vector<Armor>		kArmors;		

};


class COMMON_API P_UnitSystem: public Property {
	private:
		UnitSystems				m_kBaseSystems;
		int						m_iPlayers;
	
		vector<UnitSystems>	m_kPlayerSystems;
	
	public:
		P_UnitSystem();
		void CloneOf(Property* pkProperty) { }

		void SetupSystems();
		
		Weapon* GetWeaponPointer(int iPlayer,int iWeapon);
};

COMMON_API Property* Create_P_UnitSystem();


#endif
