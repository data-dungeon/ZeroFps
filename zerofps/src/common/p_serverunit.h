/* Richard Svensson

	Client side property for units in zerorts

*/


#ifndef _P_SERVERUNIT_H_
#define _P_SERVERUNIT_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <map>
#include <iostream>
#include <string>
#include "common_x.h"
#include "externalcommand.h"
#include "unitcommand.h"


using namespace std;

struct COMMON_API UnitInfo2
{
	unsigned char		m_cTeam;
	unsigned char		m_cHealth;
	unsigned char		m_cWeapon;
	unsigned char		m_cArmor;
	unsigned char		m_cPropultion;
	unsigned char		m_cViewDistance;	
	unsigned char		m_cWidth;			//used for buildings
	unsigned char		m_cHeight;		  //used for buildings	
};

struct COMMON_API UnitInfo
{
	UnitInfo2			m_Info2;
	char					m_cName[64];		//the units name
};


#include "p_clientunit.h"

class P_ClientUnit;

class COMMON_API P_ServerUnit: public Property {
	private:
		P_ClientUnit*		m_pkClientUnit;	
		bool					m_bHaveSetRadius;
		bool					m_bUpdateCommands;
		bool					m_bClient;
		bool					m_bHaveSetPos;
		map<string, ExternalCommand*> m_kExternalCommands;
		vector<PropertyValues> GetPropertyValues();		
		AIBase* m_pkCurrentAIState;
		
	public:
		UnitInfo				m_kInfo;
		Point					m_kTile;		
	
		P_ServerUnit();
		void Init();		
		void CloneOf(Property* pkProperty) { }
		void Update();

		void HandleGameMessage(GameMessage& Msg);


		void GetClientUnitP();
		void UpdateClient();
		void SetRadius();

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
		
		bool RegisterExternalCommand(ExternalCommand* kCommand);
		bool RemoveExternalCommand(string kCommandName);
		AIBase* RunExternalCommand(UnitCommand* kCommand);


};

COMMON_API Property* Create_P_ServerUnit();


#endif
