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


/*	Actions
	
	some HandUse actions are hardcoded
	
	Action_UsePrimary
	Action_UseSecondary
	Action_UseReload
	
*/

enum COMMON_API Action_Type
{
	NORMALUSE,
	INVENTORYUSE,
	HANDUSE,
};

struct COMMON_API UseFunktion
{
	int		iType;
	string	kName;
	string	kSignal;
};



class COMMON_API ItemProperty: public Property {
	private:
		ObjectManager *m_pkObjectMan;	
		vector<UseFunktion>	m_kUseData;
	
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
				
		bool RegisterAction(int iType,const char* acName,const char* acSignal);
		bool UnRegisterAction(int iType,const char* acName);
		bool CheckIfActionExist(int iType,const char* acName);
		
		
		bool GetUses(int iType,vector<string>* m_kNames);
		bool Use(int iType,const char* acName);

		bool UseOn(Object* pkObject);

};

COMMON_API Property* Create_ItemProperty();


#endif






