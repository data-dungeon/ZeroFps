#include "logproperty.h"

LogProperty::LogProperty()
{
	strcpy(m_acName,"LogProperty");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_sLog = "This Log is empty";
	m_kLogIcon = "noicon.tga";	

}

LogProperty::~LogProperty()
{
	UnRegisterActions();
}

void LogProperty::Init()
{
	RegisterActions();

}

void LogProperty::RegisterActions()
{
	ItemProperty* pkIP = static_cast<ItemProperty*>(m_pkObject->GetProperty("ItemProperty"));

	if(pkIP != NULL)
	{
		pkIP->RegisterAction(NORMALUSE,"Read Log","ReadLog");
	}
}	

void LogProperty::UnRegisterActions()
{
	ItemProperty* pkIP = static_cast<ItemProperty*>(m_pkObject->GetProperty("ItemProperty"));

	if(pkIP != NULL)
	{
		pkIP->UnRegisterAction(NORMALUSE,"Read Log");
	}
}	

void LogProperty::HandleGameMessage(GameMessage& Msg)
{
	if(Msg.m_Name == "ReadLog")
		ReadLog();
			
	if(Msg.m_Name == "Register_Actions")
		RegisterActions();
}


void LogProperty::ReadLog()
{
	cout<<"LOG SAYS: "<<m_sLog<<endl;

}

vector<PropertyValues> LogProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
		
	kReturn[0].kValueName="m_sLog";
	kReturn[0].iValueType=VALUETYPE_STRING;
	kReturn[0].pkValue=(void*)&m_sLog;

	kReturn[1].kValueName="m_kLogIcon";
	kReturn[1].iValueType=VALUETYPE_STRING;
	kReturn[1].pkValue=(void*)&m_kLogIcon;
	
	return kReturn;
}

void LogProperty::Save(ZFMemPackage* pkPackage)
{
	char data1[256];	
	char data2[2048];	
	
	
	strcpy(data1,m_kLogIcon.c_str());		
	pkPackage->Write((void*)&data1,256);
	
	strcpy(data2,m_sLog.c_str());		
	pkPackage->Write((void*)&data2,2048);
	

}

void LogProperty::Load(ZFMemPackage* pkPackage)
{
	char data1[256];	
	char data2[2048];	
		
	pkPackage->Read((void*)&data1,256);
	m_kLogIcon = data1;
	
	pkPackage->Read((void*)&data2,2048);
	m_sLog = data2;
		
}




Property* Create_LogProperty()
{
	return new LogProperty();
}



