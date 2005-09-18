#ifndef _P_SPAWN_H_
#define _P_SPAWN_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;

/**	\brief	Da P_Spawn
		\ingroup Common
*/
class MCOMMON_API P_Spawn: public Property 
{
	private:			
// 		ZSSZeroFps* 		m_pkFps;
// 		ZSSEntityManager* m_pkEntityManager;
// 		Render*			m_pkRender;
		
		string			m_strEntityScript;
		float				m_fSpawnDelay;
		
		float				m_fStartTime;
		
		vector<PropertyValues> GetPropertyValues();
		
	public:
		P_Spawn();
		
		void Update();
			
		void SetSpawnScript(const string& strScript)					{	m_strEntityScript = strScript;	}
		void SetSpawnDelay(float fDelay)									{	m_fSpawnDelay = fDelay;				}
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
			
};

MCOMMON_API Property* Create_P_Spawn();

#endif
