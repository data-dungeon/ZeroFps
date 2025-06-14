#include "rulesystem.h"
#include "p_characterproperty.h"
#include "../zerofpsv2/basic/globals.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h"
//#include "../zerofpsv2/engine_systems/script_interfaces/si_std.h"
#include "../zerofpsv2/basic/math.h"

using namespace ObjectManagerLua;

RuleSystem::RuleSystem() : ZFSubSystem("RuleSystem")
{ 

}

bool RuleSystem::StartUp()
{
	m_pkZeroFps= 			static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkEntityManager=	static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));			
	m_pkApplication=		static_cast<Application*>(g_ZFObjSys.GetObjectPtr("Application"));			


	g_pkScript->ExposeFunction("Damage",	SI_RuleSystem::DamageLua);
	g_pkScript->ExposeFunction("Attack",	SI_RuleSystem::AttackLua);
	
	return true;
}

bool RuleSystem::ShutDown()	
{

	return true;
}

void RuleSystem::SendPointText(const string& strText,const Vector3& kPos,int iType)
{
	const void* apParam[5];

	float fTTL = 3;
	
	apParam[0] = &strText;
	apParam[1] = &kPos;
	apParam[2] = &Vector3(0,0.3,0);
	apParam[3] = &fTTL;
	apParam[4] = &iType;
	
	m_pkApplication->OnSystemMessage("PointText",5,apParam);
// 	g_ZFObjSys.SendSystemMessage("Application","PointText",5,apParam);
	
}


void RuleSystem::Damage(int iCharacter,float fDamage)
{
	if(Entity* pkDefender = m_pkEntityManager->GetEntityByID( iCharacter))
	{
		CharacterHit(pkDefender,-1,fDamage);
		
		SendPointText(IntToString(int(-fDamage)),pkDefender->GetWorldPosV(),0);			
	}
}

void RuleSystem::Damage(int iAttacker,int iDefender,float fDamage)
{
	if(Entity* pkDefender = m_pkEntityManager->GetEntityByID(iDefender))
	{
		CharacterHit(pkDefender,iAttacker,fDamage);			
	}
}


void RuleSystem::CharacterHit(Entity* pkCharacter,int iAttacker,int iDamage)
{
	//send hit info to client
	if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty") )
		pkCP->OnHit(iAttacker,iDamage);
	
	
	Vector3 kRandomPos(Math::Randomf(0.5)-0.25,Math::Randomf(0.5)-0.25,Math::Randomf(0.5)-0.25);
	if(iDamage<=0)
		SendPointText("NoDmg",pkCharacter->GetWorldPosV()+kRandomPos,1);
	else
		SendPointText(IntToString(-iDamage),pkCharacter->GetWorldPosV()+kRandomPos,0);				
}

void RuleSystem::CharacterMiss(Entity* pkCharacter,int iAttacker)
{
	if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty") )
		pkCP->OnMiss(iAttacker);

	Vector3 kRandomPos(Math::Randomf(0.5)-0.25,Math::Randomf(0.5)-0.25,Math::Randomf(0.5)-0.25);
	SendPointText("MISS",pkCharacter->GetWorldPosV()+kRandomPos,1);
}

float RuleSystem::Facing(Entity* pkCharacter,const Vector3& kPos)
{
	//check rotation
	Vector3 kOwnerDir = pkCharacter->GetWorldRotM().VectorTransform(Vector3(0,0,1));
	Vector3 kDir = kPos - pkCharacter->GetWorldPosV();		
	
	kOwnerDir.y = 0;
	kDir.y = 0;
	
	kOwnerDir.Normalize();
	kDir.Normalize();
	
	
	return Math::RadToDeg(kOwnerDir.Angle(kDir));

}

