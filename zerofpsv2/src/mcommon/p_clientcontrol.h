#ifndef _P_CLIENTCONTROL_H_
#define _P_CLIENTCONTROL_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <queue>

using namespace std;

class MCOMMON_API ClientOrder
{
	public:
		char		m_csOrderName[20];
		int		m_iObjectID;
		int		m_iClientID;
};


class MCOMMON_API P_ClientControl: public Property {
	private:
		ZeroFps* m_pkFps;

		int	m_iMaxOrders;
	
		queue<ClientOrder>			 m_kClientOrders;		
		static queue<ClientOrder>	m_kServerOrders;
		

	public:
		int	m_iClientID;
	
	
		void CloneOf(Property* pkProperty) { }
		P_ClientControl();
		
		void Update();
		
		void AddOrder(ClientOrder);
		
      void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);
		
};

MCOMMON_API Property* Create_P_ClientControl();

#endif
