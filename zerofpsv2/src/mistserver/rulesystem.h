#ifndef _RULESYSTEM_H_
#define _RULESYSTEM_H_


#include <iostream>
#include <string>
#include "../zerofpsv2/engine/zerofps.h"
#include "mistserver.h"

using namespace std;

class RuleSystem
{
	private:
		ZeroFps*			m_pkZeroFps;
		EntityManager*	m_pkEntityManager;		
		MistServer*		m_pkMistServer;
		
	public:
		RuleSystem();
		
		void Damage(int iCharacter,float fDamage);


};

#endif