#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "p_charactercontrol.h"

#include "../zerofpsv2/script/zfscript.h"

using namespace std;

class P_Buff;


enum MCOMMON_API TargetType
{
	ePASSIVE				=	0,
	eCHARACTER_TARGET =	1,
	eGROUND_TARGET 	=	2,
	eDIRECTION 			=	3,						
	eITEM_TARGET		=	4,
	eSELF					=	5
};

enum MCOMMON_API SkillType
{
	eMISC			=	0,
	eOFFENSIVE	=	1,
	eDEFENSIVE	=	2,
	eTRADE		=	3
};	

class MCOMMON_API Skill
{
	private:
		ZSSScriptSystem*	m_pkScript;		
		ZFResourceHandle*	m_pkScriptFileHandle;	
		ZSSZeroFps*			m_pkZeroFps;
		ZSSEntityManager*	m_pkEntityManager;
		
		string	m_strSkillScript;
		
		//owner character
		int		m_iOwnerID;				
		
		//skill data
		int		m_iLevel;
		string	m_strParentSkill;
		
		//wat base item types needs to be eqiped to use this skill?
		vector<string>	m_kBaseTypes;

		string	m_strInGameName;
		//string	m_strSchool;
		string	m_strIcon;
		string	m_strInfoText;
		
		float		m_fCastTime;
		float		m_fReloadTime;
		float		m_fLastUpdate;
		
		int		m_iTargetType;
		
		float		m_fStaminaUsage;
		float		m_fManaUsage;
				
		int		m_iSkillType;		
		float		m_fTimeLeft;
		float		m_fRange;
		
		
		void UpdateFromScript();			

	public:			
		Skill(const string& strScriptFile,const string& strParent, int iOwnerID);
		~Skill();
		
		void Update();
		int Use(int iTargetID,const Vector3& kPos,const Vector3& kDir);
		
		bool IsReloaded();
		bool CanUse();

		//sets
		void SetLevel(int iLevel);
		void SetTimeLeft(float fTime)	{	m_fTimeLeft = fTime;				}
		
		//gets
		string GetScreenName()							{	return m_strInGameName;						};
		string GetName()									{	return m_strSkillScript;					};
		string GetIcon()									{	return m_strIcon;								};
		string GetInfoText()								{	return m_strInfoText;						};
		int	 GetLevel()									{	return m_iLevel;								};
		string GetParent()								{	return m_strParentSkill;					};		
		int	 GetSkillType()							{	return m_iSkillType;							};
		float	 GetTimeLeft()								{	return m_fTimeLeft;							};
		float	 GetRange()									{	return m_fRange;								};
		float	 GetReloadTime()							{	return m_fReloadTime;						};
		float  GetStaminaUsage()						{	return m_fStaminaUsage;						};
		float  GetManaUsage()							{	return m_fManaUsage;							};
		float  GetCastTime()								{	return m_fCastTime;							};
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
 		map<string,Stat>	m_kStats;
		Stat* GetStatPointer(const string& strName);

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

struct AIDebugVar
{
	string	m_Variable;
	string	m_Value;
};

class MCOMMON_API P_CharacterProperty: public Property
{
	private:
		ZSSAudioSystem*	m_pkAudioSystem;
		ZSSRender*			m_pkRender;	
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSEntityManager*	m_pkEntityMan;
		Application*		m_pkApp;
		
		//dirs
		string	m_strBuffDir;
		
		
		//stat
		float			m_fStatTimer;		
		float			m_fLegLength;
		float			m_fMarkerSize;
		
		bool			m_bInCamp;
		
		//death
		bool			m_bIncap;
		bool			m_bDead;
		float			m_fDeadTimer;
		float			m_fDecayTime;					//how long does a non player body stay in the world before decaying
		
		//combat		
		bool								m_bCombatMode;
		int								m_iTarget;
		string							m_strNextSkill;		
		string							m_strDefaultAttackSkill;
		float								m_fCombatTimer;
		int								m_iLastDamageFrom;			//what character did the last damage on me
		
		//respawn
		float			m_fRespawnTime;
		int			m_iRespawnZone;
		Vector3		m_kRespawnPos;
		
		//skills
		vector<Skill*>	m_kSkills;				//what skills do we have
		float				m_fSkillTimer;			//last skill's update
		float				m_fSkillLockTime;		//lock skill usage until
		vector<string>	m_kSkillBar;
		
		//over head text
		ZMaterial*		m_pkTextMaterial;
		ZGuiFont*		m_pkFont;
		RenderPackage	m_kRenderPackage;		
		bool				m_bOverHeadText;			//shuld overhead text be drawn?
		
		
		string		m_strName;					//character name		
		bool			m_bIsPlayerCharacter;	//is this and NPC or PC ?
		int			m_iFaction;
		int			m_iGroup;					
		string		m_strOwnedByPlayer;		//who owns me ? if anyone
		string		m_strChatMsg;				//last chat msg
		float			m_fChatTime;				//lst chat msg time
		
		
		int			m_iConID;					//player connection ID if any				
		bool			m_bFirstUpdate;
		
		int			m_iCurrentCharacterState;
			
		//basic sounds
		bool		m_bWalkSound;		
		
		string	m_strWalkWaterSound;
		string	m_strRunWaterSound;
		string	m_strWalkSound;
		string	m_strRunSound;
		string	m_strJumpSound;
		string	m_strSwimSound;		
		int		m_iRunWaterSoundID;		
		int		m_iWalkWaterSoundID;
		int		m_iWalkSoundID;
		int		m_iRunSoundID;
		int		m_iSwimSoundID;
		
		vector<AIDebugVar>	kDebugVar;

