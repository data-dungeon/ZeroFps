#include "p_characterproperty.h"
#include "p_charactercontrol.h"
#include "p_container.h"
#include "p_item.h"
#include "p_buff.h"
#include "ml_netmessages.h"

#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 



//--------SKILL----------------

Skill::Skill(const string& strScriptFile,const string& strParent, int iOwnerID)
{
	m_pkScript = static_cast<ZFScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZFScriptSystem"));
	m_pkZeroFps= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkEntityManager = static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	
	//create and setup script
	m_pkScriptFileHandle = new ZFResourceHandle;
	

	
	m_strSkillScript = 	strScriptFile;	
	m_pkScriptFileHandle->SetRes(string("data/script/objects/game objects/skills/") + strScriptFile);
	
	
	m_strParentSkill=		strParent;	
	m_iOwnerID = 			iOwnerID;
		
	m_strInGameName =		"UnkownSkill";
	m_strIcon = 			"default.tga";	
	m_iLevel = 				0;	
	m_fReloadTime =		1;
	m_fTimeLeft =			1;
	m_fLastUpdate = 		-1;
	m_strSchool =			"UnkownSchool";
	
}

Skill::~Skill()
{
	delete m_pkScriptFileHandle;

}


void Skill::Update()
{
	if( (m_pkEntityManager->GetSimTime() < m_fLastUpdate) || m_fLastUpdate == -1)
	{
		cout<<"skill time dont match"<<endl;				
		m_fLastUpdate =m_pkEntityManager->GetSimTime();
		return; 
	}

	//already reloaded
	if(m_fTimeLeft == 0)
		return;
		
	//decrese timeleft
	m_fTimeLeft -= m_pkEntityManager->GetSimTime() - m_fLastUpdate ;			
	if(m_fTimeLeft < 0)
		m_fTimeLeft = 0;
	
	//update last update time
	m_fLastUpdate = m_pkEntityManager->GetSimTime();
	
	if(m_fTimeLeft == 0)
		cout<<"skill reloaded:"<<GetName()<<endl;

}

void Skill::UpdateFromScript()
{
	if(!m_pkScriptFileHandle->IsValid())
	{
		cout<<"WARNING: skill script "<<m_pkScriptFileHandle->GetRes()<<" not loaded"<<endl;
		return;	
	}

	//setup parameters to Update function
	vector<ScriptFuncArg> args(2);
	args[0].m_kType.m_eType = tINT;
	args[0].m_pData = &m_iOwnerID;			//owner character id
	args[1].m_kType.m_eType = tINT;
	args[1].m_pData = &m_iLevel;				//skill level
	
	
	if(!m_pkScript->Call(m_pkScriptFileHandle, "Update",args))
	{
		cout<<"WARNING: could not call update function for skill script "<<m_pkScriptFileHandle->GetRes()<<" level "<<m_iLevel<<endl;
		return;
	}

	
	//print some nice info
	cout<<"UPDATED FROM SCRIPT"<<endl;
	cout<<"script:    "<<m_pkScriptFileHandle->GetRes()<<endl;
	cout<<"ingamename:"<<m_strInGameName<<endl;
	cout<<"school:    "<<m_strSchool<<endl;
	cout<<"icon:      "<<m_strIcon<<endl;
	cout<<"reloadtime:"<<m_fReloadTime<<endl;
	cout<<"level:     "<<m_iLevel<<endl;
	cout<<"parent:    "<<m_strParentSkill<<endl;
}

void Skill::SetLevel(int iLevel)
{
	//change level
	m_iLevel = iLevel;
	
	//update skillstats from script
	UpdateFromScript();
	
	return;
}


void Skill::Use(int iTargetID,const Vector3& kPos,const Vector3& kDir)
{
// 	cout<<"using skill "<<m_pkScriptFileHandle->GetRes()<<endl;

	if(!m_pkScriptFileHandle->IsValid())
	{
		cout<<"WARNING: skill script "<<m_pkScriptFileHandle->GetRes()<<" not loaded"<<endl;
		return;	
	}
	
	//check reload
	if(m_fTimeLeft != 0)
	{
		cout<<"skill not reloaded yet"<<endl;
		return;	
	}
	
	if(m_iLevel < 1)
	{
		cout<<"got to have at least level 1 of a skill to use it"<<endl;
		return;	
	}
	
	
	static Vector3 kPosCopy,kDirCopy;
	kPosCopy = kPos;
	kDirCopy = kDir;

	//setup parameters to Update function
	vector<ScriptFuncArg> args(9);
	args[0].m_kType.m_eType = tINT;
	args[0].m_pData = &m_iOwnerID;			//owner character id
	args[1].m_kType.m_eType = tINT;
	args[1].m_pData = &m_iLevel;				//skill level
	args[2].m_kType.m_eType = tINT;
	args[2].m_pData = &iTargetID;		
	
	args[3].m_kType.m_eType = tFLOAT;
	args[3].m_pData = &kPosCopy.x;		
	args[4].m_kType.m_eType = tFLOAT;
	args[4].m_pData = &kPosCopy.y;		
	args[5].m_kType.m_eType = tFLOAT;
	args[5].m_pData = &kPosCopy.z;		
	
	args[6].m_kType.m_eType = tFLOAT;
	args[6].m_pData = &kDirCopy.x;		
	args[7].m_kType.m_eType = tFLOAT;
	args[7].m_pData = &kDirCopy.y;		
	args[8].m_kType.m_eType = tFLOAT;
	args[8].m_pData = &kDirCopy.z;		

	
		
	if(!m_pkScript->Call(m_pkScriptFileHandle, "Use",args))
	{
		cout<<"WARNING: could not call update function for skill script "<<m_pkScriptFileHandle->GetRes()<<" level "<<m_iLevel<<endl;
		return;
	}			
	
	//reset reload timer
	m_fTimeLeft = m_fReloadTime;
	m_fLastUpdate = m_pkEntityManager->GetSimTime();
}


//-------CharacterStats-----------


Stats::Stats()
{
	
}

void Stats::GetStatsList(vector<Stat>* pkStats)
{
	for(int i = 0;i<m_kStats.size();i++)
	{
		pkStats->push_back(m_kStats[i]);	
	}
}


void Stats::AddStat(const string& strName,float fValue,float fMod)
{
	//check if stat already exist
	for(int i = 0;i<m_kStats.size();i++)
	{
		if(m_kStats[i].m_strName == strName)
		{
			cout<<"Stat "<<strName<<" already exist in stats list"<<endl;
			return;
		}
	}
		
	m_kStats.push_back(Stat(strName,fValue,fMod));

}

void Stats::AddOn(Stats* pkTarget)
{
	for(int i = 0;i<m_kStats.size();i++)
	{
		pkTarget->ChangeMod(m_kStats[i].m_strName, m_kStats[i].m_fValue + m_kStats[i].m_fMod );
		
		cout<<"Applying stat "<<m_kStats[i].m_strName<<" "<<m_kStats[i].m_fValue + m_kStats[i].m_fMod<<endl;
	}
}

