#include "p_characterproperty.h"
#include "p_charactercontrol.h"



P_CharacterProperty::P_CharacterProperty()
{
	m_pkAudioSystem = 	static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));			
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
	m_pkEntityMan=			static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));			
	
	strcpy(m_acName,"P_CharacterProperty");
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;

	bNetwork = 		true;
	m_iVersion = 	1;
	
	m_kCurrentCharacterStates.reset();
	
	m_strName 				=	"NoName";
	m_strOwnedByPlayer 	=	"NoPlayer";
	m_bIsPlayerCharacter =	false;
	m_bOverHeadText		=	true;
		
	//basic sounds
	m_strWalkSound			=	"data/sound/footstep_forest.wav";
	m_strRunSound			=	"data/sound/footstep_forest_run.wav";
	m_strJumpSound			=	"data/sound/jump.wav";
	m_strSwimSound			=	"swim.wav";	
	m_iWalkSoundID 		= 	-1;
	m_iRunSoundID 			= 	-1;
	m_iSwimSoundID 		= 	-1;
	
	//animations
	m_strWalkForward		=	"walk_foward";
	m_strWalkBackward		=	"walk_backward";
	m_strWalkLeft			=	"run_left";
	m_strWalkRight			=	"run_right";
	m_strRunForward		=	"run_foward";
	m_strRunBackward		=	"run_backward";
	m_strRunLeft			=	"run_left";
	m_strRunRight			=	"run_right";
	m_strSwimForward		=	"swim_f";
	m_strSwimBackward		=	"swim_b";
	m_strSwimLeft			=	"swim_l";
	m_strSwimRight			=	"swim_r";
	m_strJump				=	"jump";
	m_strIdleStanding		=	"idle";
	m_strIdleSitting		=	"riding";
	m_strIdleSwimming		=	"idle";
	m_strTaunt				=	"taunt";	
	
	//setup material
	m_pkTextMaterial = new ZMaterial;
	m_pkTextMaterial->GetPass(0)->m_kTUs[0]->SetRes("data/textures/text/fetfont.tga");
	m_pkTextMaterial->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
	m_pkTextMaterial->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
	m_pkTextMaterial->GetPass(0)->m_bLighting = 				false;		
	m_pkTextMaterial->GetPass(0)->m_bColorMaterial = 		true;
	m_pkTextMaterial->GetPass(0)->m_kVertexColor =			Vector3(0,1,0);
	m_pkTextMaterial->GetPass(0)->m_bFog = 					true;		
	m_pkTextMaterial->GetPass(0)->m_bAlphaTest =				true;		
	m_pkTextMaterial->GetPass(0)->m_bDepthTest = 			true;

	//setup font
	m_pkFont = new ZGuiFont("CharacterFont");
	m_pkFont->Create("/data/textures/text/fetfont.fnt",-1);	
}

void P_CharacterProperty::Init()
{
	//containers
	m_pkInventory			=	new MLContainer(m_pkEntityMan,GetEntity()->GetEntityID(),10,5);

}


P_CharacterProperty::~P_CharacterProperty()
{
	//make sure all sounds are stoped
	m_pkAudioSystem->StopAudio(m_iWalkSoundID);
	m_pkAudioSystem->StopAudio(m_iRunSoundID);
	m_pkAudioSystem->StopAudio(m_iSwimSoundID);

	delete m_pkTextMaterial;
	delete m_pkFont;
	
	delete m_pkInventory;
}


void P_CharacterProperty::Update()
{
	if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL))
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
	else if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER))
	{
		if(GetIsPlayerCharacter())
		{
			if(m_bOverHeadText)
			{
				string strText = GetName()+string(" <")+GetOwnedByPlayer()+string(">");
		
				m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),GetEntity()->GetIWorldPosV()+
								Vector3(0,GetEntity()->GetRadius(),0),0.3,strText,m_pkTextMaterial,m_pkFont,true);							
			}
		}	
	}		
}

