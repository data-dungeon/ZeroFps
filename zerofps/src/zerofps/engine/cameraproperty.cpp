#include "cameraproperty.h"

CameraProperty::CameraProperty(Camera *pkCamera) 
{
	m_pkCamera = pkCamera;
	m_eCameraType = CAM_TYPEFIRSTPERSON;
	strcpy(m_acName,"CameraProperty");	

}


void CameraProperty::Update() 
{
	if(m_pkCamera!=NULL) {
		switch(m_eCameraType) {
			case CAM_TYPEFIRSTPERSON:
				m_pkCamera->SetPos(m_pkObject->GetPos());
				m_pkCamera->SetRot(m_pkObject->GetRot());
				break;
			case CAM_TYPETOPDOWN:
				m_pkCamera->SetPos(m_pkObject->GetPos() + Vector3(0,10,0));
				m_pkCamera->SetRot(Vector3(90,0,0));
				break;
		}
	}

	/*	// FPS
		//m_pkCamera->SetPos(m_pkObject->GetPos());
		//m_pkCamera->SetRot(m_pkObject->GetRot());

		// Top Down
		
		m_pkCamera->SetPos(m_pkObject->GetPos() + Vector3(0,10,0));
		m_pkCamera->SetRot(Vector3(90,0,0));*/

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


