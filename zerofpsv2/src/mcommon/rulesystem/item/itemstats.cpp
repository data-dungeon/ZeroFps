#include "itemstats.h"
#include <iostream>
#include "../../p_item.h"
#include "../../../zerofpsv2/engine/entitymanager.h"
   using namespace std;

// ---------------------------------------------------------------------------------------------

bool ItemStats::EquipOn ( CharacterStats *pkCharStat )
{
   map<string, int>::iterator kIte;

   // add skill bonuses
   for ( kIte = m_kSkillBonus.begin();
         kIte != m_kSkillBonus.end(); kIte++ )
      pkCharStat->AddSkillValue ( (*kIte).first, (*kIte).second );

   // add attribute bonuses
   for ( kIte = m_kAttributeBonus.begin();
         kIte != m_kAttributeBonus.end(); kIte++ )
      pkCharStat->AddAttributeValue ( (*kIte).first, (*kIte).second );

   // add attack bonuses
   for ( kIte = m_kFightStats.m_kAttack.begin();
         kIte != m_kFightStats.m_kAttack.end(); kIte++ )
      pkCharStat->AddAttackValue ( (*kIte).first, (*kIte).second );

   // add defence bonuses
   for ( kIte = m_kFightStats.m_kDefence.begin();
         kIte != m_kFightStats.m_kDefence.end(); kIte++ )
      pkCharStat->AddDefenceValue ( (*kIte).first, (*kIte).second );

   // add speed modifier
//   pkCharStat->m_pkParent->AddReloadTime ( m_fUseSpeed );
  
   return true;
}

// ---------------------------------------------------------------------------------------------

