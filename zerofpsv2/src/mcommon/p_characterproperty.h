#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "p_charactercontrol.h"

using namespace std;

class P_CharacterProperty: public Property
{
	private:
		ZFAudioSystem*	m_pkAudioSystem;
	
	 
		string	m_strName;			
		
		bool		m_bIsPlayerCharacter;
		string	m_strOwnedByPlayer;

		bitset<CHARACTER_STATES>	m_kCurrentCharacterStates;
			
		//basic sounds
		string	m_strWalkSound;
		string	m_strRunSound;
		string	m_strJumpSound;
		string	m_strSwimSound;		
		int		m_iWalkSoundID;
		int		m_iRunSoundID;
		int		m_iSwimSoundID;
		
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
		string	m_strIdleSwiming;
		
		void PlayCharacterMovementSounds();
		void UpdateAnimation();

		
	public:
	
		P_CharacterProperty();
		~P_CharacterProperty();

		void Update();
		
		void SetName(const string& strName)						{	m_strName = strName;						ResetAllNetUpdateFlags();}
		void SetOwnedByPlayer(const string& strPlayer)		{	m_strOwnedByPlayer = strPlayer;		ResetAllNetUpdateFlags();}
		void SetIsPlayerCharacter(bool bIsPlayer)				{	m_bIsPlayerCharacter = bIsPlayer;	ResetAllNetUpdateFlags();}
		
		void SetWalkSound(const string& strFile)				{	m_strWalkSound = strFile;				ResetAllNetUpdateFlags();}
		void SetRunSound(const string& strFile)				{	m_strRunSound = strFile;				ResetAllNetUpdateFlags();}
		void SetJumpSound(const string& strFile)				{	m_strJumpSound = strFile;				ResetAllNetUpdateFlags();}
		void SetSwimSound(const string& strFile)				{	m_strSwimSound = strFile;				ResetAllNetUpdateFlags();}
		
		string GetName()												{	return m_strName;					}
		string GetOwnedByPlayer()									{	return m_strOwnedByPlayer;		}
		bool	 GetIsPlayerCharacter()								{	return m_bIsPlayerCharacter;	}
				
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;		
};

MCOMMON_API Property* Create_P_CharacterProperty();

#endif








