#ifndef _MCOMMON_INVENTORY_INFO_H_
#define _MCOMMON_INVENTORY_INFO_H_

enum EquipmentCategory
{
	Item,	 // can be almost anything.. can only be placed in container slots.

	//
	// Items that both can be placed in a container and on a special slot.
	//
	Armor,	 
	Cloak,	 // A cloak that wizards wear to protect them self
	Helm,	 
	Amulett, 
	Shield,  
	Weapon,  
	Bracer,  // Thick bands of metal or leather tied to a characters forearm.
	Glove,   // (handskar)
	Ring,	
	Boots,
	Belt
};

#endif // #ifndef _MCOMMON_INVENTORY_INFO_H_