#include "p_scriptinterface.h" 
#include "../script_interfaces/si_objectmanager.h"

P_ScriptInterface::P_ScriptInterface()
{
	strcpy(m_acName,"P_ScriptInterface");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkScriptSys=static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));
	
	bNetwork = false;
	
	m_bHaveRunInit=false;
	m_bFirstRun=true;
	m_fHeartRate=-1;

	m_fTimer = m_pkFps->m_pkEntityManager->GetSimTime();
	
	
	//cout<<"P_ScriptInterface created"<<endl;
}

P_ScriptInterface::~P_ScriptInterface()
{
	m_pkEntityManager->CallFunction(m_pkEntity, "Destroy");	
}

void P_ScriptInterface::Update()
{
/*	
	if(m_bFirstRun)
	{
		m_bFirstRun=false;
		
		m_pkObjMan->CallFunction(m_pkObject, "FirstRun");
	}
*/

	if(!m_bHaveRunInit)
	{
		m_bHaveRunInit = true;		
		m_pkEntityManager->CallFunction(m_pkEntity, "Init");
	}

	if(m_fHeartRate != -1)
		if(m_pkFps->m_pkEntityManager->GetSimTime() - m_fTimer > m_fHeartRate)
		{
			m_pkEntityManager->CallFunction(m_pkEntity, "HeartBeat");
			
			m_fTimer = m_pkFps->m_pkEntityManager->GetSimTime();
		}
}

/*
bool P_ScriptInterface::CallFunction(const char* acFunction,vector<ARG_DATA>* pkParams)
{
	//set self id before calling the funktion
	ObjectManagerLua::g_iCurrentObjectID = m_pkObject->GetEntityID();	
		
		
	if(pkParams)
		return m_pkScriptSys->Call(m_pkObject->GetEntityScript(), (char*)acFunction,*pkParams);
	else
		return m_pkScriptSys->Call(m_pkObject->GetEntityScript(), (char*)acFunction,0,0);	
}*/

bool P_ScriptInterface::SendObjectClickEvent(const char* acType,int iCallerObject )	
{
	if(m_pkEntity->GetEntityScript() && acType != NULL)
	{
		//set self id before calling the funktion
		ObjectManagerLua::g_iCurrentObjectID = m_pkEntity->GetEntityID();
		
		//set caller id
		ObjectManagerLua::g_iCurrentPCID = iCallerObject;


		vector<ARG_DATA> args(1);
		args[0].eType = tSTRING;
		args[0].pData = new char[strlen(acType)+1];
		strcpy((char*)args[0].pData, acType);
		
		bool bSuccess = m_pkScriptSys->Call(m_pkEntity->GetEntityScript(), "Use", args);

		delete[] args[0].pData;
		
		return bSuccess;
	}

   return false;

}


bool P_ScriptInterface::SendGroudClickEvent(const char* acType,Vector3 kPos,int iCallerObject)
{
	if(m_pkEntity->GetEntityScript() && acType != NULL)
	{
		//set self id before calling the funktion
		ObjectManagerLua::g_iCurrentObjectID = m_pkEntity->GetEntityID();
		
		//set caller id
		ObjectManagerLua::g_iCurrentPCID = iCallerObject;

		vector<ARG_DATA> args(4);
		args[0].eType = tSTRING;
		args[0].pData = new char[strlen(acType)+1];
		strcpy((char*)args[0].pData, acType);
		
		args[1].eType = tFLOAT;
		args[1].pData = &kPos.x;
		args[2].eType = tFLOAT;
		args[2].pData = &kPos.y;
		args[3].eType = tFLOAT;
		args[3].pData = &kPos.z;
		
		
		bool bSuccess = m_pkScriptSys->Call( m_pkEntity->GetEntityScript(), "GroundClick", args);

		delete[] args[0].pData;
		
		return bSuccess;
	}

	return false;


}

void P_ScriptInterface::SetHeartRate(float blub) 
{
	m_fHeartRate = blub;
	
	if ( int(m_fHeartRate) <= 0 )
		m_fTimer = 0;
	else
		m_fTimer += rand() % (int)m_fHeartRate;
}

void P_ScriptInterface::Touch(int iId)
{
	vector<ARG_DATA> args(1);
	args[0].eType = tINT;
	args[0].pData = &iId;
	
	m_pkEntityManager->CallFunction(m_pkEntity, "Collission",&args);	
}

void P_ScriptInterface::OnEvent(GameMessage& Msg)
{
	cout << "[P_ScriptInterface]: Recv GM: " << m_acName << ", ";
	cout << Msg.m_ToObject << ", ";
	cout << Msg.m_FromObject << ", ";
	cout << Msg.m_Name;
	cout << endl;
	
	string strName = string("on") + Msg.m_Name;
   
	m_pkEntityManager->CallFunction(m_pkEntity, strName.c_str(), 0);
}


void P_ScriptInterface::Save(ZFIoInterface* pkPackage)
{
   pkPackage->Write ( (void*)&m_bFirstRun, sizeof(m_bFirstRun), 1 );
   pkPackage->Write ( (void*)&m_fHeartRate, sizeof(m_fHeartRate), 1 );
}

void P_ScriptInterface::Load(ZFIoInterface* pkPackage,int iVersion)
{
   pkPackage->Read ( (void*)&m_bFirstRun, sizeof(m_bFirstRun), 1 );
   pkPackage->Read ( (void*)&m_fHeartRate, sizeof(m_fHeartRate), 1 );
	SetHeartRate(m_fHeartRate);
}

bool P_ScriptInterface::SendEvent(const char* acEvent)
{
	if(m_pkEntity->GetEntityScript() != NULL)
	{
		//set self id before calling the funktion
		ObjectManagerLua::g_iCurrentObjectID = m_pkEntity->GetEntityID();
		
		bool bSuccess = m_pkScriptSys->Call(m_pkEntity->GetEntityScript(), (char*)acEvent,0,0);
		//cout << "Calling Event: " << acEvent << " = " << bSuccess << endl;

		return bSuccess;
	}

	return false;
}

Property* Create_P_ScriptInterface()
{
	return new P_ScriptInterface;
}







