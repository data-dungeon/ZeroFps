#include "p_clientunit.h"
#include "p_renderselection.h"

P_ClientUnit::P_ClientUnit() : m_bCommandsUpdated(false)
{
	strcpy(m_acName,"P_ClientUnit");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	//m_pkClientInput =static_cast<P_ClientInput*>(m_pkObject->GetProperty("P_ClientInput"));

	bNetwork = true;

	m_kInfo.m_Info2.m_cTeam =			255;
	m_kInfo.m_Info2.m_cHealth =		255;
	m_kInfo.m_Info2.m_cWeapon =		0;
	m_kInfo.m_Info2.m_cArmor = 		0;
	m_kInfo.m_Info2.m_cPropultion =	0;
	m_kInfo.m_Info2.m_cViewDistance=20;
	m_kInfo.m_Info2.m_cWidth =		3;
	m_kInfo.m_Info2.m_cHeight =		3;		
	strcpy(m_kInfo.m_cName,"NoName");
	
	m_bSelected =	false;
	
	m_bCurrentSelectionRenderState = false;/**/
	
}


void P_ClientUnit::Update()
{
	if(m_pkFps->m_bClientMode)
	{
		//enable rendering of selection
		if(m_bSelected && !m_bCurrentSelectionRenderState)
			EnableSelectionRender();
		
		//disable rendering of selection
		if(!m_bSelected && m_bCurrentSelectionRenderState)
			DisableSelectionRender();
	}	
}

void P_ClientUnit::HandleGameMessage(GameMessage& Msg)
{
		
}

void P_ClientUnit::EnableSelectionRender()
{
	P_RenderSelection* rs = (P_RenderSelection*)m_pkObject->GetProperty("P_RenderSelection");
	
	if(rs == NULL)
	{
		m_pkObject->AddProperty("P_RenderSelection");
		rs = (P_RenderSelection*)m_pkObject->GetProperty("P_RenderSelection");
	}
	
	if(rs == NULL) 	
		cout << "Something is wrong with P_RenderSelection"<<endl;
	
	rs->m_bEnabled = true;
	m_bCurrentSelectionRenderState = true;
}


void P_ClientUnit::DisableSelectionRender()
{
	P_RenderSelection* rs = (P_RenderSelection*)m_pkObject->GetProperty("P_RenderSelection");
	
	if(rs == NULL)
	{
		m_pkObject->AddProperty("P_RenderSelection");
		rs = (P_RenderSelection*)m_pkObject->GetProperty("P_RenderSelection");
	}
	
	if(rs == NULL) 	
		cout << "Something is wrong with P_RenderSelection"<<endl;
	
	rs->m_bEnabled = false;
	m_bCurrentSelectionRenderState = false;

}

void P_ClientUnit::Save(ZFMemPackage* pkPackage)
{

}

void P_ClientUnit::Load(ZFMemPackage* pkPackage)
{

}

void P_ClientUnit::PackTo(NetPacket* pkNetPacket)
{
	g_ZFObjSys.Logf("net", "PackCliUnit Start\n");
//	pkNetPacket->Write(&m_kInfo, sizeof(m_kInfo));
	pkNetPacket->Write(&m_kInfo.m_Info2, sizeof(UnitInfo2));
	pkNetPacket->Write_Str(m_kInfo.m_cName);
	
	int iCommandsToSend = -1; 
	if(m_bCommandsUpdated)
	{
		iCommandsToSend = m_kUnitCommands.size();
		pkNetPacket->Write(&iCommandsToSend, sizeof(iCommandsToSend));
		m_bCommandsUpdated = false;
		
		vector<UnitCommandInfo>::iterator kItor = m_kUnitCommands.begin();
			while (kItor != m_kUnitCommands.end())
			{
				//pkNetPacket->Write(&(*kItor), sizeof(UnitCommandInfo));
				
				pkNetPacket->Write_Str((*kItor).m_acCommandName);
				pkNetPacket->Write_Str((*kItor).m_acComments);
				pkNetPacket->Write( &(*kItor).m_bNeedArgument, sizeof(bool)			);
				pkNetPacket->Write( &(*kItor).m_iIconIndex, sizeof(int)			);

				kItor++;
			}
	} 
	else 
		pkNetPacket->Write(&iCommandsToSend, sizeof(iCommandsToSend));
	
	/*iCommandsToSend = -1; 
	if(!m_kCommandsToDo.empty())
	{
		iCommandsToSend=m_kCommandsToDo.size();
		pkNetPacket->Write(&iCommandsToSend, sizeof(iCommandsToSend));
		while(!m_kCommandsToDo.empty())
		{
			pkNetPacket->Write(&m_kCommandsToDo.front(), sizeof(UnitCommand));
			m_kCommandsToDo.pop();
		}
	}
	else 
		pkNetPacket->Write(&iCommandsToSend, sizeof(iCommandsToSend));*/
	
	g_ZFObjSys.Logf("net", "PackCliUnit End\n");
}

 
void P_ClientUnit::PackFrom(NetPacket* pkNetPacket)
{
//	pkNetPacket->Read(&m_kInfo, sizeof(m_kInfo));
	pkNetPacket->Read(&m_kInfo.m_Info2, sizeof(UnitInfo2));
	pkNetPacket->Read_Str(m_kInfo.m_cName);
	
	int iCommandsToRecive;
	pkNetPacket->Read(&iCommandsToRecive, sizeof(iCommandsToRecive));
	if(iCommandsToRecive > 0)
	{
		m_kUnitCommands.clear();
		UnitCommandInfo kTempUCInfo;
		for(int i =0; i<iCommandsToRecive; i++)
		{
			//pkNetPacket->Read(&kTempUCInfo, sizeof(UnitCommandInfo));
			pkNetPacket->Read_Str(kTempUCInfo.m_acCommandName);
			pkNetPacket->Read_Str(kTempUCInfo.m_acComments);
			pkNetPacket->Read( &kTempUCInfo.m_bNeedArgument, sizeof(bool)  );
			pkNetPacket->Read( &kTempUCInfo.m_iIconIndex, sizeof(int)  );

			m_kUnitCommands.push_back(kTempUCInfo);
		}
	}
	/*pkNetPacket->Read(&iCommandsToRecive, sizeof(iCommandsToRecive));
	if(iCommandsToRecive > 0)
	{
		UnitCommand TempCommand;
		for(int i =0; i<iCommandsToRecive; i++)
		{
			pkNetPacket->Read(&TempCommand, sizeof(UnitCommand));
			m_kCommandsPending.push(TempCommand);
		}
	}*/	
		
}

/*void P_ClientUnit::TestCommand()
{
	cout<<"TEst command!!!" <<endl; 
	UnitCommand Temp;
	strcpy(Temp.m_acCommandName,"Move");
	m_kCommandsToDo.push(Temp);
}*/


COMMON_API Property* Create_P_ClientUnit()
{
	return new P_ClientUnit();

}

/*vector<PropertyValues> P_ClientUnit::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	kReturn[0].kValueName="MoveApa!";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bTemp;;
	
	return kReturn;
};*/

