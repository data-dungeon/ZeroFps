#include "p_event.h" 

P_Event::P_Event()
{
	strcpy(m_acName,"P_Event");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkScriptSys=static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));

	m_pkScriptResHandle = NULL;
	
	m_bHaveRunInit=false;
	m_bFirstRun=true;
	m_fHeartRate=-1;

	m_fTimer = m_pkFps->m_pkObjectMan->GetGameTime();
}

P_Event::~P_Event()
{
	SendEvent("Destroy");	
}

void P_Event::Update()
{
/*
	if(m_bFirstRun)
	{
		m_bFirstRun=false;
		
		SendEvent("FirstRun");
	}
*/

	if(!m_bHaveRunInit)
	{
		m_bHaveRunInit = true;		
		SendEvent("Init");
	}

	if(m_fHeartRate != -1)
		if(m_pkFps->m_pkObjectMan->GetGameTime() - m_fTimer > m_fHeartRate)
		{
			SendEvent("HeartBeat");
			
			m_fTimer = m_pkFps->m_pkObjectMan->GetGameTime();
		}
}

bool P_Event::SendEvent(const char* acEvent)
{
	if(m_pkObject->GetEntityScript() != NULL)
	{
		//set self id before calling the funktion
		MistLandLua::g_iCurrentObjectID = m_pkObject->GetEntityID();
		
		bool bSuccess = m_pkScriptSys->Call(m_pkObject->GetEntityScript(), (char*)acEvent,0,0);
		//cout << "Calling Event: " << acEvent << " = " << bSuccess << endl;

		return bSuccess;
	}

	return false;
}

bool P_Event::SendObjectClickEvent(const char* acType,int iCallerObject )	
{
	if(m_pkObject->GetEntityScript() && acType != NULL)
	{
		//set self id before calling the funktion
		MistLandLua::g_iCurrentObjectID = m_pkObject->GetEntityID();
		
		//set caller id
		MistLandLua::g_iCurrentPCID = iCallerObject;


		vector<ARG_DATA> args(1);
		args[0].eType = tSTRING;
		args[0].pData = new char[strlen(acType)+1];
		strcpy((char*)args[0].pData, acType);
		
		bool bSuccess = m_pkScriptSys->Call(m_pkObject->GetEntityScript(), "Use", args);

		delete[] args[0].pData;
		
		return bSuccess;
	}

   return false;

}


bool P_Event::SendGroudClickEvent(const char* acType,Vector3 kPos,int iCallerObject)
{
	if(m_pkObject->GetEntityScript() && acType != NULL)
	{
		//set self id before calling the funktion
		MistLandLua::g_iCurrentObjectID = m_pkObject->GetEntityID();
		
		//set caller id
		MistLandLua::g_iCurrentPCID = iCallerObject;

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
		
		
		bool bSuccess = m_pkScriptSys->Call( m_pkObject->GetEntityScript(), "GroundClick", args);

		delete[] args[0].pData;
		
		return bSuccess;
	}

	return false;


}

void P_Event::Touch(int iId)
{
	MistLandLua::g_iLastCollidedID = iId;
	SendEvent("Collission");	
	//cout << "Collision" << endl;
/*	if(pkCol->m_pkPP1->GetObject() == m_pkObject)
		MistLandLua::g_iLastCollidedID = pkCol->m_pkPP2->GetObject()->GetEntityID();
	else
		MistLandLua::g_iLastCollidedID = pkCol->m_pkPP1->GetObject()->GetEntityID();
*/		

}


void P_Event::SetHeartRate(float blub) 
{
	m_fHeartRate = blub;
	
	m_fTimer += rand() % (int)m_fHeartRate;
}


void P_Event::Save(ZFIoInterface* pkPackage)
{
   pkPackage->Write ( (void*)&m_bFirstRun, sizeof(m_bFirstRun), 1 );
   pkPackage->Write ( (void*)&m_fHeartRate, sizeof(m_fHeartRate), 1 );
}

void P_Event::Load(ZFIoInterface* pkPackage)
{
   pkPackage->Read ( (void*)&m_bFirstRun, sizeof(m_bFirstRun), 1 );
   pkPackage->Read ( (void*)&m_fHeartRate, sizeof(m_fHeartRate), 1 );
	SetHeartRate(m_fHeartRate);
}


Property* Create_P_Event()
{
	return new P_Event;
}
