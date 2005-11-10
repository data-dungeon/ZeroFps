#include "p_scriptinterface.h" 
#include "../script_interfaces/si_objectmanager.h"

using namespace ObjectManagerLua;

P_ScriptInterface::P_ScriptInterface() : Property("P_ScriptInterface")
{

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_pkFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkScriptSys	= static_cast<ZSSScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZSSScriptSystem"));
	
	m_bNetwork = false;
	
	m_iVersion = 2;
	m_fHeartRate =- 1;

	m_fTimer = 0;
}

P_ScriptInterface::~P_ScriptInterface() { }

void P_ScriptInterface::Update()
{
	if(m_fHeartRate != -1)
	{
		if(m_pkFps->m_pkEntityManager->GetSimTime() - m_fTimer > m_fHeartRate)
		{
			m_pkEntityManager->CallFunction(m_pkEntity, "HeartBeat");		
			m_fTimer = m_pkFps->m_pkEntityManager->GetSimTime();
		}		
	}
}

void P_ScriptInterface::SetHeartRate(float blub) 
{
	m_fHeartRate = blub;
	m_fTimer = m_pkFps->m_pkEntityManager->GetSimTime();
}

void P_ScriptInterface::Touch(int iId)
{  

}

void P_ScriptInterface::OnEvent(GameMessage& Msg)
{
}

void P_ScriptInterface::Save(ZFIoInterface* pkPackage)
{
   pkPackage->Write(m_fHeartRate);
   pkPackage->Write(m_fTimer);	

}

void P_ScriptInterface::Load(ZFIoInterface* pkPackage,int iVersion)
{
	if(iVersion == 2)
	{
	   pkPackage->Read(m_fHeartRate);
	   pkPackage->Read(m_fTimer);	
	}
	else
	{

		bool apa;
		pkPackage->Read ( (void*)&apa, sizeof(apa), 1 );
		pkPackage->Read ( (void*)&m_fHeartRate, sizeof(m_fHeartRate), 1 );
		SetHeartRate(m_fHeartRate);
	}
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

/**	\fn SetHeartRate( Entity, HeartRate)
 		\relates SIScriptInterface
		\param Entity Id of entity to set rate on.
		\param HeartRate Time in Second between each heartbeat.
		\brief Set a timer that run a heartbeat function on the entity.
*/
	int SetHeartRateLua(lua_State* pkLua)
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

void ENGINE_SYSTEMS_API Register_PScriptInterface(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_ScriptInterface", Create_P_ScriptInterface);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("SetHeartRate",		SI_PScriptInterface::SetHeartRateLua);
}







