#ifndef _P_CLIENTCONTROL_H_
#define _P_CLIENTCONTROL_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <queue>

#include "p_serverinfo.h"

using namespace std;

enum MCOMMON_API Control
{
	CTRL_UP = 0,
	CTRL_DOWN = 1,
	CTRL_LEFT = 2,
	CTRL_RIGHT = 3,	
};


class MCOMMON_API Controls
{
	public:
		bitset<4> m_akControls;
		float     m_fYRot;
		
		
		Controls()
		{
			m_akControls.reset();		
		}
	
};

class MCOMMON_API ClientOrder
{
	public:
		string	m_sOrderName;
		int		m_iObjectID;
		int		m_iClientID;
		
		int		m_iCharacter;
		
		Vector3	m_kPos;
		int		m_iUseLess;
		
};

/**	\brief	Da P_ClientControl
		\ingroup Common
*/

class MCOMMON_API P_ClientControl: public Property {
	private:
		ZeroFps* 		m_pkFps;
		P_ServerInfo*	m_pkServerInfo;
		
		int				m_iMaxOrders;
		
	
		queue<ClientOrder>			 m_kClientOrders;		
		static queue<ClientOrder>	m_kServerOrders;
		

		void UpdateCharacter();
		bool CheckValidOrder(ClientOrder* temporder);
		void SetupServerinfoP();

	public:
		int 		m_iClientID, m_iActiveCaracterObjectID;
		Controls m_kControls;
		
		void CloneOf(Property* pkProperty) { }
		P_ClientControl();
		
		void Update();
		
		void AddOrder(ClientOrder);
		
      void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
		
		
		
		static ClientOrder* GetNextOrder();				
		static void PopOrder(){m_kServerOrders.pop();};
		static int NrOfOrders(){return m_kServerOrders.size();};				

};

MCOMMON_API Property* Create_P_ClientControl();

#endif
