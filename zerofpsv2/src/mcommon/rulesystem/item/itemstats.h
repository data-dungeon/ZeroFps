#pragma warning (disable :4786)

#ifndef _RULESYSTEM_ITEMSTATS_H_
#define _RULESYSTEM_ITEMSTATS_H_

#include "../../mcommon_x.h"
#include "../character/characterstats.h"
#include "itemdatabase.h"

#include <map>
#include <string>
	using namespace std;

class MCOMMON_API ItemStats
{
private:
	ItemType *m_pkItemType;
public:
   ItemStats (ItemType *pkItemType)       { m_pkItemType = pkItemType; }

   bool EquipOn ( CharacterStats *pkCharStat );
   bool UnEquip ( CharacterStats *pkCharStat );
};

#endif










