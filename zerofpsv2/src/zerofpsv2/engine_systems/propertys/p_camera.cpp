#include "p_camera.h"
#include "p_mad.h"

#include <iostream>
#include "../../engine/camera.h"
#include "../../engine/zerofps.h"
#include "../../basic/math.h"

#define CHASE_CAM_DISTANCE	5 

 
P_Camera::P_Camera() 
{
	strcpy(m_acName,"P_Camera");	

	//no sides no nothing, this property is always updated manualy by the camera
	m_iType=0;
	m_iSide=0;

	m_pkFps = static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));

	m_pkCamera = NULL;
	m_eCameraType = CAM_TYPEFIRSTPERSON;
		
	m_kDynamicIso = 			Vector3(0,0,0);
	m_kInterPos =				Vector3(0,0,0);		
	m_fFov = 					90;
	m_f3PYAngle = 				0;
	m_f3PPAngle = 				0;
	m_f3PDistance = 			3;
	m_f3pCurrentDistance = 	3;
	m_kOffset =					Vector3(0,2.5,0);

	
	m_bAttachToMadBone =		false;
	m_strBone			=		"eye";
} 

P_Camera::~P_Camera()
{
	SetCamera(NULL);
}

void P_Camera::Update() 
{
	if(!m_pkCamera)
		return;

// 	StartProfileTimer("r___Camera");
		
	Vector3		kYawVector;
	string		strCamName;

	if(m_pkCamera!=NULL) {
		switch(m_eCameraType) {
			case CAM_TYPEBIRDSEYE:
			{
				Matrix4 r;
				r.Identity();
				r.Rotate(Vector3(-90,0,0));
				m_pkCamera->SetRotM(r);
				
				Vector3 kPos = m_pkEntity->GetIWorldPosV();
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
		
			case CAM_TYPEMLFRONTVIEW:
			{
				float fYAngle = 180;
				float fPAngle = 0;
			
				Vector3 kCamPos;				
				if(m_bAttachToMadBone)
				{				
					if(P_Mad* pkMad = (P_Mad*)m_pkEntity->GetProperty("P_Mad"))
					{
						kCamPos = m_pkEntity->GetIWorldPosV() + pkMad->GetJointPosition(m_strBone) + m_kOffset;			
						
					}
				}
				else					
					kCamPos = m_pkEntity->GetIWorldPosV() + m_kOffset;					
			
			
				Matrix4 kRot;
				kRot.Identity();
				
				kRot.Rotate(fPAngle,m_f3PYAngle - 180 , 0);
				kRot.Transponse();
				
				Vector3 kOffset;				
				kOffset = kRot.VectorTransform(Vector3(0,0,-1));
				kOffset *= m_f3PDistance;									
				
				//check camera against enviroment so nothing is betwean camera and player				
				float fD = LineTest(kCamPos,kCamPos+kOffset);				
				static float fZS = 0.2;
				
				if(fD < m_f3PDistance)
				{
					float fDist = fD - 0.5;
					if(fDist <= 0.1)
						fDist = 0.1;
					
					kOffset = kRot.VectorTransform(Vector3(0,0,-1));
					kOffset *= fDist;	
				}
				
				
				LookAt(kCamPos + kOffset,kCamPos,Vector3(0,1,0));					
					
				strCamName = " 3P ";
				break;			
			}
			
			case CAM_TYPE3PERSON:
			{	
				if(m_f3PPAngle > 85)
					m_f3PPAngle = 85;
				if(m_f3PPAngle < -85)
					m_f3PPAngle = -85;

				
				Vector3 kCamPos;				
				if(m_bAttachToMadBone)
				{				
					if(P_Mad* pkMad = (P_Mad*)m_pkEntity->GetProperty("P_Mad"))
					{
						kCamPos = m_pkEntity->GetIWorldPosV() + pkMad->GetJointPosition(m_strBone) + m_kOffset;			
						
					}
				}
				else					
					kCamPos = m_pkEntity->GetIWorldPosV() + m_kOffset;					
				
					 	
				Matrix4 kRot;
				kRot.Identity();
				
				kRot.Rotate(m_f3PPAngle,m_f3PYAngle,0);
				kRot.Transponse();
				
				Vector3 kOffset;				
				kOffset = kRot.VectorTransform(Vector3(0,0,-1));
				kOffset *= m_f3PDistance;									
				
				//check camera against enviroment so nothing is betwean camera and player				
				float fD = LineTest(kCamPos,kCamPos+kOffset);				
				static float fZS = 0.2;
				
				if(fD < m_f3PDistance)
				{
					float fDist = fD - 0.5;
					if(fDist <= 0.1)
						fDist = 0.1;
					
					kOffset = kRot.VectorTransform(Vector3(0,0,-1));
					kOffset *= fDist;	
				}
				
				
				LookAt(kCamPos + kOffset,kCamPos,Vector3(0,1,0));

				strCamName = " 3P ";
				break;
			}	
			case CAM_TYPEFIRSTPERSON_NON_EA:
			{				
				if(m_f3PPAngle > 90)
					m_f3PPAngle = 90;
				if(m_f3PPAngle < -90)
					m_f3PPAngle = -90;
				
				if(m_bAttachToMadBone)
				{
					if(P_Mad* pkMad = (P_Mad*)m_pkEntity->GetProperty("P_Mad"))
						m_pkCamera->SetPos(m_pkEntity->GetIWorldPosV() + pkMad->GetJointPosition(m_strBone) + m_kOffset );			
				}
				else					
					m_pkCamera->SetPos(m_pkEntity->GetIWorldPosV() + m_kOffset );			
					
					
				Matrix4 kRot;											
				kRot.Identity();
				kRot.Rotate(0,180,0);				
				kRot.Rotate(-m_f3PPAngle,m_f3PYAngle,0);
			
				m_pkCamera->SetRotM(kRot);
				break;
			}			
			
			case CAM_TYPEFIRSTPERSON:
			{
				m_pkCamera->SetPos(m_pkEntity->GetIWorldPosV() + m_kOffset );
				Matrix4 kMat4;
				kMat4 = m_pkEntity->GetWorldRotM();		
				kMat4.Rotate(0,180,0);
				kMat4.Transponse();				
				m_pkCamera->SetRotM(kMat4);
				strCamName = " 1P ";

				break; 
			}
			case CAM_TYPETOPDOWN:
				m_pkCamera->SetPos(m_pkEntity->GetWorldPosV() + Vector3(0,10,0));
				m_pkCamera->SetRot(Vector3(90,0,0));
				strCamName = " TD ";

				break;

			case CAM_TYPEISO:
				m_pkCamera->SetPos(m_pkEntity->GetWorldPosV() + Vector3(10,10,10));
				m_pkCamera->SetRot(Vector3(45,-45,0));
				strCamName = " ISO ";

				break;

				
			case CAM_TYPECHASE:
				kYawVector = Math::GetYawVector2(m_pkEntity->GetWorldRotV().y);

				m_pkCamera->SetPos(m_pkEntity->GetWorldPosV() - (kYawVector * CHASE_CAM_DISTANCE) + 
					Vector3(0,CHASE_CAM_DISTANCE,0));
				m_pkCamera->SetRot(Vector3(m_pkEntity->GetWorldRotV() + Vector3(0,90,0)));
				strCamName = " Chase ";

				break;

			case CAM_TYPEDYNAMICISO:
				kYawVector = Math::GetYawVector2(m_kDynamicIso.y);

				m_pkCamera->SetPos(m_pkEntity->GetWorldPosV() - (kYawVector * CHASE_CAM_DISTANCE) + 
					Vector3(0,CHASE_CAM_DISTANCE,0));
				m_pkCamera->SetRot(Vector3(m_kDynamicIso + Vector3(0,90,0)));
				strCamName = " Dyn ISO ";

				break;
				
			case CAM_TYPESIDE:
				strCamName = " Side ";
				m_pkCamera->SetPos(m_pkEntity->GetWorldPosV() + Vector3(0,0,10));
				m_pkCamera->SetRot(Vector3(0,20,0));

				break;
		
		}
	}

	
// 	StopProfileTimer("r___Camera");	
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

	if(kLookDir.IsZero(0.001))
		return;
		
	kCamera.LookDir(kLookDir,kCamUp);

	m_pkCamera->SetRotM(kCamera);
	m_pkCamera->SetPos(kCamPosition);
}

void P_Camera::OrthoMove(Vector3 kMove)
{
	if(!m_pkCamera)	
		return;

	Vector3 kPos = m_pkEntity->GetLocalPosV();
	kPos += m_pkCamera->GetOrthoAxisX() * kMove.x;
	kPos += m_pkCamera->GetOrthoAxisY() * kMove.y;
	m_pkEntity->SetLocalPosV(kPos);
}

void P_Camera::SetCamera(Camera *pkCamera) 
{
	//set old camera's entityID to -1 and reset cameraproperty pointer
	if(m_pkCamera)
	{		
		m_pkCamera->m_iEntity = -1;
		m_pkCamera->m_pkCameraProp = NULL;
	}
		
	//set new camera
	m_pkCamera = pkCamera; 
	
	//update camera entity ID and prop pointer
	if(m_pkCamera)
	{
		m_pkCamera->m_iEntity = m_pkEntity->GetEntityID();		
		m_pkCamera->m_pkCameraProp = this;
	}
}

float P_Camera::LineTest(const Vector3& kStart,const Vector3& kStop)
{	
	Vector3 dir = (kStop - kStart).Unit();

	vector<Entity*> kObjects;
	m_pkEntityManager->GetZoneEntity()->GetAllEntitys(&kObjects);
	
	float closest = 999999999;
	Entity* pkClosest = NULL;	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		if(kObjects[i] == m_pkEntity)
			continue;
			

		P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
		if(mp)
		{
			if(mp->TestLine(kStart,dir))
			{	
				float d = (kStart - mp->GetLastColPos()).Length();
					
				if(d < closest)
				{
					closest = d;
					pkClosest = kObjects[i];
				}				
			}
		}				
	}
	
	return closest;
}


Property* Create_CameraProperty()
{
	return new P_Camera();
}