void Stats::RemoveOn(Stats* pkTarget)
{
	for(int i = 0;i<m_kStats.size();i++)
	{
		pkTarget->ChangeMod(m_kStats[i].m_strName, -(m_kStats[i].m_fValue + m_kStats[i].m_fMod) );
		
		cout<<"removing stat "<<m_kStats[i].m_strName<<" "<<m_kStats[i].m_fValue + m_kStats[i].m_fMod<<endl;
	}
}

void Stats::Save(ZFIoInterface* pkPackage)
{
	//stat version
	static int iVersion = 2;
	pkPackage->Write(iVersion);	

	pkPackage->Write(m_kStats.size());
	for(int i = 0;i<m_kStats.size();i++)
	{
		pkPackage->Write_Str(m_kStats[i].m_strName);
		pkPackage->Write(m_kStats[i].m_fValue);
		pkPackage->Write(m_kStats[i].m_fMod);	
	}

	//pkPackage->Write_Str(string(""));
	
	
// 	cout<<"saved stats"<<endl;
}

void Stats::Load(ZFIoInterface* pkPackage)
{
	static string strName;
	static float fValue,fMod;
	
	
	int iVersion;
	pkPackage->Read(iVersion);		
	
	if(iVersion == 1)
	{	
		m_kStats.clear();
		pkPackage->Read_Str(strName);
		while(!strName.empty())
		{
			pkPackage->Read(fValue);
			pkPackage->Read(fMod);
			
			m_kStats.push_back(Stat(strName,fValue,0)); //dont load mod value
			
			pkPackage->Read_Str(strName);
		}
	}
	
	if(iVersion == 2)
	{	
		m_kStats.clear();
		
		int iNr;
		pkPackage->Read(iNr);
		for(int i =0;i<iNr;i++)
		{
			pkPackage->Read_Str(strName);			
			pkPackage->Read(fValue);
			pkPackage->Read(fMod);
			
			m_kStats.push_back(Stat(strName,fValue,0)); //dont load mod value
		}
		
	}	
// 	cout<<"loaded stats"<<endl;
}

void Stats::SetStat(const string& strName,float fValue)
{
	for(int i=0;i<m_kStats.size();i++)
		if(m_kStats[i].m_strName == strName)
		{
			m_kStats[i].m_fValue = fValue;
			return;
		}
		
	cout<<"WARNING:SetStat requested stat "<<strName<<" was not found"<<endl;
}

float Stats::GetStat(const string& strName)
{
	for(int i=0;i<m_kStats.size();i++)
		if(m_kStats[i].m_strName == strName)
		{
			return m_kStats[i].m_fValue;
		}		

	cout<<"WARNING:GetStat requested stat "<<strName<<" was not found"<<endl;
	return 0;
}
		
void Stats::ChangeStat(const string& strName,float fValue)
{
	for(int i=0;i<m_kStats.size();i++)
		if(m_kStats[i].m_strName == strName)
		{
			m_kStats[i].m_fValue += fValue;
			return;
		}
		
	cout<<"WARNING:ChangeStat requested stat "<<strName<<" was not found"<<endl;
}


void Stats::SetMod(const string& strName,float fValue)
{
	for(int i=0;i<m_kStats.size();i++)
		if(m_kStats[i].m_strName == strName)
		{
			m_kStats[i].m_fMod = fValue;
			return;
		}
	
	cout<<"WARNING:SetMod requested stat "<<strName<<" was not found"<<endl;
}

float Stats::GetMod(const string& strName)
{
	for(int i=0;i<m_kStats.size();i++)
		if(m_kStats[i].m_strName == strName)
		{
			return m_kStats[i].m_fMod;
		}		

		
	cout<<"WARNING:GetMod requested stat "<<strName<<" was not found"<<endl;
	return 0;
}		

void Stats::ChangeMod(const string& strName,float fValue)
{
	for(int i=0;i<m_kStats.size();i++)
		if(m_kStats[i].m_strName == strName)
		{
			m_kStats[i].m_fMod += fValue;
			return;	
		}
		
	cout<<"WARNING:ChangeMod requested stat "<<strName<<" was not found"<<endl;
}

float Stats::GetTotal(const string& strName)
{
	for(int i=0;i<m_kStats.size();i++)
		if(m_kStats[i].m_strName == strName)
		{
			return m_kStats[i].m_fValue + m_kStats[i].m_fMod;
		}				
		
		
	cout<<"WARNING:GetTotal requested stat "<<strName<<" was not found"<<endl;
	return 0;
}


//-------P_CharacterProperty---------
P_CharacterProperty::P_CharacterProperty()
{
	m_pkAudioSystem = 	static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));			
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
	m_pkEntityMan=			static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));			
	m_pkApp=					static_cast<Application*>(g_ZFObjSys.GetObjectPtr("Application"));			
	
	strcpy(m_acName,"P_CharacterProperty");
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;

	m_bNetwork = 	true;
	m_iVersion = 	8;
	
	//initiate stuff
	m_strBuffDir			=	"data/script/objects/game objects/buffs/";
	
	m_kCurrentCharacterStates.reset();
	
	m_iConID					=	-1;
	m_strName 				=	"NoName";
	m_strOwnedByPlayer 	=	"NoPlayer";
	m_bIsPlayerCharacter =	false;
	m_bOverHeadText		=	true;	
	m_bFirstUpdate			=	true;
	m_fChatTime				=	0;
	m_strChatMsg			=	"";
	m_fStatTimer			=	0;
	m_fSkillTimer			=	0;
	m_iFaction				=	0;
	m_bWalkSound			=	true;
	m_fLegLength			=	0;
	m_fMarkerSize			=	1;
	m_bDead					=	false;
	
	//container id's
	m_iInventory	= -1;		
	m_iHead			= -1;
	m_iGloves		= -1;
	m_iCape			= -1;		
	m_iNecklace		= -1;				
	m_iBody			= -1;
	m_iLeftHand		= -1;
	m_iRightHand	= -1;		
	m_iBracers		= -1;		
	m_iRing1			= -1;
	m_iRing2			= -1;
	m_iRing3			= -1;
	m_iRing4			= -1;
	m_iBelt			= -1;		
	m_iFeets			= -1;
		
		
	//basic sounds
	m_strWalkSound			=	"data/sound/footstep_forest.wav";
	m_strRunSound			=	"data/sound/footstep_forest_run.wav";
	m_strJumpSound			=	"data/sound/jump.wav";
	m_strSwimSound			=	"swim.wav";	
	m_iWalkSoundID 		= 	-1;
	m_iRunSoundID 			= 	-1;
	m_iSwimSoundID 		= 	-1;
	
	
	//setup material
	m_pkTextMaterial = new ZMaterial;
	m_pkTextMaterial->GetPass(0)->m_kTUs[0]->SetRes("data/textures/text/fetfont.tga");
	m_pkTextMaterial->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
	m_pkTextMaterial->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
	m_pkTextMaterial->GetPass(0)->m_bLighting = 				false;		
	m_pkTextMaterial->GetPass(0)->m_bColorMaterial = 		true;
	m_pkTextMaterial->GetPass(0)->m_kVertexColor =			Vector3(0,1,0);
	m_pkTextMaterial->GetPass(0)->m_bFog = 					true;		
	m_pkTextMaterial->GetPass(0)->m_bAlphaTest =				true;		
	m_pkTextMaterial->GetPass(0)->m_bDepthTest = 			true;

	//setup font
	m_pkFont = new ZGuiFont("CharacterFont");
	m_pkFont->Create("/data/textures/text/fetfont.fnt",-1);	
}

