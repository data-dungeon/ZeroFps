#include "p_camera.h"
#include "p_mad.h"

#define CHASE_CAM_DISTANCE	5 

 
P_Camera::P_Camera() 
{
	m_pkCamera = NULL;
	m_eCameraType = CAM_TYPEFIRSTPERSON;
	strcpy(m_acName,"P_Camera");	

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	m_fFov = 90;
	m_kDynamicIso.Set(0,0,0);
	m_kInterPos.Set(0,0,0);
	m_f3PYAngle = 0;
	m_f3PPAngle = 0;
	m_f3PDistance = 3;
	m_kOffset.Set(0,2.5,0);

} 


void P_Camera::Update() 
{
	if(!m_pkCamera)
		return;

	Vector3		kYawVector;
	string		strCamName;


	//P_Mad* madp = dynamic_cast<P_Mad*>(m_pkObject->GetProperty("P_Mad"));

	if(m_pkCamera!=NULL) {
		switch(m_eCameraType) {
			case CAM_TYPEBIRDSEYE:
			{
				Matrix4 r;
				r.Identity();
				r.Rotate(Vector3(-90,0,0));
				m_pkCamera->SetRotM(r);
				
				Vector3 kPos = m_pkObject->GetIWorldPosV();
				Vector3 dir = kPos - m_kInterPos;
				
				m_kInterPos +=dir/8;
								
				float xp = sin(m_f3PYAngle);
				float zp = cos(m_f3PYAngle);				
				float yp = tan(m_f3PPAngle);								
				
				Vector3 campos = m_kInterPos + Vector3(xp,yp,zp).Unit() * m_f3PDistance;
				
				LookAt(campos, m_kInterPos,Vector3(0,1,0));
			
				strCamName = " BirdeEye ";
			
				break;		
			}
		
			case CAM_TYPE3PERSON:
			{
				Vector3 dir = m_pkObject->GetIWorldPosV() - m_kInterPos;
				m_kInterPos +=dir/8;
				
				
				Matrix4 kRot;
				kRot.Identity();
				kRot.Rotate(RadToDeg(m_f3PPAngle),RadToDeg(m_f3PYAngle),0);
				
				m_pkCamera->SetRotM(kRot);
				
				Vector3 offset = Vector3(0,0,1);
				kRot.Transponse();
				Vector3 pos = m_kInterPos + m_kOffset + kRot.VectorRotate(offset)*m_f3PDistance;
				m_pkCamera->SetPos(pos);
				
				strCamName = " 3P ";
				break;
			}	
			case CAM_TYPEFIRSTPERSON:
			{
				m_pkCamera->SetPos(m_pkObject->GetIWorldPosV() + Vector3(0,1.0,0) );
				Matrix4 kMat4;
				kMat4 = m_pkObject->GetWorldRotM();		
				kMat4.Transponse();				
				m_pkCamera->SetRotM(kMat4);
				strCamName = " 1P ";

				break;
			}
			case CAM_TYPETOPDOWN:
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(0,10,0));
				m_pkCamera->SetRot(Vector3(90,0,0));
				strCamName = " TD ";

				break;

			case CAM_TYPEISO:
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(10,10,10));
				m_pkCamera->SetRot(Vector3(45,-45,0));
				strCamName = " ISO ";

				break;

				
			case CAM_TYPECHASE:
				kYawVector = GetYawVector2(m_pkObject->GetWorldRotV().y);

				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() - (kYawVector * CHASE_CAM_DISTANCE) + 
					Vector3(0,CHASE_CAM_DISTANCE,0));
				m_pkCamera->SetRot(Vector3(m_pkObject->GetWorldRotV() + Vector3(0,90,0)));
				strCamName = " Chase ";

				break;

			case CAM_TYPEDYNAMICISO:
				kYawVector = GetYawVector2(m_kDynamicIso.y);

				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() - (kYawVector * CHASE_CAM_DISTANCE) + 
					Vector3(0,CHASE_CAM_DISTANCE,0));
				m_pkCamera->SetRot(Vector3(m_kDynamicIso + Vector3(0,90,0)));
				strCamName = " Dyn ISO ";

				break;
				
			case CAM_TYPESIDE:
				strCamName = " Side ";
				m_pkCamera->SetPos(m_pkObject->GetWorldPosV() + Vector3(0,0,10));
				m_pkCamera->SetRot(Vector3(0,20,0));

				break;
		
		}
	}

//	m_pkCamera->SetName(strCamName);
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

	m_pkCamera->SetRotM(kCamera);
	m_pkCamera->SetPos(kCamPosition);
}

void P_Camera::OrthoMove(Vector3 kMove)
{
	if(!m_pkCamera)	
		return;

	Vector3 kPos = m_pkObject->GetLocalPosV();
	kPos += m_pkCamera->GetOrthoAxisX() * kMove.x;
	kPos += m_pkCamera->GetOrthoAxisY() * kMove.y;
	m_pkObject->SetLocalPosV(kPos);
}

void P_Camera::SetCamera(Camera *pkCamera) 
{
	if(m_pkCamera)		
		m_pkCamera->m_iEntity = -1;
	
	m_pkCamera = pkCamera; 
	
	if(m_pkCamera)
	{
		m_pkCamera->m_iEntity = m_pkObject->GetEntityID();		
		
		//cout<<"Attached a new camera"<<endl;		
		//cout<<"current camera position "<<m_pkCamera->GetPos().x<<" "<<m_pkCamera->GetPos().y<<" "<<m_pkCamera->GetPos().z<<endl;
		//cout<<"new position "<<m_pkObject->GetWorldPosV().x<<" "<< m_pkObject->GetWorldPosV().y<<" "<<m_pkObject->GetWorldPosV().z<<endl;
	}
}


Property* Create_CameraProperty()
{
	return new P_Camera();
}


