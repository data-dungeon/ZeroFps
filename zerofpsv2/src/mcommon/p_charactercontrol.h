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
	eWALKING	=	0,
	eRUNNING	=	1,
	eJUMPING	=	2,
	eSWIMING	=	3,
};

class MCOMMON_API P_CharacterControl: public Property
{
	private:
		bitset<6>	m_kControls;
		bitset<8>	m_kCharacterStates;
		float			m_fSpeed;
		float			m_fJumpForce;
		
		float			m_fYAngle;
		float			m_fPAngle;
		
		bool			m_bHaveJumped;
		
		float			m_fSoundFixDelay;


		
	public:
	
		P_CharacterControl();
		~P_CharacterControl();	

		void Update();
		void Init();		
		 
		void SetKeys(bitset<6>* kControls) 					{	m_kControls = *kControls;	}
		void SetRotation(float fYAngle,float fPAngle) 	{	m_fYAngle = fYAngle;
																			m_fPAngle = fPAngle;			}
										
		void SetCharacterState(int iState,bool bValue);
		bool GetCharacterState(int iState);																			
																												
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;		
																			
};

MCOMMON_API Property* Create_P_CharacterControl();


#endif