void P_CharacterProperty::Init()
{
	SetupCharacterStats();
}


vector<PropertyValues> P_CharacterProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);

	kReturn[0].kValueName = "walksound";
	kReturn[0].iValueType = VALUETYPE_BOOL; 
	kReturn[0].pkValue    = (void*)&m_bWalkSound;	

	kReturn[1].kValueName = "markersize";
	kReturn[1].iValueType = VALUETYPE_FLOAT; 
	kReturn[1].pkValue    = (void*)&m_fMarkerSize;	
		
	return kReturn;	
}

void P_CharacterProperty::SetupCharacterStats()
{
	m_kCharacterStats.AddStat("Level"			,0,0);
	m_kCharacterStats.AddStat("Experience"	,0,0);
	m_kCharacterStats.AddStat("NextLevel"		,0,0);
	
	m_kCharacterStats.AddStat("Speed"			,0,0);
	m_kCharacterStats.AddStat("Jump"			,0,0);
	
	m_kCharacterStats.AddStat("Mana"			,0,0);
	m_kCharacterStats.AddStat("ManaMax"		,0,0);
	m_kCharacterStats.AddStat("ManaRegen"		,0,0);	
	
	m_kCharacterStats.AddStat("Health"			,0,0);
	m_kCharacterStats.AddStat("HealthMax"		,0,0);
	m_kCharacterStats.AddStat("HealthRegen"	,0,0);

	m_kCharacterStats.AddStat("Stamina"		,0,0);
	m_kCharacterStats.AddStat("StaminaMax"	,0,0);
	m_kCharacterStats.AddStat("StaminaRegen"	,0,0);	
		
	m_kCharacterStats.AddStat("Strength"		,0,0);
	m_kCharacterStats.AddStat("Dexterity"		,0,0);
	m_kCharacterStats.AddStat("Vitality"		,0,0);
	m_kCharacterStats.AddStat("Intelligence"	,0,0);
	m_kCharacterStats.AddStat("Wisdom"			,0,0);
	m_kCharacterStats.AddStat("Charisma"		,0,0);
			
	m_kCharacterStats.AddStat("Load"			,0,0);
	m_kCharacterStats.AddStat("LoadMax"		,0,0);

	m_kCharacterStats.AddStat("Attack"			,0,0);	
	m_kCharacterStats.AddStat("DamageSlashingMin"	,0,0);
	m_kCharacterStats.AddStat("DamageSlashingMax"	,0,0);
	m_kCharacterStats.AddStat("DamageCrushingMin"	,0,0);
	m_kCharacterStats.AddStat("DamageCrushingMax"	,0,0);
	m_kCharacterStats.AddStat("DamagePiercingMin"	,0,0);
	m_kCharacterStats.AddStat("DamagePiercingMax"	,0,0);
	m_kCharacterStats.AddStat("DamageFireMin"			,0,0);
	m_kCharacterStats.AddStat("DamageFireMax"			,0,0);
	m_kCharacterStats.AddStat("DamageIceMin"			,0,0);
	m_kCharacterStats.AddStat("DamageIceMax"			,0,0);
	m_kCharacterStats.AddStat("DamageLightningMin"	,0,0);
	m_kCharacterStats.AddStat("DamageLightningMax"	,0,0);
	m_kCharacterStats.AddStat("DamageMagicMin"		,0,0);
	m_kCharacterStats.AddStat("DamageMagicMax"		,0,0);
	m_kCharacterStats.AddStat("DamagePoisonMin"		,0,0);
	m_kCharacterStats.AddStat("DamagePoisonMax"		,0,0);
	
	m_kCharacterStats.AddStat("AbsorbSlashing"	,0,0);
	m_kCharacterStats.AddStat("AbsorbCrushing"	,0,0);
	m_kCharacterStats.AddStat("AbsorbPiercing"	,0,0);
	m_kCharacterStats.AddStat("AbsorbFire"			,0,0);
	m_kCharacterStats.AddStat("AbsorbIce"			,0,0);
	m_kCharacterStats.AddStat("AbsorbLightning"	,0,0);
	m_kCharacterStats.AddStat("AbsorbMagic"		,0,0);
	m_kCharacterStats.AddStat("AbsorbPoison"		,0,0);
	
	m_kCharacterStats.AddStat("Avoid"				,0,0);		//chance to avoid an attack in %
	m_kCharacterStats.AddStat("Block"				,0,0);		//chance to block an attack in %	
}

void P_CharacterProperty::UpdateStats()
{
	//get character control;
	P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl");
	if(!pkCC)
	{
		cout<<"WARNING: character missing P_CharacterControl"<<endl;
		return;	
	}
	
	
	//check if character is dead
	if(m_kCharacterStats.GetTotal("Health") <= 0)
	{
		cout<<"Character is soooo dead"<<endl;
		OnDeath();
		return;
	}
		
	
	float fTime = m_pkZeroFps->GetTicks();
	
	//update stats each seccond	
	if(fTime > m_fStatTimer + 1.0)
	{
		m_fStatTimer = fTime;
	
		
		//stamina
		int iDrain = 0;
		if(pkCC->GetCharacterState(eWALKING))
			iDrain = 2;		
		if(pkCC->GetCharacterState(eRUNNING))
			iDrain = 5;
		if(pkCC->GetCharacterState(eJUMPING))
			iDrain = 10;
		if(pkCC->GetCharacterState(eSWIMMING))
			iDrain = 8;

			
		string strStamina("Stamina");		
		m_kCharacterStats.ChangeStat(strStamina,-iDrain);
		m_kCharacterStats.ChangeStat(strStamina,m_kCharacterStats.GetTotal("StaminaRegen"));
		if(m_kCharacterStats.GetTotal(strStamina) > m_kCharacterStats.GetTotal("StaminaMax"))
			m_kCharacterStats.SetStat(strStamina,m_kCharacterStats.GetTotal("StaminaMax"));
		if(m_kCharacterStats.GetTotal(strStamina) < 0)
			m_kCharacterStats.SetStat(strStamina,0);
			
			
		//health
		string strHealth("Health");
		m_kCharacterStats.ChangeStat(strHealth,m_kCharacterStats.GetTotal("HealthRegen"));
		if(m_kCharacterStats.GetTotal(strHealth) > m_kCharacterStats.GetTotal("HealthMax"))
			m_kCharacterStats.SetStat(strHealth,m_kCharacterStats.GetTotal("HealthMax"));
		if(m_kCharacterStats.GetTotal(strHealth) < 0)
			m_kCharacterStats.SetStat(strHealth,0);

		//mana
		string strMana("Mana");
		m_kCharacterStats.ChangeStat(strMana,m_kCharacterStats.GetTotal("ManaRegen"));
		if(m_kCharacterStats.GetTotal(strMana) > m_kCharacterStats.GetTotal("ManaMax"))
			m_kCharacterStats.SetStat(strMana,m_kCharacterStats.GetTotal("ManaMax"));
		if(m_kCharacterStats.GetTotal(strMana) < 0)
			m_kCharacterStats.SetStat(strMana,0);
				
			
		SendStats();
	}






	//setup charactercontrol
	int iSpeed = m_kCharacterStats.GetTotal("Speed");
	int iJump = m_kCharacterStats.GetTotal("Jump");
	
	if(m_kCharacterStats.GetTotal("Stamina") < 1 )
	{
		iSpeed = 0;
		iJump = 0;
	}
	
	//speed
	pkCC->SetSpeed(iSpeed);
	
	//jump
	pkCC->SetJumpForce(iJump);		
	
	
	
	
}

