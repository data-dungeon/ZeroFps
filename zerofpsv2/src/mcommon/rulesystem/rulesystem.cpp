#include "rulesystem.h"
#include "character/characterstats.h"
#include <iostream>
   using namespace std;

MCOMMON_API map<string, CharacterStats> g_kCharacters;
MCOMMON_API vector<string> g_kSkills;
MCOMMON_API vector<string> g_kAttributes;
MCOMMON_API vector<string> g_kData;
MCOMMON_API vector<string> g_kCounters;
MCOMMON_API map<string, SkillType> g_kSkillExps;
MCOMMON_API map<string, vector<string> > g_kSkillGroups;


int DealDamage ( FightStats *pkAttacker, CharacterStats *pkVictim )
{
   float fTotalDmg = 0;

   // räkna ut damage på nån vänster...


   return int(fTotalDmg);
}



void LoadStatTypes()
{
		ZFIni m_kIniLoader;

      // Load attributes
		string kLoadName = "/data/stats/attributes.zs";

		if( !m_kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find attributes.zs!!!" << endl;
		else
		{
			m_kIniLoader.GetSectionNames ( g_kAttributes );

         cout << "Attributes loaded!!" << endl;
		}

      // Load data
		kLoadName = "/data/stats/data.zs";

		if( !m_kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find data.zs!!!" << endl;
		else
		{
			m_kIniLoader.GetSectionNames ( g_kData );

         cout << "DataStats loaded!!" << endl;
		}

      // Load counters
		kLoadName = "/data/stats/counters.zs";

		if( !m_kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find counters.zs!!!" << endl;
		else
		{
			m_kIniLoader.GetSectionNames ( g_kCounters );

         cout << "Attributes loaded!!" << endl;
		}

      // Load skills
		kLoadName = "/data/stats/skills.zs";

		if( !m_kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find skills.zs!!!" << endl;
		else
		{
         // Get all skilltypes
			m_kIniLoader.GetSectionNames ( g_kSkills );

         unsigned int i;

         // loop through all skills to get attributebonuses
         for ( i = 0; i < g_kSkills.size(); i++ )
         {
            unsigned int j;

            // check if, and which group the skill belongs to
            if ( m_kIniLoader.KeyExist( (char*)g_kSkills[i].c_str(), "group") )
               g_kSkillGroups[ m_kIniLoader.GetValue( (char*)g_kSkills[i].c_str(), "group" ) ]
                  .push_back ( (char*)g_kSkills[i].c_str() );


            // loop through all attributes and search for bonuses
            for ( j = 0; j < g_kAttributes.size(); j++ )
            {
               if ( m_kIniLoader.KeyExist( (char*)g_kSkills[i].c_str(), (char*)g_kAttributes[j].c_str()) )
               {
                  g_kSkillExps[ g_kSkills[i] ].m_kAttributeExp[g_kAttributes[j]] =
                     m_kIniLoader.GetFloatValue( (char*)g_kSkills[i].c_str(), (char*)g_kAttributes[j].c_str() );
               }
            }

            // loop through all skills and search for bonuses
            for ( j = 0; j < g_kSkills.size(); j++ )
            {
               if ( m_kIniLoader.KeyExist( (char*)g_kSkills[i].c_str(), (char*)g_kSkills[j].c_str()) )
               {
                  g_kSkillExps[ g_kSkills[i] ].m_kSkillExp[g_kSkills[j]] =
                     m_kIniLoader.GetFloatValue( (char*)g_kSkills[i].c_str(), (char*)g_kSkills[j].c_str() );
               }
                
            }

         }


         // loop through all skills and see which groups they give exp. to
         for ( i = 0; i < g_kSkills.size(); i++ )
            // loop through all groups and search for bonuses
            for ( map<string, vector<string> >::iterator kIte = g_kSkillGroups.begin();
                  kIte != g_kSkillGroups.end(); kIte++)
               if ( m_kIniLoader.KeyExist( (char*)g_kSkills[i].c_str(), (char*)(*kIte).first.c_str() ) )
                  g_kSkillExps[ g_kSkills[i] ].m_kGroupExp[(char*)(*kIte).first.c_str()] =
                     m_kIniLoader.GetFloatValue( (char*)g_kSkills[i].c_str(), (char*)(*kIte).first.c_str() );
      
         cout << "Skills loaded!!" << endl;
		}

}
