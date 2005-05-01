#include "p_charactercontrol.h"
#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 
#include "../zerofpsv2/engine_systems/propertys/p_tcstrigger.h" 


AnimationSet::AnimationSet()
{
	//animations
	m_strWalkForward		=	"walk_foward";
	m_strWalkBackward		=	"walk_backward";
	m_strWalkLeft			=	"run_left";
	m_strWalkRight			=	"run_right";
	m_strRunForward		=	"run_foward";
	m_strRunBackward		=	"run_backward";
	m_strRunLeft			=	"run_left";
	m_strRunRight			=	"run_right";
	m_strSwimForward		=	"swim_foward";
	m_strSwimBackward		=	"swim_backward";
	m_strSwimLeft			=	"swim_left";
	m_strSwimRight			=	"swim_rright";
	m_strJump				=	"jump";
	m_strIdleStanding		=	"idle";
	m_strIdleSitting		=	"sit_ground";
	m_strIdleSwimming		=	"swim-idle";
	m_strEmote				=	"taunt";		
	m_strDie					=	"die";	
	m_strDead				=	"dead";	

}

P_CharacterControl::P_CharacterControl()
{
	strcpy(m_acName,"P_CharacterControl");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_bNetwork = 			true;
	m_iVersion = 			5;
		
	
	m_iConnectionID	=	-1;
	
	m_fLockTime = 			-1;
	m_bEnabled=				true;
	m_bNoClientRotate=	false;
	
	m_fYAngle = 			0;
	m_fPAngle = 			0;	
	m_fSoundWalkDelay = 	0;
	m_fSoundRunDelay = 	0;
	m_fJumpDelay =			0;
	
	m_fSpeed = 				40.0;
	m_fJumpForce = 		5.0; 	
	m_bHaveJumped = 		false;	
	m_bInWater=				false;
	m_iDirection = 		eMOVE_NONE;
	m_iCharacterState	=	eIDLE_STANDING;	
	
	m_kControls.reset();
	
	//animations
	m_iCurrentSet = 0;	
	AnimationSet kNormal;
	m_kAnimationSets.push_back(kNormal);
	
	AnimationSet kCombatMode;
	kCombatMode.m_strIdleStanding = "combat_idle";
	m_kAnimationSets.push_back(kCombatMode);
	
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
	vector<PropertyValues> kReturn(1);
	
// 	kReturn[2].kValueName = "WalkForwardAnim";
// 	kReturn[2].iValueType = VALUETYPE_STRING;
// 	kReturn[2].pkValue    = (void*)&m_strWalkForward;		
// 	
	kReturn[0].kValueName = "speed";
	kReturn[0].iValueType = VALUETYPE_FLOAT;
	kReturn[0].pkValue    = &m_fSpeed;	
	
// 	kReturn[0].kValueName = "RunForwardAnim";
// 	kReturn[0].iValueType = VALUETYPE_STRING;
// 	kReturn[0].pkValue    = (void*)&m_strRunForward;	

	
		
	return kReturn;
}

