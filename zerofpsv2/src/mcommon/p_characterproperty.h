#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"

using namespace std;

class P_CharacterProperty: public Property
{
	private:
		ZFAudioSystem*	m_pkAudioSystem;
	
	
		string	m_strName;			
		
		bool		m_bIsPlayerCharacter;
		string	m_strOwnedByPlayer;

		bitset<8>	m_kCurrentCharacterStates;
			
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
		string	m_strWalkSideway;
		string	m_strRunForward;
		string	m_strRunBackward;
		string	m_strRunSideway;		
		string	m_strSwimForward;
		string	m_strSwimBackward;
		string	m_strSwimSideway;				
		string	m_strJump;
		
		void PlayCharacterMovementSounds();
		void UpdateAnimation();

	public:
	
		P_CharacterProperty();
		~P_CharacterProperty();

		void Update();
		
		void SetName(const string& strName)						{	m_strName = strName;						SetNetUpdateFlag(true);}
		void SetOwnedByPlayer(const string& strPlayer)		{	m_strOwnedByPlayer = strPlayer;		SetNetUpdateFlag(true);}
		void SetIsPlayerCharacter(bool bIsPlayer)				{	m_bIsPlayerCharacter = bIsPlayer;	SetNetUpdateFlag(true);}
		
		void SetWalkSound(const string& strFile)				{	m_strWalkSound = strFile;				SetNetUpdateFlag(true);}
		void SetRunSound(const string& strFile)				{	m_strRunSound = strFile;				SetNetUpdateFlag(true);}
		void SetJumpSound(const string& strFile)				{	m_strJumpSound = strFile;				SetNetUpdateFlag(true);}
		void SetSwimSound(const string& strFile)				{	m_strSwimSound = strFile;				SetNetUpdateFlag(true);}
		
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








