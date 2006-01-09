#include "p_item.h"
#include "p_container.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "p_characterproperty.h"
#include "p_buff.h"

#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 


P_Item::P_Item(): Property("P_Item")
{
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_bNetwork = true;
	m_iVersion = 10;

	// ITEM STUFF
	m_strBaseName=	"misc";
	m_strName = 	"Unnamed Item";
	m_strIcon = 	"default.bmp";
	m_iSizeX = 		1;	
	m_iSizeY = 		1;
	m_iType = 		0;
	
	m_iStackSize = 1;
	m_iStackMax =	1;	

	m_fWeight	=	1;
	m_iValue		=	1;
	
	m_bTwoHanded=	false;
	
	m_bActionOnInventoryDrop = false;
	
	//item info
	m_strInfo = 	"Nothing is known about this item";
	m_strImage = 	"default.bmp"; // zeb: ändrade till bmp... skall vara bmp.
	
	//container
	m_iInContainerID = 	-1;
	m_iInContainerPosX = 0;
	m_iInContainerPosY = 0;
	
	
	//buff
	m_strBuffName 				=	"";
	m_iBuffEntityID 			= -1;
	m_iEquipedOnCharacterID = -1;




	m_kPropertyValues.push_back(PropertyValues("name",VALUETYPE_STRING,(void*)&m_strName));
	m_kPropertyValues.push_back(PropertyValues("sizex",VALUETYPE_INT,(void*)&m_iSizeX));
	m_kPropertyValues.push_back(PropertyValues("sizey",VALUETYPE_INT,(void*)&m_iSizeY));
	m_kPropertyValues.push_back(PropertyValues("icon",VALUETYPE_STRING,(void*)&m_strIcon));
	m_kPropertyValues.push_back(PropertyValues("stacksize",VALUETYPE_INT,(void*)&m_iStackSize));
	m_kPropertyValues.push_back(PropertyValues("stackmax",VALUETYPE_INT,(void*)&m_iStackMax));
	m_kPropertyValues.push_back(PropertyValues("itemtype",VALUETYPE_INT,(void*)&m_iType));
	m_kPropertyValues.push_back(PropertyValues("buffname",VALUETYPE_STRING,(void*)&m_strBuffName));
	m_kPropertyValues.push_back(PropertyValues("info",VALUETYPE_STRING,(void*)&m_strInfo));
	m_kPropertyValues.push_back(PropertyValues("image",VALUETYPE_STRING,(void*)&m_strImage));	
	m_kPropertyValues.push_back(PropertyValues("weight",VALUETYPE_FLOAT,(void*)&m_fWeight));
	m_kPropertyValues.push_back(PropertyValues("value",VALUETYPE_INT,(void*)&m_iValue));
	m_kPropertyValues.push_back(PropertyValues("basename",VALUETYPE_STRING,(void*)&m_strBaseName));
	m_kPropertyValues.push_back(PropertyValues("twohanded",VALUETYPE_BOOL,(void*)&m_bTwoHanded));
	m_kPropertyValues.push_back(PropertyValues("invdropaction",VALUETYPE_BOOL,(void*)&m_bActionOnInventoryDrop));


}


P_Item::~P_Item()
{

}

void P_Item::Init()
{

}

void P_Item::InventoryDropAction(int m_iOwnerID)
{
	//cout<<"P_ITEM: doing action on "<<m_iOwnerID<<endl;

	vector<ScriptFuncArg> args(1);
	args[0].m_kType.m_eType = tINT;
	args[0].m_pData = &m_iOwnerID;

	m_pkEntityManager->CallFunction(m_pkEntity, "InventoryDrop",&args);
	
}

void P_Item::Equip(int iEntity)
{
	if(m_iEquipedOnCharacterID != -1)
	{
		cout<<"item already equiped on character"<<m_iEquipedOnCharacterID<<endl;
		return;	
	}

	
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iEntity))
	{
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
		{
			m_iEquipedOnCharacterID = pkEnt->GetEntityID();
		
			//apply stats
			m_kItemStats.AddOn(&pkCP->m_kCharacterStats);			
		
			// ADD BUFF ?		
			if(!m_strBuffName.empty())					
				if(P_Buff* pkBuff = pkCP->AddBuff(m_strBuffName))
					m_iBuffEntityID = pkBuff->GetEntity()->GetEntityID();
				
			
		}
	}	
}

void P_Item::UnEquip()
{
	if(m_iEquipedOnCharacterID == -1)
	{
		//cout<<"Item not eqiped"<<endl;
		return;
	}

	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iEquipedOnCharacterID))
	{
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
		{
			//remove stats
			m_kItemStats.RemoveOn(&pkCP->m_kCharacterStats);			


			if(m_iBuffEntityID != -1)
			{
				//get buff pointer
				if(P_Buff* pkBuff = (P_Buff*)m_pkEntityManager->GetPropertyFromEntityID(m_iBuffEntityID,"P_Buff"))
				{
					pkCP->RemoveBuff(pkBuff);	
				}			
			
				m_iBuffEntityID = -1;
			}
		}
		
		m_iEquipedOnCharacterID = -1;
	}	
}