void P_CharacterControl::Update()
{	
	m_bNoClientRotate = false;

	
	if(!m_bEnabled || m_pkEntityManager->GetSimTime() < m_fLockTime)
	{
		if(m_iConnectionID != -1)
		{
	 		m_pkEntity->SetNetIgnoreFlag(m_iConnectionID,NETUPDATEFLAG_ROT,false);
	 		m_bNoClientRotate = true;
	 	}
			
// 		SetCharacterState(eNONE);
		return;
	}
		
	
// 	if(m_fLockTime != -1)
// 	{
// 		if(m_pkEntityManager->GetSimTime() < m_fLockTime)
// 			return;
// 		else
// 			m_fLockTime = -1;	
// 	}
	
	//is sitting ?
	if(m_iConnectionID != -1)
	{
		m_pkEntity->SetNetIgnoreFlag(m_iConnectionID,NETUPDATEFLAG_ROT,true);		
	}
	
	
	

	if(P_Tcs* pkTcs = (P_Tcs*)GetEntity()->GetProperty("P_Tcs"))
	{
		//water check
		m_bInWater = false;
		if(pkTcs->GetTrigger() != -1)
		{
			//check for trigger
			if(P_TcsTrigger* pkTrigger = (P_TcsTrigger*)m_pkEntityManager->GetPropertyFromEntityID(pkTcs->GetTrigger(),"P_TcsTrigger"))
			{
				if(pkTrigger->GetTriggerID() == 10)
				{
					m_bInWater = true;
				}
			}
		}		
	
		
		//disable gravity if in water
		if(m_bInWater)
		{
			pkTcs->SetGravity(false);
			SetCharacterState(eIDLE_SWIMING);
		}
		else
			pkTcs->SetGravity(true);
					
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
		if(m_kControls[eCRAWL] || m_bForceCrawl)
			kVel *= 0.45;
			
		//character moves slower while in the air
		if(!pkTcs->GetOnGround())
			kVel *= 0.25;
						
			
		//apply movement force					
		if(!kVel.IsZero())
		{
			if(m_bInWater)
				SetCharacterState(eSWIMMING);
		
			pkTcs->ClearExternalForces();
			pkTcs->ApplyForce(Vector3(0,0,0),kVel);
		}
			

		
		
		//check if where walking or running or nothing
		if(kVel.Length() > 0 && pkTcs->GetOnGround())
		{
			if(m_kControls[eCRAWL] || m_bForceCrawl)
			{
				m_fSoundWalkDelay = m_pkZeroFps->GetEngineTime();
				SetCharacterState(eWALKING);
			}
			else
			{
				m_fSoundWalkDelay = m_pkZeroFps->GetEngineTime();
				SetCharacterState(eRUNNING);
			}
		}
		
		//set idle standing i we havent touched the ground for some time
		if(GetCharacterState() == eWALKING || GetCharacterState() == eRUNNING)
		{
			if(m_pkZeroFps->GetEngineTime() - m_fSoundWalkDelay > 0.25)
			{
				SetCharacterState(eIDLE_STANDING);
			}
		}			
		
		
		//jump
		if(!m_bInWater)
		{			
			if(pkTcs->GetOnGround())
			{
				if(m_bHaveJumped)
				{
					SetCharacterState(eIDLE_STANDING);
					m_bHaveJumped = false;
					m_fJumpDelay = m_pkZeroFps->GetEngineTime();								
				}
				else
				{
					if(m_kControls[eJUMP])
					{
						if(m_pkZeroFps->GetEngineTime() - m_fJumpDelay > 0.5)
						{
							if(pkTcs->GetOnGround())
							{
								m_bHaveJumped = true;
								pkTcs->ApplyImpulsForce(Vector3(0,m_fJumpForce,0));		
							}
						}
					}
				}
			}
		}
		else
		{
			m_bHaveJumped = false;
		
			if(m_kControls[eJUMP])
			{			
				pkTcs->ApplyForce(Vector3(0,0,0),Vector3(0,3,0));			
			}	
		}
		
		
						
		if(m_bHaveJumped)
		{
			SetCharacterState(eJUMPING);
		}			
	}

	
	// dont let the client rotate a sitting character
	if(m_iCharacterState == eSITTING)
	{
		cout<<"sitting"<<endl;
		m_bNoClientRotate = true;
	}	
	
	//only rotate non sitting characters
	if(m_iCharacterState != eSITTING)
	{
		//rotate character
		static Matrix4 kRot;
		kRot.Identity();
		kRot.Rotate(0,m_fYAngle,0);
		kRot.Transponse();				
		GetEntity()->SetLocalRotM(kRot);	
	}	
	
	//update animation
	UpdateAnimation();

}

void P_CharacterControl::Sit()
{
	if(m_iCharacterState == eIDLE_STANDING)
	{
		SetCharacterState(eSITTING);		
	}
	else
	{
		SetCharacterState(eIDLE_STANDING);
	}
}

