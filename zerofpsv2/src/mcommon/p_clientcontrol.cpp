#include "p_clientcontrol.h" 
#include "../zerofpsv2/engine_systems/propertys/p_tcs.h"


queue<ClientOrder> P_ClientControl::m_kServerOrders;


P_ClientControl::P_ClientControl()
{
	strcpy(m_acName,"P_ClientControl");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_bNetwork = true;
	
	m_pkFps=static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkRender=static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			
	
	m_iMaxOrders = 1;

   m_iActiveCaracterObjectID = -1; // I guess -1 equals no character...// Zerom...i think so //dvoid
	m_pkServerInfo = NULL;
}

void P_ClientControl::Update()
{
	// try getting serverinfo property pointer
	if(!m_pkServerInfo)
		SetupServerinfoP();

	//update character controls
	UpdateCharacter();

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
	
	//client controls
	pkNetPacket->Write(&m_iActiveCaracterObjectID,sizeof(m_iActiveCaracterObjectID));										
	pkNetPacket->Write(&m_kControls,sizeof(m_kControls));											
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
		
		temporder.m_iConnectID = pkNetPacket->m_iClientID;
		//if we already gotten max nr of orders, dont add this one
		if(i <= m_iMaxOrders)
			if(CheckValidOrder(&temporder))
				m_kServerOrders.push(temporder);
			else
				cout<<"Client :"<<m_iClientID<<" is trying to cheat"<<endl;
	}

	//client controls
	pkNetPacket->Read(&m_iActiveCaracterObjectID,sizeof(m_iActiveCaracterObjectID));										
	pkNetPacket->Read(&m_kControls,sizeof(m_kControls));											
	
	if(m_pkServerInfo)
	{
		if(m_pkServerInfo->GetCharacterID(m_iClientID) != m_iActiveCaracterObjectID)
		{
			cout<<"invalid controls from connection:"<<m_iClientID<<endl;
			m_iActiveCaracterObjectID = -1;
		}	
	}
}

void P_ClientControl::AddServerOrder(ClientOrder kOrder)
{
	m_kServerOrders.push(kOrder);
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

void P_ClientControl::UpdateCharacter()
{
	if(m_iActiveCaracterObjectID == -1)
		return;
	
	//hack
	Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iActiveCaracterObjectID);
	
	if(pkEnt)
	{
		P_Tcs* tcs = (P_Tcs*)pkEnt->GetProperty("P_Tcs");
		if(tcs)	
		{
			//setup rotation
			Matrix3 temp;
			temp.Identity();
			temp.RadRotate(0,m_kControls.m_fYRot,0);
			pkEnt->SetLocalRotM(temp);

			//setup movement
			Vector3 kVel(0,0,0);

			if(m_kControls.m_akControls[CTRL_UP])
				kVel.z += 1;
	
			if(m_kControls.m_akControls[CTRL_DOWN])
				kVel.z += -1;

			if(m_kControls.m_akControls[CTRL_LEFT])
				kVel.x += 1;

			if(m_kControls.m_akControls[CTRL_RIGHT])
				kVel.x += -1;

			if(kVel.Length() > 0)
				kVel.Normalize();
			
			float fSpeed = 20;
			
			if(!tcs->GetOnGround())
				fSpeed /= 4;
			
			kVel = temp.VectorTransform(kVel) * fSpeed;
		
			//update walk velocity on the body
			tcs->ApplyForce(Vector3(0,0,0),kVel);
			//tcs->SetWalkVel(kVel);
		}
	}
}


void P_ClientControl::SetupServerinfoP()
{
	Entity* pkEnt = m_pkEntityManager->GetEntityByName("A t_serverinfo.lua");
	
	if(pkEnt)
	{
		m_pkServerInfo = (P_ServerInfo*)pkEnt->GetProperty("P_ServerInfo");
	}
}


Property* Create_P_ClientControl()
{
	return new P_ClientControl;
}
