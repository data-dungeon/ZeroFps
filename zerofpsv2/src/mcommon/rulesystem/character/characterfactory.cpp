#include "characterfactory.h"
#include <iostream>
	using namespace std;


// ------------------------------------------------------------------------------------------

CharacterStats* CharacterFactory::GetCharacterData(string kCharacterName)
{
	// if stat-types isn't loaded
	if ( !g_kSkills.size() && !g_kAttributes.size() && !g_kData.size() )
      LoadStatTypes();


	map<string, CharacterStats>::iterator kIte = g_kCharacters.find(kCharacterName);

	// try to load charactertype from file
	if ( kIte == g_kCharacters.end() )
	{

		ZFIni m_kIniLoader;

		string kLoadName = "/data/characters/" + kCharacterName + ".zch";

		// Open file
		if( !m_kIniLoader.Open(kLoadName.c_str(), 0) )		
		{
			cout << "Error! Failed to find charactertype:" << kCharacterName << endl;
			return 0;
		}

		// See if the file really contains a character
		if( !m_kIniLoader.SectionExist("character") )
		{
			cout << "This file doesn't contain a charactertype!!" << endl;
			return false;
		}

		
		int i;

		// load skills
      vector<string> *pkData = &g_kSkills;

      for ( i = 0; i < pkData->size(); i++ )
		{
			if( m_kIniLoader.KeyExist("skills", (char*)pkData->at(i).c_str() ) )
         {
            // set value
				g_kCharacters[kCharacterName].SetSkill(  (char*)pkData->at(i).c_str(), 
					m_kIniLoader.GetIntValue("skills", (char*)pkData->at(i).c_str() )  );
            // set exp
            g_kCharacters[kCharacterName].SetSkillExp(  (char*)pkData->at(i).c_str(), 0 );
         }
			else
				g_kCharacters[kCharacterName].SetSkill((char*)pkData->at(i).c_str(), 0);
		}


      // load attributes
      pkData = &g_kAttributes;

		for ( i = 0; i < pkData->size(); i++ )
		{
			if( m_kIniLoader.KeyExist("attributes", (char*)pkData->at(i).c_str() ) )
         {
            // set value
				g_kCharacters[kCharacterName].SetAttribute(  (char*)pkData->at(i).c_str(), 
					m_kIniLoader.GetIntValue("attributes", (char*)pkData->at(i).c_str() )  );
            // set exp
               g_kCharacters[kCharacterName].SetAttributeExp(  (char*)pkData->at(i).c_str(), 0 );
         }
			else
				g_kCharacters[kCharacterName].SetAttribute((char*)pkData->at(i).c_str(), 0);
		}

      // load character data (name, sex...)
      pkData = &g_kData;

		for ( i = 0; i < pkData->size(); i++ )
		{
			if( m_kIniLoader.KeyExist("data", (char*)pkData->at(i).c_str() ) )
				g_kCharacters[kCharacterName].SetData(  (char*)pkData->at(i).c_str(), 
					m_kIniLoader.GetValue("data", (char*)pkData->at(i).c_str() )  );
			else
				g_kCharacters[kCharacterName].SetData((char*)pkData->at(i).c_str(), "none");
		}
	}

   CharacterStats *pkNewStats = new CharacterStats;

	*pkNewStats = g_kCharacters[kCharacterName];

	return pkNewStats;
}

// ------------------------------------------------------------------------------------------

void CharacterFactory::LoadStatTypes()
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
	/*	kLoadName = "/data/stats/counters.zs";

		if( !m_kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find counters.zs!!!" << endl;
		else
		{
			m_kIniLoader.GetSectionNames ( g_kData );

         cout << "Attributes loaded!!" << endl;
		}*/

      // Load skills
		kLoadName = "/data/stats/skills.zs";

		if( !m_kIniLoader.Open(kLoadName.c_str(), 0) )		
			cout << "Error! Couldn't find skills.zs!!!" << endl;
		else
		{
         // Get all skilltypes
			m_kIniLoader.GetSectionNames ( g_kSkills );

         // loop through all skills to get attributebonuses
         for ( int i = 0; i < g_kSkills.size(); i++ )
         {
            int j;

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

         cout << "Skills loaded!!" << endl;
		}

}

// ------------------------------------------------------------------------------------------
