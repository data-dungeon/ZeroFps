#include "rulesystem.h"
#include "character/characterstats.h"
#include "../p_charstats.h"
#include <iostream>
   using namespace std;

MCOMMON_API map<string, CharacterStats> g_kCharacters;
MCOMMON_API vector<string> g_kSkills;
MCOMMON_API vector<string> g_kAttributes;
MCOMMON_API vector<string> g_kData;
MCOMMON_API vector<string> g_kCounters;
MCOMMON_API map<string, SkillType> g_kSkillExps;
MCOMMON_API map<string, vector<string> > g_kSkillGroups;
MCOMMON_API map<string, SpellStats*> g_kSpells;

// -----------------------------------------------------------------------------------------------

int DealDamage ( FightStats *pkAttacker, CharacterStats* pkVictim )
{
   return DealDamage ( &pkAttacker->m_kAttack, pkVictim );
}

// -----------------------------------------------------------------------------------------------

int DealDamage ( map<string,int>* pkDamage, Entity* pkVictim )
{
   // check if victim has character stats
   CharacterProperty* pkCharProp = (CharacterProperty*)pkVictim->GetProperty("P_CharStats");

   if ( pkCharProp )
   {
      CharacterStats* pkCharStats = pkCharProp->GetCharStats();

      return ( DealDamage ( pkDamage, pkCharStats ) );
   }

   return 0;
}

// -----------------------------------------------------------------------------------------------

// the function that actually calculates the damage dealt
int DealDamage ( map<string,int>* pkDamage, CharacterStats* pkVictim )
{
   // räkna ut damage på nån vänster...

   float fTotalDamage = 0;
   int iDamage, iDefence;
   string kDamageType;

   map<string, int>::iterator kDefIte;
   map<string, int>* pkDefStats = &pkVictim->GetFightStats()->m_kDefence;


   // loop through types of damage
   for ( map<string, int>::iterator kIte = pkDamage->begin();
         kIte != pkDamage->end(); kIte++ )
   {
      iDamage = (*kIte).second;
      kDamageType = (*kIte).first;

      // check if character has defence against attacktype
      kDefIte = pkDefStats->find( kDamageType );

      if ( kDefIte != pkDefStats->end() )
         iDefence = (*kDefIte).second;
      else
         iDefence = 0;


      // TEMPTEMPTEMP...this armoursystem SUXXXX!!!!
      if ( iDefence > 0 )
         iDamage -= rand()%iDefence;

      if ( iDamage < 0 )
         iDamage = 0;

      if ( iDamage > 0 )
         fTotalDamage += rand()%iDamage;
         
   }

   // lose life
   pkVictim->AddHP ( -int(fTotalDamage) );

   //cout << "Lost " << fTotalDamage << " life! " << pkVictim->GetHP() << " hp left." << endl;

   return int(fTotalDamage);

}

// -----------------------------------------------------------------------------------------------

