#include "logproperty.h"
#include "adaptorsniper.h"
#include "itemproperty.h"

LogProperty::LogProperty()
{
	strcpy(m_acName,"LogProperty");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_sLog = "This Log is empty";
	m_kLogIcon = "noicon.tga";	
	m_strLogKey = "nokey";

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
		pkIP->RegisterAction(INVENTORYUSE,"Read Log","ReadLog");
		pkIP->RegisterAction(NORMALUSE,"Read Log","ReadLog");
	}
}	

void LogProperty::UnRegisterActions()
{
	ItemProperty* pkIP = static_cast<ItemProperty*>(m_pkObject->GetProperty("ItemProperty"));

	if(pkIP != NULL)
	{
		pkIP->UnRegisterAction(INVENTORYUSE,"Read Log");
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
	vector<PropertyValues> kReturn(3);
		
	kReturn[0].kValueName="m_sLog";
	kReturn[0].iValueType=VALUETYPE_STRING;
	kReturn[0].pkValue=(void*)&m_sLog;

	kReturn[1].kValueName="m_kLogIcon";
	kReturn[1].iValueType=VALUETYPE_STRING;
	kReturn[1].pkValue=(void*)&m_kLogIcon;

	kReturn[2].kValueName="m_strLogKey";
	kReturn[2].iValueType=VALUETYPE_STRING;
	kReturn[2].pkValue=(void*)&m_strLogKey;
	
	return kReturn;
}

void LogProperty::Save(ZFMemPackage* pkPackage)
{
	char data1[256];
	char data2[2048];
	char data3[128];	
		
	strcpy(data1,m_kLogIcon.c_str());		
	pkPackage->Write((void*)&data1,256);
	
	strcpy(data2,m_sLog.c_str());		
	pkPackage->Write((void*)&data2,2048);

	strcpy(data3,m_strLogKey.c_str());		
	pkPackage->Write((void*)&data3,128);
}

void LogProperty::Load(ZFMemPackage* pkPackage)
{
	char data1[256];	
	char data2[2048];	
	char data3[128];

	pkPackage->Read((void*)&data1,256);
	m_kLogIcon = data1;
	
	pkPackage->Read((void*)&data2,2048);
	m_sLog = data2;

	pkPackage->Read((void*)&data3,128);
	m_strLogKey = data3;
		
}

COMMON_API Property* Create_LogProperty()
{
	return new LogProperty();
}



