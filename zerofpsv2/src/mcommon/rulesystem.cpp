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
	
	return true;
}

bool RuleSystem::ShutDown()	
{

	return true;
}

void RuleSystem::SendPointText(const string& strText,const Vector3& kPos,int iType)
{
	static struct pointdatatext
	{
		string	strText;
		Vector3	kPos;
		Vector3	kVel;
		float		fTTL;
		int		iType;
	} kPointTextData;

	kPointTextData.strText = strText;
	kPointTextData.kPos = kPos;
	kPointTextData.kVel.Set(0,0.3,0);
	kPointTextData.fTTL = 3;
	kPointTextData.iType = iType;
	
	m_pkApplication->OnSystemMessage("PointText",&kPointTextData);
	//g_ZFObjSys.SendSystemMessage("Application","PointText",&kPointTextData);
}


void RuleSystem::Damage(int iCharacter,float fDamage)
{
	cout<<"Damage"<<endl;

	if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iCharacter,"P_CharacterProperty"))
	{
		pkCP->m_kCharacterStats.ChangeStat("Health",-fDamage);
		
		SendPointText(IntToString(int(fDamage)),pkCP->GetEntity()->GetWorldPosV(),0);
// 		m_pkMistServer->SendPointText(IntToString(int(fDamage)),pkCP->GetEntity()->GetWorldPosV(),Vector3(0,0.3,0),3,0);
	}
}


namespace SI_RuleSystem
{
	int DamageLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;			
			
		int iCharacter;
		double dDamage;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharacter);		
		g_pkScript->GetArgNumber(pkLua, 1, &dDamage);

		if(RuleSystem* pkRuleSystem = static_cast<RuleSystem*>(g_ZFObjSys.GetObjectPtr("RuleSystem")))		
			pkRuleSystem->Damage(iCharacter,float(dDamage));

		return 0;
	}	

}