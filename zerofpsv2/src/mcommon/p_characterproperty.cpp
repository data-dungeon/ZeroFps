#include "p_characterproperty.h"
#include "p_charactercontrol.h"



P_CharacterProperty::P_CharacterProperty()
{
	m_pkAudioSystem = m_pkZeroFps->m_pkAudioSystem;

	strcpy(m_acName,"P_CharacterProperty");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;

	bNetwork = 		true;
	m_iVersion = 	1;
	
	m_kCurrentCharacterStates.reset();
	
	m_strName 				=	"NoName";
	m_strOwnedByPlayer 	=	"NoPlayer";
	m_bIsPlayerCharacter =	false;
	
	//basic sounds
	m_strWalkSound			=	"data/sound/footstep_forest.wav";
	m_strRunSound			=	"data/sound/footstep_forest_run.wav";
	m_strJumpSound			=	"data/sound/jump.wav";
	m_strSwimSound			=	"swim.wav";	
	m_iWalkSoundID 		= 	-1;
	m_iRunSoundID 			= 	-1;
	m_iSwimSoundID 		= 	-1;
	
	//animations
	m_strWalkForward		=	"walk_forward";
	m_strWalkBackward		=	"walk_backward";
	m_strWalkSideway		=	"walk_sideway";
	m_strRunForward		=	"run_forward";
	m_strRunBackward		=	"run_backward";
	m_strRunSideway		=	"run_sideway";
	m_strSwimForward		=	"swim_forward";
	m_strSwimBackward		=	"swim_backward";
	m_strSwimSideway		=	"swimsideway";
	m_strJump				=	"jump";
}


P_CharacterProperty::~P_CharacterProperty()
{
	//make sure all sounds are stoped
	m_pkAudioSystem->StopSound(m_iWalkSoundID);
	m_pkAudioSystem->StopSound(m_iRunSoundID);
	m_pkAudioSystem->StopSound(m_iSwimSoundID);

}


void P_CharacterProperty::Update()
{
	if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_SERVER))
	{
		UpdateAnimation();
	}
		
	if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_CLIENT))
	{
		PlayCharacterMovementSounds();			
	}
	
	
}

void P_CharacterProperty::UpdateAnimation()
{
	if(P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad"))
	{
		if(P_CharacterControl* pkCC = (P_CharacterControl*)GetEntity()->GetProperty("P_CharacterControl"))
		{		
			if(pkCC->GetCharacterState(eJUMPING))
			{			
			
			}
			else if(pkCC->GetCharacterState(eRUNNING))
			{
			
			}
			else if(pkCC->GetCharacterState(eWALKING))
			{
				
			}
			else if(pkCC->GetCharacterState(eSWIMING))
			{
				
			}
			else
			{
				//idle
				
				
			}		
		}
	}
}

void P_CharacterProperty::PlayCharacterMovementSounds()
{
	if(P_CharacterControl* pkCC = (P_CharacterControl*)GetEntity()->GetProperty("P_CharacterControl"))
	{
		//check states and play sounds
		
		//walk sound
		if(pkCC->GetCharacterState(eWALKING))
		{
			if(!m_kCurrentCharacterStates[eWALKING])
			{
				//m_pkAudioSystem->StopSound(m_iWalkSoundID);
				m_iWalkSoundID = m_pkAudioSystem->StartSound(m_strWalkSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),true);
			}
			else
				m_pkAudioSystem->MoveSound(m_iWalkSoundID, GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eWALKING])
				m_pkAudioSystem->StopSound(m_iWalkSoundID);
		}	
		
		//run sound
		if(pkCC->GetCharacterState(eRUNNING))
		{
			if(!m_kCurrentCharacterStates[eRUNNING])
			{
				//m_pkAudioSystem->StopSound(m_iRunSoundID);
				m_iRunSoundID = m_pkAudioSystem->StartSound(m_strRunSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),true);
			}
			else
				m_pkAudioSystem->MoveSound(m_iRunSoundID, GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eRUNNING])
				m_pkAudioSystem->StopSound(m_iRunSoundID);
		}	
		
				
		
		//swim sound
		if(pkCC->GetCharacterState(eSWIMING))
		{
			if(!m_kCurrentCharacterStates[eSWIMING])
			{	
				//m_pkAudioSystem->StopSound(m_iSwimSoundID);
				m_iSwimSoundID = m_pkAudioSystem->StartSound(m_strSwimSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),true);
			}
			else
				m_pkAudioSystem->MoveSound(m_iSwimSoundID,GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eSWIMING])
				m_pkAudioSystem->StopSound(m_iSwimSoundID);
		}			

		//jump sound
		if(pkCC->GetCharacterState(eJUMPING))
			if(!m_kCurrentCharacterStates[eJUMPING])
				m_pkAudioSystem->StartSound(m_strJumpSound,GetEntity()->GetIWorldPosV());
		
														
		//update staes
		m_kCurrentCharacterStates[eRUNNING] = pkCC->GetCharacterState(eRUNNING);
		m_kCurrentCharacterStates[eWALKING] = pkCC->GetCharacterState(eWALKING);
		m_kCurrentCharacterStates[eJUMPING] = pkCC->GetCharacterState(eJUMPING);
		m_kCurrentCharacterStates[eSWIMING] = pkCC->GetCharacterState(eSWIMING);
	}
}


void P_CharacterProperty::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write_Str(m_strName);
	pkPackage->Write_Str(m_strOwnedByPlayer);
	pkPackage->Write(m_bIsPlayerCharacter);
}

void P_CharacterProperty::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read_Str(m_strName);	
	pkPackage->Read_Str(m_strOwnedByPlayer);	
	pkPackage->Read(m_bIsPlayerCharacter); 
}

void P_CharacterProperty::PackTo( NetPacket* pkNetPacket, int iConnectionID ) 
{
	pkNetPacket->Write_Str(m_strName);
	pkNetPacket->Write_Str(m_strOwnedByPlayer);
	pkNetPacket->Write(m_bIsPlayerCharacter);
	
	pkNetPacket->Write_Str(m_strWalkSound);
	pkNetPacket->Write_Str(m_strRunSound);
	pkNetPacket->Write_Str(m_strJumpSound);
	pkNetPacket->Write_Str(m_strSwimSound);
	
	SetNetUpdateFlag(iConnectionID,false);
}

void P_CharacterProperty::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read_Str(m_strName);
	pkNetPacket->Read_Str(m_strOwnedByPlayer);
	pkNetPacket->Read(m_bIsPlayerCharacter);

	pkNetPacket->Read_Str(m_strWalkSound);
	pkNetPacket->Read_Str(m_strRunSound);
	pkNetPacket->Read_Str(m_strJumpSound);
	pkNetPacket->Read_Str(m_strSwimSound);		

}




Property* Create_P_CharacterProperty()
{
	return new P_CharacterProperty;
}





