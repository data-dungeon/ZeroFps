#include "p_item.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "p_characterproperty.h"
#include "p_buff.h"

#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 

P_Item::P_Item()
{
	strcpy(m_acName,"P_Item");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_bNetwork = true;
	m_iVersion = 7;

	// ITEM STUFF
	m_strName = 	"Unnamed Item";
	m_strIcon = 	"default.bmp";
	m_iSizeX = 		1;	
	m_iSizeY = 		1;
	m_iType = 		0;
	
	m_iStackSize = 1;
	m_iStackMax =	1;	

	m_fWeight	=	1;
	m_iValue		=	1;
	
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
}

P_Item::~P_Item()
{

}

void P_Item::Init()
{


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
	
	
	pkPackage->Write_Str(m_strBuffName);
	
	pkPackage->Write_Str(m_strInfo);
	pkPackage->Write_Str(m_strImage);
	
	pkPackage->Write(m_fWeight);
	pkPackage->Write(m_iValue);
	
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
	}

// 	if(iVersion == 3)
// 	{
// 		pkPackage->Read_Str(m_strName);
// 		pkPackage->Read_Str(m_strIcon);
// 		
// 		pkPackage->Read(m_iSizeX);
// 		pkPackage->Read(m_iSizeY);		
// 		pkPackage->Read(m_iType);
// 		
// 		pkPackage->Read(m_iStackSize);
// 		pkPackage->Read(m_iStackMax);		
// 		
// 		pkPackage->Read(m_iInContainerID);		
// 		pkPackage->Read(m_iInContainerPosX);
// 		pkPackage->Read(m_iInContainerPosY);		
// 	}
// 	
// 	if(iVersion == 2)
// 	{
// 		
// 	}
// 	
// 	if(iVersion == 4)
// 	{
// 		pkPackage->Read_Str(m_strName);
// 		pkPackage->Read_Str(m_strIcon);
// 		
// 		pkPackage->Read(m_iSizeX);
// 		pkPackage->Read(m_iSizeY);		
// 		pkPackage->Read(m_iType);
// 		
// 		pkPackage->Read(m_iStackSize);
// 		pkPackage->Read(m_iStackMax);		
// 		
// 		pkPackage->Read(m_iInContainerID);		
// 		pkPackage->Read(m_iInContainerPosX);
// 		pkPackage->Read(m_iInContainerPosY);	
// 	
// 		pkPackage->Read_Str(m_strBuffName);
// 	
// 	}
}

vector<PropertyValues> P_Item::GetPropertyValues()
{
	vector<PropertyValues> kReturn(12);
	
		
	kReturn[0].kValueName = "name";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = &m_strName;		

	kReturn[1].kValueName = "sizex";
	kReturn[1].iValueType = VALUETYPE_INT;
	kReturn[1].pkValue    = &m_iSizeX;		

	kReturn[2].kValueName = "sizey";
	kReturn[2].iValueType = VALUETYPE_INT;
	kReturn[2].pkValue    = &m_iSizeY;		

	kReturn[3].kValueName = "icon";
	kReturn[3].iValueType = VALUETYPE_STRING;
	kReturn[3].pkValue    = &m_strIcon;			
	
	kReturn[4].kValueName = "stacksize";
	kReturn[4].iValueType = VALUETYPE_INT;
	kReturn[4].pkValue    = &m_iStackSize;		

	kReturn[5].kValueName = "stackmax";
	kReturn[5].iValueType = VALUETYPE_INT;
	kReturn[5].pkValue    = &m_iStackMax;					
	
	kReturn[6].kValueName = "itemtype";
	kReturn[6].iValueType = VALUETYPE_INT;
	kReturn[6].pkValue    = &m_iType;			
	
	kReturn[7].kValueName = "buffname";
	kReturn[7].iValueType = VALUETYPE_STRING;
	kReturn[7].pkValue    = &m_strBuffName;	
	
	kReturn[8].kValueName = "info";
	kReturn[8].iValueType = VALUETYPE_STRING;
	kReturn[8].pkValue    = &m_strInfo;		

	kReturn[9].kValueName = "image";
	kReturn[9].iValueType = VALUETYPE_STRING;
	kReturn[9].pkValue    = &m_strImage;		

	kReturn[10].kValueName = "weight";
	kReturn[10].iValueType = VALUETYPE_FLOAT;
	kReturn[10].pkValue    = &m_fWeight;	
	
	kReturn[11].kValueName = "value";
	kReturn[11].iValueType = VALUETYPE_INT;
	kReturn[11].pkValue    = &m_iValue;			
			
	return kReturn;
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

void Register_P_Item(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Item", Create_P_Item);					

	// Register Property Script Interface
	
	//item stats
	g_pkScript->ExposeFunction("SetItemStat",			SI_P_Item::SetItemStatLua);
	g_pkScript->ExposeFunction("AddItemStat",			SI_P_Item::AddItemStatLua);
	
}