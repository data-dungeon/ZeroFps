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

class COMMON_API Weapon
{
	string	sName;
	int		iType;

	int		iDamage;
	float		afModifiers[4];
	
	float		fRange;
	float		fFireRate;
	
	int		iProjectile;
	int		iHit;
	
	
};

class COMMON_API UnitSystems
{
	vector<Weapon>	kWeapons;

};


class COMMON_API P_UnitSystem: public Property {
	private:
		vector<UnitSystems>	m_kPlayerSystems;
	
	public:
		P_UnitSystem();
		void CloneOf(Property* pkProperty) { }


};

COMMON_API Property* Create_P_UnitSystem();


#endif
