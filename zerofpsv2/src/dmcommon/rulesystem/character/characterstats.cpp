#include "../../p_item.h"
//#include "../../p_event.h"
#include "../../../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"
#include "../../../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../../../zerofpsv2/engine_systems/propertys/p_linktojoint.h"
#include "../../../zerofpsv2/engine/p_pfpath.h"
#include "characterstats.h"
#include "../../p_charstats.h"
#include "../item/itemstats.h"
#include <iostream>
#include <string>
	using namespace std;

// ------------------------------------------------------------------------------------------

CharacterStats::CharacterStats( Entity *pkParent )
{
	m_kRecalPos.Set(0,0,0);
	
   m_uiVersion = 0;

   m_fHearing = 3;
   m_fVision = 3;
   m_fVisibility = 1;
   m_fLoudness = 1;

   m_pkParent = pkParent;

   //SetMoveSpeed (5);

	m_strScriptWhenHit = "";

   m_bIsPlayer = m_pkParent->GetType() == "t_player.lua";

   // only on server
   if ( m_pkParent->m_pkZeroFps->m_bServerMode )
   {
      // find serverInfoObject
      Entity* pkEnt = m_pkParent->m_pkEntityManager->GetEntityByType("t_serverinfo.lua");

      if ( !pkEnt )
      {
         cout << "ERROR!!! CharStats couln't find ServerInfoObject!!! :(" << endl;
         return;
      }

      m_pkServInf = (P_ServerInfo*)pkEnt->GetProperty("P_ServerInfo");

   }
   else 
      m_pkServInf = 0;

	// if stat-types isn't loaded
	if ( !g_kSkills.size() && !g_kAttributes.size() && !g_kData.size() )
      LoadStatTypes();
}

// ------------------------------------------------------------------------------------------

int CharacterStats::GetSkillValue (string kName)
{
	map<string, StatDescriber>::iterator kIte = m_kSkills.find(kName);

	if ( kIte == m_kSkills.end() )
		return 0;
	else
		return m_kSkills[kName].m_iValue;
}

// ------------------------------------------------------------------------------------------

int CharacterStats::GetAttributeValue (string kName)
{
	map<string, StatDescriber>::iterator kIte = m_kAttributes.find(kName);

	if ( kIte == m_kAttributes.end() )
		return 0;
	else
		return m_kAttributes[kName].m_iValue;
}

// ------------------------------------------------------------------------------------------

