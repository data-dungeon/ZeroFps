#include "p_item.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "p_characterproperty.h"
#include "p_buff.h"

P_Item::P_Item()
{
	strcpy(m_acName,"P_Item");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_bNetwork = true;
	m_iVersion = 4;

	// ITEM STUFF
	m_strName = "Unnamed Item";
	m_strIcon = "default.bmp";
	m_iSizeX = 1;	
	m_iSizeY = 1;
	m_iType = 0;
	
	m_iStackSize = 1;
	m_iStackMax = 1;	

	m_iInContainerID = -1;
	m_iInContainerPosX = 0;
	m_iInContainerPosY = 0;
	
	
	
	m_strBuffName = "";
	m_iBuffEntityID = -1;
	
}

P_Item::~P_Item()
{

}

void P_Item::Init()
{


}

void P_Item::Equip(int iEntity)
{
	if(m_strBuffName.empty())
		return;

	if(m_iBuffEntityID != -1)
	{
		cout<<"item already eqiped"<<endl;
		return;
	}
		
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iEntity))
	{
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
		{
			if(P_Buff* pkBuff = pkCP->AddBuff(m_strBuffName))
				m_iBuffEntityID = pkBuff->GetEntity()->GetEntityID();
		}
	}
}

void P_Item::UnEquip()
{
	if(m_iBuffEntityID == -1)
	{
		cout<<"Item not eqiped"<<endl;
		return;
	}

	//get character and 
	if(P_Buff* pkBuff = (P_Buff*)m_pkEntityManager->GetPropertyFromEntityID(m_iBuffEntityID,"P_Buff"))
	{
		if(P_CharacterProperty* pkCP = pkBuff->GetCharacter())
			pkCP->RemoveBuff(pkBuff);	
	}
	
// 	m_pkEntityManager->Delete(m_iBuffEntityID);
	m_iBuffEntityID = -1;
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
}

void P_Item::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 3)
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
	}
	
	if(iVersion == 2)
	{
		pkPackage->Read_Str(m_strName);
		pkPackage->Read_Str(m_strIcon);
		
		pkPackage->Read(m_iSizeX);
		pkPackage->Read(m_iSizeY);		
		pkPackage->Read(m_iType);
		
		pkPackage->Read(m_iInContainerID);		
		pkPackage->Read(m_iInContainerPosX);
		pkPackage->Read(m_iInContainerPosY);		
	}
	
	if(iVersion == 4)
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
	
	}
}

vector<PropertyValues> P_Item::GetPropertyValues()
{
	vector<PropertyValues> kReturn(8);
	
		
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
	
	return kReturn;
}


Property* Create_P_Item()
{
	return new P_Item;
}

