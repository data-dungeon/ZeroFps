#include "p_clientinput.h"


vector<UnitCommand>	P_ClientInput::m_kServerCommands;		//all orders
		
P_ClientInput::P_ClientInput()
{
	strcpy(m_acName,"P_ClientInput");		

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;


	m_pkFps = 		static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));			
	bNetwork =		true;
	m_iMaxOrders = 200;	
	
	m_iPlayerID =	-1;
	m_bGod =			false;
}

void P_ClientInput::Update()
{

}

void P_ClientInput::AddOrder(UnitCommand kCommand)
{
	if(m_bGod)	//if server's input
	{
		cout<<"GOD says"<<endl;
		kCommand.m_cPlayerID = 255;
		cout<<"bla:"<<(int)kCommand.m_cPlayerID<<endl;
		m_kServerCommands.push_back(kCommand);
	}
	else
	{
		if(m_kCommands.size() < m_iMaxOrders)
			m_kCommands.push_back(kCommand);
	}
}


void P_ClientInput::PackTo(NetPacket* pkNetPacket)
{	
	int nrofcommands = m_kCommands.size();

	pkNetPacket->Write(&m_iPlayerID,sizeof(m_iPlayerID));	
	pkNetPacket->Write(&nrofcommands,sizeof(nrofcommands));
	
	for(int i=0;i<nrofcommands;i++)
		pkNetPacket->Write(&m_kCommands[i],sizeof(UnitCommand));
	
	m_kCommands.clear();
}

void P_ClientInput::PackFrom(NetPacket* pkNetPacket)
{
	int nrofcommands;
	UnitCommand tempcommand;
		
	pkNetPacket->Read(&m_iPlayerID,sizeof(m_iPlayerID));				
	pkNetPacket->Read(&nrofcommands,sizeof(nrofcommands));	
	
	for(int i=0;i<nrofcommands;i++)
	{
		pkNetPacket->Read(&tempcommand,sizeof(UnitCommand));
		
		//only add commands if its a server
		if(m_pkFps->m_bServerMode){		
			//set player id to this client
			tempcommand.m_cPlayerID = (unsigned char)m_iPlayerID;		
			m_kServerCommands.push_back(tempcommand);
		}
	}
}

COMMON_API Property* Create_P_ClientInput()
{
	return new P_ClientInput();

}
