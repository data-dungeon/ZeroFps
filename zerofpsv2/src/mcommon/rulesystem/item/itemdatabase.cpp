#include "itemdatabase.h"
#include "../../../zerofpsv2/basic/zfini.h"
#include <iostream>
#include <string>
   using namespace std;

bool ItemDataBase::LoadItemData (string kItemName)
{
  	ZFIni kIniLoader;

	string kLoadName = "/data/items/" + kItemName + ".zit";

	// Open file
	if( !kIniLoader.Open(kLoadName.c_str(), 0) )		
	{
		cout << "Error! Failed to load item:" << kItemName << endl;
		return false;
	}

   // Load data (name, weight...common stuff)

   if ( kIniLoader.SectionExist ("data") )
   {
      // name
     	if( kIniLoader.KeyExist("data", "name") )
			m_kItemType[kItemName].m_kName = kIniLoader.GetValue( "data", "name" );
      else
         m_kItemType[kItemName].m_kName = kItemName;

      // weight
     	if( kIniLoader.KeyExist("data", "weight") )
			m_kItemType[kItemName].m_fWeight = kIniLoader.GetFloatValue( "data", "weight" );
      else
         m_kItemType[kItemName].m_fWeight = 0;

      // value
     	if( kIniLoader.KeyExist("data", "value") )
			m_kItemType[kItemName].m_iValue = kIniLoader.GetIntValue( "data", "value" );
      else
         m_kItemType[kItemName].m_iValue = 0;

      // value
     	if( kIniLoader.KeyExist("data", "value") )
			m_kItemType[kItemName].m_iValue = kIniLoader.GetIntValue( "data", "value" );
      else
         m_kItemType[kItemName].m_iValue = 0;
   }

   
   // Can be equipped on...
   if ( kIniLoader.SectionExist ("equip_on") )
   {
       //m_kEquippableOn


   }


   return true;



	

}