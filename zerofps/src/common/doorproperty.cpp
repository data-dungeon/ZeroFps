#include "doorproperty.h"

DoorProperty::DoorProperty()
{
	strcpy(m_acName,"DoorProperty");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_bOpen = false;
	m_bLocked = false;
	m_sKeyName = "";

}

void DoorProperty::RegisterActions()
{
	ItemProperty* pkIP = static_cast<ItemProperty*>(m_pkObject->GetProperty("ItemProperty"));

	if(pkIP != NULL)
	{
		pkIP->RegisterAction(NORMALUSE,"Open the door","OpenDoor");
		pkIP->RegisterAction(NORMALUSE,"Close the door","CloseDoor");	
	}
}	


void DoorProperty::OpenDoor()
{
	if(!m_bOpen)
	{
		PhysicProperty* kPP = static_cast<PhysicProperty*>(m_pkObject->GetProperty("PhysicProperty"));
	
		if(kPP != NULL)
		{
			kPP->m_bSolid = false;
		}
		else
			cout<<"Door has no PhysicProperty"<<endl;
	}
}

void DoorProperty::CloseDoor()
{
	if(m_bOpen)
	{
		PhysicProperty* kPP = static_cast<PhysicProperty*>(m_pkObject->GetProperty("PhysicProperty"));
	
		if(kPP != NULL)
		{
			kPP->m_bSolid = true;
		}
		else
			cout<<"Door has no PhysicProperty"<<endl;
	}
}


void DoorProperty::HandleGameMessage(GameMessage& Msg)
{
	if(Msg.m_Name == "OpenDoor")
		OpenDoor();
		
	if(Msg.m_Name == "CloseDoor")
		CloseDoor();
		
	if(Msg.m_Name == "Register_Actions")
		RegisterActions();
}

vector<PropertyValues> DoorProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(3);
		
	kReturn[0].kValueName="m_bOpen";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bOpen;

	kReturn[1].kValueName="m_bLocked";
	kReturn[1].iValueType=VALUETYPE_BOOL;
	kReturn[1].pkValue=(void*)&m_bLocked;
	
	kReturn[2].kValueName="m_sKeyName";
	kReturn[2].iValueType=VALUETYPE_STRING;
	kReturn[2].pkValue=(void*)&m_sKeyName;

	return kReturn;
}

bool DoorProperty::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_bOpen") == 0) {
		if(kValue == "true")		
			OpenDoor();
		
		if(kValue == "false")		
			CloseDoor();

	}
	

	return false;
}




Property* Create_DoorProperty()
{
	return new DoorProperty();
}