void P_CharacterProperty::MakeAlive()
{
	m_bDead = false;

	//make sure character has some life
	if(m_kCharacterStats.GetTotal("Health") <= 0)
		m_kCharacterStats.SetStat("Health",1);
	
	
	//disable character movement
	if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl"))
	{
		pkCC->SetEnabled(true);	
	}
}


void P_CharacterProperty::OnDeath()
{
	m_bDead = true;
	
	m_pkEntityManager->CallFunction(m_pkEntity, "Death");
	
	if(P_Mad* pkMad = (P_Mad*)m_pkEntity->GetProperty("P_Mad"))
	{
		pkMad->SetAnimation("Death",0);
		pkMad->SetNextAnimation(MAD_NOLOOP);	
	}
	
	
	//disable character movement
	if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl"))
	{
		pkCC->SetEnabled(false);	
	}
}

void P_CharacterProperty::UpdateSkills()
{
	float fTime = m_pkZeroFps->GetTicks();
	
	//update each seccond
	if( fTime > m_fSkillTimer + 0.1)
	{
		m_fSkillTimer = fTime;
		
		for(int i = 0;i<m_kSkills.size();i++)
		{
			m_kSkills[i]->Update();
		}
	}
}


void P_CharacterProperty::SetupContainers()
{
	vector<Entity*>	kEntitys;
	m_pkEntity->GetChilds(&kEntitys);
	
	//cout<<"seting up character containers"<<endl;
	for(int i = 0;i<kEntitys.size();i++)
	{
		if(P_Container* pkContainer = (P_Container*)kEntitys[i]->GetProperty("P_Container"))
		{
			//set owner
			pkContainer->SetOwnerID(GetEntity()->GetEntityID());
			
			//setup container id's
			switch(pkContainer->GetContainerType())
			{
				case eInventory:
					//cout<<"found inventory"<<endl;
					m_iInventory = kEntitys[i]->GetEntityID();
					break;
				case eBody:
					//cout<<"found body"<<endl;
					m_iBody = kEntitys[i]->GetEntityID();
					break;
				case eHead:
					//cout<<"found head"<<endl;
					m_iHead= kEntitys[i]->GetEntityID();
					break;
				case eLeftHand:
					//cout<<"found left hand"<<endl;
					m_iLeftHand = kEntitys[i]->GetEntityID();
					break;
				case eRightHand:
					//cout<<"found right hand"<<endl;
					m_iRightHand = kEntitys[i]->GetEntityID();
					break;			
				case eGloves:
					//cout<<"found gloves"<<endl;
					m_iGloves = kEntitys[i]->GetEntityID();
					break;
				case eCape:
					//cout<<"found Cape"<<endl;
					m_iCape = kEntitys[i]->GetEntityID();
					break;										
				case eNecklace:
					//cout<<"found Necklace"<<endl;
					m_iNecklace = kEntitys[i]->GetEntityID();
					break;										
				case eBracers:
					//cout<<"found Bracers"<<endl;
					m_iBracers = kEntitys[i]->GetEntityID();
					break;										
				case eBelt:
					//cout<<"found Belt"<<endl;
					m_iBelt = kEntitys[i]->GetEntityID();
					break;										
				case eFeets:
					//cout<<"found feets"<<endl;
					m_iFeets = kEntitys[i]->GetEntityID();
					break;										
				case eRing1:
					//cout<<"found ring1"<<endl;
					m_iRing1= kEntitys[i]->GetEntityID();
					break;										
				case eRing2:
					//cout<<"found ring2"<<endl;
					m_iRing2 = kEntitys[i]->GetEntityID();
					break;										
				case eRing3:
					//cout<<"found ring3"<<endl;
					m_iRing3 = kEntitys[i]->GetEntityID();
					break;										
				case eRing4:
					//cout<<"found ring4"<<endl;
					m_iRing4 = kEntitys[i]->GetEntityID();
					break;										
					
			}
		}
	}

	if(m_iInventory == -1)
	{
		P_Container* pkCon;
		Entity* pkContainer;
		
		//cout<<"no containers found, creating new ones"<<endl;
		
		//inventory
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iInventory = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());			
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(6,9);
				pkCon->SetContainerType(eInventory);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);			
				
		//body	
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iBody = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());			
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");
				pkCon->SetSize(3,4);
				pkCon->SetContainerType(eBody);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_BODY);
				pkCon->SetEquip(true);
				
		//head
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iHead = pkContainer->GetEntityID();		
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");
				pkCon->SetSize(2,2);
				pkCon->SetContainerType(eHead);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_HEAD);
				pkCon->SetEquip(true);
				
		//left hand
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iLeftHand = pkContainer->GetEntityID();		
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");
				pkCon->SetSize(2,4);
				pkCon->SetContainerType(eLeftHand);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_HAND);
				pkCon->SetJoint("hand_left");
				pkCon->SetEquip(true);
				
		//right hand
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRightHand = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,4);
				pkCon->SetContainerType(eRightHand);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_HAND);
				pkCon->SetJoint("hand_right");
				pkCon->SetEquip(true);
				
		//eGloves
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iGloves = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,2);
				pkCon->SetContainerType(eGloves);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_GLOVES);
				pkCon->SetEquip(true);
				
		//eCape
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iCape = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,2);
				pkCon->SetContainerType(eCape);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_CAPE);																					
				pkCon->SetEquip(true);
		
		//eNecklace
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iNecklace = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eNecklace);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_NECKLACE);																					
				pkCon->SetEquip(true);
		
		//eBracers
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iBracers = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,2);
				pkCon->SetContainerType(eBracers);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_BRACERS);					
				pkCon->SetEquip(true);

		//eBelt
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iBelt = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,1);
				pkCon->SetContainerType(eBelt);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_BELT);					
				pkCon->SetEquip(true);
								
		//eFeets
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iFeets = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,2);
				pkCon->SetContainerType(eFeets);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_FEETS);									
				pkCon->SetEquip(true);
				
		//eRing1
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRing1 = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eRing1);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_RING);					
				pkCon->SetEquip(true);

		//eRing2
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRing2 = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eRing2);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_RING);	
				pkCon->SetEquip(true);
				
		//eRing3
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRing3 = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eRing3);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_RING);		
				pkCon->SetEquip(true);

		//eRing4
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRing4 = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eRing4);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_RING);	
				pkCon->SetEquip(true);
				
																															
		//cout<<"done"<<endl;
	}

	//update network
	SetNetUpdateFlag(true);
}


