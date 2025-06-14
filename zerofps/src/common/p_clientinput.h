#ifndef _P_CLIENTINPUT_H_
#define _P_CLIENTINPUT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

#include "unitcommand.h"
#include <vector>
using namespace std;

class COMMON_API P_ClientInput: public Property {
	private:
		queue<UnitCommand>			m_kCommands;
		static vector<UnitCommand>	m_kServerCommands;		//all orders
		int							m_iPlayerID;
		ZeroFps*					m_pkFps;
		int							m_iMaxOrders;
		int							m_iMaxOrdersPerpkg;
		bool						m_bGod;					//is this the server?

	public:
		P_ClientInput();
		void CloneOf(Property* pkProperty) { }
		void Update();

		void AddOrder(UnitCommand kCommand);
		
		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

		int GetPlayerID() { return m_iPlayerID; }

	friend class ZeroRTS;
};

COMMON_API Property* Create_P_ClientInput();

#endif






