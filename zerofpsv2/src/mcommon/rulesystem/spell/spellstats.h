#ifndef SPELLSTATS_H
#define SPELLSTATS_H

#include <vector>
#include <string>
#include <map>
#include "../../../zerofpsv2/basic/vector3.h"
#include "../fightstats.h"
   using namespace std;


enum eSpellTarget {eON_CASTER, eON_FRIEND, eON_ENEMY, eFROM_CASTER_TO_ENEMY, 
                   eFROM_ENEMY_TO_CASTER, eON_GROUND, eFROM_CASTER_TO_GROUND};

enum eOnDestrucion {eKILL_PARENT, eKILL_SELF};

struct GfxSpellEffect
{
   float m_fStartTime; // delay from when Spell was created
   string m_kPSystemName;
   int m_iAttackType;
   Vector3 m_kOffset;
};

class SpellStats
{
public:   
   int 
      m_iCost,
      m_iCastOn,
      m_iRequiredLevel,
      m_iOnDestruction;


   float 
      m_fLifeTime, // how long the spell lasts
      m_fStartRadius, // collision radius, that is...
      m_fEndRadius,
      m_fDamageEvery;

   string 
      m_kSpellGroup, // which type of spell it is (fire, healing....)
      m_kSpellName,
      m_kOnHit[2], // 0=event 1=spelltype on event
      m_kAffectedObjects; // characters, items, all, characters_and_items
    
   // Stat bonuses (or curses :) )
   map<string, int> 
      m_kSkillBonuses,
      m_kAttributeBonuses,
      m_kDamage;           // types of damage and how much damage they do

   // attack&defence bonuses / curses
   FightStats m_kFightBonuses;

   vector<GfxSpellEffect> m_kGraphicEffects;

   friend class P_Spell;

};

#endif