		void PlayCharacterMovementSounds();
		void SetupContainers();		
		void SendBuffList();
		void SendStats();
		void SendDeathInfo();
		void SendAliveInfo();
		void SendTextToClient(const string& strText);
		void SendPointText(const string& strText,const Vector3& kPos,int iType);
		void SendSkillbar(const string& strSkill = "");
		
		void UpdateStats();					//updates character stats
		void UpdateSkillQueue();			//updates combat, updates and performs combat queue
		void UpdateSkills();					//updates skill reloads and so on

		void SetupCharacterStats();		//first tim character stat setup
		void SetupSpawnPos();
		void CreateSpawner();	
		
		void OnDeath();
		void OnIncap();
		void OnLevelUP();
		
		float Facing(Entity* pkCharacter,const Vector3& kPos);
		void DoParryAnimation(int iAttacker);
		
	public:
		Stats		m_kCharacterStats;
		bool		m_iDMNoTarget;				

	
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
		void GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState);
		
		void DrawEditor();

				
		//public network stuff
		void SendCloseContainer(int iContainerID);
		void SendSkillTree();
		void SendSkillInfo(const string& strSkill);
		
		//alive
		void MakeAlive();
		bool IsDead()													{	return m_bDead;					}
		bool IsIncap()													{	return m_bIncap;					}
			
		//sound
		void SetWalkSound(const string& strFile)				{	m_strWalkSound = strFile;				ResetAllNetUpdateFlags();}
		void SetRunSound(const string& strFile)				{	m_strRunSound = strFile;				ResetAllNetUpdateFlags();}
		void SetJumpSound(const string& strFile)				{	m_strJumpSound = strFile;				ResetAllNetUpdateFlags();}
		void SetSwimSound(const string& strFile)				{	m_strSwimSound = strFile;				ResetAllNetUpdateFlags();}
		
		//sets
		void SetName(const string& strName)						{	m_strName = strName;						ResetAllNetUpdateFlags();}
		void SetOwnedByPlayer(const string& strPlayer)		{	m_strOwnedByPlayer = strPlayer;		ResetAllNetUpdateFlags();}
		void SetIsPlayerCharacter(bool bIsPlayer)				{	m_bIsPlayerCharacter = bIsPlayer;	ResetAllNetUpdateFlags();}
		void SetOverHeadText(bool bOverHead)					{	m_bOverHeadText = bOverHead;	}		
		void SetClient(int iConID)									{	m_iConID = iConID;				}	
		int  Getclient()												{	return m_iConID;					}
		
		//gets
		string GetName()												{	return m_strName;					}
		string GetOwnedByPlayer()									{	return m_strOwnedByPlayer;		}
		bool	 GetIsPlayerCharacter()								{	return m_bIsPlayerCharacter;	}
		float  GetLegLength()										{	return m_fLegLength;				}
		float  GetMarkerSize()										{	return m_fMarkerSize;			}

		bool	CanRest();
		bool	InCamp();

		//faction  stuff
		void SetFaction(int iFaction)								{	m_iFaction = iFaction;		}
		int  GetFaction()												{	return m_iFaction;			}
		void SetGroup(int iGroup)									{	m_iGroup = iGroup;			}
		int  GetGroup()												{	return m_iGroup;				}
				
		bool IsEnemy(int iCharacterID);						//is specified character an enemy to this character
		bool IsFriend(int iCharacterID);						//is specified character a friend to this character
		bool IsNeutral();											//is this character neutral
		
		
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
		vector<Skill*>*	GetSkillList()			{	return &m_kSkills;				};
		void  LockSkillUsage(float fTime);
		void	AddSkillToSkillbar(const string& strSkill,int iPos);
		void  RemoveItemFromSkillbar(int iPos);
		void  ReloadAllSkills();

		//experience and level
		void  GiveExperience(int iXP);		
		void  GiveGroupExperience(int iXP);		
		void  AddStatPoint(int iStat);
		bool	AddSkillPoint(const string& strSkillName);
		
		//mana and stamina
		bool UseMana(float fMana);                                 
		bool UseStamina(float fStamina);
		
		//combat
		void SetTarget(int iTargetID);
		void SetCombatMode(bool bCombat)								{	m_bCombatMode = bCombat;		}
		bool GetCombatMode()												{	return m_bCombatMode;			}
		void AddSkillToQueue(const string& strSkill,int iTargetID);
		void SetDefaultAttackSkill(const string& strDA);
		string GetDefaultAttackSkill()								{	return m_strDefaultAttackSkill;}		
		void SetLastDamageFrom(int iCharacterID)					{	m_iLastDamageFrom=iCharacterID;}
		
		void OnHit(int iAttacker,int iDamage);
		void OnMiss(int iAttacker);
		
		//client code
		void AddChatMsg(const string& strChatMsg);
		
		//inventory
		bool HaveEqipedBaseType(const string& strBaseName);
		
				
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;		

		// AI Debug var
		void DebugVarClear() { kDebugVar.clear();  }
		void DebugSet(string strName, string strValue)
		{
			for(unsigned int i=0; i<kDebugVar.size(); i++)
			{
				if(kDebugVar[i].m_Variable == strName)
				{
					kDebugVar[i].m_Value = strValue;
					return;
				}
			}
			
			AIDebugVar kVar;
			kVar.m_Variable = strName;
			kVar.m_Value = strValue;
			kDebugVar.push_back(kVar);
		}

		void DebugSet(string strName, int iValue)
		{
			char szTxt[20];
			sprintf(szTxt,"%d", iValue);
			DebugSet("Health: ",szTxt);
		}

};

MCOMMON_API Property* Create_P_CharacterProperty();
MCOMMON_API void Register_P_CharacterProperty(ZSSZeroFps* pkZeroFps);

#endif








