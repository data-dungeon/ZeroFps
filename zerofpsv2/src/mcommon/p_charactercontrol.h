#ifndef _P_CHARACTERCONTROL_H_
#define _P_CHARACTERCONTROL_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine_systems/propertys/p_tcs.h"
#include <bitset>

using namespace std;

enum ControlKeys
{
	eUP = 	0,
	eDOWN = 	1,
	eLEFT = 	2,
	eRIGHT = 3,
	eJUMP = 	4,
	eCRAWL = 5,
};

enum CharcterStates
{
	eIDLE_STANDING=0,
	eSITTING =		1,
	eSWIMMING =		2,
	eWALKING	=		3,
	eRUNNING	=		4,
	eJUMPING	=		5,
	eIDLE_SWIMING= 6,
	eNONE	       = 7,
};

enum CharacterMoveDirection
{
	eMOVE_NONE		=0,
	eMOVE_FORWARD	=1,
	eMOVE_BACKWARD	=2,
	eMOVE_LEFT		=3,
	eMOVE_RIGHT		=4,
};

#define CHARACTER_STATES 6

class MCOMMON_API P_CharacterControl: public Property
{
	private:
	
		//control stuff
		bitset<6>	m_kControls;
		int			m_iDirection;
		
		int			m_iCharacterState;
		
		float			m_fLockTime;
		bool			m_bEnabled;
		
		float			m_fSpeed;
		float			m_fJumpForce;
		
		float			m_fYAngle;
		float			m_fPAngle;
		
		bool			m_bHaveJumped;
		bool			m_bInWater;
		
		float			m_fSoundWalkDelay;
		float			m_fSoundRunDelay;
		float			m_fJumpDelay;

		
		//animation
		
		//basic animations
		string	m_strWalkForward;
		string	m_strWalkBackward;
		string	m_strWalkLeft;
		string	m_strWalkRight;
		string	m_strRunForward;
		string	m_strRunBackward;
		string	m_strRunLeft;		
		string	m_strRunRight;
		string	m_strSwimForward;
		string	m_strSwimBackward;
		string	m_strSwimLeft;				
		string	m_strSwimRight;						
		string	m_strJump;
		string	m_strIdleStanding;
		string	m_strIdleSitting;
		string	m_strIdleSwimming;
		string	m_strEmote;			
		string	m_strDead;			
		string	m_strDie;	
		
		void UpdateAnimation();			
		vector<PropertyValues> GetPropertyValues();
		
	public:
	
		P_CharacterControl();
		~P_CharacterControl();	
		void Update();
		void Init();				
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);		
		
		
		void SetControl(int iControl,bool bState);
		void SetKeys(bitset<6>* kControls) 					{	m_kControls = *kControls;	}
		void SetRotation(float fYAngle,float fPAngle) 	{	m_fYAngle = fYAngle;
																			m_fPAngle = fPAngle;			}
		float GetYAngle()											{	return m_fYAngle;				};
		void  SetYAngle(float fYAngle)						{	m_fYAngle = fYAngle;			};
														
		void RotateTowards(const Vector3& kPos);
							
		void SetMoveDirection(int iDir);
		int  GetMovedirection()									{	return m_iDirection;			}
		bool GetControl(int iKey);																			
		void SetCharacterState(int iState);
		int  GetCharacterState()								{	return m_iCharacterState;	};
		
																												
		void SetSpeed(float fSpeed)							{	m_fSpeed = fSpeed;			}
		void SetJumpForce(float fJump)						{	m_fJumpForce = fJump;		}

		
			
		void DoEmote(int iEmoteID);	
		void Lock(float fTime);
		void SetEnabled(bool bEnabled)						{	m_bEnabled = bEnabled;		}
};

MCOMMON_API Property* Create_P_CharacterControl();
MCOMMON_API void Register_P_CharacterControl(ZeroFps* pkZeroFps);

#endif