P_CharacterProperty::~P_CharacterProperty()
{
	//make sure all sounds are stoped
	m_pkAudioSystem->StopAudio(m_iWalkSoundID);
	m_pkAudioSystem->StopAudio(m_iRunSoundID);
	m_pkAudioSystem->StopAudio(m_iSwimSoundID);

	delete m_pkTextMaterial;
	delete m_pkFont;	
}


void P_CharacterProperty::Update()
{
	if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL))
	{
		//SERVER
		if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_SERVER))
		{
			//try to find items on load
			if(m_bFirstUpdate)
			{
				m_bFirstUpdate = false;
				
				//setup containers
				SetupContainers();
				
				//find leg length
				if(P_Tcs* pkTcs = (P_Tcs*)m_pkEntity->GetProperty("P_Tcs"))
				{
					m_fLegLength = pkTcs->GetLegLength();				
				}				
			}
		
			if(!m_bDead)
			{
				//update stats
				UpdateStats();
			
				//update skills
				UpdateSkills();
			}
		}
			
		//CLIENT
		if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_CLIENT))
			if(m_bWalkSound)
				PlayCharacterMovementSounds();					
		
		//CLIENT AND SERVER
		//reset chat msg
		if(m_pkZeroFps->GetTicks() > m_fChatTime + 10.0)
			m_strChatMsg = "";						
		
	}
	else if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER))
	{
		if(GetIsPlayerCharacter())
		{
			if(m_bOverHeadText)
			{
				string strText = GetName()+string(" <")+GetOwnedByPlayer()+string("> ") + m_strChatMsg;		
				float fDistance = m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(GetEntity()->GetWorldPosV());
				if(fDistance < 20)
				{
					float fScale = 0.05 * fDistance;
					
					m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),GetEntity()->GetIWorldPosV()+
									Vector3(0,1.0,0),fScale,strText,m_pkTextMaterial,m_pkFont,true);													
				}
			}
		}	
	}		
}

void P_CharacterProperty::AddChatMsg(const string& strChatMsg)
{
	m_strChatMsg = strChatMsg;
	m_fChatTime = m_pkZeroFps->GetTicks();	
}


void P_CharacterProperty::PlayCharacterMovementSounds()
{
	static float fWalkGain = 0.05;
	static Vector3 kOffset(0,0.5,0);

	if(P_CharacterControl* pkCC = (P_CharacterControl*)GetEntity()->GetProperty("P_CharacterControl"))
	{
		//check states and play sounds
		
		//walk sound
		if(pkCC->GetCharacterState(eWALKING))
		{
			if(!m_kCurrentCharacterStates[eWALKING])
			{
				//m_pkAudioSystem->StopAudio(m_iWalkSoundID);
				m_iWalkSoundID = m_pkAudioSystem->PlayAudio(m_strWalkSound,GetEntity()->GetIWorldPosV()+kOffset,Vector3(0,0,0),ZFAUDIO_LOOP,fWalkGain);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iWalkSoundID, GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eWALKING])
				m_pkAudioSystem->StopAudio(m_iWalkSoundID);
		}	
		
		//run sound
		if(pkCC->GetCharacterState(eRUNNING))
		{
			if(!m_kCurrentCharacterStates[eRUNNING])
			{
				//m_pkAudioSystem->StopSound(m_iRunSoundID);
				m_iRunSoundID = m_pkAudioSystem->PlayAudio(m_strRunSound,GetEntity()->GetIWorldPosV()+kOffset,Vector3(0,0,0),ZFAUDIO_LOOP,fWalkGain);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iRunSoundID, GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eRUNNING])
				m_pkAudioSystem->StopAudio(m_iRunSoundID);
		}	
		
				
		
		//swim sound
		if(pkCC->GetCharacterState(eSWIMMING))
		{
			if(!m_kCurrentCharacterStates[eSWIMMING])
			{	
				//m_pkAudioSystem->StopAudio(m_iSwimSoundID);
				m_iSwimSoundID = m_pkAudioSystem->PlayAudio(m_strSwimSound,GetEntity()->GetIWorldPosV()+kOffset,Vector3(0,0,0),ZFAUDIO_LOOP,fWalkGain);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iSwimSoundID,GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eSWIMMING])
				m_pkAudioSystem->StopAudio(m_iSwimSoundID);
		}			

		//jump sound
		if(pkCC->GetCharacterState(eJUMPING))
			if(!m_kCurrentCharacterStates[eJUMPING])
				m_pkAudioSystem->PlayAudio(m_strJumpSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),0,fWalkGain*2);
		
														
		//update staes
		m_kCurrentCharacterStates[eRUNNING] =	pkCC->GetCharacterState(eRUNNING);
		m_kCurrentCharacterStates[eWALKING] =	pkCC->GetCharacterState(eWALKING);
		m_kCurrentCharacterStates[eJUMPING] =	pkCC->GetCharacterState(eJUMPING);
		m_kCurrentCharacterStates[eSWIMMING] =	pkCC->GetCharacterState(eSWIMMING);
	}
}

void P_CharacterProperty::SendStats()
{
	if(m_iConID == -1)
		return;

	NetPacket kNp;
	kNp.Write((char) MLNM_SC_CHARACTERSTATS);	

	kNp.Write(m_kCharacterStats.GetTotal("Stamina"));
	kNp.Write(m_kCharacterStats.GetTotal("StaminaMax"));
	
	kNp.Write(m_kCharacterStats.GetTotal("Health"));
	kNp.Write(m_kCharacterStats.GetTotal("HealthMax"));
	
	kNp.Write(m_kCharacterStats.GetTotal("Mana"));
	kNp.Write(m_kCharacterStats.GetTotal("ManaMax"));
	
	//send package
	kNp.TargetSetClient(m_iConID);
	m_pkApp->SendAppMessage(&kNp);	
}



void P_CharacterProperty::SendBuffList()
{
	if(m_iConID == -1)
		return;

		
	NetPacket kNp;
	kNp.Write((char) MLNM_SC_BUFFLIST);	
	
	vector<Property*> kProps;
	GetEntity()->GetAllPropertys(&kProps,PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_SERVER);

	for(int i = 0;i<kProps.size();i++)
	{
		//if(P_Buff* pkBuff = dynamic_cast<P_Buff*>(kProps[i]))
		if(!kProps[i]->IsPropertyType("P_Buff"))
			continue;
		
		if(P_Buff* pkBuff = static_cast<P_Buff*>(kProps[i]))			
		{
			if(pkBuff->m_bShow)
			{
				kNp.Write_Str(pkBuff->m_strName);
				kNp.Write_Str(pkBuff->m_strIcon);
				kNp.Write(pkBuff->m_fTimeOut);
				kNp.Write(pkBuff->m_fTimeLeft);
				kNp.Write(pkBuff->m_cType);
			}
		}
	}
	kNp.Write_Str(string(""));
	
	
	//send package
	kNp.TargetSetClient(m_iConID);
	m_pkApp->SendAppMessage(&kNp);		
}

void P_CharacterProperty::UseSkill(const string& strSkillScript,int iTarget,const Vector3& kPos,const Vector3& kDir)
{
	if(m_bDead)
		return;

	if(Skill* pkSkill = GetSkillPointer(strSkillScript))
	{
		pkSkill->Use(iTarget,kPos,kDir);	
	}
	else
	{
		cout<<"WARNING: skill "<<strSkillScript<<" not found"<<endl;	
	}
}