bool RuleSystem::Attack(int iAttacker,int iDefender)
{
	P_CharacterProperty* pkAttacker = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iAttacker,"P_CharacterProperty");
	P_CharacterProperty* pkDefender = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iDefender,"P_CharacterProperty");

	if(pkAttacker && pkDefender)
	{
		Stats* pkStatsA = &pkAttacker->m_kCharacterStats;
		Stats* pkStatsD = &pkDefender->m_kCharacterStats;
	
		float fDefense = pkStatsA->GetTotal("Attack");
		
		float fDefenderFacing = Facing(pkDefender->GetEntity(),pkAttacker->GetEntity()->GetWorldPosV());
		float fAttackerFacing = Facing(pkAttacker->GetEntity(),pkDefender->GetEntity()->GetWorldPosV());
		
		//backstab bonus
		if(fDefenderFacing >60)
		{
			Vector3 kRandomPos(Math::Randomf(0.5)-0.25,Math::Randomf(0.5)-0.25,Math::Randomf(0.5)-0.25);
			SendPointText("Backstab",pkDefender->GetEntity()->GetWorldPosV()+kRandomPos,2);
			//cout<<"character facing away, defense crippled"<<endl;
			fDefense *= 0.5;
		}
	
		float fRandA = Math::Randomf(fDefense  );
		float fRandD = Math::Randomf(pkStatsD->GetTotal("Defense") );
	
		
		if(fRandA <= fRandD)
		{			
			//tell defender its been missed
			CharacterMiss(pkDefender->GetEntity(),iAttacker);
			
			return false;			
		}
		else
		{
			//cout<<"HIT"<<endl;
		
			//calculate damage	
			int iSlashing 	=int(pkStatsA->GetTotal("DamageSlashingMin") + 
								Math::Randomi(int(pkStatsA->GetTotal("DamageSlashingMax") - pkStatsA->GetTotal("DamageSlashingMin") +1)));
			int iCrushing 	=int(pkStatsA->GetTotal("DamageCrushingMin") + 
								Math::Randomi(int(pkStatsA->GetTotal("DamageCrushingMax") - pkStatsA->GetTotal("DamageCrushingMin") +1)));
			int iPiercing 	=int(pkStatsA->GetTotal("DamagePiercingMin") + 
								Math::Randomi(int(pkStatsA->GetTotal("DamagePiercingMax") - pkStatsA->GetTotal("DamagePiercingMin") +1)));
			int iFire 		=int(pkStatsA->GetTotal("DamageFireMin") + 
								Math::Randomi(int(pkStatsA->GetTotal("DamageFireMax") - pkStatsA->GetTotal("DamageFireMin") +1)));
			int iIce 		=int(pkStatsA->GetTotal("DamageIceMin") + 
								Math::Randomi(int(pkStatsA->GetTotal("DamageIceMax") - pkStatsA->GetTotal("DamageIceMin") +1)));
			int iLightning	=int(pkStatsA->GetTotal("DamageLightningMin") + 
								Math::Randomi(int(pkStatsA->GetTotal("DamageLightningMax") - pkStatsA->GetTotal("DamageLightningMin") +1)));
			int iMagic	=	int(pkStatsA->GetTotal("DamageMagicMin") + 
								Math::Randomi(int(pkStatsA->GetTotal("DamageMagicMax") - pkStatsA->GetTotal("DamageMagicMin") +1)));
			int iPoison	=	int(pkStatsA->GetTotal("DamagePoisonMin") + 
								Math::Randomi(int(pkStatsA->GetTotal("DamagePoisonMax") - pkStatsA->GetTotal("DamagePoisonMin") +1)));
			
			//absorb	
			iSlashing 	-= Math::Randomi((int)pkStatsD->GetTotal("AbsorbSlashing"));
			iCrushing 	-= Math::Randomi((int)pkStatsD->GetTotal("AbsorbCrushing"));
			iPiercing 	-= Math::Randomi((int)pkStatsD->GetTotal("AbsorbPiercing"));
			iFire 		-= Math::Randomi((int)pkStatsD->GetTotal("AbsorbFire"));
			iIce 			-= Math::Randomi((int)pkStatsD->GetTotal("AbsorbIce"));
			iLightning 	-= Math::Randomi((int)pkStatsD->GetTotal("AbsorbLightning"));
			iMagic 		-= Math::Randomi((int)pkStatsD->GetTotal("AbsorbMagic"));
			iPoison 		-= Math::Randomi((int)pkStatsD->GetTotal("AbsorbPoison"));
			
			iSlashing = (int)Max(iSlashing,0);
			iCrushing = (int)Max(iCrushing,0);
			iPiercing = (int)Max(iPiercing,0);
			iFire = 		(int)Max(iFire,0);
			iIce =		(int)Max(iIce,0);
			iLightning =(int)Max(iLightning,0);
			iMagic = 	(int)Max(iMagic,0);
			iPoison = 	(int)Max(iPoison,0);

		
			int iTotal = iSlashing + iCrushing + iPiercing + iFire + iIce + iLightning + iMagic + iPoison;
			
// 			cout<<"__DAMAGE__"<<endl;
// 			cout<<"Slashing : "<<iSlashing<<endl;
// 			cout<<"Crushing : "<<iCrushing<<endl;
// 			cout<<"Piercing : "<<iPiercing<<endl;
// 			cout<<"Fire     : "<<iFire<<endl;
// 			cout<<"Ice      : "<<iIce<<endl;
// 			cout<<"Lightning: "<<iLightning<<endl;
// 			cout<<"MAgic    : "<<iMagic<<endl;
// 			cout<<"Poison   : "<<iPoison<<endl;
// 			cout<<"TOTAL    : "<<iTotal<<endl;
				
			//tell defender its been hit
			CharacterHit(pkDefender->GetEntity(),iAttacker,iTotal);
			
			return true;
		} 			
	}

	return false;
}

