#ifndef _P_CLIENTINPUT_H_
#define _P_CLIENTINPUT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

#include "../zerorts/zerorts.h"
#include "unitcommand.h"
#include <vector>
using namespace std;

class ZeroRTS;

class COMMON_API P_ClientInput: public Property {
	private:
		vector<UnitCommand>	m_kCommands;
		ZeroRTS*					 m_kZeroRts;
		ZeroFps*					 m_pkFps;

	public:
		P_ClientInput();
		void CloneOf(Property* pkProperty) { }
		void Update();

		void AddOrder(UnitCommand kCommand);
		
		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

};

COMMON_API Property* Create_P_ClientInput();

#endif






