#include "p_clientinput.h"


vector<UnitCommand>	P_ClientInput::m_kServerCommands;		//all orders
		
P_ClientInput::P_ClientInput()
{
	strcpy(m_acName,"P_ClientInput");		

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;

	bNetwork = true;
	
//	m_kZeroRts = static_cast<ZeroRTS*>(g_ZFObjSys.GetObjectPtr("Application"));		
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));			
}

void P_ClientInput::Update()
{

}

void P_ClientInput::AddOrder(UnitCommand kCommand)
{
	cout<<"adding order"<<endl;
	m_kCommands.push_back(kCommand);

}


void P_ClientInput::PackTo(NetPacket* pkNetPacket)
{	
	int nrofcommands = m_kCommands.size();

	pkNetPacket->Write(&nrofcommands,sizeof(nrofcommands));
	
	for(int i=0;i<nrofcommands;i++)
		pkNetPacket->Write(&m_kCommands[i],sizeof(UnitCommand));
	
	m_kCommands.clear();
}

void P_ClientInput::PackFrom(NetPacket* pkNetPacket)
{
	int nrofcommands;
	UnitCommand tempcommand;
		
	pkNetPacket->Read(&nrofcommands,sizeof(nrofcommands));

	for(int i=0;i<nrofcommands;i++)
	{
		pkNetPacket->Read(&tempcommand,sizeof(UnitCommand));
		
		//only add commands if its a server
		if(m_pkFps->m_bServerMode)
			m_kServerCommands.push_back(tempcommand);
	}
	
}

COMMON_API Property* Create_P_ClientInput()
{
	return new P_ClientInput();

}
