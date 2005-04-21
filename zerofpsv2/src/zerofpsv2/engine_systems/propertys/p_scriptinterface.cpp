#include "p_scriptinterface.h" 
#include "../script_interfaces/si_objectmanager.h"

using namespace ObjectManagerLua;

P_ScriptInterface::P_ScriptInterface()
{
	strcpy(m_acName,"P_ScriptInterface");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps			= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkScriptSys	= static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));
	
	m_bNetwork = false;
	
//	m_bHaveRunInit = false;
	m_fHeartRate =- 1;
	m_iLastTouchFrame = -1;

	m_fTimer = m_pkFps->m_pkEntityManager->GetSimTime();
}

P_ScriptInterface::~P_ScriptInterface() { }

void P_ScriptInterface::Update()
{
/*	if(!m_bHaveRunInit)
	{
		m_bHaveRunInit = true;		
		m_pkEntityManager->CallFunction(m_pkEntity, "Init");
	}
*/

	if(m_fHeartRate != -1)
		if(m_pkFps->m_pkEntityManager->GetSimTime() - m_fTimer > m_fHeartRate)
		{
			m_pkEntityManager->CallFunction(m_pkEntity, "HeartBeat");
			
			m_fTimer = m_pkFps->m_pkEntityManager->GetSimTime();
		}
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
// 	//only do one touch per frame
// 	if(m_pkZeroFps->GetCurrentFrame() == m_iLastTouchFrame)
// 		return;
// 	m_iLastTouchFrame = m_pkZeroFps->GetCurrentFrame();
// 
// 	vector<ScriptFuncArg> kArgs(1);
// 	kArgs[0].m_kType.m_eType = tINT;
// 	kArgs[0].m_pData = &iId;
// 
// 	m_pkEntityManager->CallFunction(m_pkEntity, "Touch",&kArgs);
}

void P_ScriptInterface::OnEvent(GameMessage& Msg)
{
/*	cout << "[P_ScriptInterface]: Recv GM: " << m_acName << ", ";
	cout << Msg.m_ToObject << ", ";
	cout << Msg.m_FromObject << ", ";
	cout << Msg.m_Name;
	cout << endl;
	
	string strName = string("on") + Msg.m_Name;
   
	m_pkEntityManager->CallFunction(m_pkEntity, strName.c_str(), 0);*/
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


Property* Create_P_ScriptInterface()
{
	return new P_ScriptInterface;
}

/* ********************************** SCRIPT INTERFACE ****************************************/
namespace SI_PScriptInterface
{
/** \class SIScriptInterface 
 *  \ingroup Property
 *  */

/**	\fn SISetHeartRate( Entity, HeartRate)
 		\relates SIScriptInterface
		\param Entity Id of entity to set rate on.
		\param HeartRate Time in Second between each heartbeat.
		\brief Set a timer that run a heartbeat function on the entity.
*/
int SISetHeartRateLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 2)
		return 0;

	double dId;	
	double dHeartRate;
	g_pkScript->GetArgNumber(pkLua, 0, &dId);
	g_pkScript->GetArgNumber(pkLua, 1, &dHeartRate);		

	Entity* pkObject = g_pkObjMan->GetEntityByID((int)dId);
	if(!pkObject)
		return 0;

	if(pkObject)
	{	
		P_ScriptInterface* pkScriptInterface = (P_ScriptInterface*)pkObject->GetProperty("P_ScriptInterface");
		if(!pkScriptInterface)
			pkScriptInterface = (P_ScriptInterface*)pkObject->AddProperty("P_ScriptInterface");

		if(pkScriptInterface)
			pkScriptInterface->SetHeartRate((float)dHeartRate);			
	}

	return 0;
}	
}

void ENGINE_SYSTEMS_API Register_PScriptInterface(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_ScriptInterface", Create_P_ScriptInterface);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("SISetHeartRate",		SI_PScriptInterface::SISetHeartRateLua);
}