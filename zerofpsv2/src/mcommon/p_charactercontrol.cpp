#include "p_charactercontrol.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 

P_CharacterControl::P_CharacterControl()
{
	strcpy(m_acName,"P_CharacterControl");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_bNetwork = true;
	m_iVersion = 			1;
		
	
	m_fYAngle = 			0;
	m_fPAngle = 			0;	
	m_fSoundWalkDelay = 	0;
	m_fSoundRunDelay = 	0;
	
	m_fSpeed = 				40.0;
	m_fJumpForce = 		5.0; 	
	m_bHaveJumped = 		false;	
	m_iDirection = 		eMOVE_NONE;
		
	m_kCharacterStates.reset();
	m_kControls.reset();
	
	
	
	
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
	m_strEmote				=	"taunt";		
}

P_CharacterControl::~P_CharacterControl()
{

}


void P_CharacterControl::Init()
{
	m_kControls.reset();
}

vector<PropertyValues> P_CharacterControl::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
	
	kReturn[1].kValueName = "speed";
	kReturn[1].iValueType = VALUETYPE_FLOAT;
	kReturn[1].pkValue    = &m_fSpeed;	
	
	kReturn[0].kValueName = "RunForwardAnim";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = (void*)&m_strRunForward;	
	
	return kReturn;
}

void P_CharacterControl::Update()
{	
	if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_SERVER))
	{
		//reset character states
		SetCharacterState(eJUMPING,false);
		SetCharacterState(eSWIMMING,false);
			
		if(P_Tcs* pkTcs = (P_Tcs*)GetEntity()->GetProperty("P_Tcs"))
		{
			Vector3 kVel(0,0,0);	
				
			
			if(m_kControls[eUP]) 	kVel.z +=  1;
			if(m_kControls[eDOWN])	kVel.z += -1;
			if(m_kControls[eLEFT])	kVel.x +=  1; 
			if(m_kControls[eRIGHT])	kVel.x += -1; 
		
			//determin movement direction
			if(kVel == Vector3::ZERO)
				SetMoveDirection(eMOVE_NONE);
			else if(kVel.z > 0)
				SetMoveDirection(eMOVE_FORWARD);						
			else if(kVel.z < 0)
				SetMoveDirection(eMOVE_BACKWARD);
			else if(kVel.x < 0)
				SetMoveDirection(eMOVE_RIGHT);			
			else if(kVel.x > 0)
				SetMoveDirection(eMOVE_LEFT);
				
			//transform velocity
			kVel = GetEntity()->GetWorldRotM().VectorTransform(kVel);							
			kVel.y = 0;
			
			//multiply by character speed	
			if(kVel.Length() > 0)
				kVel = kVel.Unit() * m_fSpeed;
			
			//check if where crawling
			if(m_kControls[eCRAWL])
				kVel *= 0.45;
				
			//character moves slower while in the air
			if(!pkTcs->GetOnGround())
				kVel *= 0.25;
							
			//apply movement force					
			if(!kVel.IsZero())
			{
				pkTcs->ClearExternalForces();
				pkTcs->ApplyForce(Vector3(0,0,0),kVel);
			}
				
			//check if where walking or running or nothing
			if(kVel.Length() > 0 && pkTcs->GetOnGround())
			{
				if(m_kControls[eCRAWL])
				{
					m_fSoundWalkDelay = m_pkZeroFps->GetTicks();
					SetCharacterState(eWALKING,true);
				}
				else
				{
					m_fSoundRunDelay = m_pkZeroFps->GetTicks();
					SetCharacterState(eRUNNING,true);
				}
			}
					
			//only stop play walk/run sound when we havent touch the ground for 0.25 sec
			if(m_pkZeroFps->GetTicks() - m_fSoundWalkDelay > 0.25)
				SetCharacterState(eWALKING,false);				
			if(m_pkZeroFps->GetTicks() - m_fSoundRunDelay > 0.25)
				SetCharacterState(eRUNNING,false);				
			
			
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
	
		//rotate character
		Matrix4 kRot;
		kRot.Identity();
		kRot.Rotate(0,m_fYAngle,0);
		kRot.Transponse();				
		GetEntity()->SetLocalRotM(kRot);	
		
		
		
		
		//update animation
		UpdateAnimation();
	}	
}