Skill* P_CharacterProperty::GetSkillPointer(const string& strSkillName)
{
	static string strSkill;
	 
	for(int i =0;i<m_kSkills.size();i++)
	{
		if(m_kSkills[i]->GetName() == strSkillName)
			return m_kSkills[i];
	};

	return NULL;
}

bool P_CharacterProperty::ChangeSkill(const string& strSkillScript,int iValue)
{
	if(Skill* pkSkill = GetSkillPointer(strSkillScript))
	{
		if(!pkSkill->GetParent().empty())
		{
			if(Skill* pkParent = GetSkillPointer(pkSkill->GetParent()))
			{
				if(pkParent->GetLevel() < 1)
				{
					cout<<"WARNING: Parent "<<pkSkill->GetParent()<<" have to be at least level 1 to level a child "<<strSkillScript<<endl;
					return false;
				}
			}
			else
			{
				cout<<"WARNING:"<<strSkillScript<<" missing parent skill "<<pkSkill->GetParent()<<endl;
			}
		}
						
		pkSkill->SetLevel(pkSkill->GetLevel()+iValue);
		return true;		
	}
	
	return true;
}

void P_CharacterProperty::SetSkill(const string& strSkillScript,int iLevel)
{	
	if(Skill* pkSkill = GetSkillPointer(strSkillScript))
	{
		pkSkill->SetLevel(iLevel);
		
	}
}

void P_CharacterProperty::RemoveAllSkills()
{
	for(int i = 0;i < m_kSkills.size();i++)
		delete m_kSkills[i];
		
	m_kSkills.clear();
}

bool P_CharacterProperty::AddSkill(const string& strSkillScript,const string& strParentSkill)
{
	
	//check if skill already exist
	if(Skill* pkSkill = GetSkillPointer(strSkillScript))
	{
		cout<<"character already has skill"<<endl;
		return false;		
	}
	
	//try to find parent
	if(!strParentSkill.empty())
	{
		if(!GetSkillPointer(strParentSkill))
		{
			cout<<"WARNING: parent skill "<<strParentSkill<<" not found when adding skill "<<strSkillScript<<endl;
			return false;
		}
	}
	
	
	//create new skill
	Skill* pkNewSkill = new Skill(strSkillScript,strParentSkill,m_pkEntity->GetEntityID());	
				
	//add skill in skill list
	m_kSkills.push_back(pkNewSkill);	
		
	//do a first update
	pkNewSkill->SetLevel(0);

	
	cout<<"Added skill "<<strSkillScript<<endl;
	return true;
}

P_Buff* P_CharacterProperty::AddBuff(const string& strBuffName)
{
	if(Entity* pkEnt = m_pkEntityManager->CreateEntityFromScript((m_strBuffDir+strBuffName).c_str()))	
	{	
		pkEnt->SetParent(GetEntity());
		
		cout<<"added buff "<<strBuffName<<endl;
		
		SendBuffList();
		return (P_Buff*)pkEnt->GetProperty("P_Buff");
	}
		
		
	cout<<"WARNING: could not add buff "<<strBuffName<<endl;
	return NULL;
}

void P_CharacterProperty::RemoveBuff(const string& strBuffName)
{
	vector<Property*> kProps;
	GetEntity()->GetAllPropertys(&kProps,PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_SERVER);

	for(int i = 0;i<kProps.size();i++)
	{
		if(!kProps[i]->IsPropertyType("P_Buff"))
			continue;
		
		if(P_Buff* pkBuff = static_cast<P_Buff*>(kProps[i]))			
		{
			if(pkBuff->m_strName == strBuffName)
			{
				cout<<"removing buffs by name: "<<strBuffName<<endl;
				RemoveBuff(pkBuff);
			}
		}
	}
}
 
void P_CharacterProperty::RemoveBuff(P_Buff* pkBuff)
{
	if(pkBuff->GetCharacter() == this)
	{
		m_pkEntityManager->Delete(pkBuff->GetEntity());
		pkBuff->m_bShow = false;
		
		SendBuffList();
	}
}


void P_CharacterProperty::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write_Str(m_strName);
	pkPackage->Write_Str(m_strOwnedByPlayer);
	pkPackage->Write(m_bIsPlayerCharacter);
	pkPackage->Write(m_iFaction);
	pkPackage->Write(m_bWalkSound);
	pkPackage->Write(m_fMarkerSize);		
	pkPackage->Write(m_bDead);	
	
	m_kCharacterStats.Save(pkPackage);
	
	
	//save skills
	pkPackage->Write(m_kSkills.size());
	for(int i = 0;i<m_kSkills.size();i++)
	{
		pkPackage->Write_Str(m_kSkills[i]->GetName());
		pkPackage->Write_Str(m_kSkills[i]->GetParent());
		pkPackage->Write(m_kSkills[i]->GetLevel());
		
		pkPackage->Write(m_kSkills[i]->m_fTimeLeft);
	}
	
/*	//save container settings
	m_pkInventory->Save(pkPackage);
	m_pkHead->Save(pkPackage);
	m_pkBody->Save(pkPackage);
	m_pkLeftHand->Save(pkPackage);
	m_pkRightHand->Save(pkPackage);
*/	
}

void P_CharacterProperty::Load(ZFIoInterface* pkPackage,int iVersion)
{
	switch(iVersion)
	{
		case 1:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 
		
			break;		
		}
		
		case 2:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
		
			m_kCharacterStats.Load(pkPackage);
			break;
		}
	
		case 3:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
			pkPackage->Read(m_iFaction); 		
		
			m_kCharacterStats.Load(pkPackage);
			break;
		}		
		
		case 4:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
			pkPackage->Read(m_iFaction); 		
			pkPackage->Read(m_bWalkSound); 		
		
			m_kCharacterStats.Load(pkPackage);
			break;
		}		
		
		case 5:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
			pkPackage->Read(m_iFaction); 		
			pkPackage->Read(m_bWalkSound); 		
			pkPackage->Read(m_fMarkerSize); 
			
			m_kCharacterStats.Load(pkPackage);
			break;
		}			
		
		
		case 6:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
			pkPackage->Read(m_iFaction); 		
			pkPackage->Read(m_bWalkSound); 		
			pkPackage->Read(m_fMarkerSize); 
			
			m_kCharacterStats.Load(pkPackage);
			
			//load skills
			RemoveAllSkills();
			
			int		iNrOfSkills;
			string	strSkill;
			string	strParent;
			int 		iLevel;
			
			pkPackage->Read(iNrOfSkills);			
			for(int i=0;i<iNrOfSkills;i++)
			{
				pkPackage->Read_Str(strSkill);
				pkPackage->Read_Str(strParent);
				pkPackage->Read(iLevel);
				
				AddSkill(strSkill,strParent);
				SetSkill(strSkill,iLevel);
			}
			
			break;
		}			
		
		case 7:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
			pkPackage->Read(m_iFaction); 		
			pkPackage->Read(m_bWalkSound); 		
			pkPackage->Read(m_fMarkerSize); 
			
			m_kCharacterStats.Load(pkPackage);
			
			//load skills
			RemoveAllSkills();
			
			int		iNrOfSkills;
			string	strSkill;
			string	strParent;
			int 		iLevel;
			float		fTimeLeft;
			
			pkPackage->Read(iNrOfSkills);			
			for(int i=0;i<iNrOfSkills;i++)
			{
				pkPackage->Read_Str(strSkill);
				pkPackage->Read_Str(strParent);
				pkPackage->Read(iLevel);
				pkPackage->Read(fTimeLeft);
				
				AddSkill(strSkill,strParent);
				SetSkill(strSkill,iLevel);
				
				if(Skill* pkSkill = GetSkillPointer(strSkill))
					pkSkill->SetTimeLeft(fTimeLeft);
			}
			
			break;
		}			
		
		case 8:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
			pkPackage->Read(m_iFaction); 		
			pkPackage->Read(m_bWalkSound); 		
			pkPackage->Read(m_fMarkerSize); 
			pkPackage->Read(m_bDead); 
			
			m_kCharacterStats.Load(pkPackage);
			
			//load skills
			RemoveAllSkills();
			
			int		iNrOfSkills;
			string	strSkill;
			string	strParent;
			int 		iLevel;
			float		fTimeLeft;
			
			pkPackage->Read(iNrOfSkills);			
			for(int i=0;i<iNrOfSkills;i++)
			{
				pkPackage->Read_Str(strSkill);
				pkPackage->Read_Str(strParent);
				pkPackage->Read(iLevel);
				pkPackage->Read(fTimeLeft);
				
				AddSkill(strSkill,strParent);
				SetSkill(strSkill,iLevel);
				
				if(Skill* pkSkill = GetSkillPointer(strSkill))
					pkSkill->SetTimeLeft(fTimeLeft);
			}
			
			break;
		}				
	}
}

