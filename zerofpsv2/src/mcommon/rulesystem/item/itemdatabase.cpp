#include "itemdatabase.h"
#include "itemstats.h"
#include "../../../zerofpsv2/basic/zfini.h"
#include <iostream>
#include <string>
   using namespace std;

map<string, ItemType> ItemDataBase::m_kItemType;
// ----------------------------------------------------------------------------------------------

bool ItemDataBase::LoadItemData (string kItemName)
{
  	ZFIni kIniLoader;

	string kLoadName = "/data/items/" + kItemName + ".zit";

   vector<string> kData;

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

      // quantity
     	if( kIniLoader.KeyExist("data", "quantity") )
			m_kItemType[kItemName].m_iQuantity = kIniLoader.GetIntValue( "data", "quantity" );
      else
         m_kItemType[kItemName].m_iQuantity = 1;

      // quality
     	if( kIniLoader.KeyExist("data", "quality") )
			m_kItemType[kItemName].m_fQuality = kIniLoader.GetIntValue( "data", "quality" );
      else
         m_kItemType[kItemName].m_fQuality = 1;

   }

   
   // Can be equipped on...
   if ( kIniLoader.SectionExist ("equip_on") )
       kIniLoader.GetKeyNames ("equip_on", m_kItemType[kItemName].m_kEquippableOn);


   map<string, int> *pkMod;
   string kName;

   // Load itemstat and modifiers
   for ( int type = 0; type < 4; type++ )
   {
      switch (type)
      {
         case 0:
            pkMod = &m_kItemType[kItemName].m_kSkillModifier;
            kName = "skillbonuses";
            break;
         case 1:
            pkMod = &m_kItemType[kItemName].m_kAttributeModifier;
            kName = "attributebonuses";
            break;
         case 2:
            pkMod = &m_kItemType[kItemName].m_kFightStats.m_kAttack;
            kName = "attackbonuses";
            break;
         case 3:
            pkMod = &m_kItemType[kItemName].m_kFightStats.m_kDefence;
            kName = "defencebonuses";
            break;
      }


      // Read the data from file...
      if ( kIniLoader.SectionExist (kName.c_str()) )
      {
         kIniLoader.GetKeyNames (kName.c_str(), kData);

         for ( int i = 0; i < kData.size(); i++ )
           (*pkMod)[kData[i]] = kIniLoader.GetFloatValue( kName.c_str(), (char*)kData[i].c_str() );
      }
   }

   cout << "Loaded itemtype " << kItemName << endl;


   return true;


}

// ----------------------------------------------------------------------------------------------

ItemStats* ItemDataBase::GetItem (string kName)
{
   map<string, ItemType>::iterator kIte = m_kItemType.find (kName);

   if ( kIte == m_kItemType.end() )
   {
      if ( !LoadItemData (kName) )
      {
         cout << "warning! Couln't find item of type:" << kName << endl;
         return 0;
      }
   }

   ItemStats *pkNewIS = new ItemStats( &m_kItemType[kName] );
   return pkNewIS;
}

// ----------------------------------------------------------------------------------------------