bool ItemStats::UnEquip ( CharacterStats *pkCharStat )
{
   map<string, int>::iterator kIte;

   // remove skill bonuses
   for ( kIte = m_kSkillBonus.begin();
         kIte != m_kSkillBonus.end(); kIte++ )
      pkCharStat->AddSkillValue ( (*kIte).first, -(*kIte).second );

   // remove attribute bonuses
   for ( kIte = m_kAttributeBonus.begin();
         kIte != m_kAttributeBonus.end(); kIte++ )
      pkCharStat->AddAttributeValue ( (*kIte).first, -(*kIte).second );
   
   // remove attack bonuses
   for ( kIte = m_kFightStats.m_kAttack.begin();
         kIte != m_kFightStats.m_kAttack.end(); kIte++ )
      pkCharStat->AddAttackValue ( (*kIte).first, -(*kIte).second );

   // remove defence bonuses
   for ( kIte = m_kFightStats.m_kDefence.begin();
         kIte != m_kFightStats.m_kDefence.end(); kIte++ )
      pkCharStat->AddDefenceValue ( (*kIte).first, -(*kIte).second );

   // remove speed modifier
   //pkCharStat->AddReloadTime ( -m_fUseSpeed );

   return true;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetQuality(float fQuality)
{
   m_fQuality = fQuality;

   m_uiVersion++;

   if ( m_fQuality < 0 )
      // destroy object?
      m_fQuality = 0;
   else if ( m_fQuality > 1 )
      m_fQuality = 1;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddQuality(float fAddQ)
{
   m_fQuality += fAddQ;

   m_uiVersion++;

   if ( m_fQuality < 0 )
      // destroy object?
      m_fQuality = 0;
   else if ( m_fQuality > 1 )
      m_fQuality = 1;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetQuantity(int iQuantity)
{
   m_iQuantity = iQuantity;

   m_uiVersion++;

   if ( m_iQuantity < 1 )
      m_iQuantity = 1;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddQuantity(int iAddQ)
{
   m_iQuantity += iAddQ;

   m_uiVersion++;

   // destroy object?
   if ( m_iQuantity < 0 )
      m_iQuantity = 0;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetSkillBonus ( string kSkillName, int iValue )
{
   m_uiVersion++;

   m_kSkillBonus[kSkillName] = iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetAttributeBonus ( string kAttributeName, int iValue )
{
   m_uiVersion++;

   m_kAttributeBonus[kAttributeName] = iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetAttackBonus ( string kAttackName, int iValue )
{
   m_uiVersion++;

   m_kFightStats.m_kAttack[kAttackName] = iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetDefenceBonus ( string kDefenceName, int iValue )
{
   m_uiVersion++;

   m_kFightStats.m_kDefence[kDefenceName] = iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetEquipmentCategory ( EquipmentCategory eCategory)
{
   m_uiVersion++;

   m_eEquipmentCategory = eCategory;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddCanEquipOn ( string kEquipOn )
{
   m_uiVersion++;

   m_kEquippableOn.push_back ( kEquipOn );
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddToSkillBonus ( string kSkillName, int iValue )
{
   m_uiVersion++;

   m_kSkillBonus[kSkillName] += iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddToAttributeBonus ( string kAttributeName, int iValue )
{
   m_uiVersion++;

   m_kAttributeBonus[kAttributeName] += iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddToAttackBonus ( string kAttackName, int iValue )
{
   m_uiVersion++;

   m_kFightStats.m_kAttack[kAttackName] += iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddToDefenceBonus ( string kDefenceName, int iValue )
{
   m_uiVersion++;

   m_kFightStats.m_kDefence[kDefenceName] += iValue;
}

// ---------------------------------------------------------------------------------------------

int ItemStats::GetSkillBonus ( string kSkillName )
{
   map<string, int>::iterator kIte = m_kSkillBonus.find ( kSkillName );

   if ( kIte != m_kSkillBonus.end() )
      return m_kSkillBonus[kSkillName];
   else
      return 0;
}

// ---------------------------------------------------------------------------------------------

int ItemStats::GetAttributeBonus ( string kAttributeName )
{
   map<string, int>::iterator kIte = m_kAttributeBonus.find ( kAttributeName );

   if ( kIte != m_kAttributeBonus.end() )
      return m_kAttributeBonus[kAttributeName];
   else
      return 0;
}

// ---------------------------------------------------------------------------------------------

int ItemStats::GetAttackBonus ( string kAttackName )
{
   map<string, int>::iterator kIte = m_kFightStats.m_kAttack.find ( kAttackName );

   if ( kIte != m_kFightStats.m_kAttack.end() )
      return m_kFightStats.m_kAttack[kAttackName];
   else
      return 0;
}

// ---------------------------------------------------------------------------------------------

int ItemStats::GetDefenceBonus ( string kDefenceName )
{
   map<string, int>::iterator kIte = m_kFightStats.m_kDefence.find ( kDefenceName );

   if ( kIte != m_kFightStats.m_kDefence.end() )
      return m_kFightStats.m_kDefence[kDefenceName];
   else
      return 0;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::Print()
{
   cout << "__________---------***Item Stats***---------_________" << endl << endl;

   cout << "Stats:" << endl;
   cout << "   Name:" << m_kItemName << endl;
   cout << "   Value:" << m_iValue << endl;
   cout << "   Weight:" << m_fWeight << endl;
   cout << "   Quality:" << m_fQuality << endl;
   cout << "   Quantity:" << m_iQuantity << endl;


   cout << "SkillBonuses:" << endl;
	
	map<string, int>::iterator kIte;
   
   for (kIte = m_kSkillBonus.begin(); 
         kIte != m_kSkillBonus.end(); kIte++ )
      cout << "   " << (*kIte).first << ":" << (*kIte).second << endl;


   cout << "AttributeBonuses:" << endl;
   for ( kIte = m_kAttributeBonus.begin(); kIte != m_kAttributeBonus.end(); kIte++ )
      cout << "   " << (*kIte).first << ":" << (*kIte).second << endl;

   cout << "AttackBonuses:" << endl;
   for ( kIte = m_kFightStats.m_kAttack.begin(); kIte != m_kFightStats.m_kAttack.end(); kIte++ )
      cout << "   " << (*kIte).first << ":" << (*kIte).second << endl;

   cout << "DefenceBonuses:" << endl;
   for ( kIte = m_kFightStats.m_kDefence.begin(); kIte != m_kFightStats.m_kDefence.end(); kIte++ )
      cout << "   " << (*kIte).first << ":" << (*kIte).second << endl;

  cout << "__________---------****************---------_________" << endl << endl;

}

// ---------------------------------------------------------------------------------------------

EquipmentCategory ItemStats::GetEquipmentCategory()
{
	return m_eEquipmentCategory;
}

// ---------------------------------------------------------------------------------------------

ItemStats::ItemStats(P_Item* pkProperty)
{
   m_pkProperty = pkProperty;

   m_kItemName = "nameless item";
   m_iValue = 0;
   m_fWeight = 0;
   m_fQuality = 1;
   m_iQuantity = 1;
   m_eEquipmentCategory = Item;
	strcpy(m_szPic, "dummy.bmp");
   m_uiVersion = 0;
   
   m_fUseSpeed = 0;

   m_iCurrentContainer = -1;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddItemValue ( int iValue )
{
   m_iValue += iValue;

   m_uiVersion++;

   if ( m_iValue < 0 )
      m_iValue = 0;
}

// ---------------------------------------------------------------------------------------------

bool ItemStats::operator== ( ItemStats &kItemStats )
{
   if ( m_kItemName != kItemStats.m_kItemName ||
        m_iValue != kItemStats.m_iValue ||
        m_fQuality != kItemStats.m_fQuality ||
        m_fWeight != kItemStats.m_fWeight )
      return false;

   // copy version???
   //m_uiVersion = kItemStats;

   
   map<string, int>::iterator kIte, kSearch;
   
   // compare skillbonuses
   for ( kIte = m_kSkillBonus.begin(); kIte != m_kSkillBonus.end(); kIte++ )
   {
       kSearch = kItemStats.m_kSkillBonus.find ( (*kIte).first );

       // test if bonus exists at all
       if ( kSearch == kItemStats.m_kSkillBonus.end() )
          return false;

       // test if the bonus is same
       if ( (*kSearch).second != (*kIte).second )
          return false;
   }

   // compare attributebonuses
   for ( kIte = m_kAttributeBonus.begin(); kIte != m_kAttributeBonus.end(); kIte++ )
   {
       kSearch = kItemStats.m_kAttributeBonus.find ( (*kIte).first );

       // test if bonus exists at all
       if ( kSearch == kItemStats.m_kAttributeBonus.end() )
          return false;

       // test if the bonus is same
       if ( (*kSearch).second != (*kIte).second )
          return false;
   }


   // compare attackbonuses
   for ( kIte = m_kFightStats.m_kAttack.begin(); kIte != m_kFightStats.m_kAttack.end(); kIte++ )
   {
       kSearch = kItemStats.m_kFightStats.m_kAttack.find ( (*kIte).first );

       // test if bonus exists at all
       if ( kSearch == kItemStats.m_kFightStats.m_kAttack.end() )
          return false;

       // test if the bonus is same
       if ( (*kSearch).second != (*kIte).second )
          return false;
   }

   // compare defencebonuses
   for ( kIte = m_kFightStats.m_kDefence.begin(); kIte != m_kFightStats.m_kDefence.end(); kIte++ )
   {
       kSearch = kItemStats.m_kFightStats.m_kDefence.find ( (*kIte).first );

       // test if bonus exists at all
       if ( kSearch == kItemStats.m_kFightStats.m_kDefence.end() )
          return false;

       // test if the bonus is same
       if ( (*kSearch).second != (*kIte).second )
          return false;
   }



   vector<string>::iterator kEqIte, kEqIte2;

   // compare equippibility
   for ( kEqIte = m_kEquippableOn.begin(); kEqIte != m_kEquippableOn.end(); kEqIte++ )
   {
      bool bExist = false;

      for ( kEqIte2 = kItemStats.m_kEquippableOn.begin(); kEqIte2 != kItemStats.m_kEquippableOn.end(); kEqIte2++ )
         if ( (*kEqIte) == (*kEqIte2) )
            bExist = true;

      if ( !bExist )
         return false;
   }

   return true;
}

// ---------------------------------------------------------------------------------------------

bool ItemStats::CanEquipOn ( string kSlotName )
{
   for ( unsigned int i = 0; i < m_kEquippableOn.size(); i++)
      if ( m_kEquippableOn[i] == kSlotName )
         return true;

   return false;
}

// ---------------------------------------------------------------------------------------------

ItemStats& ItemStats::operator= ( ItemStats &kItemStats )
{
  
   m_kItemName = kItemStats.m_kItemName;
   
   m_iQuantity = kItemStats.m_iQuantity;
   m_iValue = kItemStats.m_iValue;

   m_fQuality = kItemStats.m_fQuality;
   m_fWeight = kItemStats.m_fWeight;


   m_kSkillBonus = kItemStats.m_kSkillBonus;
	m_kAttributeBonus = kItemStats.m_kAttributeBonus;

   m_kFightStats = kItemStats.m_kFightStats;

   m_kEquippableOn = kItemStats.m_kEquippableOn;

   //int m_iContainerID; // id of this container
	m_iCurrentContainer = kItemStats.m_iCurrentContainer;
	
	return *this;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddAfterName ( string kAddName )
{
   m_uiVersion++;

   m_kItemName += " " + kAddName;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddBeforeName ( string kAddName )
{
   m_uiVersion++;

   m_kItemName = kAddName + " " + m_kItemName;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetIcon ( char* kIcon)
{
   strcpy( m_szPic, kIcon );
  
   m_uiVersion++;
}

// ---------------------------------------------------------------------------------------------

   