#include "p_charactercontrol.h"
#include "../zerofpsv2/engine_systems/propertys/p_sound.h"


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

	if(P_Tcs* pkTcs = (P_Tcs*)GetEntity()->GetProperty("P_Tcs"))
	{
		Vector3 kVel(0,0,0);	
			
		if(m_kControls[eUP])
			kVel.z = 1;
		if(m_kControls[eDOWN])
			kVel.z = -1;
		if(m_kControls[eLEFT])
			kVel.x = 1;
		if(m_kControls[eRIGHT])
			kVel.x = -1;
			
		//transform velocity
		kVel = GetEntity()->GetWorldRotM().VectorTransform(kVel);							
		kVel.y = 0;
			
		if(kVel.Length() > 0)
			kVel = kVel.Unit() * m_fSpeed;
		
		pkTcs->ApplyForce(Vector3(0,0,0),kVel);
		
		if(m_kControls[eJUMP])
			if(pkTcs->GetOnGround())
				if( (m_pkZeroFps->GetTicks() - m_fJumpDelay) > 0.1)
				{
					m_fJumpDelay = m_pkZeroFps->GetTicks();
					pkTcs->ApplyImpulsForce(Vector3(0,m_fJumpForce,0));
				}		
	}

		
	Matrix4 kRot;
	kRot.Identity();
	kRot.Rotate(0,m_fYAngle,0);
	kRot.Transponse();				
	GetEntity()->SetLocalRotM(kRot);	

	Entity* pkEnt = GetEntity();

	// Spela upp ett walkljud
	// OBS! Detta skall flyttas till en kommande Characterklass.
	if(P_Sound* pkSound = (P_Sound*)pkEnt->GetProperty("P_Sound"))
	{
		enum MOVE_STATE { idle, moving };
		static MOVE_STATE move_state = idle;

		if(pkEnt->GetVel().NearlyZero(1))
		{
			if(move_state == moving)
			{
				move_state = idle;
				pkSound->StopSound("data/sound/footstep_forest.wav");
			}
		}
		else
		if(move_state == idle)
		{
			move_state = moving;
			pkSound->StartSound("data/sound/footstep_forest.wav", true);
		}
	}
	
}

Property* Create_P_CharacterControl()
{
	return new P_CharacterControl;
}