bool LoadSpell (string kSpellName)
{
   ZFIni kIniLoader;

	string kLoadName = "/data/spells/" + kSpellName + ".zsp";

   if ( !kIniLoader.Open(kLoadName.c_str(), 0) )
   {
      cout << "Error! Couldn't find spell " << kLoadName << endl;
      return false;
   }
   else
   {
      SpellStats *pkNewSpell = new SpellStats;

      // spellname
	   if ( kIniLoader.KeyExist("spell", "name") )
		   pkNewSpell->m_kSpellName = kIniLoader.GetValue("spell", "name");
	   else
         pkNewSpell->m_kSpellName = kSpellName;

      // spellcost (mp)
	   if ( kIniLoader.KeyExist("spell", "cost") )
		   pkNewSpell->m_iCost = kIniLoader.GetIntValue("spell", "cost");
	   else
         pkNewSpell->m_iCost = 0;

      // required level (level what?!)
	   if ( kIniLoader.KeyExist("spell", "required_level") )
		   pkNewSpell->m_iRequiredLevel = kIniLoader.GetIntValue("spell", "required_level");
	   else
         pkNewSpell->m_iRequiredLevel = 0;


      // LifeTime
	   if ( kIniLoader.KeyExist("spell", "lifetime") )
		   pkNewSpell->m_fLifeTime = kIniLoader.GetFloatValue("spell", "lifetime");
	   else
         pkNewSpell->m_fLifeTime = 0;


       // Spellgroup
	   if ( kIniLoader.KeyExist("spell", "spellgroup") )
		   pkNewSpell->m_kSpellGroup = kIniLoader.GetValue("spell", "spellgroup");
	   else
         pkNewSpell->m_kSpellGroup = "none";


      // on hit object
      if ( kIniLoader.KeyExist("onhit", "event") )
         pkNewSpell->m_kOnHit[0] = kIniLoader.GetValue("onhit", "event");
      else
         pkNewSpell->m_kOnHit[0] = "nothing";

      if ( kIniLoader.KeyExist("onhit", "type") )
         pkNewSpell->m_kOnHit[1] = kIniLoader.GetValue("onhit", "type");
      else
         pkNewSpell->m_kOnHit[1] = "error!!";

      
      // cast on..
	   if ( kIniLoader.KeyExist("spell", "target") )
      {
         string kCastOn = kIniLoader.GetValue("spell", "target");
         if ( kCastOn == "ground" )
		      pkNewSpell->m_iCastOn = eON_GROUND;
         if ( kCastOn == "enemy" )
		      pkNewSpell->m_iCastOn = eON_ENEMY;
         if ( kCastOn == "friend" )
		      pkNewSpell->m_iCastOn = eON_FRIEND;
         if ( kCastOn == "caster" )
		      pkNewSpell->m_iCastOn = eON_CASTER;
         if ( kCastOn == "from caster to enemy" )
		      pkNewSpell->m_iCastOn = eFROM_CASTER_TO_ENEMY;
         if ( kCastOn == "from enemy to caster" )
		      pkNewSpell->m_iCastOn = eFROM_ENEMY_TO_CASTER;
         if ( kCastOn == "from caster to ground" )
		      pkNewSpell->m_iCastOn = eFROM_ENEMY_TO_CASTER;
      }
	   else
         pkNewSpell->m_iCastOn = eON_GROUND;

      // deal damage every X sec.
      if ( kIniLoader.KeyExist("spell", "dealdamage_every") )
         pkNewSpell->m_fDamageEvery = kIniLoader.GetFloatValue("spell", "dealdamage_every");
      else
         pkNewSpell->m_fDamageEvery = 1;
      
      
      // affect, which type of object is hit by the spell
	   if ( kIniLoader.KeyExist("spell", "affectedobjects") )
         pkNewSpell->m_kAffectedObjects = kIniLoader.GetValue("spell", "affectedobjects");
      else
         pkNewSpell->m_kAffectedObjects = "characters";


      // start and end radius for collisiondetecting
      if ( kIniLoader.KeyExist("collision", "start_radius") )
         pkNewSpell->m_fStartRadius = kIniLoader.GetFloatValue("collision", "start_radius");
      else
         pkNewSpell->m_fStartRadius = 0;

      if ( kIniLoader.KeyExist("collision", "end_radius") )
         pkNewSpell->m_fEndRadius = kIniLoader.GetFloatValue("collision", "end_radius");
      else
         pkNewSpell->m_fEndRadius = pkNewSpell->m_fStartRadius;



      // do what on destruction
	   if ( kIniLoader.KeyExist("spell", "ondestruction") )
      {
         string kStuff = kIniLoader.GetValue("spell", "ondestruction");
         if ( kStuff == "killparent" )
		      pkNewSpell->m_iOnDestruction = eKILL_PARENT;
         if ( kStuff == "killself" )
            pkNewSpell->m_iOnDestruction = eKILL_SELF;
      }
	   else
         pkNewSpell->m_iOnDestruction = eKILL_SELF;


      vector<string> kBonuses;
      unsigned int i;
      
      // load bonuses/curses

      // skills
      kBonuses.clear();
      kIniLoader.GetKeyNames ( "skillbonuses", kBonuses );
      for ( i = 0; i < kBonuses.size(); i++ )
         pkNewSpell->m_kSkillBonuses[kBonuses[i]] = kIniLoader.GetIntValue("skillbonuses", (char*)kBonuses[i].c_str());
      
      // attributes
      kBonuses.clear();
      kIniLoader.GetKeyNames ( "attributebonuses", kBonuses );
      for ( i = 0; i < kBonuses.size(); i++ )
         pkNewSpell->m_kAttributeBonuses[kBonuses[i]] = kIniLoader.GetIntValue("attributebonuses", (char*)kBonuses[i].c_str());
   
      // attack
      kBonuses.clear();
      kIniLoader.GetKeyNames ( "attackbonuses", kBonuses );
      for ( i = 0; i < kBonuses.size(); i++ )
         pkNewSpell->m_kFightBonuses.m_kAttack[kBonuses[i]] = kIniLoader.GetIntValue("attackbonuses", (char*)kBonuses[i].c_str());

      // defence
      kBonuses.clear();
      kIniLoader.GetKeyNames ( "defencebonuses", kBonuses );
      for ( i = 0; i < kBonuses.size(); i++ )
         pkNewSpell->m_kFightBonuses.m_kDefence[kBonuses[i]] = kIniLoader.GetIntValue("defencebonuses", (char*)kBonuses[i].c_str());


      // damage
      kBonuses.clear();
      kIniLoader.GetKeyNames ( "damage", kBonuses );
      for ( i = 0; i < kBonuses.size(); i++ )
         pkNewSpell->m_kDamage[kBonuses[i]] = kIniLoader.GetIntValue("damage", (char*)kBonuses[i].c_str());


      // test for 10 gfx effects
      for ( i = 0; i < 9; i++ )
      {
         char strName[5] = "gfx ";
         strName[3] = (49 + i); // ascii(49) = 1

         // gfx effects
         if ( kIniLoader.SectionExist(strName) )
         {
            GfxSpellEffect kGfx;
   
            // LifeTime
	         if ( kIniLoader.KeyExist(strName, "starttime") )
		         kGfx.m_fStartTime = kIniLoader.GetFloatValue(strName, "starttime");
	         else
               kGfx.m_fStartTime = 0;

            // PSystemName
	         if ( kIniLoader.KeyExist(strName, "psystem") )
		         kGfx.m_kPSystemName = kIniLoader.GetValue(strName, "psystem");
	         else
               kGfx.m_kPSystemName = "error";

            // OffsetX
	         if ( kIniLoader.KeyExist(strName, "offset_x") )
		         kGfx.m_kOffset.x = kIniLoader.GetFloatValue(strName, "offset_x");
	         else
               kGfx.m_kOffset.x = 0;

            // OffsetY
	         if ( kIniLoader.KeyExist(strName, "offset_y") )
		         kGfx.m_kOffset.y = kIniLoader.GetFloatValue(strName, "offset_y");
	         else
               kGfx.m_kOffset.y = 0;

            // OffsetZ
	         if ( kIniLoader.KeyExist(strName, "offset_z") )
		         kGfx.m_kOffset.z = kIniLoader.GetFloatValue(strName, "offset_z");
	         else
               kGfx.m_kOffset.z = 0;

            // attack type (target)
	         if ( kIniLoader.KeyExist(strName, "attacktype") )
            {
               string kType = kIniLoader.GetValue(strName, "attacktype");
               if ( kType == "caster" )
                  kGfx.m_iAttackType = eON_CASTER;
               if ( kType == "ground" )
                  kGfx.m_iAttackType = eON_GROUND;
               if ( kType == "enemy" )
                  kGfx.m_iAttackType = eON_ENEMY;
               if ( kType == "friend" )
                  kGfx.m_iAttackType = eON_FRIEND;
               if ( kType == "caster to enemy" )
                  kGfx.m_iAttackType = eFROM_CASTER_TO_ENEMY;
               if ( kType == "enemy to caster" )
                  kGfx.m_iAttackType = eFROM_ENEMY_TO_CASTER;
            }
	         else
               kGfx.m_iAttackType = eON_GROUND;

            pkNewSpell->m_kGraphicEffects.push_back(kGfx);

         }
      } // for i to 10

      g_kSpells[kSpellName] = pkNewSpell;

   }


   return true;
}

