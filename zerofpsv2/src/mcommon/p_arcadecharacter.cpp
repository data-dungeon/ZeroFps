#include "p_arcadecharacter.h"
#include "p_dmgun.h"

P_ArcadeCharacter::P_ArcadeCharacter()
{
	strcpy(m_acName,"P_ArcadeCharacter");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = true;

	m_fSpeed = 100;
	m_kDir.Set(0,0,-1);
	m_kAim.Set(0,0,-1);
	m_iTarget	= -1;
	m_kActions.reset();


	m_kCameraRotation.Identity();
	m_kCameraRotation.Rotate(0,58,0);

}



P_ArcadeCharacter::~P_ArcadeCharacter()
{
	
}


void P_ArcadeCharacter::Init()
{
	cout<< "New character created"<<endl;
	GetObject()->SetInterpolate(true);

}

void P_ArcadeCharacter::Update()
{
	if(P_Tcs* pkTcs = (P_Tcs*)GetObject()->GetProperty("P_Tcs"))
	{
		Vector3 kDir = m_kCameraRotation.VectorTransform(m_kDir);
	
		Vector3 kCurrentDir = GetObject()->GetWorldRotM().VectorTransform(Vector3(0,0,1));			
//		kCurrentDir = m_kCameraRotation.VectorTransform(kCurrentDir);		
		Vector3 kSide = kCurrentDir.Cross(Vector3(0,1,0));

		if(kDir.Length() == 0)
			kDir = Vector3(0,0,-1);		
		
		kDir.Normalize();				
		kCurrentDir.Normalize();
		
		float fS = kDir.Dot(kSide);		
		float fAng = kCurrentDir.Angle(kDir);

		//cout<<fAng<<"  "<<kCurrentDir.Length()<<endl;
		
		if(fAng > 0.01)
		{
			if(fS > 0)
				GetObject()->RotateLocalRotV(Vector3(0, 15 * fAng,0));
			else
				GetObject()->RotateLocalRotV(Vector3(0,-15 * fAng ,0));
		}
		
		
/*		
		
		Vector3 kNewDir = m_kDir  ;
		if(kNewDir.Length() == 0)
			kNewDir.Set(0,0,-1);
			
		kNewDir.Normalize();
		
		//rotate player
		Matrix4 kRot;		
		kRot = GetObject()->GetWorldRotM();
		kRot.LookDir(kNewDir,Vector3(0,1,0));		
		kRot.Transponse();
		GetObject()->SetLocalRotM(kRot);
*/		

		//move character
		Vector3 kVel(0,0,0);	
		if(m_kActions[0])
			kVel.z = -1;
		if(m_kActions[1])
			kVel.z =  1;
		if(m_kActions[2])
			kVel.x = -1;
		if(m_kActions[3])
			kVel.x =  1;

								
		if(kVel.Length() == 0)			
			pkTcs->SetWalkVel(Vector3(0,0,0));
		else
		{
		
			kVel = m_kCameraRotation.VectorTransform(kVel);			
			kVel.y = 0;		
			kVel.Normalize();
			
			float fWalkRatio = (kVel.Unit().Dot(kCurrentDir.Unit()) / 2.0) + 0.5 ;
			
			kVel *= ( (m_fSpeed/2.0)*fWalkRatio) + (m_fSpeed/2.0);					
				
			pkTcs->SetWalkVel(kVel);
		}
	}
	else
		cout<<"error p_arcadecjaracter missing p_tcs"<<endl;			


	//update target
	AutoAim();
	//m_kAim = kCurrentDir;

				
	//fire gun 
	if(m_kActions[4])
	{
		Fire();
	}
	

}

void P_ArcadeCharacter::Fire()
{
	if(P_DMGun* pkGun = (P_DMGun*)GetObject()->GetProperty ("P_DMGun"))
	{
		//Vector3 kDir = GetObject()->GetWorldRotM().VectorTransform(Vector3(0,0,1));	
	
		pkGun->Fire( GetObject()->GetWorldPosV() + m_kAim.Unit());
	}
	else
		cout<<"missing P_DMGun"<<endl;
}

void P_ArcadeCharacter::AutoAim()
{


	vector<Entity*> kObjects;	
	m_pkObjMan->GetZoneObject()->GetAllEntitys(&kObjects);	
	

	Vector3 kDir = GetObject()->GetWorldRotM().VectorTransform(Vector3(0,0,1));			
	Vector3 kStart = m_pkObject->GetWorldPosV()+kDir;

	
	float closest = 999999999;
	float d ;
	Entity* pkClosest = NULL;
	
	for(unsigned int i=0;i<kObjects.size();i++)
	{
		if(kObjects[i] == m_pkObject)
			continue;

		if(kObjects[i]->IsZone())
			continue;
							
		//get mad property and do a linetest		
		if(P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad"))
		{
			if(mp->TestLine(kStart,kDir,true,true))
			{
				d = kStart.DistanceTo(kObjects[i]->GetWorldPosV());

				if(d < closest)
				{
					closest = d;
					pkClosest = kObjects[i];
				}
			}
		}			
	}
	
	if(pkClosest == NULL)
	{
		m_kAim = kDir.Unit();
		m_iTarget = -1;
	}
	else
	{
		Vector3 kPos = kStart+kDir * closest;
		kPos.y = pkClosest->GetWorldPosV().y;
	
		m_kAim = kPos - m_pkObject->GetWorldPosV();
		
		if(m_kAim.Length() == 0)
			m_kAim.Set(0,0,1);
		else
			m_kAim.Unit();
			
			
		m_iTarget = pkClosest->GetEntityID();
		
	}
}

Property* Create_P_ArcadeCharacter()
{
	return new P_ArcadeCharacter;
}










