#include "../../engine/zerofps.h"
#include "../../engine/inputhandle.h"
#include "../../engine/entity.h"
#include "../../engine/entitymanager.h"
#include "../../engine/application.h"
#include "p_camera.h"
#include "p_tcs.h"
#include "p_controller.h"
#include "../script_interfaces/si_objectmanager.h" 
using namespace ObjectManagerLua;

// ------------------------------------------------------------------------------------------

void P_Controller::Update()
{
	for (int i = 0; i < m_kTriggers.size(); i++)
	{
		switch (m_kTriggers[i].m_eControlType)
		{
			case eKEYBOARD:
				if ( m_pkInputHandle->VKIsDown(m_kTriggers[i].m_strKey) )
					SpringTrigger (m_kTriggers[i].m_iInputTrigger, m_kTriggers[i].m_fValue);
				break;
			case eMOUSE:

				break;

			case eJOYPAD:

				break;
		}
		//if ( m_pkZeroFps->m_pkInputHandle->Pressed( m_kTriggers[i].iKey 
	}

}

// ------------------------------------------------------------------------------------------

vector<PropertyValues> P_Controller::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		
	return kReturn;
}

// ------------------------------------------------------------------------------------------

P_Controller::P_Controller()
{
	m_bNetwork = true;

	m_iType = PROPERTY_TYPE_RENDER_NOSHADOW|PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;
	m_iSortPlace =	9;

	strcpy(m_acName,"P_Controller");

    m_pkInputHandle = m_pkZeroFps->m_pkApp->m_pkInputHandle;

	m_bCameraRotation = false;
}

// ------------------------------------------------------------------------------------------

bool P_Controller::HandleSetValue( string kValueName, string kValue )
{



	return false;
}

// ------------------------------------------------------------------------------------------

void P_Controller::Save(ZFIoInterface* pkPackage)
{
}

// ------------------------------------------------------------------------------------------

void P_Controller::Load(ZFIoInterface* pkPackage,int iVersion)
{

}

// ------------------------------------------------------------------------------------------

void P_Controller::PackTo( NetPacket* pkNetPacket, int iConnectionID  )
{
   
	SetNetUpdateFlag(iConnectionID,false);   
}

// ------------------------------------------------------------------------------------------

void P_Controller::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{

}

// ------------------------------------------------------------------------------------------

P_Controller::~P_Controller()
{
  
}

// ------------------------------------------------------------------------------------------

void P_Controller::SetCameraRotation (bool bTrue)
{
	m_bCameraRotation = bTrue;
}

// ------------------------------------------------------------------------------------------

void P_Controller::AddTrigger(string strKey, int iAxis, float fValue)
{
	sTrigger kNewTrigger;

	kNewTrigger.m_eControlType = eKEYBOARD;
	kNewTrigger.m_iInputTrigger = iAxis;
	kNewTrigger.m_strKey = strKey;
	kNewTrigger.m_fValue = fValue;
	
	m_kTriggers.push_back(kNewTrigger);
}

// ------------------------------------------------------------------------------------------

void P_Controller::SpringTrigger (int iAxis, float fValue)
{
	// if object hasn't got physic_prop, it can't be moved with controller...yet
	P_Tcs* pkTcs = (P_Tcs*)GetEntity()->GetProperty("P_Tcs");

	if ( !pkTcs )
		return;

	Vector3 kVel = Vector3(0,0,0);
	//pkTcs->GetVel(kVel);

	switch (iAxis)
	{
		case VEL_X:
			kVel.x = fValue;
			break;
		case VEL_Y:
			kVel.y = fValue;
			break;
		case VEL_Z:
			kVel.z = fValue;
			break;
	}

	// check if force should be rotated
	if ( m_bCameraRotation )
	{
      Camera* pkCamera = m_pkZeroFps->GetRenderCamera("AppView");

		if ( Entity* pkEnt = m_pkEntityManager->GetEntityByID(pkCamera->GetEntityID()) )
		{
			Vector3 kTemp = kVel;

			P_Camera* pkCam = (P_Camera*)pkEnt->GetProperty("P_Camera");
			float fAngle = pkCam->Get3PYAngle();

			kTemp.x = ( kVel.x * cos(fAngle) ) + ( kVel.z * sin(fAngle) );
			kTemp.z = ( kVel.z * cos(-fAngle) ) + ( kVel.x * sin(-fAngle) );

			kVel = kTemp;

			kVel.x = int(kVel.x);
			kVel.z = int(kVel.z);
		}	

	}

	pkTcs->ApplyForce(Vector3(0,0,0), kVel);
}
// ------------------------------------------------------------------------------------------

Property* Create_P_Controller()
{
	return new P_Controller;
}



/* ********************************** SCRIPT INTERFACE ****************************************/
/**	\brief Script functions for Controller
	\ingroup si
*/
class SIController { };

namespace SI_PController
{
// p_controller: objectID(0), NameOfKey(string)(1), axis:X,Y,Z(2), force(float)(3), input (optional, default=KEYBOARD)(4)
int BindKeyLua(lua_State* pkLua)
{
	char strKeyName[255];
	double dObjectID, dAxis, dForce;

	if( g_pkScript->GetNumArgs(pkLua) == 4 )
	{
		g_pkScript->GetArgNumber(pkLua, 0, &dObjectID);
		g_pkScript->GetArgString(pkLua, 1, strKeyName);
		g_pkScript->GetArgNumber(pkLua, 2, &dAxis);
		g_pkScript->GetArgNumber(pkLua, 3, &dForce);

		Entity* pkEntity = g_pkObjMan->GetEntityByID( int(dObjectID) );
		if ( !pkEntity )
			return 0;

		if ( P_Controller* pkP_Controller = (P_Controller*)pkEntity->GetProperty("P_Controller") )
			pkP_Controller->AddTrigger (string(strKeyName), int(dAxis), dForce);
	}

	return 0;
}
// ------------------------------------------------------------------------------------------

// p_controller: makes the steering forces rotate with camera y-axis. Takes ObjectID(0), bool(1)
int RotWithCamYAxisLua(lua_State* pkLua)
{
	if ( g_pkScript->GetNumArgs(pkLua) != 2 )
	{
		cout << "Warning! RotWithCamYAxisLua: takes two arguments: objectId, bool" << endl;
		return 0;
	}
	
	double dObjectID, dBool;


	g_pkScript->GetArgNumber(pkLua, 0, &dObjectID);
	g_pkScript->GetArgNumber(pkLua, 1, &dBool);

	Entity* pkEntity = g_pkObjMan->GetEntityByID( int(dObjectID) );

	if ( !pkEntity )
		return 0;

	if ( P_Controller* pkP_Controller = (P_Controller*)pkEntity->GetProperty("P_Controller") ) 
		pkP_Controller->SetCameraRotation(dBool);

	return 0;
}

}

void Register_PController(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Controller",		Create_P_Controller);

	// Register Property Script Interface
	g_pkScript->ExposeFunction("RotWithCamYAxis",	SI_PController::RotWithCamYAxisLua);
	g_pkScript->ExposeFunction("BindKey",				SI_PController::BindKeyLua);
}
