#include "itemstats.h"

// ---------------------------------------------------------------------------------------------

bool ItemStats::EquipOn ( CharacterStats *pkCharStat )
{
   map<string, int>::iterator kIte;

   // add skill bonuses
   for ( kIte = m_pkItemType->m_kSkillModifier_equipped.begin();
         kIte != m_pkItemType->m_kSkillModifier_equipped.end(); kIte++ )
      pkCharStat->AddSkillValue ( (*kIte).first, (*kIte).second );

   // add attribute bonuses
   for ( kIte = m_pkItemType->m_kAttributeModifier_equipped.begin();
         kIte != m_pkItemType->m_kAttributeModifier_equipped.end(); kIte++ )
      pkCharStat->AddAttributeValue ( (*kIte).first, (*kIte).second );

/*   // add attack bonuses
   for ( kIte = m_pkItemType->m_kAttack_equipped.begin();
         kIte < m_pkItemType->m_kAttack_equipped.end(); kIte++ )
      pkCharStat->AddAttributeValue ( (*kIte).first, (*kIte).second );

   // add defence bonuses
   for ( kIte = m_pkItemType->m_kDefence_equipped.begin();
         kIte < m_pkItemType->m_kDefence_equipped.end(); kIte++ )
      pkCharStat->AddAttributeValue ( (*kIte).first, (*kIte).second );
  
*/


   return true;
}

// ---------------------------------------------------------------------------------------------

bool ItemStats::UnEquip ( CharacterStats *pkCharStat )
{
   map<string, int>::iterator kIte;

   // remove skill bonuses
   for ( kIte = m_pkItemType->m_kSkillModifier_equipped.begin();
         kIte != m_pkItemType->m_kSkillModifier_equipped.end(); kIte++ )
      pkCharStat->AddSkillValue ( (*kIte).first, -(*kIte).second );

   // remove attribute bonuses
   for ( kIte = m_pkItemType->m_kAttributeModifier_equipped.begin();
         kIte != m_pkItemType->m_kAttributeModifier_equipped.end(); kIte++ )
      pkCharStat->AddAttributeValue ( (*kIte).first, -(*kIte).second );


   return true;
}

// ---------------------------------------------------------------------------------------------