void P_CharacterControl::Lock(float fTime)
{
	m_fLockTime = m_pkEntityManager->GetSimTime() + fTime;
	
	if(P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad"))
	{	
		pkMad->SetAnimation(m_kAnimationSets[m_iCurrentSet].m_strIdleStanding.c_str(), 0);
	}
}

void P_CharacterControl::RotateTowards(const Vector3& kPos)
{
	static Vector3 kDir;
	static float fAngle;

	kDir = kPos - GetEntity()->GetWorldPosV();
	kDir.Normalize();
	kDir.y = 0;
	
	fAngle = Vector3(0,0,1).Angle(kDir);
	fAngle = -RadToDeg(fAngle);
	
	if(kDir.x > 0)
		fAngle = fabs(fAngle);	

	SetYAngle(fAngle);
}

void P_CharacterControl::DoAnimation(const string& strAnim)
{
	if(P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad"))
	{
	
		if(strAnim.empty())
		{
			pkMad->SetAnimation(m_kAnimationSets[m_iCurrentSet].m_strIdleStanding.c_str(), 0);
			pkMad->SetNextAnimation(m_kAnimationSets[m_iCurrentSet].m_strIdleStanding.c_str());	
		}
		else
		{
			
 			string strTemp = pkMad->GetCurrentAnimationName();
			pkMad->SetAnimation(strAnim.c_str(), 0);		
 			pkMad->SetNextAnimation(strTemp.c_str());
		}
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
		if(pkMad->GetCurrentAnimationName() == m_kAnimationSets[m_iCurrentSet].m_strIdleStanding)
		{
			pkMad->SetAnimation((m_kAnimationSets[m_iCurrentSet].m_strEmote + nr).c_str(), 0);
			pkMad->SetNextAnimation(m_kAnimationSets[m_iCurrentSet].m_strIdleStanding.c_str());	
		}
	}
}

void P_CharacterControl::UpdateAnimation()
{
	if(P_Mad* pkMad = (P_Mad*)GetEntity()->GetProperty("P_Mad"))
	{
		AnimationSet* pkAS = &m_kAnimationSets[m_iCurrentSet];
		
		int iState = GetCharacterState();
	
		//jumping
		if(iState == eJUMPING)
		{							
			if(pkMad->GetCurrentAnimationName() != pkAS->m_strJump)
			{
				pkMad->SetAnimation(pkAS->m_strJump.c_str(), 0);
				pkMad->SetNextAnimation(MAD_NOLOOP);
			}			
		}
		//RUNNING
		if(iState == eRUNNING)
		{
			switch(GetMovedirection())
			{
				case eMOVE_FORWARD:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strRunForward)
						pkMad->SetAnimation(pkAS->m_strRunForward.c_str(), 0);
					break;
				case eMOVE_BACKWARD:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strRunBackward)
						pkMad->SetAnimation(pkAS->m_strRunBackward.c_str(), 0);
					break;
				case eMOVE_LEFT:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strRunLeft)
						pkMad->SetAnimation(pkAS->m_strRunLeft.c_str(), 0);
					break;
				case eMOVE_RIGHT:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strRunRight)
						pkMad->SetAnimation(pkAS->m_strRunRight.c_str(), 0);
					break;										
			}			
		}
		//WALKING
		else if(iState == eWALKING)
		{
			switch(GetMovedirection())
			{
				case eMOVE_FORWARD:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strWalkForward)
						pkMad->SetAnimation(pkAS->m_strWalkForward.c_str(), 0);
					break;
				case eMOVE_BACKWARD:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strWalkBackward)
						pkMad->SetAnimation(pkAS->m_strWalkBackward.c_str(), 0);
					break;
				case eMOVE_LEFT:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strWalkLeft)
						pkMad->SetAnimation(pkAS->m_strWalkLeft.c_str(), 0);
					break;
				case eMOVE_RIGHT:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strWalkRight)
						pkMad->SetAnimation(pkAS->m_strWalkRight.c_str(), 0);
					break;										
			}						
		}
		//swiming
		else if(iState == eSWIMMING)
		{
			switch(GetMovedirection())
			{
				case eMOVE_FORWARD:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strSwimForward)
						pkMad->SetAnimation(pkAS->m_strSwimForward.c_str(), 0);
					break;
				case eMOVE_BACKWARD:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strSwimBackward)
						pkMad->SetAnimation(pkAS->m_strSwimBackward.c_str(), 0);
					break;
				case eMOVE_LEFT:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strSwimLeft)
						pkMad->SetAnimation(pkAS->m_strSwimLeft.c_str(), 0);
					break;
				case eMOVE_RIGHT:
					if(pkMad->GetCurrentAnimationName() != pkAS->m_strSwimRight)
						pkMad->SetAnimation(pkAS->m_strSwimRight.c_str(), 0);
					break;										
		
			}					
			
			
		}
		//sitting
 		else if(iState == eSITTING)
		{
			if(pkMad->GetCurrentAnimationName() != pkAS->m_strIdleSitting)
				pkMad->SetAnimation(pkAS->m_strIdleSitting.c_str(), 0);			
		}
		//idle standing
		else if(iState == eIDLE_STANDING)
		{
			if( pkMad->GetCurrentAnimationName() != pkAS->m_strIdleStanding
				&& pkMad->GetCurrentAnimationName().compare(0,5,pkAS->m_strEmote) )
				pkMad->SetAnimation(pkAS->m_strIdleStanding.c_str(), 0);			// walk_foward	 m_strIdleStanding "taunt1"
		}
		else if(	iState == eIDLE_SWIMING)
		{
			if(pkMad->GetCurrentAnimationName() != pkAS->m_strIdleSwimming)
				pkMad->SetAnimation(pkAS->m_strIdleSwimming.c_str(), 0);		
		}	
	}
}

