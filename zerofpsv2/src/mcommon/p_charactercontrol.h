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
		bitset<6>	m_kControls;
		bitset<CHARACTER_STATES>	m_kCharacterStates;			//sent to client
		int								m_iDirection;
		
		float			m_fSpeed;
		float			m_fJumpForce;
		
		float			m_fYAngle;
		float			m_fPAngle;
		
		bool			m_bHaveJumped;
		
		float			m_fSoundWalkDelay;
		float			m_fSoundRunDelay;


		
	public:
	
		P_CharacterControl();
		~P_CharacterControl();	

		void Update();
		void Init();		
		 
		void SetKeys(bitset<6>* kControls) 					{	m_kControls = *kControls;	}
		void SetRotation(float fYAngle,float fPAngle) 	{	m_fYAngle = fYAngle;
																			m_fPAngle = fPAngle;			}
		void SetMoveDirection(int iDir);
		int  GetMovedirection()									{	return m_iDirection;			}
		void SetCharacterState(int iState,bool bValue);
		bool GetCharacterState(int iState);		
		bool GetControl(int iKey);																	
																												
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;		
																			
};

MCOMMON_API Property* Create_P_CharacterControl();


#endif







