#ifndef  _EFFECTSYSTEM_H_
#define  _EFFECTSYSTEM_H_

/*
	Dvoid
	effect system
*/

#include <list>
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include "p_serverunit.h"
#include "common_x.h"

using namespace std;


class COMMON_API EffectSystem 
{
	private:
		HeightMap*		m_pkMap;
		LevelManager*	m_pkLevelMan;
		ObjectManager* m_pkObjectMan;
		Render*	m_pkRender;		
			
	public:
		static EffectSystem*		m_pkInstance;		
	
		EffectSystem();
		
};


#endif




