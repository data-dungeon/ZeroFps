#include "p_charactercontrol.h"


P_CharacterControl::P_CharacterControl()
{
	strcpy(m_acName,"P_CharacterControl");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = true;

	m_iVersion = 1;
	
	m_fYAngle = 0;
	m_fPAngle = 0;	
	m_fSoundFixDelay = 0;
	
	m_fSpeed = 75.0;
	m_fJumpForce = 4.0; 
	
	m_bHaveJumped = false;
		
	m_kCharacterStates.reset();
	m_kControls.reset();
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
	if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_SERVER))
	{
		//reset character states
		//SetCharacterState(eRUNNING,false);
		//SetCharacterState(eWALKING,false);
		SetCharacterState(eJUMPING,false);
		SetCharacterState(eSWIMING,false);
			
		if(P_Tcs* pkTcs = (P_Tcs*)GetEntity()->GetProperty("P_Tcs"))
		{
			Vector3 kVel(0,0,0);	
	
			if(m_kControls[eUP]) 	kVel.z =  1;
			if(m_kControls[eDOWN])	kVel.z = -1;
			if(m_kControls[eLEFT])	kVel.x =  1; 
			if(m_kControls[eRIGHT])	kVel.x = -1; 
		
			//transform velocity
			kVel = GetEntity()->GetWorldRotM().VectorTransform(kVel);							
			kVel.y = 0;
			
			//multiply by character speed	
			if(kVel.Length() > 0)
				kVel = kVel.Unit() * m_fSpeed;
			
			//check if where crawling
			if(m_kControls[eCRAWL])
				kVel *= 0.5;
				
			//character moves slower while in the air
			if(!pkTcs->GetOnGround())
				kVel *= 0.25;
							
			//apply movement force					
			pkTcs->ApplyForce(Vector3(0,0,0),kVel);
			
			//check if where walking or running or nothing
			if(kVel.Length() > 0 && pkTcs->GetOnGround())
			{
				m_fSoundFixDelay = m_pkZeroFps->GetTicks();												
				if(m_kControls[eCRAWL])					
					SetCharacterState(eWALKING,true);
				else
					SetCharacterState(eRUNNING,true);
			}
					
			if(m_pkZeroFps->GetTicks() - m_fSoundFixDelay > 0.25)
			{
				SetCharacterState(eRUNNING,false);
				SetCharacterState(eWALKING,false);
			}
			
			
			//jump
			if(pkTcs->GetOnGround())
			{
				if(m_bHaveJumped)
					m_bHaveJumped = false;
				else
				{
					if(m_kControls[eJUMP])
					{
						if(pkTcs->GetOnGround())
						{
							m_bHaveJumped = true;
							
							pkTcs->ApplyImpulsForce(Vector3(0,m_fJumpForce,0));		
						}
					}
				}
			}
				
			if(m_bHaveJumped)
			{
				SetCharacterState(eJUMPING,true);
			}
			
		}
	
		Matrix4 kRot;
		kRot.Identity();
		kRot.Rotate(0,m_fYAngle,0);
		kRot.Transponse();				
		GetEntity()->SetLocalRotM(kRot);	
	}
	
	/*
	if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_SERVER))
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
	*/
}

void P_CharacterControl::PackTo( NetPacket* pkNetPacket, int iConnectionID ) 
{
	pkNetPacket->Write(m_kCharacterStates);
	
	SetNetUpdateFlag(iConnectionID,false);
}

void P_CharacterControl::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	//cout<<"blub:"<<sizeof(m_kCharacterStates)<<endl;
	
	pkNetPacket->Read(m_kCharacterStates);

	
	/*
	for(int i = 0;i<8;i++)
		if(m_kCharacterStates[i])
			cout<<"state:"<<i<<" true"<<endl;
		else
			cout<<"state:"<<i<<" false"<<endl;
	*/
}

void P_CharacterControl::SetCharacterState(int iState,bool bValue)
{
	if(iState < 0 || iState >= 8)
		return;
		
	if(m_kCharacterStates[iState] == bValue)
		return;

			
	m_kCharacterStates[iState] = bValue;
	SetNetUpdateFlag(true);
}

bool P_CharacterControl::GetCharacterState(int iState)
{
	if(iState < 0 || iState >= 8)
		return false;

	return m_kCharacterStates[iState];
}

Property* Create_P_CharacterControl()
{
	return new P_CharacterControl;
}

