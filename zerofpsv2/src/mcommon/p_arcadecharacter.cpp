#include "p_arcadecharacter.h"
#include "p_dmgun.h"

P_ArcadeCharacter::P_ArcadeCharacter()
{
	strcpy(m_acName,"P_ArcadeCharacter");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = true;

	m_fSpeed = 20;
	m_kDir.Set(0,0,-1);
	m_kActions.reset();
}

P_ArcadeCharacter::~P_ArcadeCharacter()
{
	
}


void P_ArcadeCharacter::Init()
{
	cout<< "New character created"<<endl;


}

void P_ArcadeCharacter::Update()
{
	if(P_Tcs* pkTcs = (P_Tcs*)GetObject()->GetProperty("P_Tcs"))
	{
		Vector3 kCurrentDir = GetObject()->GetWorldRotM().VectorTransform(Vector3(0,0,1));	
/*		Vector3 kSide = kCurrentDir.Cross(Vector3(0,1,0));

		if(m_kDir.Length() == 0)
			m_kDir = Vector3(0,0,-1);		
		
		m_kDir.Normalize();				
		kCurrentDir.Normalize();
		
		float fS = m_kDir.Dot(kSide);		
		float fAng = kCurrentDir.Angle(m_kDir);

	//	cout<<fAng<<"  "<<kCurrentDir.Length()<<endl;
		
		
		if(fS > 0)
			GetObject()->RotateLocalRotV(Vector3(0, 10 *fAng,0));
		else
			GetObject()->RotateLocalRotV(Vector3(0,-10 *fAng,0));
*/		
		
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
			float fWalkRatio = (kVel.Unit().Dot(kCurrentDir.Unit()) / 2.0) + 0.5 ;
			
			kVel.Normalize();
			kVel *= ( (m_fSpeed/2.0)*fWalkRatio) + (m_fSpeed/2.0);					
				
			pkTcs->SetWalkVel(kVel);
		}
	}
	else
		cout<<"error p_arcadecjaracter missing p_tcs"<<endl;			



	//fire gun 
	if(m_kActions[4])
		Fire();
}

void P_ArcadeCharacter::Fire()
{
	if(P_DMGun* pkGun = (P_DMGun*)GetObject()->GetProperty ("P_DMGun"))
	{
		Vector3 kDir = GetObject()->GetWorldRotM().VectorTransform(Vector3(0,0,1));	
	
		pkGun->Fire( GetObject()->GetWorldPosV()+Vector3(0,-0.4,0) + kDir.Unit());
	}
	else
		cout<<"missing P_DMGun"<<endl;
}



Property* Create_P_ArcadeCharacter()
{
	return new P_ArcadeCharacter;
}