void P_CharacterControl::DoEmote(int iEmoteID)
{
	if(iEmoteID <0 || iEmoteID > 99)
		return;
	
	char nr[4];
	IntToChar(nr,iEmoteID);	
	
	if(P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad"))
	{
		if(pkMad->GetCurrentAnimationName() == m_strIdleStanding)
		{
			pkMad->SetAnimation((m_strEmote + nr).c_str(), 0);
			pkMad->SetNextAnimation(m_strIdleStanding.c_str());	
		}
	}
}

void P_CharacterControl::UpdateAnimation()
{
	if(P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad"))
	{
		//jumping
		if(GetCharacterState(eJUMPING))
		{							
			if(pkMad->GetCurrentAnimationName() != m_strJump)
			{
				pkMad->SetAnimation(m_strJump.c_str(), 0);
				pkMad->SetNextAnimation(MAD_NOLOOP);
			}			
		}
		//RUNNING
		else if(GetCharacterState(eRUNNING))
		{
			switch(GetMovedirection())
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
		else if(GetCharacterState(eWALKING))
		{
			switch(GetMovedirection())
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
		else if(GetCharacterState(eSWIMMING))
		{
			if(pkMad->GetCurrentAnimationName() != m_strIdleSwimming)
				pkMad->SetAnimation(m_strIdleSwimming.c_str(), 0);
			
		}
		//sitting
		else if(GetCharacterState(eSITTING))
		{
			if(pkMad->GetCurrentAnimationName() != m_strIdleSitting)
				pkMad->SetAnimation(m_strIdleSitting.c_str(), 0);			
		}
		//idle standing
		else
		{
			if( pkMad->GetCurrentAnimationName() != m_strIdleStanding
				&& pkMad->GetCurrentAnimationName().compare(0,5,m_strEmote) )
				pkMad->SetAnimation(m_strIdleStanding.c_str(), 0);				
		}		
	}
}



void P_CharacterControl::PackTo( NetPacket* pkNetPacket, int iConnectionID ) 
{
	pkNetPacket->Write(m_kCharacterStates);
	pkNetPacket->Write(m_iDirection);
	
	SetNetUpdateFlag(iConnectionID,false);
}

void P_CharacterControl::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read(m_kCharacterStates);
	pkNetPacket->Read(m_iDirection);
}


void P_CharacterControl::SetControl(int iControl,bool bState)
{
	if(iControl < 0 || iControl > 5)
		return;

	m_kControls[iControl] = bState;
}


void P_CharacterControl::SetMoveDirection(int iDir)
{
	if(m_iDirection == iDir)
		return;
			
	m_iDirection = iDir;
	SetNetUpdateFlag(true);
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

bool P_CharacterControl::GetControl(int iKey)
{
	if(iKey < 0 || iKey >= 6)
		return false;

	return m_kControls[iKey];
}


// P_CharacterControl script interface


using namespace ObjectManagerLua;

namespace SI_P_CharacterControl
{
	int SetCharacterControlLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
			return 0;
		
		int id;
		int iControl;
		int iState;
		
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		id = (int)dTemp;		
		
		g_pkScript->GetArgNumber(pkLua, 1, &dTemp);
		iControl = (int)dTemp;			
		
		g_pkScript->GetArgNumber(pkLua, 2, &dTemp);
		iState = (int)dTemp;			

		if(Entity* pkObject = g_pkObjMan->GetEntityByID(id))
			if(P_CharacterControl* pkCC = (P_CharacterControl*)pkObject->GetProperty("P_CharacterControl"))
				pkCC->SetControl(iControl,bool(iState));
		
		return 0;
	}
	
	int GetCharacterYAngleLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 1)
			return 0;
		
		int id;
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		id = (int)dTemp;			

		double dAngle=0;
			
		if(Entity* pkObject = g_pkObjMan->GetEntityByID(id))
			if(P_CharacterControl* pkCC = (P_CharacterControl*)pkObject->GetProperty("P_CharacterControl"))
			{
				g_pkScript->AddReturnValue(pkLua, pkCC->GetYAngle());					
				return 1;
			}
			
		return 0;
	}
	
	int SetCharacterYAngleLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;
		
		int id;
		double dTemp;
		double dAngle;
				
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		id = (int)dTemp;			

		g_pkScript->GetArgNumber(pkLua, 1, &dAngle);

					
		if(Entity* pkObject = g_pkObjMan->GetEntityByID(id))
			if(P_CharacterControl* pkCC = (P_CharacterControl*)pkObject->GetProperty("P_CharacterControl"))
				pkCC->SetYAngle(dAngle);

											
		return 0;
	}
	
	
	
	int ClearCharacterControlsLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 1)
			return 0;
		
		int id;
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		id = (int)dTemp;		
		
		bitset<6> kKeys;
		
		if(Entity* pkObject = g_pkObjMan->GetEntityByID(id))
			if(P_CharacterControl* pkCC = (P_CharacterControl*)pkObject->GetProperty("P_CharacterControl"))
				pkCC->SetKeys(&kKeys);
		
		return 0;
	}	
}


Property* Create_P_CharacterControl()
{
	return new P_CharacterControl;
}

void Register_P_CharacterControl(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_CharacterControl", Create_P_CharacterControl);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("SetCharacterControl",		SI_P_CharacterControl::SetCharacterControlLua);
	g_pkScript->ExposeFunction("ClearCharacterControls",	SI_P_CharacterControl::ClearCharacterControlsLua);
	
	g_pkScript->ExposeFunction("GetCharacterYAngle",	SI_P_CharacterControl::GetCharacterYAngleLua);
	g_pkScript->ExposeFunction("SetCharacterYAngle",	SI_P_CharacterControl::SetCharacterYAngleLua);

}