void P_Item::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write_Str(m_strBaseName);
	pkPackage->Write_Str(m_strName);
	pkPackage->Write_Str(m_strIcon);
	
	pkPackage->Write(m_iSizeX);
	pkPackage->Write(m_iSizeY);	
	pkPackage->Write(m_iType);
	
	pkPackage->Write(m_iStackSize);
	pkPackage->Write(m_iStackMax);
	
	pkPackage->Write(m_iInContainerID);
	pkPackage->Write(m_iInContainerPosX);
	pkPackage->Write(m_iInContainerPosY);
	
	pkPackage->Write(m_bTwoHanded);
	
	pkPackage->Write_Str(m_strBuffName);
	
	pkPackage->Write_Str(m_strInfo);
	pkPackage->Write_Str(m_strImage);
	
	pkPackage->Write(m_fWeight);
	pkPackage->Write(m_iValue);
	
	pkPackage->Write(m_bActionOnInventoryDrop);
	
	
	m_kItemStats.Save(pkPackage);
	
}

void P_Item::Load(ZFIoInterface* pkPackage,int iVersion)
{
	switch(iVersion)
	{
		case 2:
		{
			pkPackage->Read_Str(m_strName);
			pkPackage->Read_Str(m_strIcon);
			
			pkPackage->Read(m_iSizeX);
			pkPackage->Read(m_iSizeY);		
			pkPackage->Read(m_iType);
			
			pkPackage->Read(m_iInContainerID);		
			pkPackage->Read(m_iInContainerPosX);
			pkPackage->Read(m_iInContainerPosY);				
		
			break;
		}
	
		case 3:
		{
			pkPackage->Read_Str(m_strName);
			pkPackage->Read_Str(m_strIcon);
			
			pkPackage->Read(m_iSizeX);
			pkPackage->Read(m_iSizeY);		
			pkPackage->Read(m_iType);
			
			pkPackage->Read(m_iStackSize);
			pkPackage->Read(m_iStackMax);		
			
			pkPackage->Read(m_iInContainerID);		
			pkPackage->Read(m_iInContainerPosX);
			pkPackage->Read(m_iInContainerPosY);				
		
			break;
		}	
		
		case 4:
		{
			pkPackage->Read_Str(m_strName);
			pkPackage->Read_Str(m_strIcon);
			
			pkPackage->Read(m_iSizeX);
			pkPackage->Read(m_iSizeY);		
			pkPackage->Read(m_iType);
			
			pkPackage->Read(m_iStackSize);
			pkPackage->Read(m_iStackMax);		
			
			pkPackage->Read(m_iInContainerID);		
			pkPackage->Read(m_iInContainerPosX);
			pkPackage->Read(m_iInContainerPosY);	
		
			pkPackage->Read_Str(m_strBuffName);			
		
			break;
		}	

		case 5:
		{
			pkPackage->Read_Str(m_strName);
			pkPackage->Read_Str(m_strIcon);
			
			pkPackage->Read(m_iSizeX);
			pkPackage->Read(m_iSizeY);		
			pkPackage->Read(m_iType);
			
			pkPackage->Read(m_iStackSize);
			pkPackage->Read(m_iStackMax);		
			
			pkPackage->Read(m_iInContainerID);		
			pkPackage->Read(m_iInContainerPosX);
			pkPackage->Read(m_iInContainerPosY);	
		
			pkPackage->Read_Str(m_strBuffName);			
		
			pkPackage->Read_Str(m_strInfo);
			pkPackage->Read_Str(m_strImage);			
			break;
		}					
		
		case 6:
		{
			pkPackage->Read_Str(m_strName);
			pkPackage->Read_Str(m_strIcon);
			
			pkPackage->Read(m_iSizeX);
			pkPackage->Read(m_iSizeY);		
			pkPackage->Read(m_iType);
			
			pkPackage->Read(m_iStackSize);
			pkPackage->Read(m_iStackMax);		
			
			pkPackage->Read(m_iInContainerID);		
			pkPackage->Read(m_iInContainerPosX);
			pkPackage->Read(m_iInContainerPosY);	
		
			pkPackage->Read_Str(m_strBuffName);			
		
			pkPackage->Read_Str(m_strInfo);
			pkPackage->Read_Str(m_strImage);			
			
			pkPackage->Read(m_fWeight);
			pkPackage->Read(m_iValue);			
			
			break;
		}
		
		case 7:
		{
			pkPackage->Read_Str(m_strName);
			pkPackage->Read_Str(m_strIcon);
			
			pkPackage->Read(m_iSizeX);
			pkPackage->Read(m_iSizeY);		
			pkPackage->Read(m_iType);
			
			pkPackage->Read(m_iStackSize);
			pkPackage->Read(m_iStackMax);		
			
			pkPackage->Read(m_iInContainerID);		
			pkPackage->Read(m_iInContainerPosX);
			pkPackage->Read(m_iInContainerPosY);	
		
			pkPackage->Read_Str(m_strBuffName);			
		
			pkPackage->Read_Str(m_strInfo);
			pkPackage->Read_Str(m_strImage);			
			
			pkPackage->Read(m_fWeight);
			pkPackage->Read(m_iValue);			
			
			m_kItemStats.Load(pkPackage);
			
			break;
		}		
		
		case 8:
		{
			pkPackage->Read_Str(m_strBaseName);
			pkPackage->Read_Str(m_strName);
			pkPackage->Read_Str(m_strIcon);
			
			pkPackage->Read(m_iSizeX);
			pkPackage->Read(m_iSizeY);		
			pkPackage->Read(m_iType);
			
			pkPackage->Read(m_iStackSize);
			pkPackage->Read(m_iStackMax);		
			
			pkPackage->Read(m_iInContainerID);		
			pkPackage->Read(m_iInContainerPosX);
			pkPackage->Read(m_iInContainerPosY);	
		
			pkPackage->Read_Str(m_strBuffName);			
		
			pkPackage->Read_Str(m_strInfo);
			pkPackage->Read_Str(m_strImage);			
			
			pkPackage->Read(m_fWeight);
			pkPackage->Read(m_iValue);			
			
			m_kItemStats.Load(pkPackage);
			
			break;
		}	
		
		case 9:
		{
			pkPackage->Read_Str(m_strBaseName);
			pkPackage->Read_Str(m_strName);
			pkPackage->Read_Str(m_strIcon);
			
			pkPackage->Read(m_iSizeX);
			pkPackage->Read(m_iSizeY);		
			pkPackage->Read(m_iType);
			
			pkPackage->Read(m_iStackSize);
			pkPackage->Read(m_iStackMax);		
			
			pkPackage->Read(m_iInContainerID);		
			pkPackage->Read(m_iInContainerPosX);
			pkPackage->Read(m_iInContainerPosY);	
		
			pkPackage->Read(m_bTwoHanded);									
			
			pkPackage->Read_Str(m_strBuffName);			
		
			pkPackage->Read_Str(m_strInfo);
			pkPackage->Read_Str(m_strImage);			
			
			pkPackage->Read(m_fWeight);
			pkPackage->Read(m_iValue);			
			
			m_kItemStats.Load(pkPackage);
			
			break;
		}		
			
		case 10:
		{
			pkPackage->Read_Str(m_strBaseName);
			pkPackage->Read_Str(m_strName);
			pkPackage->Read_Str(m_strIcon);
			
			pkPackage->Read(m_iSizeX);
			pkPackage->Read(m_iSizeY);		
			pkPackage->Read(m_iType);
			
			pkPackage->Read(m_iStackSize);
			pkPackage->Read(m_iStackMax);		
			
			pkPackage->Read(m_iInContainerID);		
			pkPackage->Read(m_iInContainerPosX);
			pkPackage->Read(m_iInContainerPosY);	
		
			pkPackage->Read(m_bTwoHanded);									
			
			pkPackage->Read_Str(m_strBuffName);			
		
			pkPackage->Read_Str(m_strInfo);
			pkPackage->Read_Str(m_strImage);			
			
			pkPackage->Read(m_fWeight);
			pkPackage->Read(m_iValue);			
			
			pkPackage->Read(m_bActionOnInventoryDrop);			
			
			
			m_kItemStats.Load(pkPackage);
			
			break;
		}				
	}
	
	SetNetUpdateFlag( true);
}