void P_CharacterProperty::DoTaunt(int iTauntID)
{
	if(iTauntID <0 || iTauntID > 99)
		return;
	
	char nr[4];
	IntToChar(nr,iTauntID);	
	
	if(P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad"))
	{
		if(pkMad->GetCurrentAnimationName() == m_strIdleStanding)
		{
			pkMad->SetAnimation((m_strTaunt + nr).c_str(), 0);
			pkMad->SetNextAnimation(m_strIdleStanding.c_str());	
		}
	}
}

void P_CharacterProperty::UpdateAnimation()
{
	if(P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad"))
	{
		if(P_CharacterControl* pkCC = (P_CharacterControl*)GetEntity()->GetProperty("P_CharacterControl"))
		{		
			//jumping
			if(pkCC->GetCharacterState(eJUMPING))
			{							
				if(pkMad->GetCurrentAnimationName() != m_strJump)
				{
					pkMad->SetAnimation(m_strJump.c_str(), 0);
					pkMad->SetNextAnimation(MAD_NOLOOP);
				}			
			}
			//RUNNING
			else if(pkCC->GetCharacterState(eRUNNING))
			{
				switch(pkCC->GetMovedirection())
				{
					case eMOVE_FORWARD:
						if(pkMad->GetCurrentAnimationName() != m_strRunForward)
							pkMad->SetAnimation(m_strRunForward.c_str(), 0);
						break;
					case eMOVE_BACKWARD:
						if(pkMad->GetCurrentAnimationName() != m_strRunBackward)
							pkMad->SetAnimation(m_strRunBackward.c_str(), 0);
						break;
					case eMOVE_LEFT:
						if(pkMad->GetCurrentAnimationName() != m_strRunLeft)
							pkMad->SetAnimation(m_strRunLeft.c_str(), 0);
						break;
					case eMOVE_RIGHT:
						if(pkMad->GetCurrentAnimationName() != m_strRunRight)
							pkMad->SetAnimation(m_strRunRight.c_str(), 0);
						break;										
				}			
			}
			//WALKING
			else if(pkCC->GetCharacterState(eWALKING))
			{
				switch(pkCC->GetMovedirection())
				{
					case eMOVE_FORWARD:
						if(pkMad->GetCurrentAnimationName() != m_strWalkForward)
							pkMad->SetAnimation(m_strWalkForward.c_str(), 0);
						break;
					case eMOVE_BACKWARD:
						if(pkMad->GetCurrentAnimationName() != m_strWalkBackward)
							pkMad->SetAnimation(m_strWalkBackward.c_str(), 0);
						break;
					case eMOVE_LEFT:
						if(pkMad->GetCurrentAnimationName() != m_strWalkLeft)
							pkMad->SetAnimation(m_strWalkLeft.c_str(), 0);
						break;
					case eMOVE_RIGHT:
						if(pkMad->GetCurrentAnimationName() != m_strWalkRight)
							pkMad->SetAnimation(m_strWalkRight.c_str(), 0);
						break;										
				}						
			}
			//swiming
			else if(pkCC->GetCharacterState(eSWIMMING))
			{
				if(pkMad->GetCurrentAnimationName() != m_strIdleSwimming)
					pkMad->SetAnimation(m_strIdleSwimming.c_str(), 0);
				
			}
			//sitting
			else if(pkCC->GetCharacterState(eSITTING))
			{
				if(pkMad->GetCurrentAnimationName() != m_strIdleSitting)
					pkMad->SetAnimation(m_strIdleSitting.c_str(), 0);			
			}
			//idle standing
			else
			{
				if( pkMad->GetCurrentAnimationName() != m_strIdleStanding
				 && pkMad->GetCurrentAnimationName().compare(0,5,m_strTaunt) )
					pkMad->SetAnimation(m_strIdleStanding.c_str(), 0);				
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
				//m_pkAudioSystem->StopAudio(m_iWalkSoundID);
				m_iWalkSoundID = m_pkAudioSystem->PlayAudio(m_strWalkSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),ZFAUDIO_LOOP);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iWalkSoundID, GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eWALKING])
				m_pkAudioSystem->StopAudio(m_iWalkSoundID);
		}	
		
		//run sound
		if(pkCC->GetCharacterState(eRUNNING))
		{
			if(!m_kCurrentCharacterStates[eRUNNING])
			{
				//m_pkAudioSystem->StopSound(m_iRunSoundID);
				m_iRunSoundID = m_pkAudioSystem->PlayAudio(m_strRunSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),ZFAUDIO_LOOP);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iRunSoundID, GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eRUNNING])
				m_pkAudioSystem->StopAudio(m_iRunSoundID);
		}	
		
				
		
		//swim sound
		if(pkCC->GetCharacterState(eSWIMMING))
		{
			if(!m_kCurrentCharacterStates[eSWIMMING])
			{	
				//m_pkAudioSystem->StopAudio(m_iSwimSoundID);
				m_iSwimSoundID = m_pkAudioSystem->PlayAudio(m_strSwimSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),ZFAUDIO_LOOP);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iSwimSoundID,GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eSWIMMING])
				m_pkAudioSystem->StopAudio(m_iSwimSoundID);
		}			

		//jump sound
		if(pkCC->GetCharacterState(eJUMPING))
			if(!m_kCurrentCharacterStates[eJUMPING])
				m_pkAudioSystem->PlayAudio(m_strJumpSound,GetEntity()->GetIWorldPosV());
		
														
		//update staes
		m_kCurrentCharacterStates[eRUNNING] =	pkCC->GetCharacterState(eRUNNING);
		m_kCurrentCharacterStates[eWALKING] =	pkCC->GetCharacterState(eWALKING);
		m_kCurrentCharacterStates[eJUMPING] =	pkCC->GetCharacterState(eJUMPING);
		m_kCurrentCharacterStates[eSWIMMING] =	pkCC->GetCharacterState(eSWIMMING);
	}
}



void P_CharacterProperty::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write_Str(m_strName);
	pkPackage->Write_Str(m_strOwnedByPlayer);
	pkPackage->Write(m_bIsPlayerCharacter);
		
	m_pkInventory->Save(pkPackage);
}

void P_CharacterProperty::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read_Str(m_strName);	
	pkPackage->Read_Str(m_strOwnedByPlayer);	
	pkPackage->Read(m_bIsPlayerCharacter); 
	
	m_pkInventory->Load(pkPackage);
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





