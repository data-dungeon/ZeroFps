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
	
	m_strName =					"NoName";
	m_strOwnedByPlayer =		"NoPlayer";
	m_bIsPlayerCharacter =	false;
	
	m_strWalkSound			=	"data/sound/footstep_forest.wav";
	m_strRunSound			=	"data/sound/footstep_forest.wav";
	m_strJumpSound			=	"data/sound/jump.wav";
	m_strSwimSound			=	"swim.wav";
}


P_CharacterProperty::~P_CharacterProperty()
{


}


void P_CharacterProperty::Update()
{
	if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_CLIENT))
	{
		PlayCharacterMovementSounds();	
		
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
				m_pkAudioSystem->StopSound(m_strWalkSound,GetEntity()->GetIWorldPosV());
				m_pkAudioSystem->StartSound(m_strWalkSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),true);
			}
			else
				m_pkAudioSystem->MoveSound(m_strWalkSound.c_str(), GetEntity()->GetIWorldPosV(), GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eWALKING])
				m_pkAudioSystem->StopSound(m_strWalkSound,GetEntity()->GetIWorldPosV());
		}	
		
		//run sound
		if(pkCC->GetCharacterState(eRUNNING))
		{
			if(!m_kCurrentCharacterStates[eRUNNING])
			{
				m_pkAudioSystem->StopSound(m_strRunSound,GetEntity()->GetIWorldPosV());
				m_pkAudioSystem->StartSound(m_strRunSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),true);
			}
			else
				m_pkAudioSystem->MoveSound(m_strRunSound.c_str(), GetEntity()->GetIWorldPosV(), GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eRUNNING])
				m_pkAudioSystem->StopSound(m_strRunSound,GetEntity()->GetIWorldPosV());
		}			
			
		//swim sound
		if(pkCC->GetCharacterState(eSWIMING))
		{
			if(!m_kCurrentCharacterStates[eSWIMING])
			{
				m_pkAudioSystem->StopSound(m_strSwimSound,GetEntity()->GetIWorldPosV());
				m_pkAudioSystem->StartSound(m_strSwimSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),true);
			}
			else
				m_pkAudioSystem->MoveSound(m_strSwimSound.c_str(), GetEntity()->GetIWorldPosV(), GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eSWIMING])
				m_pkAudioSystem->StopSound(m_strSwimSound,GetEntity()->GetIWorldPosV());
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
/*	
	pkNetPacket->Write_Str(m_strWalkSound);
	pkNetPacket->Write_Str(m_strRunSound);
	pkNetPacket->Write_Str(m_strJumpSound);
	pkNetPacket->Write_Str(m_strSwimSound);
*/	
	SetNetUpdateFlag(iConnectionID,false);
}

void P_CharacterProperty::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read_Str(m_strName);
	pkNetPacket->Read_Str(m_strOwnedByPlayer);
	pkNetPacket->Read(m_bIsPlayerCharacter);
/*
	pkNetPacket->Read_Str(m_strWalkSound);
	pkNetPacket->Read_Str(m_strRunSound);
	pkNetPacket->Read_Str(m_strJumpSound);
	pkNetPacket->Read_Str(m_strSwimSound);		
*/	
}




Property* Create_P_CharacterProperty()
{
	return new P_CharacterProperty;
}





