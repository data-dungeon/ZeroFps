#include "rulesystem.h"
#include "p_characterproperty.h"
#include "../zerofpsv2/basic/globals.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h"
//#include "../zerofpsv2/engine_systems/script_interfaces/si_std.h"

using namespace ObjectManagerLua;

RuleSystem::RuleSystem() : ZFSubSystem("RuleSystem")
{

}

bool RuleSystem::StartUp()
{
	m_pkZeroFps= 			static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkEntityManager=	static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));			
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
	if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iCharacter,"P_CharacterProperty"))
	{
		pkCP->SetLastDamageFrom(-1);
		pkCP->m_kCharacterStats.ChangeStat("Health",-fDamage);
		SendPointText(IntToString(int(fDamage)),pkCP->GetEntity()->GetWorldPosV(),0);
	}
}

void RuleSystem::Damage(int iAttacker,int iDefender,float fDamage)
{
	if(Entity* pkDefender = m_pkEntityManager->GetEntityByID(iDefender))
	{
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkDefender->GetProperty("P_CharacterProperty"))
		{
			pkCP->SetLastDamageFrom(iAttacker);
			pkCP->m_kCharacterStats.ChangeStat("Health",-fDamage);
			SendPointText(IntToString(int(fDamage)),pkCP->GetEntity()->GetWorldPosV(),0);
			
			CharacterHit(pkDefender,iAttacker);
		}
	}
}


void RuleSystem::CharacterHit(Entity* pkCharacter,int iAttacker)
{
	vector<ScriptFuncArg> args(1);
	args[0].m_kType.m_eType = tINT;
	args[0].m_pData = &iAttacker;			//owner character id
			
	m_pkEntityManager->CallFunction(pkCharacter,"Hit",&args);	
}

void RuleSystem::CharacterMiss(Entity* pkCharacter,int iAttacker)
{
	vector<ScriptFuncArg> args(1);
	args[0].m_kType.m_eType = tINT;
	args[0].m_pData = &iAttacker;			//owner character id
			
	m_pkEntityManager->CallFunction(pkCharacter,"Miss",&args);	

}

bool RuleSystem::Attack(int iAttacker,int iDefender)
{
	P_CharacterProperty* pkAttacker = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iAttacker,"P_CharacterProperty");
	P_CharacterProperty* pkDefender = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iDefender,"P_CharacterProperty");

	if(pkAttacker && pkDefender)
	{
		Stats* pkStatsA = &pkAttacker->m_kCharacterStats;
		Stats* pkStatsD = &pkDefender->m_kCharacterStats;
	
		float fRandA = Randomf(pkStatsA->GetTotal("Attack")  );
		float fRandD = Randomf(pkStatsD->GetTotal("Defense") );
	
		cout<<"ATTACK "<<pkStatsA->GetTotal("Attack")<< " VS "<<pkStatsD->GetTotal("Defense")<<endl;
		
		if(fRandA <= fRandD)
		{
			//cout<<"MISS"<<endl;
			SendPointText("MISS",pkDefender->GetEntity()->GetWorldPosV(),1);
			
			//tell defender its been missed
			CharacterMiss(pkDefender->GetEntity(),iAttacker);
			
			return false;			
		}
		else
		{
			//cout<<"HIT"<<endl;
		
			//calculate damage	
			int iSlashing 	=pkStatsA->GetTotal("DamageSlashingMin") + 
								Randomi(pkStatsA->GetTotal("DamageSlashingMax") - pkStatsA->GetTotal("DamageSlashingMin") +1);
			int iCrushing 	=pkStatsA->GetTotal("DamageCrushingMin") + 
								Randomi(pkStatsA->GetTotal("DamageCrushingMax") - pkStatsA->GetTotal("DamageCrushingMin") +1);
			int iPiercing 	=pkStatsA->GetTotal("DamagePiercingMin") + 
								Randomi(pkStatsA->GetTotal("DamagePiercingMax") - pkStatsA->GetTotal("DamagePiercingMin") +1);
			int iFire 		=pkStatsA->GetTotal("DamageFireMin") + 
								Randomi(pkStatsA->GetTotal("DamageFireMax") - pkStatsA->GetTotal("DamageFireMin") +1);
			int iIce 		=pkStatsA->GetTotal("DamageIceMin") + 
								Randomi(pkStatsA->GetTotal("DamageIceMax") - pkStatsA->GetTotal("DamageIceMin") +1);
			int iLightning	=pkStatsA->GetTotal("DamageLightningMin") + 
								Randomi(pkStatsA->GetTotal("DamageLightningMax") - pkStatsA->GetTotal("DamageLightningMin") +1);
			int iMagic	=	pkStatsA->GetTotal("DamageMagicMin") + 
								Randomi(pkStatsA->GetTotal("DamageMagicMax") - pkStatsA->GetTotal("DamageMagicMin") +1);
			int iPoison	=	pkStatsA->GetTotal("DamagePoisonMin") + 
								Randomi(pkStatsA->GetTotal("DamagePoisonMax") - pkStatsA->GetTotal("DamagePoisonMin") +1);
			
			//absorb
			iSlashing 	-= pkStatsD->GetTotal("AbsorbSlashing");
			iCrushing 	-= pkStatsD->GetTotal("AbsorbCrushing");
			iPiercing 	-= pkStatsD->GetTotal("AbsorbPiercing");
			iFire 		-= pkStatsD->GetTotal("AbsorbFire");
			iIce 			-= pkStatsD->GetTotal("AbsorbIce");
			iLightning 	-= pkStatsD->GetTotal("AbsorbLightning");
			iMagic 		-= pkStatsD->GetTotal("AbsorbMagic");
			iPoison 		-= pkStatsD->GetTotal("AbsorbPoison");
		
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
		
			
			//Damage(iAttacker,iDefender,iTotal);
			pkDefender->SetLastDamageFrom(iAttacker);
			pkStatsD->ChangeStat("Health",-iTotal);		
			SendPointText(IntToString(iTotal),pkDefender->GetEntity()->GetWorldPosV(),0);			
		
			//tell defender its been hit
			CharacterHit(pkDefender->GetEntity(),iAttacker);
			
			return true;
		} 			
	}

	return false;
}


namespace SI_RuleSystem
{
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