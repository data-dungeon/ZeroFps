#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "p_charactercontrol.h"

#include "mlcontainer.h"

using namespace std;

class MCOMMON_API P_CharacterProperty: public Property
{
	private:
		ZFAudioSystem*	m_pkAudioSystem;
		Render*			m_pkRender;	
		ZShaderSystem*	m_pkZShaderSystem;
		EntityManager*	m_pkEntityMan;

		
		
		//over head text
		ZMaterial*	m_pkTextMaterial;
		ZGuiFont*	m_pkFont;		
		bool			m_bOverHeadText;			//shuld overhead text be drawn?
			 
		
		string		m_strName;					//character name		
		bool			m_bIsPlayerCharacter;	//is this and NPC or PC ?
		string		m_strOwnedByPlayer;		//who owns me ? if anyone

		bool			m_bFirstUpdate;
		
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
		string	m_strIdleSwimming;
		string	m_strTaunt;
		
		void PlayCharacterMovementSounds();
		void UpdateAnimation();

		
	public:
		//containers
		MLContainer*	m_pkInventory;
		MLContainer*	m_pkHead;
		MLContainer*	m_pkBody;
		MLContainer*	m_pkLeftHand;
		MLContainer*	m_pkRightHand;
	
	
		P_CharacterProperty();
		~P_CharacterProperty();
		
		void Init();
		void Update();
		
		void SetName(const string& strName)						{	m_strName = strName;						ResetAllNetUpdateFlags();}
		void SetOwnedByPlayer(const string& strPlayer)		{	m_strOwnedByPlayer = strPlayer;		ResetAllNetUpdateFlags();}
		void SetIsPlayerCharacter(bool bIsPlayer)				{	m_bIsPlayerCharacter = bIsPlayer;	ResetAllNetUpdateFlags();}
		void SetOverHeadText(bool bOverHead)					{	m_bOverHeadText = bOverHead;	}
		
		void SetWalkSound(const string& strFile)				{	m_strWalkSound = strFile;				ResetAllNetUpdateFlags();}
		void SetRunSound(const string& strFile)				{	m_strRunSound = strFile;				ResetAllNetUpdateFlags();}
		void SetJumpSound(const string& strFile)				{	m_strJumpSound = strFile;				ResetAllNetUpdateFlags();}
		void SetSwimSound(const string& strFile)				{	m_strSwimSound = strFile;				ResetAllNetUpdateFlags();}
		
		void DoTaunt(int iTountID);
		
		string GetName()												{	return m_strName;					}
		string GetOwnedByPlayer()									{	return m_strOwnedByPlayer;		}
		bool	 GetIsPlayerCharacter()								{	return m_bIsPlayerCharacter;	}
				
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;		
};

MCOMMON_API Property* Create_P_CharacterProperty();
MCOMMON_API void Register_P_CharacterProperty(ZeroFps* pkZeroFps);

#endif








