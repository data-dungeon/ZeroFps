#include "../../engine/zerofps.h"
#include "../../engine/inputhandle.h"
#include "../../engine/entity.h"
#include "../../engine/entitymanager.h"
#include "../../engine/application.h"
#include "p_camera.h"
#include "p_tcs.h"
#include "p_controller.h"
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
	bNetwork = true;

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

void P_Controller::Load(ZFIoInterface* pkPackage)
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
	P_Tcs* pkTcs = (P_Tcs*)GetObject()->GetProperty("P_Tcs");

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
      Camera* pkCamera = m_pkZeroFps->GetRenderTarget("AppView");

		if ( Entity* pkEnt = m_pkObjMan->GetObjectByNetWorkID(pkCamera->m_iEntity) )
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

