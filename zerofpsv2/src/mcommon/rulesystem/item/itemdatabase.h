#ifndef ITEMDATABASE_H
#define ITEMDATABASE_H

#include <map>
#include <string>
	using namespace std;

struct ItemType
{
	string m_kName;
	float m_fWeight;
	int m_iPrice;

	map<string, float> m_kStatModifier;
	map<string, float> m_kAttributeModifier;
};


class RULESYSTEM_API ItemDataBase
{
private:
	map<string, ItemType> m_kItemType;


public:
	LoadItemData (string kItemName);




};

#endif













