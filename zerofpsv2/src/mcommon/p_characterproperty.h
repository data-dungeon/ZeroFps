#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "p_charactercontrol.h"


using namespace std;

class P_Buff;



class MCOMMON_API CharacterStat
{
	public:
		string	m_strName;
		int		m_iValue;
		int		m_iMod;
	
		CharacterStat(const string& strName,int iValue,int iMod)
		{
			m_strName = strName;
			m_iValue = iValue;
			m_iMod = iMod;
		};
				
};

class MCOMMON_API CharacterStats
{
	private:
		vector<CharacterStat>	m_kStats;

	public:
		
		
		CharacterStats()
		{
			m_kStats.push_back(CharacterStat("Level"			,0,0));
			m_kStats.push_back(CharacterStat("Experience"	,0,0));
			m_kStats.push_back(CharacterStat("NextLevel"		,0,0));
			m_kStats.push_back(CharacterStat("Speed"			,0,0));
			m_kStats.push_back(CharacterStat("Mana"			,0,0));
			m_kStats.push_back(CharacterStat("ManaMax"		,0,0));
			m_kStats.push_back(CharacterStat("Life"			,0,0));
			m_kStats.push_back(CharacterStat("LifeMax"		,0,0));
			m_kStats.push_back(CharacterStat("Strength"		,0,0));
			m_kStats.push_back(CharacterStat("Dexterity"		,0,0));
			m_kStats.push_back(CharacterStat("Vitality"		,0,0));
			m_kStats.push_back(CharacterStat("Intelligence"	,0,0));
			m_kStats.push_back(CharacterStat("Wisdom"			,0,0));
			m_kStats.push_back(CharacterStat("Charisma"		,0,0));
			m_kStats.push_back(CharacterStat("Armor"			,0,0));
			m_kStats.push_back(CharacterStat("DamageMin"		,0,0));
			m_kStats.push_back(CharacterStat("DamageMax"		,0,0));
		
		}
		
		void SetStat(const string& strName,int iValue)
		{
			for(int i=0;i<m_kStats.size();i++)
				if(m_kStats[i].m_strName == strName)
				{
					m_kStats[i].m_iValue = iValue;
					return;
				}
		}
		
		void ChangeStat(const string& strName,int iValue)
		{
			for(int i=0;i<m_kStats.size();i++)
				if(m_kStats[i].m_strName == strName)
				{
					m_kStats[i].m_iValue += iValue;
					return;
				}
		}
		
		
		void SetMod(const string& strName,int iValue)
		{
			for(int i=0;i<m_kStats.size();i++)
				if(m_kStats[i].m_strName == strName)
				{
					m_kStats[i].m_iMod = iValue;
					return;
				}
		}
		
		void ChangeMod(const string& strName,int iValue)
		{
			for(int i=0;i<m_kStats.size();i++)
				if(m_kStats[i].m_strName == strName)
				{
					m_kStats[i].m_iMod += iValue;
					return;	
				}
		}

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
		CharacterStats	m_kCharacterStats;

	
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
		void RemoveBuff(const string& strBuffName);
		
		
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