int CharacterStats::GetPointStatValue (string kName)
{
	map<string, StatDescriber>::iterator kIte = m_kAttributes.find(kName);

	if ( kIte == m_kAttributes.end() )
		return 0;
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
   // check if the skill exists
   for ( unsigned int i = 0; i < g_kSkills.size(); i++ )
      if ( g_kSkills[i] == kName )
      {
	      m_kSkills[kName].m_iValue = iStartValue;
         m_kSkills[kName].m_fExp = 0;

         m_uiVersion++;
      }
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetAttribute (string kName, int iStartValue)
{

//	printf("kName = %s = %i\n", kName.c_str() , iStartValue);

      // check if the skill exists
   for ( unsigned int i = 0; i < g_kAttributes.size(); i++ )
      if ( g_kAttributes[i] == kName )
      {
   	   m_kAttributes[kName].m_iValue = iStartValue;
         m_kAttributes[kName].m_fExp = 0;

         m_uiVersion++;
      }
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetData (string kName, string kStartValue)
{
   for ( unsigned int i = 0; i < g_kData.size(); i++ )
      if ( g_kData[i] == kName )
	      m_kData[kName] = kStartValue;

    m_uiVersion++;

}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetSkillExp (string kName, float fExp)
{
   map<string, StatDescriber>::iterator kIte = m_kSkills.find(kName);

   if ( kIte != m_kSkills.end() )
   {
      m_kSkills[kName].m_fExp = fExp;
      m_uiVersion++;
   }
   else
      cout << "Warning! Couln't find skill " << kName << " (tried to set exp. value)" << endl;

}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetAttributeExp (string kName, float fExp)
{
   map<string, StatDescriber>::iterator kIte = m_kAttributes.find(kName);

   if ( kIte != m_kAttributes.end() )
   {
      m_kAttributes[kName].m_fExp = fExp;
      m_uiVersion++;
   }
   else
      cout << "Warning! Couln't find attribute " << kName << " (tried to set exp. value)" << endl;
}

// ------------------------------------------------------------------------------------------

bool CharacterStats::RollSkillDice ( string kSkill, float fDifficulty )
{

   map<string, StatDescriber>::iterator kIte = m_kSkills.find(kSkill);

   if ( kIte != m_kSkills.end() )
   {
      if ( m_kSkills[kSkill].m_iValue < 1 )
         return false;

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

   m_uiVersion++;

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


   // recieve group exps
   for ( kIte = g_kSkillExps[kName].m_kGroupExp.begin();
         kIte != g_kSkillExps[kName].m_kGroupExp.end(); kIte++ )
      for ( unsigned int i = 0; i < g_kSkillGroups[(*kIte).first].size(); i++ )
      {
         m_kSkills[ g_kSkillGroups[(*kIte).first].at(i) ].m_fExp += (*kIte).second;
         TestLevelUp ( &m_kSkills[ g_kSkillGroups[(*kIte).first].at(i) ] );
      }


   TestLevelUp ( pkStat );

}

// ------------------------------------------------------------------------------------------

void CharacterStats::RecieveAttrExp ( StatDescriber *pkStat, float fExp )
{
   m_uiVersion++;

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

      cout << "LevelUp!!" << endl;

      Print();
   }

}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddSkillValue ( string kSkillName, int iValue )
{
   m_uiVersion++;
   m_kSkills[kSkillName].m_iValue += iValue;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddAttributeValue ( string kAttributeName, int iValue )
{
   m_kAttributes[kAttributeName].m_iValue += iValue;
   m_uiVersion++;
}

// ------------------------------------------------------------------------------------------

bool CharacterStats::SetCurrentSkill ( string kNewCurrentSkill )
{
   map<string, StatDescriber>::iterator kIte = m_kSkills.find(kNewCurrentSkill);

   if ( kIte != m_kSkills.end() )
   {
      m_kCurrentSkill = kNewCurrentSkill;
      m_uiVersion++;
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
   cout << "-o~O~o--o~O~o--o~O~o--o~O~o--o~O~o--o~O~o--o~O~o--o~O~o--o~O~o-" << endl;

   // print data
   cout << endl << "Data:" << endl;
   cout << "-----" << endl;

   for ( map<string, string>::iterator kIte = m_kData.begin(); kIte != m_kData.end(); kIte++ )
      cout << (*kIte).first << ":" << (*kIte).second << endl;

   // print hp/mp
   cout << endl << "Stuff:" << endl;
   cout << "-----------" << endl;
   for ( map<string, StatCounter>::iterator kCounterIte = m_kPointStats.begin(); 
         kCounterIte != m_kPointStats.end(); kCounterIte++ )
   {
      cout << (*kCounterIte).first << ":" << (*kCounterIte).second.Value() << "/";
      cout << (*kCounterIte).second.Max2() << endl;
   }


   // Print attributes
   cout << endl << "Attributes:" << endl;
   cout << "-----------" << endl;
   for ( map<string, StatDescriber>::iterator kAttIte = m_kAttributes.begin(); 
         kAttIte != m_kAttributes.end(); kAttIte++ )
   {
      cout << (*kAttIte).first << ":" << (*kAttIte).second.m_iValue;
      cout << "  exp:" << (*kAttIte).second.m_fExp << endl;
   }

  
   // print skills
   cout << endl << "Skills:" << endl;
   cout << "-------" << endl;
   for ( map<string, StatDescriber>::iterator kSklIte = m_kSkills.begin(); 
         kSklIte != m_kSkills.end(); kSklIte++ )
   {
      cout << (*kSklIte).first << ":" << (*kSklIte).second.m_iValue;
      cout << "  exp:" << (*kSklIte).second.m_fExp << endl;
   }

   // Print attack values
   cout << endl << "Attack:" << endl;
   cout << "-------" << endl;

   for ( map<string, int>::iterator kAtt = m_kFightStats.m_kAttack.begin(); 
         kAtt != m_kFightStats.m_kAttack.end(); kAtt++ )
      cout << (*kAtt).first << ":" << (*kAtt).second << endl;


   // print defence values
   cout << endl << "Defence:" << endl;
   cout << "-------" << endl;

   for ( map<string, int>::iterator kDef = m_kFightStats.m_kDefence.begin(); 
         kDef != m_kFightStats.m_kDefence.end(); kDef++ )
      cout << (*kDef).first << ":" << (*kDef).second << endl;

   cout << endl << "-o~O~o--o~O~o--o~O~o--o~O~o--o~O~o--o~O~o--o~O~o--o~O~o--o~O~o-" << endl;

}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetHP( string kValue )
{
   m_kPointStats["hp"] = kValue;
   m_uiVersion++;

   // if character is player, send updated life to client
   if ( m_bIsPlayer && m_pkServInf )
   {
      for ( unsigned int i = 0; i < m_pkServInf->GetPlayers()->size(); i++ )
         for ( unsigned int j = 0; j < m_pkServInf->GetPlayers()->at(i).kControl.size(); j++ )
            if (m_pkServInf->GetPlayers()->at(i).kControl[j].first == m_pkParent->GetEntityID())
            {
               SendType kNewSend;

               kNewSend.m_iClientID = m_pkServInf->GetPlayers()->at(i).iId;
               kNewSend.m_kSendType = "hp";
               ((CharacterProperty*)m_pkParent->GetProperty("P_CharStats"))->AddSendsData (kNewSend);
            }
   }
   
   if(m_kPointStats["hp"].Value() <= 0)
   {
		m_pkParent->m_pkEntityManager->CallFunction(m_pkParent,"OnDeath",NULL);
   	/*P_ScriptInterface* pe = (P_ScriptInterface*)m_pkParent->GetProperty("P_ScriptInterface");
   
   	if(pe)
   	{
   		pe->SendEvent("OnDeath");
   	}*/
   }   
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetMP( string kValue )
{
   m_kPointStats["mp"] = kValue;
   m_uiVersion++;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddHP( int iValue )
{
	
   m_kPointStats["hp"] += iValue;
   m_uiVersion++;

   // if character is player, send updated life to client
   if ( m_bIsPlayer && m_pkServInf )
   {
      for ( unsigned int i = 0; i < m_pkServInf->GetPlayers()->size(); i++ )
         for ( unsigned int j = 0; j < m_pkServInf->GetPlayers()->at(i).kControl.size(); j++ )
            if (m_pkServInf->GetPlayers()->at(i).kControl[j].first == m_pkParent->GetEntityID())
            {
               SendType kNewSend;

               kNewSend.m_iClientID = m_pkServInf->GetPlayers()->at(i).iId;
               kNewSend.m_kSendType = "hp";
               ((CharacterProperty*)m_pkParent->GetProperty("P_CharStats"))->AddSendsData (kNewSend);
            }
   }
   
   if(m_kPointStats["hp"].Value() <= 0)
   {
		m_pkParent->m_pkEntityManager->CallFunction(m_pkParent,"OnDeath",NULL);
   	/*P_ScriptInterface* pe = (P_ScriptInterface*)m_pkParent->GetProperty("P_ScriptInterface");
   
   	if(pe)
   	{
   		pe->SendEvent("OnDeath");
   	}*/
   }
}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddMP( int iValue )
{ 
   m_kPointStats["mp"] += iValue;
   m_uiVersion++; 
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetCounter( string kName, int iValue )
{
   m_kPointStats[kName] = iValue;
   m_kPointStats[kName].SetMaxValue(iValue);
   m_uiVersion++; 
}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddAttackValue ( string kAttackType, int iValue )
{
   m_kFightStats.m_kAttack[kAttackType] += iValue;

   if ( m_kFightStats.m_kAttack[kAttackType] < 0 )
      m_kFightStats.m_kAttack[kAttackType] = 0;

   m_uiVersion++;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddDefenceValue ( string kDefenceType, int iValue )
{
   m_kFightStats.m_kDefence[kDefenceType] += iValue;

   if ( m_kFightStats.m_kDefence[kDefenceType] < 0 )
      m_kFightStats.m_kDefence[kDefenceType] = 0;

   m_uiVersion++;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetAttackValue (string kAttackType, int iValue)
{
   m_kFightStats.m_kAttack[kAttackType] = iValue;

   m_uiVersion++;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetDefenceValue (string kDefenceType, int iValue)
{
   m_kFightStats.m_kDefence[kDefenceType] = iValue;
   
   m_uiVersion++;
}

// ------------------------------------------------------------------------------------------

bool CharacterStats::Equip ( Entity *pkObject, int iSlot )
{
   switch (iSlot)
   {
      case Item:
         break;
      case Armour:
         return Equip ( pkObject, "chest" );
      case Cloak:
         break;
      case Amulett:
         break;
      case Shield:
         return Equip ( pkObject, "lefthand" );
      case Weapon:
         return Equip ( pkObject, "righthand" );
   };

   return false;
}

// ------------------------------------------------------------------------------------------

bool CharacterStats::Equip ( Entity *pkObject, string kSlot )
{
//    // check if object is itemobject (has a itemproperty)
// 	P_Item* pkP_Item = (P_Item*)pkObject->GetProperty("P_Item");
// 
//    if ( !pkP_Item )
//       return false;
// 
//    // Test if the item is equipable at the chosen slot
//    if ( pkP_Item->m_pkItemStats->CanEquipOn(kSlot) )
//    {
// 
//       pkObject->SetUseZones(false);
// 
//       pkP_Item->m_pkItemStats->EquipOn ( this );
// 
//       // check if the slot already is taken, if so, switch objects...somehow!?
//       m_kEquipment[kSlot] = pkObject;
// 
//       m_uiVersion++;
// 
//       // stick Object to MAD model
//       m_pkParent->AddChild ( pkObject );
// 
//       pkObject->SetUpdateStatus ( UPDATE_ALL );
// 
//       P_LinkToJoint* pkLink = (P_LinkToJoint*)pkObject->AddProperty ("P_LinkToJoint");
//       
//       pkLink->SetJoint( kSlot.c_str() );
// 
//       //cout << "Equipped item:" << pkP_Item->m_pkItemStats->m_kItemName << " on " << kSlot << endl;
// 
//       // if equipped primary weapon, set skill!!
//       if ( kSlot == "righthand" || kSlot == "RightHand" || kSlot == "cavetroll_r_u_finger_end")
//       {
//          m_strPrimSkill = pkP_Item->m_pkItemStats->m_kUsesSkill;
//          //cout << "SetPrimSkillTo:" << m_strPrimSkill << endl;
//       }
// 
//       return true;
//    }

   return false;
}

// ---------------------------------------------------------------------------------------------

Entity* CharacterStats::UnEquip (string kSlot)
{
//    if ( m_kEquipment[kSlot] != 0 )
//    {
//       Entity *pkTemp = m_kEquipment[kSlot];
// 
//       P_Item* pkP_Item = (P_Item*)pkTemp->GetProperty("P_Item");
// 
//       pkP_Item->m_pkItemStats->UnEquip( this );
//       
//       m_kEquipment[kSlot] = 0;
// 
//       // TODO!!!! Remove P_LinkToJoint from item!!!
// 
//       m_uiVersion++;
// 
//       return pkTemp;
//    }

   return 0;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::SetMoveSpeed (float fValue)
{
   m_fMoveSpeed = fValue;

   // set move speed on path_finding
   P_PfPath* pkPf = (P_PfPath*)m_pkParent->GetProperty("P_PfPath");

   if ( pkPf )
      pkPf->SetSpeed ( m_fMoveSpeed );
   else
      cout << "Error! Tried to SetMoveSpeed on a character withour P_PfPath property." << endl;
}

// ------------------------------------------------------------------------------------------

void CharacterStats::AddMoveSpeed (float fValue)
{
   m_fMoveSpeed += fValue;

   // set move speed on path_finding
   P_PfPath* pkPf = (P_PfPath*)m_pkParent->GetProperty("P_PfPath");

   if ( pkPf )
      pkPf->SetSpeed ( m_fMoveSpeed );
   else
      cout << "Error! Tried to SetMoveSpeed on a character withour P_PfPath property." << endl;
}

// ------------------------------------------------------------------------------------------

// temp rulez:

// diceroll = Dskillvalue, ska vara <= skill-difficulty
// exp = difficulty / value
// levelup = 10exp








