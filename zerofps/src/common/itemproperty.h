/* Richard Svensson

	This property describes a basic ingame item.	

*/


#ifndef _ITEMPROPERTY_H_
#define _ITEMPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

using namespace std;

class COMMON_API ItemProperty: public Property {
	private:
		vector<PropertyValues> GetPropertyValues();
		
	public:
		bool	m_bPickable;			//can the item be picked up by the player
		int 	m_iItemSizeX;			 //size in invetory
		int 	m_iItemSizeY;
		string m_kItemIcon;			//icon in inventory (texture)
		string m_kDescription;		//showed when examining the item
		string m_kItemName;			//the items name

		ItemProperty();

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
};

COMMON_API Property* Create_ItemProperty();


#endif






