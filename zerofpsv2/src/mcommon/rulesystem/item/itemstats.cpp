#include "itemstats.h"
#include <iostream>
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

void ItemStats::CanEquipOn ( string kEquipOn )
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
   for ( map<string, int>::iterator kIte = m_kSkillBonus.begin(); 
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

ItemStats::ItemStats()
{
   m_kItemName = "nameless item";
   m_iValue = 0;
   m_fWeight = 0;
   m_fQuality = 1;
   m_iQuantity = 1;
}

// ---------------------------------------------------------------------------------------------

void ItemStats::AddItemValue ( int iValue )
{
   m_iValue += iValue;

   if ( m_iValue < 0 )
      m_iValue = 0;
}

// ---------------------------------------------------------------------------------------------
