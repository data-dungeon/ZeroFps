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
	m_bRun1SUpdate=false;

	m_fTimer = m_pkFps->GetGameTime();
}

P_Event::~P_Event()
{
	SendEvent("Destroy");	
}

void P_Event::Update()
{
	if(!m_bHaveRunInit)
	{
		m_bHaveRunInit = true;		
		SendEvent("Init");
	}

	if(m_bRun1SUpdate)
		if(m_pkFps->GetGameTime() - m_fTimer > 1.0)
		{
			SendEvent("Update1S");
			
			m_fTimer = m_pkFps->GetGameTime();
		}
}


bool P_Event::SendEvent(const char* acEvent, const char* acType,int iCallerObject)
{
	if(m_pkObject->GetObjectScript() && acType != NULL)
	{
		//set self id before calling the funktion
		MistLandLua::g_iCurrentObjectID = m_pkObject->iNetWorkID;
		
		//set caller id
		MistLandLua::g_iCurrentPCID = iCallerObject;

		vector<ARG_DATA> args(1);
		args[0].eType = tSTRING;
		args[0].pData = new char[strlen(acType)+1];
		strcpy((char*)args[0].pData, acType);
		
		bool bSuccess = m_pkScriptSys->Call(
			m_pkObject->GetObjectScript(), (char*)acEvent, args);

		delete[] args[0].pData;
		
		return bSuccess;
	}

	return false;
}

void P_Event::Touch(Collision* pkCol)
{
	if(pkCol->m_pkPP1->GetObject() == m_pkObject)
		MistLandLua::g_iLastCollidedID = pkCol->m_pkPP2->GetObject()->iNetWorkID;
	else
		MistLandLua::g_iLastCollidedID = pkCol->m_pkPP1->GetObject()->iNetWorkID;
		
	SendEvent("Collission");	
}
/*
void P_Event::ZoneChange(int iCurrent,int iNew)
{
	cout<<"changed zone from "<<iCurrent<<" to "<<iNew<<endl;

}
*/

Property* Create_P_Event()
{
	return new P_Event;
}
