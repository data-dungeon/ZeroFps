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
class MCOMMON_API P_Spawn: public Property {
	private:
		enum Spawn_Mode
		{
			POINT_SPAWN,
			AREA_SPAWN,		
		};
		
		
		ZeroFps* 		m_pkFps;
		EntityManager* m_pkObjectMan;
		Render*			m_pkRender;
		
		vector<int>	m_kEntitys;

		int	m_iEntityCounter;
		int	m_iMaxEntitys;
		float	m_fSpawnDelay;
		
		int	m_iSpawnMode;
		float	m_fSpawnRadius;

		string m_strTemplate;

		float m_fTimer;

		vector<PropertyValues> GetPropertyValues();
		
	public:
		P_Spawn();
		
		void Update();
			
		void SpawnEntity(Vector3 kPos);
		void RemoveEntity(Entity* pkEnt);
			
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
			
//		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
//		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

MCOMMON_API Property* Create_P_Spawn();

#endif