// -----------------------------------------------------------------------------------------------

void LoadStatTypes()
{
		ZFIni kIniLoader;

      // Load attributes
		string kLoadName = "/data/stats/attributes.zs";

		if( !kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find attributes.zs!!!" << endl;
		else
		{
			kIniLoader.GetSectionNames ( g_kAttributes );

         cout << "Attributes loaded!!" << endl;
		}

      // Load data
		kLoadName = "/data/stats/data.zs";

		if( !kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find data.zs!!!" << endl;
		else
		{
			kIniLoader.GetSectionNames ( g_kData );

         cout << "DataStats loaded!!" << endl;
		}

      // Load counters
		kLoadName = "/data/stats/counters.zs";

		if( !kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find counters.zs!!!" << endl;
		else
		{
			kIniLoader.GetSectionNames ( g_kCounters );

         cout << "Attributes loaded!!" << endl;
		}

      // Load skills
		kLoadName = "/data/stats/skills.zs";

		if( !kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find skills.zs!!!" << endl;
		else
		{
         // Get all skilltypes
			kIniLoader.GetSectionNames ( g_kSkills );

         unsigned int i;

         // loop through all skills to get attributebonuses
         for ( i = 0; i < g_kSkills.size(); i++ )
         {
            unsigned int j;

            // check if, and which group the skill belongs to
            if ( kIniLoader.KeyExist( (char*)g_kSkills[i].c_str(), "group") )
               g_kSkillGroups[ kIniLoader.GetValue( (char*)g_kSkills[i].c_str(), "group" ) ]
                  .push_back ( (char*)g_kSkills[i].c_str() );


            // loop through all attributes and search for bonuses
            for ( j = 0; j < g_kAttributes.size(); j++ )
            {
               if ( kIniLoader.KeyExist( (char*)g_kSkills[i].c_str(), (char*)g_kAttributes[j].c_str()) )
               {
                  g_kSkillExps[ g_kSkills[i] ].m_kAttributeExp[g_kAttributes[j]] =
                     kIniLoader.GetFloatValue( (char*)g_kSkills[i].c_str(), (char*)g_kAttributes[j].c_str() );
               }
            }

            // loop through all skills and search for bonuses
            for ( j = 0; j < g_kSkills.size(); j++ )
            {
               if ( kIniLoader.KeyExist( (char*)g_kSkills[i].c_str(), (char*)g_kSkills[j].c_str()) )
               {
                  g_kSkillExps[ g_kSkills[i] ].m_kSkillExp[g_kSkills[j]] =
                     kIniLoader.GetFloatValue( (char*)g_kSkills[i].c_str(), (char*)g_kSkills[j].c_str() );
               }
                
            }

         }


         // loop through all skills and see which groups they give exp. to
         for ( i = 0; i < g_kSkills.size(); i++ )
            // loop through all groups and search for bonuses
            for ( map<string, vector<string> >::iterator kIte = g_kSkillGroups.begin();
                  kIte != g_kSkillGroups.end(); kIte++)
               if ( kIniLoader.KeyExist( (char*)g_kSkills[i].c_str(), (char*)(*kIte).first.c_str() ) )
                  g_kSkillExps[ g_kSkills[i] ].m_kGroupExp[(char*)(*kIte).first.c_str()] =
                     kIniLoader.GetFloatValue( (char*)g_kSkills[i].c_str(), (char*)(*kIte).first.c_str() );
      
         cout << "Skills loaded!!" << endl;
		}

}

// -----------------------------------------------------------------------------------------------