void P_CharacterProperty::PackTo( NetPacket* pkNetPacket, int iConnectionID ) 
{
	pkNetPacket->Write(m_iInventory);
	pkNetPacket->Write(m_iHead);
	pkNetPacket->Write(m_iBody);
	pkNetPacket->Write(m_iLeftHand);
	pkNetPacket->Write(m_iRightHand);	
	pkNetPacket->Write(m_iGloves);
	pkNetPacket->Write(m_iCape);
	pkNetPacket->Write(m_iNecklace);
	pkNetPacket->Write(m_iBracers);
	pkNetPacket->Write(m_iRing1);
	pkNetPacket->Write(m_iRing2);
	pkNetPacket->Write(m_iRing3);
	pkNetPacket->Write(m_iRing4);
	pkNetPacket->Write(m_iBelt);
	pkNetPacket->Write(m_iFeets);
	
	

	pkNetPacket->Write_Str(m_strName);
	pkNetPacket->Write_Str(m_strOwnedByPlayer);
	pkNetPacket->Write(m_bIsPlayerCharacter);
	pkNetPacket->Write(m_fLegLength);
	pkNetPacket->Write(m_iFaction);
	pkNetPacket->Write(m_fMarkerSize);

	pkNetPacket->Write(m_bWalkSound);
	
		
	pkNetPacket->Write_Str(m_strWalkSound);
	pkNetPacket->Write_Str(m_strRunSound);
	pkNetPacket->Write_Str(m_strJumpSound);
	pkNetPacket->Write_Str(m_strSwimSound);
	
	SetNetUpdateFlag(iConnectionID,false);
	
}

void P_CharacterProperty::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read(m_iInventory);
	pkNetPacket->Read(m_iHead);
	pkNetPacket->Read(m_iBody);
	pkNetPacket->Read(m_iLeftHand);
	pkNetPacket->Read(m_iRightHand);	
	pkNetPacket->Read(m_iGloves);
	pkNetPacket->Read(m_iCape);
	pkNetPacket->Read(m_iNecklace);
	pkNetPacket->Read(m_iBracers);
	pkNetPacket->Read(m_iRing1);
	pkNetPacket->Read(m_iRing2);
	pkNetPacket->Read(m_iRing3);
	pkNetPacket->Read(m_iRing4);
	pkNetPacket->Read(m_iBelt);
	pkNetPacket->Read(m_iFeets);


	pkNetPacket->Read_Str(m_strName);
	pkNetPacket->Read_Str(m_strOwnedByPlayer);
	pkNetPacket->Read(m_bIsPlayerCharacter);
	pkNetPacket->Read(m_fLegLength);
	pkNetPacket->Read(m_iFaction);
	pkNetPacket->Read(m_fMarkerSize);
	
	pkNetPacket->Read(m_bWalkSound);
	
	pkNetPacket->Read_Str(m_strWalkSound);
	pkNetPacket->Read_Str(m_strRunSound);
	pkNetPacket->Read_Str(m_strJumpSound);
	pkNetPacket->Read_Str(m_strSwimSound);		

}


// SCRIPT INTERFACE FOR P_CharacterProperty
using namespace ObjectManagerLua;