void P_Item::PackTo( NetPacket* pkNetPacket, int iConnectionID  )
{
	pkNetPacket->Write_Str(m_strName);
}

void P_Item::PackFrom( NetPacket* pkNetPacket, int iConnectionID  )
{
	pkNetPacket->Read_Str(m_strName);
}


// SCRIPT INTERFACE FOR P_Item
using namespace ObjectManagerLua;

namespace SI_P_Item
{
	int SetItemStatLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
		{
			cout<<"WARNING: SetItemStat - wrong number of arguments"<<endl;
			return 0;		
		}
			
		int iCharcterID;
		double dValue;
		string strStat;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strStat);
		g_pkScript->GetArgNumber(pkLua, 2, &dValue);
		
		if(P_Item* pkItem = (P_Item*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_Item"))
		{
			pkItem->m_kItemStats.SetStat(strStat,float(dValue));		
		}
	
		return 0;				
	}		

	int AddItemStatLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
		{
			cout<<"WARNING: AddItemStat - wrong number of arguments"<<endl;
			return 0;		
		}
			
		int iCharcterID;
		double dValue;
		string strStat;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strStat);
		g_pkScript->GetArgNumber(pkLua, 2, &dValue);
		
		if(P_Item* pkItem = (P_Item*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_Item"))
		{
			pkItem->m_kItemStats.AddStat(strStat,float(dValue));		
		}
	
		return 0;				
	}		
}

Property* Create_P_Item()
{
	return new P_Item;
}

void Register_P_Item(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Item", Create_P_Item);					

	// Register Property Script Interface
	
	//item stats
	g_pkScript->ExposeFunction("SetItemStat",			SI_P_Item::SetItemStatLua);
	g_pkScript->ExposeFunction("AddItemStat",			SI_P_Item::AddItemStatLua);
	
}




