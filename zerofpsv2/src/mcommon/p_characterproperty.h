#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "p_charactercontrol.h"


using namespace std;

class P_Buff;




class MCOMMON_API CharacterStats
{
	public:
		int	m_iLevel; 
		int	m_iExperience;
		int	m_iNextLevel;		
		
		int	m_iSpeed;
		int	m_iSpeedMod;
		
		int	m_iMana;
		int	m_iManaMax;
		int	m_iManaMaxMod;
		
		int	m_iLife;
		int	m_iLifeMax;
		int	m_iLifeMaxMod;
		
		int	m_iStrength;
		int	m_iStrengthMod;
		int	m_iDexterity;
		int	m_iDexterityMod;
		int	m_iVitality;
		int	m_iVitalityMod;
		int	m_iIntelligence;
		int	m_iIntelligenceMod;
		int	m_iWisdom;
		int	m_iWisdomMod;
		
		int	m_iArmor;
		int	m_iArmorMod;
		
		int	m_iDamageMin;
		int	m_iDamageMinMod;
		int	m_iDamageMax;
		int	m_iDamageMaxMod;
				
};


class MCOMMON_API P_CharacterProperty: public Property
{
	private:
		ZFAudioSystem*	m_pkAudioSystem;
		Render*			m_pkRender;	
		ZShaderSystem*	m_pkZShaderSystem;
		EntityManager*	m_pkEntityMan;
		Application*	m_pkApp;
		
		
		//over head text
		ZMaterial*	m_pkTextMaterial;
		ZGuiFont*	m_pkFont;		
		bool			m_bOverHeadText;			//shuld overhead text be drawn?
			 
		
		string		m_strName;					//character name		
		bool			m_bIsPlayerCharacter;	//is this and NPC or PC ?
		string		m_strOwnedByPlayer;		//who owns me ? if anyone
		string		m_strChatMsg;				//last chat msg
		float			m_fChatTime;				//lst chat msg time
		
		
		int			m_iConID;					//player connection ID if any				
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
		
	
		void PlayCharacterMovementSounds();
		void SetupContainers();
		
		vector<PropertyValues> GetPropertyValues();
		
		
		void SendBuffList();
		
	public:
		//containers
		int	m_iInventory;
		
		int	m_iHead;
		int	m_iGloves;
		int	m_iCape;		
		int	m_iNecklace;				
		int	m_iBody;
		int	m_iLeftHand;
		int	m_iRightHand;		
		int	m_iBracers;		
		int	m_iRing1;
		int	m_iRing2;
		int	m_iRing3;
		int	m_iRing4;
		int	m_iBelt;		
		int	m_iFeets;
		
	
		P_CharacterProperty();
		~P_CharacterProperty();
		
		void Init();
		void Update();
		
		void SetClient(int iConID)									{	m_iConID = iConID;				}
	
		void SetName(const string& strName)						{	m_strName = strName;						ResetAllNetUpdateFlags();}
		void SetOwnedByPlayer(const string& strPlayer)		{	m_strOwnedByPlayer = strPlayer;		ResetAllNetUpdateFlags();}
		void SetIsPlayerCharacter(bool bIsPlayer)				{	m_bIsPlayerCharacter = bIsPlayer;	ResetAllNetUpdateFlags();}
		void SetOverHeadText(bool bOverHead)					{	m_bOverHeadText = bOverHead;	}
		
		void SetWalkSound(const string& strFile)				{	m_strWalkSound = strFile;				ResetAllNetUpdateFlags();}
		void SetRunSound(const string& strFile)				{	m_strRunSound = strFile;				ResetAllNetUpdateFlags();}
		void SetJumpSound(const string& strFile)				{	m_strJumpSound = strFile;				ResetAllNetUpdateFlags();}
		void SetSwimSound(const string& strFile)				{	m_strSwimSound = strFile;				ResetAllNetUpdateFlags();}
		
		string GetName()												{	return m_strName;					}
		string GetOwnedByPlayer()									{	return m_strOwnedByPlayer;		}
		bool	 GetIsPlayerCharacter()								{	return m_bIsPlayerCharacter;	}
		
		
		P_Buff* AddBuff(const string& strBuffName);			//creates and adds buff to character 
		void RemoveBuff(P_Buff* pkBuff);							//removes buffs
		
		
		//client code
		void AddChatMsg(const string& strChatMsg);
		
				
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;		
};

MCOMMON_API Property* Create_P_CharacterProperty();
MCOMMON_API void Register_P_CharacterProperty(ZeroFps* pkZeroFps);

#endif








