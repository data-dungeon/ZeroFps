/* Richard Svensson

	Client side property for units in zerorts

*/


#ifndef _P_CLIENTUNIT_H_
#define _P_CLIENTUNIT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include <vector>
#include <queue>
#include "common_x.h"
#include "p_serverunit.h"
#include "unitcommandinfo.h"
#include "p_clientinput.h"
#include "unitcommand.h"

using namespace std;


class COMMON_API P_ClientUnit: public Property {
	private:
		ZeroFps*				 m_pkFps;
//		queue<UnitCommand> m_kCommandsToDo;
		
		bool					 m_bCurrentSelectionRenderState;
		//P_ClientInput*     m_pkClientInput;
	
	public:
		UnitInfo				m_kInfo;	
		bool					m_bSelected;
		bool					m_bCommandsUpdated;
		vector<UnitCommandInfo> m_kUnitCommands;
	
		P_ClientUnit();
		void CloneOf(Property* pkProperty) { }
		void Update();

		void EnableSelectionRender();
		void DisableSelectionRender();		

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);

		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);
		
//		void TestCommand();
		void HandleGameMessage(GameMessage& Msg);
//		queue<UnitCommand> m_kCommandsPending;
/// TempTemp
//		vector<PropertyValues> GetPropertyValues();
//		bool m_bTemp;
};

COMMON_API Property* Create_P_ClientUnit();


#endif






