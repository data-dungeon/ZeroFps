#ifndef ITEMDATABASE_H
#define ITEMDATABASE_H

#pragma warning (disable :4786)

#include "../../mcommon_x.h"
#include "../fightstats.h"
#include <map>
#include <string>
#include <vector>
	using namespace std;

class ItemStats;

struct ItemType
{

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













