#include "p_charactercontrol.h"


P_CharacterControl::P_CharacterControl()
{
	strcpy(m_acName,"P_CharacterControl");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = false;

	m_iVersion = 1;
	
	m_fYAngle = 0;
	m_fPAngle = 0;	
	m_fJumpDelay = 0;
	
	m_fSpeed = 75.0;
	m_fJumpForce = 4.0; 
	
	m_eMoveState = idle;
	m_kPrevPos = Vector3(-9999,-9999,-9999);
	m_bMoveButtonReleased = true;
}

P_CharacterControl::~P_CharacterControl()
{

}


void P_CharacterControl::Init()
{
	m_kControls.reset();
}

void P_CharacterControl::Update()
{	
	bool bMoveButtonPressed = false;

	if(P_Tcs* pkTcs = (P_Tcs*)GetEntity()->GetProperty("P_Tcs"))
	{
		Vector3 kVel(0,0,0);	

		if(m_kControls[eUP])
		{ kVel.z =  1; bMoveButtonPressed=true; }
		if(m_kControls[eDOWN])
		{ kVel.z = -1; bMoveButtonPressed=true; }
		if(m_kControls[eLEFT])
		{ kVel.x =  1; bMoveButtonPressed=true; }
		if(m_kControls[eRIGHT])
		{ kVel.x = -1; bMoveButtonPressed=true; }
	
		//transform velocity
		kVel = GetEntity()->GetWorldRotM().VectorTransform(kVel);							
		kVel.y = 0;
			
		if(kVel.Length() > 0)
			kVel = kVel.Unit() * m_fSpeed;
		
		if(m_kControls[eCRAWL])
			kVel *= 0.5;
			
		//character moves slower while in the air
		if(!pkTcs->GetOnGround())
			kVel *= 0.25;
		
		pkTcs->ApplyForce(Vector3(0,0,0),kVel);
		
		if(m_kControls[eJUMP])
			if(pkTcs->GetOnGround())
				if( (m_pkZeroFps->GetTicks() - m_fJumpDelay) > 0.1)
				{
					m_fJumpDelay = m_pkZeroFps->GetTicks();
					pkTcs->ApplyImpulsForce(Vector3(0,m_fJumpForce,0));

					if(P_Sound* pkSound = (P_Sound*)GetEntity()->GetProperty("P_Sound"))
						pkSound->StartSound("data/sound/jump.wav", false);
				}		
	}

	Matrix4 kRot;
	kRot.Identity();
	kRot.Rotate(0,m_fYAngle,0);
	kRot.Transponse();				
	GetEntity()->SetLocalRotM(kRot);	

	if(P_Sound* pkSound = (P_Sound*)GetEntity()->GetProperty("P_Sound"))
	{
		if(bMoveButtonPressed == true)
		{
			if(m_bMoveButtonReleased == true)
			{
				m_bMoveButtonReleased = false;
				pkSound->StartSound("data/sound/footstep_forest.wav", true);
			}
		}
		else
		{
			if(m_bMoveButtonReleased == false)
				pkSound->StopSound("data/sound/footstep_forest.wav");

			m_bMoveButtonReleased = true;
		}
	}
	
}

Property* Create_P_CharacterControl()
{
	return new P_CharacterControl;
}

