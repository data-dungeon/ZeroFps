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

	Vector3 kVel(0,0,0);	

	bool bHoppa = false;

	if(P_Tcs* pkTcs = (P_Tcs*)GetEntity()->GetProperty("P_Tcs"))
	{
		
			
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
					bHoppa = true;
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
		if(bHoppa)
		{
			pkSound->StartSound("data/sound/jump.wav", false);
		}

		enum MOVE_STATE { idle, moving };
		static MOVE_STATE move_state = idle;

		static Vector3 prevpos = Vector3(-9999,-9999,-9999);
		Vector3 currpos = pkEnt->GetWorldPosV();

		if(prevpos.NearlyEquals(currpos,0.1f)) // står stilla
		{
			if(move_state == moving)
			{
				move_state = idle;
				pkSound->StopSound("data/sound/footstep_forest.wav");
			}
		}
		else
		if(move_state == idle && !kVel.NearlyZero(0.1f) )
		{
			move_state = moving;

			if(!bHoppa)
			{
				pkSound->StartSound("data/sound/footstep_forest.wav", true);
			}
		}

		prevpos = currpos;

	}
	
}

Property* Create_P_CharacterControl()
{
	return new P_CharacterControl;
}

