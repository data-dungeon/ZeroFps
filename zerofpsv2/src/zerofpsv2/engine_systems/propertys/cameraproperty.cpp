#include "cameraproperty.h"
#include "madproperty.h"

#define CHASE_CAM_DISTANCE	5

 
CameraProperty::CameraProperty() 
{
	m_pkCamera = NULL;
	m_eCameraType = CAM_TYPEFIRSTPERSON;
	//m_eCameraType = CAM_TYPE3PERSON;	
	strcpy(m_acName,"CameraProperty");	

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	m_fFov = 90;
	m_kDynamicIso.Set(0,0,0);
	m_kInterPos.Set(0,0,0);
	
}


void CameraProperty::Update() 
{
	if(!m_pkCamera)
		return;

	Vector3		kYawVector;
	string		strCamName;

	MadProperty* madp = dynamic_cast<MadProperty*>(m_pkObject->GetProperty("MadProperty"));

	if(m_pkCamera!=NULL) {
		switch(m_eCameraType) {
			case CAM_TYPE3PERSON:
			{
				Matrix4 r;
				r.Identity();
				r.Rotate(Vector3(-90,0,0));
				m_pkCamera->SetRotM(r);
				
				Vector3 dir = m_pkObject->GetWorldPosV() - m_kInterPos;
				m_kInterPos +=dir/8;
				m_pkCamera->SetPos(m_kInterPos + Vector3(0,20,0));
				
				strCamName = " 3P ";
				if(madp)
					madp->m_bIsVisible = true;
				m_pkCamera->SetFov(m_fFov);				
				break;
			}	
			case CAM_TYPEFIRSTPERSON:
//				m_pkCamera->SetPos(m_pkObject->GetPos() + Vector3(0,0.95,0));
//				m_pkCamera->SetRot(m_pkObject->GetRot() + Vector3(0,90,0));
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(0,0.95,0));
				//m_pkCamera->SetRot(m_pkObject->GetWorldRotV() + Vector3(0,90,0));
				m_pkCamera->SetRotM(m_pkObject->GetWorldRotM());
				
				strCamName = " 1P ";
				if(madp)
					madp->m_bIsVisible = false;
				m_pkCamera->SetFov(m_fFov);
				break;

			case CAM_TYPETOPDOWN:
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(0,10,0));
				m_pkCamera->SetRot(Vector3(90,0,0));
				strCamName = " TD ";
				if(madp)
					madp->m_bIsVisible = true;
				m_pkCamera->SetFov(90);
				break;

			case CAM_TYPEISO:
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(10,10,10));
				m_pkCamera->SetRot(Vector3(45,-45,0));
				strCamName = " ISO ";
				if(madp)
					madp->m_bIsVisible = true;
				m_pkCamera->SetFov(m_fFov);
				break;

				
			case CAM_TYPECHASE:
				kYawVector = GetYawVector2(m_pkObject->GetWorldRotV().y);

				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() - (kYawVector * CHASE_CAM_DISTANCE) + 
					Vector3(0,CHASE_CAM_DISTANCE,0));
				m_pkCamera->SetRot(Vector3(m_pkObject->GetWorldRotV() + Vector3(0,90,0)));
				strCamName = " Chase ";
				if(madp)
					madp->m_bIsVisible = true;
				m_pkCamera->SetFov(m_fFov);
				break;

			case CAM_TYPEDYNAMICISO:
				kYawVector = GetYawVector2(m_kDynamicIso.y);

				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() - (kYawVector * CHASE_CAM_DISTANCE) + 
					Vector3(0,CHASE_CAM_DISTANCE,0));
				m_pkCamera->SetRot(Vector3(m_kDynamicIso + Vector3(0,90,0)));
				strCamName = " Dyn ISO ";
				if(madp)
					madp->m_bIsVisible = true;
				m_pkCamera->SetFov(m_fFov);
				break;
				
			case CAM_TYPESIDE:
				strCamName = " Side ";
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(0,0,10));
				m_pkCamera->SetRot(Vector3(0,20,0));
				if(madp)
					madp->m_bIsVisible = true;
				m_pkCamera->SetFov(90);
				break;
		
		}
	}

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

void CameraProperty::SetFpFov(float fFov)
{
	m_fFov = fFov;
	
}


Property* Create_CameraProperty()
{
	return new CameraProperty();
}


