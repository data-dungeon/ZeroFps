#include "rulesystem.h"
#include "../mcommon/p_characterproperty.h"
#include "../zerofpsv2/basic/globals.h"

RuleSystem::RuleSystem()
{
	m_pkZeroFps= 			static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkEntityManager=	static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));			
	m_pkMistServer=		static_cast<MistServer*>(g_ZFObjSys.GetObjectPtr("Application"));			

}


void RuleSystem::Damage(int iCharacter,float fDamage)
{
	if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iCharacter,"P_CharacterProperty"))
	{
		pkCP->m_kCharacterStats.ChangeStat("Health",-fDamage);
		
		
		m_pkMistServer->SendPointText(IntToString(int(fDamage)),pkCP->GetEntity()->GetWorldPosV(),Vector3(0,0.3,0),3,0);
	}
}