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
	
	m_kDynamicIso = 			Vector3(0,0,0);
	m_kInterPos =				Vector3(0,0,0);		
	m_fFov = 					90;
	m_f3PYAngle = 				0;
	m_f3PPAngle = 				0;
	m_f3PDistance = 			3;
	m_f3pCurrentDistance = 	3;
	m_kOffset =					Vector3(0,2.5,0);

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
		
			case CAM_TYPE3PERSON:
			{	
				if(m_f3PPAngle > 45)
					m_f3PPAngle = 45;
				if(m_f3PPAngle < -45)
					m_f3PPAngle = -45;
 	
				Matrix4 kRot;
				kRot.Identity();
				
				kRot.Rotate(m_f3PPAngle,m_f3PYAngle,0);
				kRot.Transponse();
				
				Vector3 kOffset;				
				kOffset = kRot.VectorTransform(Vector3(0,0,-1));
				kOffset *= m_f3PDistance;									
//	 			kOffset += m_kOffset;
				
				//check camera against enviroment so nothing is betwean camera and player				
				float fD = LineTest(m_pkEntity->GetIWorldPosV() +m_kOffset,m_pkEntity->GetIWorldPosV() + m_kOffset+kOffset);				
				static float fZS = 0.2;
				if(fD < m_f3PDistance)
				{
/*					m_f3pCurrentDistance -= fZS;
					
					kOffset.Set(0,0,-1);				
					kOffset = kRot.VectorTransform(kOffset);
					kOffset *= m_f3pCurrentDistance;									
		 			kOffset += m_kOffset;
*/					
					float fDist = fD - 0.5;
					if(fDist <= 0.1)
						fDist = 0.1;
					
					kOffset = kRot.VectorTransform(Vector3(0,0,-1));
					kOffset *= fDist;	
		 			//kOffset += m_kOffset;				
				}
/*				else
					if(m_f3pCurrentDistance < m_f3PDistance - fZS)
						m_f3pCurrentDistance += fZS;
					else
						if(m_f3pCurrentDistance > m_f3PDistance + fZS)
							m_f3pCurrentDistance -= fZS;
*/
				
										
												
				LookAt(m_pkEntity->GetIWorldPosV() +m_kOffset+ kOffset,m_pkEntity->GetIWorldPosV() + m_kOffset,Vector3(0,1,0));
				
				

				strCamName = " 3P ";
				break;
			}	
			case CAM_TYPEFIRSTPERSON_NON_EA:
			{				
				if(m_f3PPAngle > 90)
					m_f3PPAngle = 90;
				if(m_f3PPAngle < -90)
					m_f3PPAngle = -90;
				
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
				kYawVector = GetYawVector2(m_pkEntity->GetWorldRotV().y);

				m_pkCamera->SetPos(m_pkEntity->GetWorldPosV() - (kYawVector * CHASE_CAM_DISTANCE) + 
					Vector3(0,CHASE_CAM_DISTANCE,0));
				m_pkCamera->SetRot(Vector3(m_pkEntity->GetWorldRotV() + Vector3(0,90,0)));
				strCamName = " Chase ";

				break;

			case CAM_TYPEDYNAMICISO:
				kYawVector = GetYawVector2(m_kDynamicIso.y);

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

	if(kLookDir.NearlyZero(0.001))
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
	//set old camera's entityID to -1
	if(m_pkCamera)		
		m_pkCamera->m_iEntity = -1;
	
	//set new camera
	m_pkCamera = pkCamera; 
	
	//update camera entity ID 
	if(m_pkCamera)
	{
		m_pkCamera->m_iEntity = m_pkEntity->GetEntityID();		
		
		//cout<<"Attached a new camera to" << m_pkEntity->GetType() << endl;		
		//cout<<"current camera position "<<m_pkCamera->GetPos().x<<" "<<m_pkCamera->GetPos().y<<" "<<m_pkCamera->GetPos().z<<endl;
		//cout<<"new position "<<m_pkObject->GetWorldPosV().x<<" "<< m_pkObject->GetWorldPosV().y<<" "<<m_pkObject->GetWorldPosV().z<<endl;
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
			
		//if(!kObjects[i]->IsZone())
		//	continue;
			
		//get mad property and do a linetest
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


