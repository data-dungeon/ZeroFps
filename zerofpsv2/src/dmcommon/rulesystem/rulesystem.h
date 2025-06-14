#ifndef _RULESYSTEM_RULESYSTEM_H_
#define _RULESYSTEM_RULESYSTEM_H_

#pragma warning (disable :4786)

#include "../mcommon_x.h"
#include "fightstats.h"
#include "spell/spellstats.h"
#include <map>
#include <string>
#include <vector>
	using namespace std;

class CharacterStats;
class Entity;

struct SkillType
{
   map<string, float> m_kAttributeExp;
   map<string, float> m_kSkillExp;
   map<string, float> m_kGroupExp;
};

int DealDamage ( FightStats *pkAttacker, CharacterStats *pkVictim );
int DealDamage ( map<string,int>* pkDamage, Entity *pkVictim );
int DealDamage ( map<string,int>* pkDamage, CharacterStats *pkVictim );

void LoadStatTypes();
bool LoadSpell(string kSpellName);

extern MCOMMON_API map<string, CharacterStats> g_kCharacters;
extern MCOMMON_API vector<string> g_kSkills;
extern MCOMMON_API map<string, SkillType> g_kSkillExps;
extern MCOMMON_API vector<string> g_kAttributes;
extern MCOMMON_API vector<string> g_kData;
extern MCOMMON_API vector<string> g_kCounters;
extern MCOMMON_API map<string, vector<string> > g_kSkillGroups;
extern MCOMMON_API map<string, SpellStats*> g_kSpells;


#endif







