#pragma warning (disable :4786)

#ifndef _RULESYSTEM_ITEMSTATS_H_
#define _RULESYSTEM_ITEMSTATS_H_

#include "../../mcommon_x.h"
#include "../character/characterstats.h"

#include <map>
#include <string>
	using namespace std;

class MCOMMON_API ItemStats
{
private:

   // TODO! 
   // add inventory icon
   
   string m_kItemName;
   
   int 
      m_iQuantity,
      m_iValue; // gold value

   float 
      m_fQuality,
      m_fWeight;

   // status modifiers
   map<string, int> m_kSkillBonus;
	map<string, int> m_kAttributeBonus;
   FightStats m_kFightStats;

   vector<string> m_kEquippableOn;

public:
   ItemStats();

   void AddToSkillBonus ( string kSkillName, int iValue );
   void AddToAttributeBonus ( string kAttributeName, int iValue );
   void AddToAttackBonus ( string kAttackName, int iValue );
   void AddToDefenceBonus ( string kDefenceName, int iValue );

   int GetSkillBonus ( string kSkillName );
   int GetAttributeBonus ( string kAttributeName );
   int GetAttackBonus ( string kAttackName );
   int GetDefenceBonus ( string kDefenceName );

   void SetSkillBonus ( string kSkillName, int iValue );
   void SetAttributeBonus ( string kAttributeName, int iValue );
   void SetAttackBonus ( string kAttackName, int iValue );
   void SetDefenceBonus ( string kDefenceName, int iValue );
   void CanEquipOn ( string kEquipOn );


   bool EquipOn ( CharacterStats *pkCharStat );
   bool UnEquip ( CharacterStats *pkCharStat );

   float GetQuality()                          { return m_fQuality; }
   void SetQuality(float fQuality);
   void AddQuality(float fAddQ);

   int GetQuantity()                          { return m_iQuantity; }
   void SetQuantity(int iQuality);
   void AddQuantity(int iAddQ);

   void SetItemName ( string kName )          { m_kItemName = kName; }
   void SetItemWeight ( float fWeight )       { m_fWeight = fWeight; }

   void SetItemValue ( int iValue )           { m_iValue = iValue; }
   int GetItemValue ()                        { return int(m_iValue * m_iQuantity * m_fQuality); }
   void AddItemValue ( int iValue );

   void Print();

};

#endif