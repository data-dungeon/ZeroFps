#ifndef FIGHTSTATS_H
#define FIGHTSTATS_H

#include <string>
#include <map>
   using namespace std;

class FightStats
{
public:
   map<string, int> m_kAttack;
   map<string, int> m_kDefence;
};

#endif