#include "characterstats.h"
#include <iostream>
#include <string>
	using namespace std;

// ------------------------------------------------------------------------------------------

CharacterStats::CharacterStats()
{ 
}

// ------------------------------------------------------------------------------------------

int CharacterStats::GetSkillValue (string kName)
{
	map<string, StatDescriber>::iterator kIte = m_kSkills.find(kName);

	if ( kIte == m_kSkills.end() )
		return -1;
	else
		return m_kSkills[kName].m_iValue;
}

// ------------------------------------------------------------------------------------------

int CharacterStats::GetAttributeValue (string kName)
{
	map<string, StatDescriber>::iterator kIte = m_kAttributes.find(kName);

	if ( kIte == m_kAttributes.end() )
		return -1;
	else
		return m_kAttributes[kName].m_iValue;
}

// ------------------------------------------------------------------------------------------

int CharacterStats::GetPointStatValue (string kName)
{
	map<string, StatDescriber>::iterator kIte = m_kAttributes.find(kName);

	if ( kIte == m_kAttributes.end() )
		return -1;
	else
		return m_kAttributes[kName].m_iValue;
}

// ------------------------------------------------------------------------------------------

string CharacterStats::GetCommonStatValue (string kName)
{
	map<string, string>::iterator kIte =  m_kData.find(kName);

	if ( kIte ==  m_kData.end() )
		return "error!";
	else
		return m_kData[kName];
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetSkill (string kName, int iStartValue)
{
	m_kSkills[kName].m_iValue = iStartValue;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetAttribute (string kName, int iStartValue)
{
	m_kAttributes[kName].m_iValue = iStartValue;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetData (string kName, string kStartValue)
{
	m_kData[kName] = kStartValue;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetSkillExp (string kName, float fExp)
{
   map<string, StatDescriber>::iterator kIte = m_kSkills.find(kName);

   if ( kIte != m_kSkills.end() )
      m_kSkills[kName].m_fExp = fExp;
   else
      cout << "Warning! Couln't find skill " << kName << " (tried to set exp. value)" << endl;

}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetAttributeExp (string kName, float fExp)
{
   map<string, StatDescriber>::iterator kIte = m_kAttributes.find(kName);

   if ( kIte != m_kAttributes.end() )
      m_kAttributes[kName].m_fExp = fExp;
   else
      cout << "Warning! Couln't find attribute " << kName << " (tried to set exp. value)" << endl;
}

// ------------------------------------------------------------------------------------------

bool CharacterStats::RollSkillDice ( string kSkill, float fDifficulty )
{

   map<string, StatDescriber>::iterator kIte = m_kSkills.find(kSkill);

   if ( kIte != m_kSkills.end() )
   {
      int iRoll = rand()%m_kSkills[kSkill].m_iValue;

      if ( iRoll <= m_kSkills[kSkill].m_iValue - fDifficulty )
      {
         RecieveSkillExp ( &m_kSkills[kSkill], fDifficulty, kSkill );
         return true;
      }
      else
         return false;
   }
   else
      return false;

}

// ------------------------------------------------------------------------------------------

bool CharacterStats::RollAttributeDice ( string kAttribute, float fDifficulty )
{
   map<string, StatDescriber>::iterator kIte = m_kAttributes.find(kAttribute);

   if ( kIte != m_kAttributes.end() )
   {
      int iRoll = rand()%m_kAttributes[kAttribute].m_iValue;

      if ( iRoll <= m_kAttributes[kAttribute].m_iValue - fDifficulty )
      {
         RecieveAttrExp ( &m_kAttributes[kAttribute], fDifficulty / float(m_kAttributes[kAttribute].m_iValue) );
         return true;
      }
      else
         return false;
   }
   else
      return false;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::RecieveSkillExp ( StatDescriber *pkStat, float fDifficulty, string kName )
{
   // TODO: add difficulty to bonuses....somehow :)

   pkStat->m_fExp += fDifficulty / float(pkStat->m_iValue);

   map<string, float>::iterator kIte;

   // attributeexp. bonuses
   for ( kIte = g_kSkillExps[kName].m_kAttributeExp.begin();
         kIte != g_kSkillExps[kName].m_kAttributeExp.end(); kIte++ )
   {
      RecieveAttrExp ( &m_kAttributes[(*kIte).first], (*kIte).second );
   }

   // skillexp. bonuses
   for ( kIte = g_kSkillExps[kName].m_kSkillExp.begin();
         kIte != g_kSkillExps[kName].m_kSkillExp.end(); kIte++ )
   {
      m_kSkills [ (*kIte).first ].m_fExp += (*kIte).second;
      TestLevelUp ( &m_kSkills [ (*kIte).first ] );
   }

   TestLevelUp ( pkStat );

   // Levelup?
   if ( pkStat->m_fExp > 10 )
   {
      pkStat->m_iValue++;
      pkStat->m_fExp -= 10;
   }
}

// ------------------------------------------------------------------------------------------

void CharacterStats::RecieveAttrExp ( StatDescriber *pkStat, float fExp )
{
   pkStat->m_fExp += fExp;
   TestLevelUp ( pkStat );
}

// ------------------------------------------------------------------------------------------

void CharacterStats::TestLevelUp ( StatDescriber *pkStat )
{
   // Levelup?
   if ( pkStat->m_fExp > 10 )
   {
      pkStat->m_iValue++;
      pkStat->m_fExp -= 10;
   }

}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddSkillValue ( string kSkillName, int iValue )
{
   m_kSkills[kSkillName].m_iValue += iValue;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddAttributeValue ( string kAttributeName, int iValue )
{
   m_kAttributes[kAttributeName].m_iValue += iValue;
}

// ------------------------------------------------------------------------------------------

bool CharacterStats::SetCurrentSkill ( string kNewCurrentSkill )
{
   map<string, StatDescriber>::iterator kIte = m_kSkills.find(kNewCurrentSkill);

   if ( kIte != m_kSkills.end() )
   {
      m_kCurrentSkill = kNewCurrentSkill;
      return true;
   }
   else
   {
      cout << "Warning! Tried to select a unknown skill!" << endl;
      return false;
   }
}

// ------------------------------------------------------------------------------------------

void CharacterStats::Print()
{
   // print data
   cout << endl << "Data:" << endl;
   cout << "-----" << endl;

   for ( map<string, string>::iterator kIte = m_kData.begin(); kIte != m_kData.end(); kIte++ )
      cout << (*kIte).first << ":" << (*kIte).second << endl;

   // Print attributes
   cout << endl << "Attributes:" << endl;
   cout << "-----------" << endl;
   for ( map<string, StatDescriber>::iterator kAttIte = m_kAttributes.begin(); 
         kAttIte != m_kAttributes.end(); kAttIte++ )
      cout << (*kAttIte).first << ":" << (*kAttIte).second.m_iValue << endl;

  
   // print skills
   cout << endl << "Skills:" << endl;
   cout << "-------" << endl;
   for ( map<string, StatDescriber>::iterator kSklIte = m_kSkills.begin(); 
         kSklIte != m_kSkills.end(); kSklIte++ )
      cout << (*kSklIte).first << ":" << (*kSklIte).second.m_iValue << endl;


}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetHP( string kValue )
{
   m_kPointStats["hp"] = kValue;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetMP( string kValue )
{
   m_kPointStats["hp"] = kValue;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddHP( int iValue )
{
   m_kPointStats["hp"] += iValue;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddMP( int iValue )
{
   m_kPointStats["mp"] += iValue;
}

// ------------------------------------------------------------------------------------------


// temp rulez:

// diceroll = Dskillvalue, ska vara <= skill-difficulty
// exp = difficulty / value
// levelup = 10exp








