#include "p_camera.h"
#include "p_mad.h"

#define CHASE_CAM_DISTANCE	5 

 
P_Camera::P_Camera() 
{
	m_pkCamera = NULL;
	m_eCameraType = CAM_TYPEFIRSTPERSON;
	strcpy(m_acName,"P_Camera");	

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	m_fFov = 90;
	m_kDynamicIso.Set(0,0,0);
	m_kInterPos.Set(0,0,0);
	m_f3PYAngle = 0;
	m_f3PYPos = 1;
	m_f3PDistance = 4;
}


void P_Camera::Update() 
{
	if(!m_pkCamera)
		return;

	Vector3		kYawVector;
	string		strCamName;

	P_Mad* madp = dynamic_cast<P_Mad*>(m_pkObject->GetProperty("P_Mad"));

	if(m_pkCamera!=NULL) {
		switch(m_eCameraType) {
			case CAM_TYPE3PERSON:
			{
				Matrix4 r;
				r.Identity();
				r.Rotate(Vector3(-90,0,0));
				m_pkCamera->SetRotM(r);
				
				Vector3 dir = m_pkObject->GetIWorldPosV() - m_kInterPos;
				m_kInterPos +=dir/8;
				
				
				float xp = sin(m_f3PYAngle);
				float zp = cos(m_f3PYAngle);				
				
				Vector3 campos = m_kInterPos + Vector3(xp,m_f3PYPos,zp).Unit() * m_f3PDistance;
				
				LookAt(campos, m_kInterPos,Vector3(0,1,0));
				
				
				strCamName = " 3P ";
				/*if(madp)
					madp->m_bIsVisible = true;*/
				//m_pkCamera->SetFov(m_fFov);				
				break;
			}	
			case CAM_TYPEFIRSTPERSON:
			{
//				m_pkCamera->SetPos(m_pkObject->GetPos() + Vector3(0,0.95,0));
//				m_pkCamera->SetRot(m_pkObject->GetRot() + Vector3(0,90,0));
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(0,0.95,0));
				//m_pkCamera->SetRot(m_pkObject->GetWorldRotV() + Vector3(0,90,0));
				m_pkCamera->SetRotM(m_pkObject->GetWorldRotM());
				
				strCamName = " 1P ";
	/*			if(madp)
					madp->m_bIsVisible = false;*/
				//m_pkCamera->SetFov(m_fFov);
				break;
			}
			case CAM_TYPETOPDOWN:
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(0,10,0));
				m_pkCamera->SetRot(Vector3(90,0,0));
				strCamName = " TD ";
/*				if(madp)
					madp->m_bIsVisible = true;*/
				//m_pkCamera->SetFov(90);
				break;

			case CAM_TYPEISO:
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(10,10,10));
				m_pkCamera->SetRot(Vector3(45,-45,0));
				strCamName = " ISO ";
	/*			if(madp)
					madp->m_bIsVisible = true;*/
				//m_pkCamera->SetFov(m_fFov);
				break;

				
			case CAM_TYPECHASE:
				kYawVector = GetYawVector2(m_pkObject->GetWorldRotV().y);

				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() - (kYawVector * CHASE_CAM_DISTANCE) + 
					Vector3(0,CHASE_CAM_DISTANCE,0));
				m_pkCamera->SetRot(Vector3(m_pkObject->GetWorldRotV() + Vector3(0,90,0)));
				strCamName = " Chase ";
/*				if(madp)
					madp->m_bIsVisible = true;*/
				//m_pkCamera->SetFov(m_fFov);
				break;

			case CAM_TYPEDYNAMICISO:
				kYawVector = GetYawVector2(m_kDynamicIso.y);

				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() - (kYawVector * CHASE_CAM_DISTANCE) + 
					Vector3(0,CHASE_CAM_DISTANCE,0));
				m_pkCamera->SetRot(Vector3(m_kDynamicIso + Vector3(0,90,0)));
				strCamName = " Dyn ISO ";
/*				if(madp)
					madp->m_bIsVisible = true;*/
				//m_pkCamera->SetFov(m_fFov);
				break;
				
			case CAM_TYPESIDE:
				strCamName = " Side ";
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(0,0,10));
				m_pkCamera->SetRot(Vector3(0,20,0));
	/*			if(madp)
					madp->m_bIsVisible = true;*/
				//m_pkCamera->SetFov(90);
				break;
		
		}
	}

	m_pkCamera->SetName(strCamName);
}

void P_Camera::SetType(CamType_e kType)
{
	m_eCameraType = kType;
}

void P_Camera::NextType(CamType_e kType)
{
	int iType = m_eCameraType;
	iType++;
	if(iType >= CAM_TYPEMAX)
		iType = CAM_TYPEFIRSTPERSON;

	SetType((CamType_e)iType);
}

void P_Camera::SetFpFov(float fFov)
{
	m_fFov = fFov;
	
}


void P_Camera::LookAt(Vector3 kCamPosition, Vector3 kCamTarget,Vector3 kCamUp) 
{
	Vector3 kLookDir = kCamPosition - kCamTarget;
	Look(kCamPosition, kLookDir, kCamUp);
}

void P_Camera::Look(Vector3 kCamPosition, Vector3 kLookDir,Vector3 kCamUp) {
	Matrix4 kCamera;

   if ( kLookDir == kLookDir.ZERO )
      return;

	kCamera.LookDir(kLookDir,kCamUp);

/*	Vector3 kRight;
	
	kLookDir.Normalize();
	kCamUp.Normalize();

	kRight = kCamUp.Cross(kLookDir);
	kRight.Normalize();
	kCamUp = kLookDir.Cross(kRight);
	kCamUp.Normalize();

	kCamera.Identity();
	kCamera.SetAxis(0,kRight);
	kCamera.SetAxis(1,kCamUp);
	kCamera.SetAxis(2,kLookDir);
	kCamera.Transponse();
*/
	m_pkCamera->SetRotM(kCamera);
	m_pkCamera->SetPos(kCamPosition);
}



Property* Create_CameraProperty()
{
	return new P_Camera();
}


