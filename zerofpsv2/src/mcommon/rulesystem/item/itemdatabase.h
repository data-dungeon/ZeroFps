#ifndef ITEMDATABASE_H
#define ITEMDATABASE_H

#pragma warning (disable :4786)

#include "../../mcommon_x.h"
#include <map>
#include <string>
#include <vector>
	using namespace std;

class ItemStats;

struct ItemType
{
	string m_kName;
	float m_fWeight;  //kg
	int m_iValue;     // gold value
   int m_iQuantity;  // how many in a heap from start
   float m_fQuality; // default = 1 = perfect quality

   // when equipped
	map<string, int> m_kSkillModifier_equipped;
	map<string, int> m_kAttributeModifier_equipped;
   map<string, int> m_kAttack_equipped;
   map<string, int> m_kDefence_equipped;
   
   // when unequipped
   map<string, int> m_kSkillModifier_unequipped;
	map<string, int> m_kAttributeModifier_unequipped;
   map<string, int> m_kAttack_unequipped;
   map<string, int> m_kDefence_unequipped;

   vector<string> m_kEquippableOn;

};


class ItemDataBase
{
private:
	static map<string, ItemType> m_kItemType;


public:
   static bool LoadItemData (string kItemName);
   static ItemStats* GetItem (string kName);

};

#endif













