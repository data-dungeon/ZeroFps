#ifndef _MCOMMON_INVENTORY_INFO_H_
#define _MCOMMON_INVENTORY_INFO_H_

enum EquipmentCategory
{
	Item,	// kan vara allt möjligt (utan nedastående),
			// dessa föremål går endast att placera
			// i container slots.

	Armor,	// dessa kan endast placeras i de 13
	Cloak,	// unika slots som representerar vad
	Helm,	// spelaren bär på kroppen.
	Amulett,
	Shield,
	Weapon,
	Bracer,
	Glove,
	Ring,
	Boots,
	Belt
};

#endif // #ifndef _MCOMMON_INVENTORY_INFO_H_