void P_CharacterControl::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_fSpeed);
	pkPackage->Write(m_fJumpForce);
	
	pkPackage->Write(m_bEnabled);
	
	
}

void P_CharacterControl::Load(ZFIoInterface* pkPackage,int iVersion)
{
		string old;

	if(iVersion == 2)
	{
		pkPackage->Read(m_fSpeed);
		pkPackage->Read(m_fJumpForce);
		pkPackage->Read_Str(old);
	
	}
	if(iVersion == 3)
	{
		pkPackage->Read(m_fSpeed);
		pkPackage->Read(m_fJumpForce);
		pkPackage->Read_Str(old);
		pkPackage->Read(m_bEnabled);
	}
	
	if(iVersion == 4)
	{
		string old;
	
		pkPackage->Read(m_fSpeed);
		pkPackage->Read(m_fJumpForce);
		pkPackage->Read_Str(old);
		pkPackage->Read_Str(old);
		pkPackage->Read(m_bEnabled);
	}	
	
	if(iVersion == 5)
	{
		pkPackage->Read(m_fSpeed);
		pkPackage->Read(m_fJumpForce);
		pkPackage->Read(m_bEnabled);
	}		
}

void P_CharacterControl::PackTo( NetPacket* pkNetPacket, int iConnectionID ) 
{
	pkNetPacket->Write(m_iCharacterState);
	pkNetPacket->Write(m_iDirection);
	
	
	pkNetPacket->Write(m_bNoClientRotate);
	
	
	SetNetUpdateFlag(iConnectionID,false);
}

void P_CharacterControl::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read(m_iCharacterState);
	pkNetPacket->Read(m_iDirection);
	
	pkNetPacket->Read(m_bNoClientRotate);
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


