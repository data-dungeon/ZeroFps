#include "itemstats.h"
#include <iostream>
   using namespace std;

int ItemStats::s_iContainerCounter = 0;

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

   return true;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetQuality(float fQuality)
{
   m_fQuality = fQuality;

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

   if ( m_iQuantity < 1 )
      m_iQuantity = 1;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddQuantity(int iAddQ)
{
   m_iQuantity += iAddQ;

   // destroy object?
   if ( m_iQuantity < 0 )
      m_iQuantity = 0;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetSkillBonus ( string kSkillName, int iValue )
{
   m_kSkillBonus[kSkillName] = iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetAttributeBonus ( string kAttributeName, int iValue )
{
   m_kAttributeBonus[kAttributeName] = iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetAttackBonus ( string kAttackName, int iValue )
{
   m_kFightStats.m_kAttack[kAttackName] = iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetDefenceBonus ( string kDefenceName, int iValue )
{
   m_kFightStats.m_kDefence[kDefenceName] = iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::SetEquipmentCategory ( EquipmentCategory eCategory)
{
   m_eEquipmentCategory = eCategory;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddCanEquipOn ( string kEquipOn )
{
   m_kEquippableOn.push_back ( kEquipOn );
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddToSkillBonus ( string kSkillName, int iValue )
{
   m_kSkillBonus[kSkillName] += iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddToAttributeBonus ( string kAttributeName, int iValue )
{
   m_kAttributeBonus[kAttributeName] += iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddToAttackBonus ( string kAttackName, int iValue )
{
   m_kFightStats.m_kAttack[kAttackName] += iValue;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddToDefenceBonus ( string kDefenceName, int iValue )
{
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

ItemStats::ItemStats()
{
   m_kItemName = "nameless item";
   m_iValue = 0;
   m_fWeight = 0;
   m_fQuality = 1;
   m_iQuantity = 1;
   m_eEquipmentCategory = Item;
   m_iContainerID = -1;
	strcpy(m_szPic[0], "dummy.bmp");
	strcpy(m_szPic[1], "dummy_a.bmp");

}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddItemValue ( int iValue )
{
   m_iValue += iValue;

   if ( m_iValue < 0 )
      m_iValue = 0;
}

// ---------------------------------------------------------------------------------------------

bool ItemStats::Stock ( ItemStats *pkItemStats )
{
   if ( pkItemStats == this )
   {
      m_iQuantity += pkItemStats->m_iQuantity;

      // remove the stacked object


      return true;
   }
   else
      return false;
}

// ---------------------------------------------------------------------------------------------

bool ItemStats::operator== ( ItemStats &kItemStats )
{
   if ( m_kItemName != kItemStats.m_kItemName ||
        m_iValue != kItemStats.m_iValue ||
        m_fQuality != kItemStats.m_fQuality ||
        m_fWeight != kItemStats.m_fWeight )
      return false;

   
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

void ItemStats::RegisterAsContainer()
{ 
	s_iContainerCounter++;
	m_iContainerID = s_iContainerCounter;
}