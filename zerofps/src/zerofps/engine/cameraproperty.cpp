#include "cameraproperty.h"

CameraProperty::CameraProperty(Camera *pkCamera) 
{
	m_pkCamera=pkCamera;
}


void CameraProperty::Update() 
{

	if(m_pkCamera!=NULL) {
		m_pkCamera->SetPos(m_pkObject->GetPos());
		m_pkCamera->SetRot(m_pkObject->GetRot());
	}
}