void P_CharacterControl::SetCharacterState(int iState)
{
	if(m_iCharacterState == iState)
		return;

		
	m_iCharacterState = iState;
	SetNetUpdateFlag(true);
};


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
	
	int SetCharacterStateLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,2))
			return 0;
		
		int iCharacterID;
		int iState;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharacterID);
		g_pkScript->GetArgInt(pkLua, 1, &iState);

		
		if(P_CharacterControl* pkCC = (P_CharacterControl*)g_pkObjMan->GetPropertyFromEntityID(iCharacterID,"P_CharacterControl"))
			pkCC->SetCharacterState(iState);
		
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
	
	int IsMovementEnabledLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua, 1))
			return 0;
		
		int id;		
		g_pkScript->GetArgInt(pkLua, 0, &id);

		int iReturnValue = 0;
				
		if(Entity* pkObject = g_pkObjMan->GetEntityByID(id))
			if(P_CharacterControl* pkCC = (P_CharacterControl*)pkObject->GetProperty("P_CharacterControl"))
				if(pkCC->GetEnabled())
					iReturnValue = 1;

		g_pkScript->AddReturnValue( pkLua, iReturnValue );
		
							
		return 1;		
	}
	
	int MovementEnabledLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;
		
		int id;
		int iEnabled;
		
		g_pkScript->GetArgInt(pkLua, 0, &id);
		g_pkScript->GetArgInt(pkLua, 1, &iEnabled);
		
		if(Entity* pkObject = g_pkObjMan->GetEntityByID(id))
			if(P_CharacterControl* pkCC = (P_CharacterControl*)pkObject->GetProperty("P_CharacterControl"))
				pkCC->SetEnabled(iEnabled);
		
		return 0;		
	}
	
	int LockCharacterLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;
		
		int id;
		double dDelay;
		
		g_pkScript->GetArgInt(pkLua, 0, &id);
		g_pkScript->GetArgNumber(pkLua, 1, &dDelay);
		
		if(Entity* pkObject = g_pkObjMan->GetEntityByID(id))
			if(P_CharacterControl* pkCC = (P_CharacterControl*)pkObject->GetProperty("P_CharacterControl"))
				pkCC->Lock(dDelay);
		
		return 0;
	}		
	
	int DoEmoteLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;
		
		int id;
		int iEmote;
		
		g_pkScript->GetArgInt(pkLua, 0, &id);
		g_pkScript->GetArgInt(pkLua, 1, &iEmote);
		
		if(Entity* pkObject = g_pkObjMan->GetEntityByID(id))
			if(P_CharacterControl* pkCC = (P_CharacterControl*)pkObject->GetProperty("P_CharacterControl"))
				pkCC->DoEmote(iEmote);
		
		return 0;
	}		
	
	int DoAnimationLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;
		
		int id;
		string strAnim;
		
		g_pkScript->GetArgInt(pkLua, 0, &id);
		g_pkScript->GetArgString(pkLua, 1, strAnim);
		
		if(Entity* pkObject = g_pkObjMan->GetEntityByID(id))
			if(P_CharacterControl* pkCC = (P_CharacterControl*)pkObject->GetProperty("P_CharacterControl"))
				pkCC->DoAnimation(strAnim);
		
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
	g_pkScript->ExposeFunction("SetCharacterState",			SI_P_CharacterControl::SetCharacterStateLua);
	
	
	g_pkScript->ExposeFunction("GetCharacterYAngle",	SI_P_CharacterControl::GetCharacterYAngleLua);
	g_pkScript->ExposeFunction("SetCharacterYAngle",	SI_P_CharacterControl::SetCharacterYAngleLua);
	
	g_pkScript->ExposeFunction("IsMovementEnabled",	SI_P_CharacterControl::IsMovementEnabledLua);
	g_pkScript->ExposeFunction("MovementEnabled",	SI_P_CharacterControl::MovementEnabledLua);
	g_pkScript->ExposeFunction("LockCharacter",		SI_P_CharacterControl::LockCharacterLua);
	g_pkScript->ExposeFunction("DoEmote",				SI_P_CharacterControl::DoEmoteLua);
	g_pkScript->ExposeFunction("DoAnimation",			SI_P_CharacterControl::DoAnimationLua);
	
		
}


