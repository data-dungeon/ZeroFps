#ifndef _P_ML_H_
#define _P_ML_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>
#include "p_spawn.h"

using namespace std;

class MCOMMON_API P_Ml: public Property {
	private:
		ZeroFps* m_pkFps;
		EntityManager* m_pkEntityMan;
		
		vector<string>	m_kActions;	///> Det skall finnas en 32x32 ikon med samma namn i mappen "data/textures/gui/actions"
		
		int				m_iSpawnZon;
		int				m_iSpawn;

		bool				m_bJustSaved;

	public:

		void GetActions(vector<string>& actions) { actions = m_kActions; }

		void CloneOf(Property* pkProperty) { }
		P_Ml();
		~P_Ml();
		
		void Update();
		
		void AddAction(const char* csAction);
		
		void SetSpawnPointer(int iSpawner,int iZon) {m_iSpawn = iSpawner;m_iSpawnZon = iZon;};
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

MCOMMON_API Property* Create_P_Ml();

#endif
