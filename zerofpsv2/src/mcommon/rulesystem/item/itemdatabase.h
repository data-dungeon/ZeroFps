#ifndef ITEMDATABASE_H
#define ITEMDATABASE_H

#pragma warning (disable :4786)

#include "../../mcommon_x.h"
#include <map>
#include <string>
#include <vector>
	using namespace std;

struct ItemType
{
	string m_kName;
	float m_fWeight;  //kg
	int m_iValue;     // gold value
   int m_iAntal;     // how many in a heap from start

   // when equipped
	map<string, float> m_kStatModifier_equipped;
	map<string, float> m_kAttributeModifier_equipped;
   map<string, float> m_kAttack_equipped;
   map<string, float> m_kDefence_equipped;
   
   // when unequipped
   map<string, float> m_kStatModifier_unequipped;
	map<string, float> m_kAttributeModifier_unequipped;
   map<string, float> m_kAttack_unequipped;
   map<string, float> m_kDefence_unequipped;

   vector<string> m_kEquippableOn;

};


class MCOMMON_API ItemDataBase
{
private:
	map<string, ItemType> m_kItemType;


public:
	bool LoadItemData (string kItemName);

};

#endif













