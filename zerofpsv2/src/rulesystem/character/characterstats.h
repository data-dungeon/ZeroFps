#ifndef _RULESYSTEM_CHARACTERSTATS_H_
#define _RULESYSTEM_CHARACTERSTATS_H_

#pragma warning (disable :4786)

//#include "../rulesystemx.h"
#include "../../mcommon/mcommon_x.h"
#include "../rulesystem.h"
#include "statcounter.h"

#include <string>
#include <map>
#include <vector>
#include "../../zerofpsv2/basic/zfini.h"
	using namespace std;

struct StatDescriber
{
	int m_iValue;
	float m_fExp;
};

extern MCOMMON_API map<string, SkillType> g_kSkillExps;
extern MCOMMON_API vector<string> g_kSkills;
extern MCOMMON_API vector<string> g_kAttributes;
extern MCOMMON_API vector<string> g_kData;

class MCOMMON_API CharacterStats
{
private:

	map<string, StatDescriber> m_kSkills;		// swordskill, picklock...
	map<string, StatDescriber> m_kAttributes; // sty, smi...
	map<string, StatCounter> m_kPointStats;   // mp, hp...
	map<string, string> m_kData; // name, rase, sex

   void RecieveSkillExp ( StatDescriber *pkStat, float fDifficulty, string kName );
   void RecieveAttrExp ( StatDescriber *pkStat, float fDifficulty );

   void TestLevelUp ( StatDescriber *pkStat );

   string m_kCurrentSkill;
public:
	CharacterStats();

   // Get stat.
	int GetSkillValue				(string kName);
	int GetAttributeValue		(string kName);
	int GetPointStatValue		(string kName);
	string GetCommonStatValue  (string kName);

   // hp/mp stuff
   float GetHP()                                   { return m_kPointStats["hp"].Value(); }
   float GetMP()                                   { return m_kPointStats["mp"].Value(); }

   void SetHP( string kValue );
   void SetMP( string kValue );
   
   void AddHP( int iValue );
   void AddMP( int iValue );

   float GetHPPercent()                            { return m_kPointStats["hp"].Percent(); }
   float GetMPPercent()                            { return m_kPointStats["mp"].Percent(); }

   string GetCurrentSkill()                        { return m_kCurrentSkill; }
   bool SetCurrentSkill ( string kNewCurrentSkill );

   // Add points to stats.
   void AddSkillValue ( string kSkillName, int iValue );
   void AddAttributeValue ( string kAttributeName, int iValue );

	// Set stat.
   void SetSkill		 (string kName, int fStartValue);
	void SetAttribute  (string kName, int fStartValue);
	void SetData (string kName, string kStartValue);

   // Set stat. exp
   void SetSkillExp (string kName, float fExp);
   void SetAttributeExp (string kName, float fExp);

   // Test skill (returns failed/sucessed)
   bool RollSkillDice ( string kSkill, float fDifficulty);
   // Test attribute (returns failed/sucessed)
   bool RollAttributeDice ( string kAttribute, float fDifficulty);

	void SetCharacterType( string kName );

	vector<string>* GetSkills()								{ return &g_kSkills; }
	vector<string>* GetAttributes()							{ return &g_kAttributes; } 
	vector<string>* GetDataStats()							{ return &g_kData; }

   void Print();

};

#endif