/** \class RuleSystem 
 *  \ingroup NewSi
 *  */

namespace SI_RuleSystem
{

/**	\fn Damage( Target, dDamage )
		\param Target Id of entity do damage.
		\param dDamage How much damage.
		\brief Damages target entity.
		\relates RuleSystem

		Only character can be damaged right now. 
*/

/**	\fn Damage( Attacker,  Target, dDamage )
		\param Attacker Id of entity that was responsible for the damage.
		\param Target Id of entity do damage.
		\param dDamage How much damage.
		\brief Damages target entity.
		\relates RuleSystem

		Only character can be damaged right now. 
*/
	int DamageLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) < 2)
			return 0;			
			
		if(g_pkScript->GetNumArgs(pkLua)  == 2)
		{
			int iDefender;
			double dDamage;
			
			g_pkScript->GetArgInt(pkLua, 0, &iDefender);		
			g_pkScript->GetArgNumber(pkLua, 1, &dDamage);
				
			if(RuleSystem* pkRuleSystem = static_cast<RuleSystem*>(g_ZFObjSys.GetObjectPtr("RuleSystem")))		
				pkRuleSystem->Damage(iDefender,float(dDamage));
		
		}
		else
		{
			int iAttacker;
			int iDefender;
			double dDamage;
	
			g_pkScript->GetArgInt(pkLua, 0, &iAttacker);		
			g_pkScript->GetArgInt(pkLua, 1, &iDefender);		
			g_pkScript->GetArgNumber(pkLua, 2, &dDamage);
		
			if(RuleSystem* pkRuleSystem = static_cast<RuleSystem*>(g_ZFObjSys.GetObjectPtr("RuleSystem")))		
				pkRuleSystem->Damage(iAttacker,iDefender,float(dDamage));
		
		}				


		return 0;
	}	

/**	\fn Attack( Attacker, Defender )
		\param Attacker Id of entity that should attack.
		\param Defender Id of entity that is the target of the attack.
		\return Returns 1 if hit and 0 if miss.
		\brief Makes a character attacks another character
		\relates RuleSystem
*/
	int AttackLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;			
			
		int iAttacker;
		int iDefender;
		
		g_pkScript->GetArgInt(pkLua, 0, &iAttacker);		
		g_pkScript->GetArgInt(pkLua, 1, &iDefender);		

		if(RuleSystem* pkRuleSystem = static_cast<RuleSystem*>(g_ZFObjSys.GetObjectPtr("RuleSystem")))
			if(pkRuleSystem->Attack(iAttacker,iDefender))
			{
				g_pkScript->AddReturnValue(pkLua, 1);
				return 1;
			}
	
		g_pkScript->AddReturnValue(pkLua, 0);			
		return 1;
	}		
}




