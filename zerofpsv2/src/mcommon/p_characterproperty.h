#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "p_charactercontrol.h"


using namespace std;

class P_Buff;

enum MCOMMON_API TargetType
{
	ePASSIVE				=	0,
	eCHARACTER_TARGET =	1,
	eGROUND_TARGET 	=	2,
	eDIRECTION 			=	3,						
	eITEM_TARGET		=	4,
	eSELF					=	5,
};

enum MCOMMON_API SkillType
{
	eMISC			=	0,
	eOFFENSIVE	=	1,
	eDEFENSIVE	=	2,
	eTRADE		=	3,
};	

class MCOMMON_API Skill
{
	private:
		ZFScriptSystem*	m_pkScript;		
		ZFResourceHandle*	m_pkScriptFileHandle;	
		ZeroFps*				m_pkZeroFps;
		EntityManager*		m_pkEntityManager;
		
		string	m_strSkillScript;
		
		//owner character
		int		m_iOwnerID;				
		
		//skill data
		int		m_iLevel;
		string	m_strParentSkill;
		
		void UpdateFromScript();			

	public:
			
		string	m_strInGameName;
		string	m_strSchool;
		string	m_strIcon;
		
		
		float		m_fReloadTime;
		float		m_fTimeLeft;
		float		m_fLastUpdate;
		
		int		m_iTargetType;
		int		m_iSkillType;
		
		float		m_fRange;
		
		Skill(const string& strScriptFile,const string& strParent, int iOwnerID);
		~Skill();
		
		void Update();
		int Use(int iTargetID,const Vector3& kPos,const Vector3& kDir);
		
		//sets
		void SetLevel(int iLevel);
		void SetTimeLeft(float fTime)	{	m_fTimeLeft = fTime;				}
		
		//gets
		string GetName()									{	return m_strSkillScript;					};
		int	 GetLevel()									{	return m_iLevel;								};
		string GetParent()								{	return m_strParentSkill;					};		
};


class MCOMMON_API Stat
{
	public:
		string	m_strName;
		float		m_fValue;
		float		m_fMod;
	
		Stat(const string& strName,float fValue,float fMod)
		{
			m_strName = strName;
			m_fValue = fValue;
			m_fMod = fMod;
		};
				
};

class MCOMMON_API Stats
{
	private:		
		vector<Stat>	m_kStats;

	public:
		
		Stats();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
		void AddOn(Stats* pkTarget);
		void RemoveOn(Stats* pkTarget);
		
		void AddStat(const string& strName,float fValue=0,float fMod = 0);
		
		void 	SetStat(const string& strName,float fValue);
		float GetStat(const string& strName);
		void 	ChangeStat(const string& strName,float fValue);
		void 	SetMod(const string& strName,float fValue);
		float GetMod(const string& strName);
		void 	ChangeMod(const string& strName,float fValue);
		float GetTotal(const string& strName);
 
		void GetStatsList(vector<Stat>* pkStats);
};


class MCOMMON_API P_CharacterProperty: public Property
{
	private:
		ZFAudioSystem*	m_pkAudioSystem;
		Render*			m_pkRender;	
		ZShaderSystem*	m_pkZShaderSystem;
		EntityManager*	m_pkEntityMan;
		Application*	m_pkApp;
		
		//dirs
		string	m_strBuffDir;
		
		
		//stat
		float			m_fStatTimer;		
		float			m_fLegLength;
		float			m_fMarkerSize;
		
		bool			m_bDead;
		bool			m_bCombatMode;
		
		float			m_fDeadTimer;
		
		vector<Skill*>	m_kSkills;
		float				m_fSkillTimer;
			
		//over head text
		ZMaterial*	m_pkTextMaterial;
		ZGuiFont*	m_pkFont;		
		bool			m_bOverHeadText;			//shuld overhead text be drawn?
		
		
		string		m_strName;					//character name		
		bool			m_bIsPlayerCharacter;	//is this and NPC or PC ?
		int			m_iFaction;
		string		m_strOwnedByPlayer;		//who owns me ? if anyone
		string		m_strChatMsg;				//last chat msg
		float			m_fChatTime;				//lst chat msg time
		
		
		int			m_iConID;					//player connection ID if any				
		bool			m_bFirstUpdate;
		
		int			m_iCurrentCharacterState;
			
		//basic sounds
		bool		m_bWalkSound;		
		
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
		void SendStats();
		void SendDeathInfo();
		void SendAliveInfo();
		
		void UpdateStats();		
		void SetupCharacterStats();
		
		void OnDeath();
		
	public:
		Stats	m_kCharacterStats;

	
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
		
		//alive
		void MakeAlive();
		bool IsDead()													{	return m_bDead;					}
		
		//combat mode
		void SetCombatMode(bool bCombat)							{	m_bCombatMode = bCombat;		}
		bool GetCombatMode()											{	return m_bCombatMode;			}
		
		
		void SetName(const string& strName)						{	m_strName = strName;						ResetAllNetUpdateFlags();}
		void SetOwnedByPlayer(const string& strPlayer)		{	m_strOwnedByPlayer = strPlayer;		ResetAllNetUpdateFlags();}
		void SetIsPlayerCharacter(bool bIsPlayer)				{	m_bIsPlayerCharacter = bIsPlayer;	ResetAllNetUpdateFlags();}
		void SetOverHeadText(bool bOverHead)					{	m_bOverHeadText = bOverHead;	}
		
		//sound
		void SetWalkSound(const string& strFile)				{	m_strWalkSound = strFile;				ResetAllNetUpdateFlags();}
		void SetRunSound(const string& strFile)				{	m_strRunSound = strFile;				ResetAllNetUpdateFlags();}
		void SetJumpSound(const string& strFile)				{	m_strJumpSound = strFile;				ResetAllNetUpdateFlags();}
		void SetSwimSound(const string& strFile)				{	m_strSwimSound = strFile;				ResetAllNetUpdateFlags();}
		
		void SetFaction(int iFaction)								{	m_iFaction = iFaction;		}
		
		string GetName()												{	return m_strName;					}
		string GetOwnedByPlayer()									{	return m_strOwnedByPlayer;		}
		bool	 GetIsPlayerCharacter()								{	return m_bIsPlayerCharacter;	}
		int	 GetFaction()											{	return m_iFaction;				}
		float  GetLegLength()										{	return m_fLegLength;				}
		float  GetMarkerSize()										{	return m_fMarkerSize;			}
		
		//buff
		P_Buff* AddBuff(const string& strBuffName);			//creates and adds buff to character 
		void RemoveBuff(P_Buff* pkBuff);							//removes buffs
		void RemoveBuff(const string& strBuffName);
		
		//skills
		bool	AddSkill(const string& strSkillScript,const string& strParentSkill);
		bool	ChangeSkill(const string& strSkillScript,int iValue);
		void	SetSkill(const string& strSkillScript,int iLevel);
		Skill* GetSkillPointer(const string& strSkillName);
		int	UseSkill(const string& strSkillScript,int iTarget,const Vector3& kPos,const Vector3& kDir);
		void	RemoveAllSkills();		
		void	UpdateSkills();
		vector<Skill*>*	GetSkillList()			{	return &m_kSkills;	};
		
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








