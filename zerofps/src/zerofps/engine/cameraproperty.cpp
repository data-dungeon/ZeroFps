#include "cameraproperty.h"

CameraProperty::CameraProperty(Camera *pkCamera) 
{
	m_pkCamera = pkCamera;
	m_eCameraType = CAM_TYPEFIRSTPERSON;
	strcpy(m_acName,"CameraProperty");	

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
}


void CameraProperty::Update() 
{
//	float fYawAngle;
	Vector3 kYawVector;
	string strCamName;

	if(m_pkCamera!=NULL) {
		switch(m_eCameraType) {
			case CAM_TYPEFIRSTPERSON:
				m_pkCamera->SetPos(m_pkObject->GetPos() + Vector3(0,0.6,0));
				m_pkCamera->SetRot(m_pkObject->GetRot() + Vector3(0,90,0));
				strCamName = " 1P ";
				break;
			case CAM_TYPETOPDOWN:
				m_pkCamera->SetPos(m_pkObject->GetPos() + Vector3(0,10,0));
				m_pkCamera->SetRot(Vector3(90,0,0));
				strCamName = " TD ";
				break;

			case CAM_TYPEISO:
			case CAM_TYPESIDE:
				strCamName = " Side ";
				m_pkCamera->SetPos(m_pkObject->GetPos() + Vector3(0,0,10));
				m_pkCamera->SetRot(Vector3(0,0,0));
				/*kYawVector = GetYawVector2(m_pkObject->GetRot().y);
				fYawAngle  = m_pkObject->GetRot().y;
				m_pkCamera->SetPos(m_pkObject->GetPos() + kYawVector*10);
				m_pkCamera->SetRot(Vector3(0,- fYawAngle,0));*/
				break;
		
//			case CAM_TYPESIDE:
				//m_pkCamera->SetPos(m_pkObject->GetPos() + Vector3(0,0,10));
				//m_pkCamera->SetRot(Vector3(0,0,0));
//				break;
		}
	}

	/*	// FPS
		//m_pkCamera->SetPos(m_pkObject->GetPos());
		//m_pkCamera->SetRot(m_pkObject->GetRot());

		// Top Down
		
		m_pkCamera->SetPos(m_pkObject->GetPos() + Vector3(0,10,0));
		m_pkCamera->SetRot(Vector3(90,0,0));*/

	m_pkCamera->SetName(strCamName);
}

void CameraProperty::SetType(CamType_e kType)
{
	m_eCameraType = kType;
}

void CameraProperty::NextType(CamType_e kType)
{
	int iType = m_eCameraType;
	iType++;
	if(iType >= CAM_TYPEMAX)
		iType = CAM_TYPEFIRSTPERSON;

	SetType((CamType_e)iType);
}


Property* Create_CameraProperty()
{
	return new CameraProperty(NULL);
}


