#include "p_clientcontrol.h" 

queue<ClientOrder> P_ClientControl::m_kServerOrders;


P_ClientControl::P_ClientControl()
{
	strcpy(m_acName,"P_ClientControl");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=0;//PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;
	
	bNetwork = true;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	m_iMaxOrders = 1;

   m_iActiveCaracterObjectID = -1; // I guess -1 equals no character...// Zerom
}

void P_ClientControl::Update()
{
	//cout<<"Got "<<m_kServerOrders.size()<< " pending orders"<<endl;
}


void P_ClientControl::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	int iNrOO = m_kClientOrders.size();
	
	//make sure we dont send more orders than allowed
	if(iNrOO > m_iMaxOrders)
		iNrOO = m_iMaxOrders;
	
	pkNetPacket->Write(&iNrOO,sizeof(iNrOO));	

   for ( int i = 0; i < iNrOO; i++ )
   {
		pkNetPacket->Write_Str(m_kClientOrders.front().m_sOrderName.c_str());		
		pkNetPacket->Write(&m_kClientOrders.front().m_iObjectID,sizeof(m_kClientOrders.front().m_iObjectID));		
		pkNetPacket->Write(&m_kClientOrders.front().m_iClientID,sizeof(m_kClientOrders.front().m_iClientID));				
		pkNetPacket->Write(&m_kClientOrders.front().m_iCharacter,sizeof(m_kClientOrders.front().m_iCharacter));									
		pkNetPacket->Write(&m_kClientOrders.front().m_kPos,sizeof(m_kClientOrders.front().m_kPos));								
		pkNetPacket->Write(&m_kClientOrders.front().m_iUseLess,sizeof(m_kClientOrders.front().m_iUseLess));										
		m_kClientOrders.pop(); 
	}
} 

void P_ClientControl::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	int iNrOO;
	pkNetPacket->Read(&iNrOO,sizeof(iNrOO));
	
	//never process more orders then clients are allowed to do
	if(iNrOO > m_iMaxOrders)
		cout<<"Error client has added to many orders in queue:"<<endl;

	ClientOrder temporder;
	
	char name[128];
	for(int i = 0;i<iNrOO;i++)
	{
		pkNetPacket->Read_Str(name);
		temporder.m_sOrderName=name;
		pkNetPacket->Read(&temporder.m_iObjectID,sizeof(temporder.m_iObjectID));
		pkNetPacket->Read(&temporder.m_iClientID,sizeof(temporder.m_iClientID));		
		pkNetPacket->Read(&temporder.m_iCharacter,sizeof(temporder.m_iCharacter));				
		pkNetPacket->Read(&temporder.m_kPos,sizeof(temporder.m_kPos));						
		pkNetPacket->Read(&temporder.m_iUseLess,sizeof(temporder.m_iUseLess));								
		
		//if we already gotten max nr of orders, dont add this one
		if(i <= m_iMaxOrders)
			if(CheckValidOrder(&temporder))
				m_kServerOrders.push(temporder);
			else
				cout<<"Client :"<<m_iClientID<<" is trying to cheat"<<endl;
	}
}

bool P_ClientControl::CheckValidOrder(ClientOrder* temporder)
{
	if(temporder->m_iClientID != m_iClientID)
		return false;
		
	return true;
}

void P_ClientControl::AddOrder(ClientOrder kNewOrder)
{
	m_kClientOrders.push(kNewOrder);
	cout<<"added order"<<endl;
}

ClientOrder* P_ClientControl::GetNextOrder()
{
	if(!m_kServerOrders.empty())
	{	
		return &m_kServerOrders.front();
	}
	
	return NULL;
}


Property* Create_P_ClientControl()
{
	return new P_ClientControl;
}
