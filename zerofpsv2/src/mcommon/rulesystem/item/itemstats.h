#pragma warning (disable :4786)

#ifndef _RULESYSTEM_ITEMSTATS_H_
#define _RULESYSTEM_ITEMSTATS_H_

#include "../../mcommon_x.h"
#include "../character/characterstats.h"
#include "inventory_info.h"

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

   // what kind of item is this?
   EquipmentCategory m_eEquipmentCategory;
   
   int m_iContainerID; // id of this container
	int m_iCurrentContainer; // id to the container where this item is.

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
   EquipmentCategory GetEquipmentCategory();

	// gör detta föremål till en container
   void RegisterAsContainer();

	//	returns -1 if item isn't a container 
	// OBS! Blanda inte ihop denna funktion och GetCurrentContainer
   int GetContainerID() { return m_iContainerID; }  
		
	// hämta ID't till den container där detta föremål ligger
	int GetCurrentContainer() { return m_iCurrentContainer; } 

	// sätt föremålet i en ny container
	void PlaceInContainer(int iContainer) { m_iCurrentContainer = iContainer; } 

   void SetSkillBonus ( string kSkillName, int iValue );
   void SetAttributeBonus ( string kAttributeName, int iValue );
   void SetAttackBonus ( string kAttackName, int iValue );
   void SetDefenceBonus ( string kDefenceName, int iValue );
   void AddCanEquipOn ( string kEquipOn );

   bool CanEquipOn ( string kSlotName );


   bool EquipOn ( CharacterStats *pkCharStat );
   bool UnEquip ( CharacterStats *pkCharStat );
   bool UnEquip ();

   float GetQuality()                          { return m_fQuality; }
   void SetQuality(float fQuality);
   void AddQuality(float fAddQ);

   int GetQuantity()                          { return m_iQuantity; }
   void SetQuantity(int iQuality);
   void AddQuantity(int iAddQ);

   void SetItemName ( string kName )          { m_kItemName = kName; }
   void SetItemWeight ( float fWeight )       { m_fWeight = fWeight; }
   void SetEquipmentCategory( EquipmentCategory eCategory);

   void SetItemValue ( int iValue )           { m_iValue = iValue; }
   int GetItemValue ()                        { return int(m_iValue * m_iQuantity * m_fQuality); }
   void AddItemValue ( int iValue );

   void Print();

   void Split ( int iTookens );
   bool Stock ( ItemStats *pkItemStats );

   bool operator== ( ItemStats &kItemStats );

};

#endif