namespace SI_P_CharacterProperty
{
	//skill
	int AddSkillLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
			return 0;		

			
		int iCharcterID;
		string strSkill;
		string strParent;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strSkill);
		g_pkScript->GetArgString(pkLua, 2,strParent);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			pkCP->AddSkill(strSkill,strParent);		
	
		return 0;			
	}

	int ChangeSkillLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
			return 0;		

			
		int iCharcterID;
		string strSkill;
		int iValue;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strSkill);
		g_pkScript->GetArgInt(pkLua, 2, &iValue);		
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			pkCP->ChangeSkill(strSkill,iValue);		
	
		return 0;			
	}
	
	int SetupSkillLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 6)
			return 0;		

			
		int iCharcterID;
		string strSkill;
		string strInGameName;
		string strSchool;
		string strIcon;
		double dReloadTime;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);		
		
		g_pkScript->GetArgString(pkLua, 1, strSkill);				
		g_pkScript->GetArgString(pkLua, 2, strInGameName);		
		g_pkScript->GetArgString(pkLua, 3, strSchool);		
		g_pkScript->GetArgString(pkLua, 4, strIcon);		
		g_pkScript->GetArgNumber(pkLua, 5, &dReloadTime);		
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			if(Skill* pkSkill = pkCP->GetSkillPointer(strSkill))
			{
				pkSkill->m_strInGameName = strInGameName;
				pkSkill->m_strSchool = 		strSchool;
				pkSkill->m_strIcon = 		strIcon;
				pkSkill->m_fReloadTime = 	float(dReloadTime);
				//pkSkill->m_fTimeLeft = 		float(dReloadTime);
				
				cout<<"skill setup complete"<<endl;
			}
			else
			{
				cout<<"ERROR: setupskill called , but character does not have the skill "<<strSkill<<endl;			
			}
		}
	
		return 0;			
	}	
	
	//eqipment
	int GetCharacterContainerIDLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
		{
			cout<<"WARNING: EquipNewItem - wrong number of arguments"<<endl;
			return 0;		
		}
					
		int iCharcterID;
		string strContainerName;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1, strContainerName);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			if(strContainerName == "Inventory") g_pkScript->AddReturnValue(pkLua,pkCP->m_iHead);
			if(strContainerName == "Head") 		g_pkScript->AddReturnValue(pkLua,pkCP->m_iHead);
			if(strContainerName == "Gloves") 	g_pkScript->AddReturnValue(pkLua,pkCP->m_iGloves);
			if(strContainerName == "Cape") 		g_pkScript->AddReturnValue(pkLua,pkCP->m_iCape);
			if(strContainerName == "Necklace")	g_pkScript->AddReturnValue(pkLua,pkCP->m_iNecklace);
			if(strContainerName == "Body")		g_pkScript->AddReturnValue(pkLua,pkCP->m_iBody);
			if(strContainerName == "LeftHand")	g_pkScript->AddReturnValue(pkLua,pkCP->m_iLeftHand);
			if(strContainerName == "RightHand")	g_pkScript->AddReturnValue(pkLua,pkCP->m_iRightHand);
			if(strContainerName == "Bracers")	g_pkScript->AddReturnValue(pkLua,pkCP->m_iBracers);
			if(strContainerName == "Ring1")		g_pkScript->AddReturnValue(pkLua,pkCP->m_iRing1);
			if(strContainerName == "Ring2")		g_pkScript->AddReturnValue(pkLua,pkCP->m_iRing2);
			if(strContainerName == "Ring3")		g_pkScript->AddReturnValue(pkLua,pkCP->m_iRing3);
			if(strContainerName == "Ring4")		g_pkScript->AddReturnValue(pkLua,pkCP->m_iRing4);
			if(strContainerName == "Belt")		g_pkScript->AddReturnValue(pkLua,pkCP->m_iBelt);
			if(strContainerName == "Feets")		g_pkScript->AddReturnValue(pkLua,pkCP->m_iFeets);	
			
			return 1;		
		}
	
		return 0;				
	}
	

	//faction
	int SetFactionLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
		{
			cout<<"WARNING: SetFaction - wrong number of arguments"<<endl;
			return 0;		
		}
					
		int iCharcterID;
		int iFaction;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgInt(pkLua, 1, &iFaction);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			pkCP->SetFaction(iFaction);
		}
	
		return 0;				
	}
	

	//stat modification
	int ChangeStatLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
		{
			cout<<"WARNING: ChangeStat - wrong number of arguments"<<endl;
			return 0;		
		}
					
		int iCharcterID;
		double dValue;
		string strStat;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strStat);
		g_pkScript->GetArgNumber(pkLua, 2, &dValue);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			pkCP->m_kCharacterStats.ChangeStat(strStat,float(dValue));		
		}
	
		return 0;				
	}
		
	int ChangeStatModLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
		{
			cout<<"WARNING: ChangeStatMod - wrong number of arguments"<<endl;
			return 0;		
		}		

			
		int iCharcterID;
		double dValue;
		string strStat;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strStat);
		g_pkScript->GetArgNumber(pkLua, 2, &dValue);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			pkCP->m_kCharacterStats.ChangeMod(strStat,float(dValue));		
		}
	
		return 0;			
	}
	
	int SetStatLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
		{
			cout<<"WARNING: SetStat - wrong number of arguments"<<endl;
			return 0;		
		}
			
		int iCharcterID;
		double dValue;
		string strStat;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strStat);
		g_pkScript->GetArgNumber(pkLua, 2, &dValue);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			pkCP->m_kCharacterStats.SetStat(strStat,float(dValue));		
		}
	
		return 0;				
	}	
	
	//buffs
	int AddBuffLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		

			
		int iCharcterID;
		string strBuff;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strBuff);
		
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			pkCP->AddBuff(strBuff);		
	
		return 0;			
	}

	int RemoveBuffLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		

			
		int iCharcterID;
		string strBuff;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strBuff);
		
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			pkCP->RemoveBuff(strBuff);		
	
		return 0;			
	}
		
	
	int PickupItemLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		
	
		int iCharcterID;
		int iItemID;
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iCharcterID = (int)dTemp;
		g_pkScript->GetArgNumber(pkLua, 1, &dTemp);
		iItemID = (int)dTemp;
		
		cout<<iCharcterID<<"trying to pickup "<<iItemID<<endl;
		
		if(Entity* pkEnt = g_pkObjMan->GetEntityByID(iCharcterID))
			if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))		
			{
				if(Entity* pkContainerEnt = g_pkObjMan->GetEntityByID(pkCP->m_iInventory))
					if(P_Container* pkContainer = (P_Container*)pkContainerEnt->GetProperty("P_Container"))
					{
						if(pkContainer->AddMove(iItemID,-1,-1,-1))
						{
							cout<<"sucessfully picked up item"<<endl;
							return 0;
						}
						else
							cout<<"could not pick that up"<<endl;
					}
			}
			else
				cout<<"WARNING: entity without P_CharacterProperty tried to pickup an item"<<endl;
				
		return 0;
	}
	
	int HaveItemLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		
		
		int iCharcterID;
		double dTemp;
		double dItemID = -1;
		string strItemName;
		
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iCharcterID = (int)dTemp;
		
		g_pkScript->GetArgString(pkLua, 1,strItemName);
		
		if(Entity* pkEnt = g_pkObjMan->GetEntityByID(iCharcterID))
			if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
				if(Entity* pkContainerEnt = g_pkObjMan->GetEntityByID(pkCP->m_iInventory))
					if(P_Container* pkContainer = (P_Container*)pkContainerEnt->GetProperty("P_Container"))
						dItemID = pkContainer->HaveItem(strItemName);
			

		g_pkScript->AddReturnValue(pkLua, dItemID);							
		return 1;		
	}	
}




Property* Create_P_CharacterProperty()
{
	return new P_CharacterProperty;
}


void Register_P_CharacterProperty(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_CharacterProperty", Create_P_CharacterProperty);					

	// Register Property Script Interface
	
	//item handling
	g_pkScript->ExposeFunction("PickupItem",		SI_P_CharacterProperty::PickupItemLua);
	g_pkScript->ExposeFunction("HaveItem",			SI_P_CharacterProperty::HaveItemLua);
	g_pkScript->ExposeFunction("GetCharacterContainerID",		SI_P_CharacterProperty::GetCharacterContainerIDLua);

	
	//stats
	g_pkScript->ExposeFunction("SetStat",			SI_P_CharacterProperty::SetStatLua);
	g_pkScript->ExposeFunction("ChangeStat",		SI_P_CharacterProperty::ChangeStatLua);
	g_pkScript->ExposeFunction("ChangeStatMod",	SI_P_CharacterProperty::ChangeStatModLua);
	
	
	//buffs
	g_pkScript->ExposeFunction("AddBuff",			SI_P_CharacterProperty::AddBuffLua);
	g_pkScript->ExposeFunction("RemoveBuff",		SI_P_CharacterProperty::RemoveBuffLua);
	
	//skills
	g_pkScript->ExposeFunction("AddSkill",			SI_P_CharacterProperty::AddSkillLua);
	g_pkScript->ExposeFunction("ChangeSkill",		SI_P_CharacterProperty::ChangeSkillLua);
	g_pkScript->ExposeFunction("SetupSkill",		SI_P_CharacterProperty::SetupSkillLua);
	
	//faction
	g_pkScript->ExposeFunction("SetFaction",		SI_P_CharacterProperty::SetFactionLua);
	
	
}


