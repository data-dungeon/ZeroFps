#ifndef _RULESYSTEM_H_
#define _RULESYSTEM_H_


#include <iostream>
#include <string>
#include "../zerofpsv2/engine/zerofps.h"
//#include "mistserver.h"

#include "mcommon_x.h"

using namespace std;

class MCOMMON_API RuleSystem : public ZFSubSystem
{
	private:
		ZeroFps*				m_pkZeroFps;
		ZSSEntityManager*	m_pkEntityManager;		
		Application*		m_pkApplication;
		
		void SendPointText(const string& strText,const Vector3& kPos,int iType);
		
		void CharacterHit(Entity* pkCharacter,int iAttacker);
		void CharacterMiss(Entity* pkCharacter,int iAttacker);
		
	public:
		RuleSystem();
		
		bool StartUp();
		bool ShutDown();
		bool IsValid()		{	return true;	};		
		
		void Damage(int iCharacter,float fDamage);
		void Damage(int iAttacker,int iDefender,float fDamage);
		bool Attack(int iAttacker,int iDefender);
};


namespace SI_RuleSystem
{
	//rule system
	int DamageLua(lua_State* pkLua);
	int AttackLua(lua_State* pkLua);
	
};


#endif









