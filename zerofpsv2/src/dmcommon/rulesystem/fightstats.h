#ifndef FIGHTSTATS_H
#define FIGHTSTATS_H

#pragma warning (disable :4786)
#pragma warning (disable :4251)

#include "../mcommon_x.h"
#include <string>
#include <map>
   using namespace std;

class MCOMMON_API FightStats
{
public:
   map<string, int> m_kAttack;
   map<string, int> m_kDefence;
};

#endif





