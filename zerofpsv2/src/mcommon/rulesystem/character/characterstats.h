#ifndef _RULESYSTEM_CHARACTERSTATS_H_
#define _RULESYSTEM_CHARACTERSTATS_H_

#pragma warning (disable :4786)

//#include "../rulesystemx.h"
#include "../../mcommon_x.h"
#include "../rulesystem.h"
#include "statcounter.h"
#include "../fightstats.h"
#include "../../p_serverinfo.h"

#include <string>
#include <map>
#include <vector>
#include "../../../zerofpsv2/basic/zfini.h"
#include "../../../zerofpsv2/engine/entity.h"

	using namespace std;

class ItemStats;

class MCOMMON_API StatDescriber
{
public:
	int m_iValue;
	float m_fExp;


   StatDescriber() 
	{ 
		m_iValue = 0;
		m_fExp = 0; 
	}

};

extern MCOMMON_API map<string, SkillType> g_kSkillExps;
extern MCOMMON_API vector<string> g_kSkills;
extern MCOMMON_API vector<string> g_kAttributes;
extern MCOMMON_API vector<string> g_kData;
extern MCOMMON_API vector<string> g_kCounters;
extern MCOMMON_API map<string, vector<string> > g_kSkillGroups;

class MCOMMON_API CharacterStats
{
private:

	map<string, StatDescriber> m_kSkills;		// swordskill, picklock...
	map<string, StatDescriber> m_kAttributes; // sty, smi...
	map<string, StatCounter> m_kPointStats;   // mp, hp...
	map<string, string> m_kData; // name, race, sex

   map<string, StatDescriber> m_kSpells;

   map<string, Entity*> m_kEquipment;

   Entity *m_pkParent;

   FightStats m_kFightStats;

   P_ServerInfo* m_pkServInf;

   
   bool m_bIsPlayer;

   void RecieveSkillExp ( StatDescriber *pkStat, float fDifficulty, string kName );
   void RecieveAttrExp ( StatDescriber *pkStat, float fDifficulty );

   void TestLevelUp ( StatDescriber *pkStat );

   string m_kCurrentSkill;

	Vector3 m_kRecalPos;

   float 
      m_fMoveSpeed,
      m_fReloadTime,  // time before next action (attackspeed, shootspeed..etc)
      m_fReloadTimer; // how long time has passed before last used action

public:
   float
      m_fHearing,     // How good the character can hear
      m_fVision,      // How good the character can see
      m_fVisibility,  // How easy it is to see the character
      m_fLoudness;    // How much noise the character make

   string 
      m_strPrimSkill, // attack skill
      m_strSecSkill;  // choosable skill

   bool
      m_bRider,       // is riding something
      m_bMounted;     // has a rider


   // call this when the character has attacked, cast spell or such
   void ResetActionTimer ()                { m_fReloadTimer = m_fReloadTime; }
   void SetReloadTime ( float fTime )      { m_fReloadTime = fTime; }
   void AddReloadTime ( float fTime )      { m_fReloadTime += fTime; }
   bool ReadyForAction ()                  { return m_fReloadTimer <= 0; }

   void SetMoveSpeed (float fValue);
   void AddMoveSpeed (float fValue);

   // which script is to be run when the player is hit
   string m_strScriptWhenHit;
	
   unsigned int m_uiVersion; // which version of char the prop. holds. Server always have the latest.

	CharacterStats( Entity *pkParent );

   FightStats* GetFightStats()             { return &m_kFightStats; }

   // Get stat.
	int GetSkillValue				(string kName);
	int GetAttributeValue		(string kName);
	int GetPointStatValue		(string kName);
	string GetCommonStatValue  (string kName);

   // hp/mp stuff
   int GetHP()                                   { return m_kPointStats["hp"].Value(); }
   int GetMP()                                   { return m_kPointStats["mp"].Value(); }

   void SetHP( string kValue );
   void SetMP( string kValue );

   void SetMaxHP ( int iValue )                    { m_kPointStats["hp"].SetMaxValue(iValue); }
   void SetMaxMP ( int iValue )                    { m_kPointStats["mp"].SetMaxValue(iValue); }
   
   void AddHP( int iValue );
   void AddMP( int iValue );

   float GetHPPercent()                            { return m_kPointStats["hp"].Percent(); }
   float GetMPPercent()                            { return m_kPointStats["mp"].Percent(); }

   string GetCurrentSkill()                        { return m_kCurrentSkill; }
   bool SetCurrentSkill ( string kNewCurrentSkill );

	void SetRecalPos(Vector3 kPos) { m_kRecalPos = kPos;};
	Vector3 GetRecalPos() { return m_kRecalPos;};	

   // Add points to stats.
   void AddSkillValue ( string kSkillName, int iValue );
   void AddAttributeValue ( string kAttributeName, int iValue );
   void AddAttackValue ( string kAttackType, int iValue );
   void AddDefenceValue ( string kDefenceType, int iValue );

	// Set stat.
   void SetSkill		 (string kName, int fStartValue);
	void SetAttribute  (string kName, int fStartValue);
	void SetData (string kName, string kStartValue);
   void SetAttackValue (string kAttackType, int iValue);
   void SetDefenceValue (string kDefenceType, int iValue);

   // Set stat. exp
   void SetSkillExp (string kName, float fExp);
   void SetAttributeExp (string kName, float fExp);

   // Test skill (returns failed/sucessed)
   bool RollSkillDice ( string kSkill, float fDifficulty);
   // Test attribute (returns failed/sucessed)
   bool RollAttributeDice ( string kAttribute, float fDifficulty);

	void SetCharacterType( string kName );

   void Print();

   void SetCounter( string kName, int iValue );

   map<string, Entity*>* GetEquippedList()           { return &m_kEquipment; }
   
   bool Equip ( Entity *pkObject, string kSlot );
   bool Equip ( Entity *pkObject, int iSlot );
   Entity* UnEquip ( string kSlot );

   void GetSkills (map<string, StatDescriber>& pkSkillList)      { pkSkillList = m_kSkills; }
   void GetAttributes (map<string, StatDescriber>& pkAttrList)   { pkAttrList = m_kAttributes; }
   void GetData (map<string, string>& pkData)                    { pkData = m_kData; }


   friend class CharacterProperty;
};

#endif








