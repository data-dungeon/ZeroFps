#ifndef _MCOMMON_INVENTORY_INFO_H_
#define _MCOMMON_INVENTORY_INFO_H_

enum EquipmentCategory
{
	Item,	// kan vara allt m�jligt (utan nedast�ende),
			// dessa f�rem�l g�r endast att placera
			// i container slots.

	Armor,	// dessa kan endast placeras i de 13
	Cloak,	// unika slots som representerar vad
	Helm,	// spelaren b�r p� kroppen.
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