#include "p_characterproperty.h"
#include "p_charactercontrol.h"
#include "p_container.h"
#include "p_item.h"
#include "p_buff.h"
#include "p_spawn.h"
#include "ml_netmessages.h"

#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 
#include "../zerofpsv2/basic/math.h"

#include "p_ai.h"


//--------SKILL----------------

Skill::Skill(const string& strScriptFile,const string& strParent, int iOwnerID)
{
	m_pkScript = static_cast<ZSSScriptSystem*>(g_ZFObjSys.GetObjectPtr("ZSSScriptSystem"));
	m_pkZeroFps= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkEntityManager = static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));
	
	//create and setup script
	m_pkScriptFileHandle = new ZFResourceHandle;
	

	
	m_strSkillScript = 	strScriptFile;	
	m_pkScriptFileHandle->SetRes(string("data/script/objects/game objects/skills/") + strScriptFile);
	
	
	m_strParentSkill	=	strParent;	
	m_iOwnerID 			=	iOwnerID;
		
	m_strInGameName 	=	"UnkownSkill";
	m_strIcon 			= 	"default.tga";
	m_strInfoText		=	"Nothing known about this skill.";
	m_iLevel 			= 	0;	
	m_fReloadTime 		=	1;
	m_fCastTime			=	1;
	m_fTimeLeft 		=	1;
	m_fLastUpdate 		= 	-1;
//	m_strSchool 		=	"UnkownSchool";
	m_iTargetType		=	0;	
	m_iSkillType		=	0;
	m_fRange				=	0;
	
	m_fStaminaUsage	=	0;
	m_fManaUsage		=	0;
	
}

Skill::~Skill()
{
	delete m_pkScriptFileHandle;

}



void Skill::Update()
{
	if( (m_pkEntityManager->GetSimTime() < m_fLastUpdate) || m_fLastUpdate == -1)
	{
// 		cout<<"skill time dont match"<<endl;				
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
	
// 	if(m_fTimeLeft == 0)
// 		cout<<"skill reloaded:"<<GetName()<<endl;

}

bool Skill::IsReloaded()
{
	//check reload
	if(m_fTimeLeft != 0)
		return false;	

	return true;
}

void Skill::UpdateFromScript()
{
	if(!m_pkScriptFileHandle->IsValid())
	{
		cerr<<"WARNING: skill script "<<m_strSkillScript<<" not loaded"<<endl;
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
		cerr<<"WARNING: could not call update function for skill script "<<m_pkScriptFileHandle->GetRes()<<" level "<<m_iLevel<<endl;
		return;
	}
	
	
	//get variables
	ZFScript* pkScript = (ZFScript*)m_pkScriptFileHandle->GetResourcePtr();

	double dtemp;
	char ctemp[128];
		
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"ingamename",ctemp))
		m_strInGameName = ctemp;
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"parent",ctemp))
		m_strParentSkill = ctemp;
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"icon",ctemp))
		m_strIcon = ctemp;
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"infotext",ctemp))
		m_strInfoText = ctemp;					
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"reloadtime",dtemp))
		m_fReloadTime = float(dtemp);
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"targettype",dtemp))
		m_iTargetType = int(dtemp);
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"skilltype",dtemp))
		m_iSkillType = int(dtemp);
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"range",dtemp))
		m_fRange = float(dtemp);
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"casttime",dtemp))
		m_fCastTime = float(dtemp);	
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"staminausage",dtemp))
		m_fStaminaUsage = float(dtemp);
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"manausage",dtemp))
		m_fManaUsage = float(dtemp);
		
		
	m_kBaseTypes.clear();
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"basetype0",ctemp))
		m_kBaseTypes.push_back(string(ctemp));
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"basetype1",ctemp))
		m_kBaseTypes.push_back(string(ctemp));
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"basetype2",ctemp))
		m_kBaseTypes.push_back(string(ctemp));
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"basetype3",ctemp))
		m_kBaseTypes.push_back(string(ctemp));
	if(m_pkScript->GetGlobal(pkScript->m_pkLuaState,"basetype4",ctemp))
		m_kBaseTypes.push_back(string(ctemp));

/*
	//print some nice info
	cout<<"UPDATED FROM SCRIPT"<<endl;
	cout<<"script:    "<<m_pkScriptFileHandle->GetRes()<<endl;
	cout<<"ingamename:"<<m_strInGameName<<endl;
	cout<<"school:    "<<m_strSchool<<endl;
	cout<<"icon:      "<<m_strIcon<<endl;
	cout<<"reloadtime:"<<m_fReloadTime<<endl;
	cout<<"level:     "<<m_iLevel<<endl;
	cout<<"parent:    "<<m_strParentSkill<<endl;*/
}

void Skill::SetLevel(int iLevel)
{
	//change level
	m_iLevel = iLevel;
	
	//update skillstats from script
	UpdateFromScript();
	
	return;
}

bool Skill::CanUse()
{
	if(!m_pkScriptFileHandle->IsValid())
	{
		//cerr<<"WARNING: skill script "<<m_strSkillScript<<" not loaded"<<endl;
		return false;	
	}

	//check reload
	if(m_fTimeLeft > 4)
		return false;	

	//check level
	if(m_iLevel < 1)
	{
// 		cout<<"got to have at least level 1 of skill "<<GetName()<<" to use it"<<endl;
		return false;
	}

	//get character property from owner
	P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(m_iOwnerID,"P_CharacterProperty");
	if(!pkCP)
	{
// 		cout<<"WARNING, CHARACTER is missing Character property"<<endl;
		return false;
	}
			
	//check baseitems
	if(!m_kBaseTypes.empty())
	{
		bool bOk = false;
		for(int i = 0;i<m_kBaseTypes.size();i++)
		{
			if(pkCP->HaveEqipedBaseType(m_kBaseTypes[i]))
			{
				bOk = true;
				break;
			}
		}
			
		if(bOk == false)
		{
			//cout<<"ned one of the folowing items:"<<endl;
			//for(int i = 0;i<m_kBaseTypes.size();i++)
			//	cout<< m_kBaseTypes[i]<<endl;
// 			cout<<"missing basetype"<<endl;
			return false;		
		}
	}
	
	//use mana & stamina
	if(pkCP->m_kCharacterStats.GetTotal("Mana") < m_fManaUsage)
		return false;
	
	if(pkCP->m_kCharacterStats.GetTotal("Stamina") < m_fStaminaUsage)
		return false;

	return true;
}

int Skill::Use(int iTargetID,const Vector3& kPos,const Vector3& kDir)
{		
	if(!m_pkScriptFileHandle->IsValid())
	{
		cerr<<"WARNING: skill script "<<m_strSkillScript<<" not loaded"<<endl;
		return -1;	
	}

	//check reload
	if(m_fTimeLeft != 0)
		return 1;	

	//check level
	if(m_iLevel < 1)
	{
 		cout<<"got to have at least level 1 of skill "<<GetName()<<" to use it"<<endl;
		return 2;	
	}

	//get character property from owner
	P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(m_iOwnerID,"P_CharacterProperty");
	if(!pkCP)
	{
 		cout<<"WARNING, CHARACTER is missing Character property"<<endl;
		return 7;
	}
			
	//check baseitems
	if(!m_kBaseTypes.empty())
	{
		bool bOk = false;
		for(int i = 0;i<m_kBaseTypes.size();i++)
		{
			if(pkCP->HaveEqipedBaseType(m_kBaseTypes[i]))
			{
				bOk = true;
				break;
			}
		}
			
		if(bOk == false)
		{
			//cout<<"ned one of the folowing items:"<<endl;
			//for(int i = 0;i<m_kBaseTypes.size();i++)
			//	cout<< m_kBaseTypes[i]<<endl;
 			cout<<"missing basetype"<<endl;
			return 7;			
		}
	}
	
	//character target  check if a character is targeted
	if(m_iTargetType == eCHARACTER_TARGET)
	{
		//get target character property
		P_CharacterProperty* pkTargetCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iTargetID,"P_CharacterProperty");
	
		//does target have a character property?
		if(!pkTargetCP)
			return 3;
		else 			//yes character have a characterproperty, if this is an offensive skill, check that target is alive
			if(m_iSkillType==eOFFENSIVE && pkTargetCP->IsDead())
				return 3;		
			
	 
		//Do rotation and distance check
		if(Entity* pkOwner = m_pkEntityManager->GetEntityByID(m_iOwnerID))
		{
			if(Entity* pkTarget = m_pkEntityManager->GetEntityByID(iTargetID))
			{
		
				//check range
				if(pkOwner->GetWorldPosV().DistanceTo(pkTarget->GetWorldPosV()) > m_fRange)
					return 6;
				
				//check rotation
				Vector3 kOwnerDir = pkOwner->GetWorldRotM().VectorTransform(Vector3(0,0,1));
				Vector3 kDir = (pkTarget->GetWorldPosV() - pkOwner->GetWorldPosV());		
				
				kOwnerDir.y = 0;
				kDir.y = 0;
				
				kOwnerDir.Normalize();
				kDir.Normalize();
				
				if(Math::RadToDeg(kOwnerDir.Angle(kDir)) > 30)
					return 4;				
			}
		}
	}
	
	//item target  check that an item is targeted
	if(m_iTargetType == eITEM_TARGET && !m_pkEntityManager->GetPropertyFromEntityID(iTargetID,"P_Item")) 
		return 5;
		
	
	//use mana & stamina
	if(!pkCP->UseMana(m_fManaUsage))
		return 10;
		
	if(!pkCP->UseStamina(m_fStaminaUsage))
		return 11;
		
	
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

	
	
	//lock skill usage	
	if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(m_iOwnerID,"P_CharacterProperty"))
		pkCP->LockSkillUsage(m_fCastTime);
	
	//lock character
	if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntityManager->GetPropertyFromEntityID(m_iOwnerID,"P_CharacterControl"))
		pkCC->Lock(m_fCastTime);
		
	//call use function
	if(!m_pkScript->Call(m_pkScriptFileHandle, "Use",args))
	{
		cerr<<"WARNING: could not call update function for skill script "<<m_pkScriptFileHandle->GetRes()<<" level "<<m_iLevel<<endl;
		return -1;
	}			
	
	//reset reload timer
	m_fTimeLeft = m_fReloadTime;
	m_fLastUpdate = m_pkEntityManager->GetSimTime();
	
	
	return 0;
}


//-------CharacterStats-----------


Stats::Stats()
{
	
}

Stat* Stats::GetStatPointer(const string& strName)
{
	map<string,Stat>::iterator kFind = m_kStats.find(strName);
	
	if(kFind == m_kStats.end())
	{
		return NULL;
	}

	return &((*kFind).second);
}

void Stats::GetStatsList(vector<Stat>* pkStats)
{
	for(map<string,Stat>::iterator it = m_kStats.begin();it!=m_kStats.end();it++)
	{
		pkStats->push_back((*it).second);	
	}
}


void Stats::AddStat(const string& strName,float fValue,float fMod)
{
	//check if stat already exist
	if(GetStatPointer(strName))
	{
		cout<<"Stat "<<strName<<" already exist in stats list"<<endl;		
		return;				
	}
		
	m_kStats.insert(pair<string,Stat>(strName,Stat(strName,fValue,fMod))  );
}

void Stats::AddOn(Stats* pkTarget)
{
	for(map<string,Stat>::iterator it = m_kStats.begin();it!=m_kStats.end();it++)
	{
		pkTarget->ChangeMod((*it).second.m_strName, (*it).second.m_fValue + (*it).second.m_fMod );
	}
}

void Stats::RemoveOn(Stats* pkTarget)
{
	for(map<string,Stat>::iterator it = m_kStats.begin();it!=m_kStats.end();it++)
	{
		pkTarget->ChangeMod((*it).second.m_strName, -((*it).second.m_fValue + (*it).second.m_fMod) );
	}
}

void Stats::Save(ZFIoInterface* pkPackage)
{
	//stat version
	static int iVersion = 2;
	pkPackage->Write(iVersion);	

	pkPackage->Write(m_kStats.size());
	for(map<string,Stat>::iterator it = m_kStats.begin();it!=m_kStats.end();it++)
	{		
		pkPackage->Write_Str((*it).second.m_strName);
		pkPackage->Write((*it).second.m_fValue);
		pkPackage->Write((*it).second.m_fMod);	
	}
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
			
			m_kStats.insert(pair<string,Stat>(strName,Stat(strName,fValue,0)) ); //dont load mod value
			
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
			
			m_kStats.insert(pair<string,Stat>(strName,Stat(strName,fValue,0)) ); //dont load mod value		
		}
		
	}	
}

void Stats::SetStat(const string& strName,float fValue)
{
	if(Stat* pkStat = GetStatPointer(strName))
	{
		pkStat->m_fValue = fValue;	
		return;
	}
	
		
	cout<<"WARNING:SetStat requested stat "<<strName<<" was not found"<<endl;
}

float Stats::GetStat(const string& strName)
{
	if(Stat* pkStat = GetStatPointer(strName))
	{
		return pkStat->m_fValue;
	}

	cout<<"WARNING:GetStat requested stat "<<strName<<" was not found"<<endl;	
	return 0;
}
		
void Stats::ChangeStat(const string& strName,float fValue)
{
	if(Stat* pkStat = GetStatPointer(strName))
	{
		pkStat->m_fValue += fValue;
		return;
	}				
		
	cout<<"WARNING:ChangeStat requested stat "<<strName<<" was not found"<<endl;
}


void Stats::SetMod(const string& strName,float fValue)
{
	if(Stat* pkStat = GetStatPointer(strName))
	{
		pkStat->m_fMod = fValue;	
		return;
	}
	
	cout<<"WARNING:SetMod requested stat "<<strName<<" was not found"<<endl;
}

float Stats::GetMod(const string& strName)
{
	if(Stat* pkStat = GetStatPointer(strName))
	{
		return pkStat->m_fMod;
	}
		
	cout<<"WARNING:GetMod requested stat "<<strName<<" was not found"<<endl;
	return 0;
}		

void Stats::ChangeMod(const string& strName,float fValue)
{
	if(Stat* pkStat = GetStatPointer(strName))
	{
		pkStat->m_fMod += fValue;	
		return;
	}		
		
	cout<<"WARNING:ChangeMod requested stat "<<strName<<" was not found"<<endl;
}

float Stats::GetTotal(const string& strName)
{
	if(Stat* pkStat = GetStatPointer(strName))
	{
		return pkStat->m_fValue + pkStat->m_fMod;
	}		
		
	cout<<"WARNING:GetTotal requested stat "<<strName<<" was not found"<<endl;
	return 0;
}


//-------P_CharacterProperty---------
P_CharacterProperty::P_CharacterProperty() : Property("P_CharacterProperty")
{
	m_pkAudioSystem = 	static_cast<ZSSAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZSSAudioSystem"));			
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
	m_pkEntityMan=			static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));			
	m_pkApp=					static_cast<Application*>(g_ZFObjSys.GetObjectPtr("Application"));			
	
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;

	m_bNetwork = 	true;
	m_iVersion = 	11;
	
	//initiate stuff
	m_strBuffDir			=	"data/script/objects/game objects/buffs/";
	
	m_iCurrentCharacterState = eIDLE_STANDING;

	
	m_bAutomaticSkill		=	false;
	
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
	m_fSkillLockTime		=	0;
	m_iFaction				=	0;
	m_iGroup					=  -1;
	m_bWalkSound			=	true;
	m_fLegLength			=	0;
	m_fMarkerSize			=	1;
	m_bDead					=	false;
	m_bIncap					=  false;
	m_fDeadTimer			=	0;
	m_fDecayTime			=	60;
	m_iDMNoTarget        =  false;
	
	m_bInCamp				=	false;
	
	m_fRespawnTime			=	-1;
	m_iRespawnZone			=	-1;
	m_kRespawnPos			=	Vector3(0,0,0);
	
	m_fCombatTimer			=	0;
// 	m_strDefaultAttackSkill = "";
	m_iTarget				=	-1;
	m_bCombatMode			=	false;
	m_iLastDamageFrom		=	-1;
	
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
		
	
	//setup skillbar
	for(int i=0;i<19;i++)
		m_kSkillBar.push_back(string(""));
		
	//basic sounds
	m_strWalkSound			=	"footstep_forest.wav";
	m_strRunSound			=	"footstep_forest_run.wav";
	m_strJumpSound			=	"jump.wav";
	m_strSwimSound			=	"walkwater.wav";	
	m_strWalkWaterSound	=	"walkwater.wav";	
	m_strRunWaterSound	=	"walkwater.wav";	
	m_iWalkSoundID 		= 	-1;
	m_iRunSoundID 			= 	-1;
	m_iSwimSoundID 		= 	-1;
	m_iWalkWaterSoundID	=	-1;
	m_iRunWaterSoundID	=	-1;
	
	//setup material
	m_pkTextMaterial = new ZMaterial;
	m_pkTextMaterial->GetPass(0)->m_pkTUs[0]->SetRes("text/fetfont.tga");
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



	m_kPropertyValues.push_back(PropertyValues("walksound",VALUETYPE_BOOL,(void*)&m_bWalkSound));
	m_kPropertyValues.push_back(PropertyValues("markersize",VALUETYPE_FLOAT,(void*)&m_fMarkerSize));
	m_kPropertyValues.push_back(PropertyValues("name",VALUETYPE_STRING,(void*)&m_strName));
	m_kPropertyValues.push_back(PropertyValues("faction",VALUETYPE_INT,(void*)&m_iFaction));
	m_kPropertyValues.push_back(PropertyValues("respawntime",VALUETYPE_FLOAT,(void*)&m_fRespawnTime));
}

void P_CharacterProperty::Init()
{
	SetupCharacterStats();
}




void P_CharacterProperty::SetupCharacterStats()
{
	m_kCharacterStats.AddStat("Level"			,1,0);		//alla karaktärer börjar på lvl 1
	m_kCharacterStats.AddStat("Experience"		,0,0);
	m_kCharacterStats.AddStat("PrevLevel"		,0,0);
	m_kCharacterStats.AddStat("NextLevel"		,0,0);
	
	m_kCharacterStats.AddStat("StatPoints"		,0,0);
	m_kCharacterStats.AddStat("SkillPoints"	,0,0);
	
	m_kCharacterStats.AddStat("ExperienceValue",0,0);
	m_kCharacterStats.AddStat("Money"			,0,0);
	
	m_kCharacterStats.AddStat("Speed"			,0,0);
	m_kCharacterStats.AddStat("Jump"				,0,0);
	
	m_kCharacterStats.AddStat("Mana"				,999,0);		//ser till att man börjar med full mana
	m_kCharacterStats.AddStat("ManaMax"			,0,0);
	m_kCharacterStats.AddStat("ManaRegen"		,0,0);	
	
	m_kCharacterStats.AddStat("Health"			,999,0);		//börjar med full hp
	m_kCharacterStats.AddStat("HealthMax"		,1,0);		
	m_kCharacterStats.AddStat("HealthRegen"	,0,0);

	m_kCharacterStats.AddStat("Stamina"			,999,0);		//börjar med full stamina
	m_kCharacterStats.AddStat("StaminaMax"		,0,0);
	m_kCharacterStats.AddStat("StaminaRegen"	,0,0);	

	m_kCharacterStats.AddStat("Oxygen"			,100,0);		//börjar med full stamina

	m_kCharacterStats.AddStat("Strength"		,0,0);
	m_kCharacterStats.AddStat("Dexterity"		,0,0);
	m_kCharacterStats.AddStat("Vitality"		,0,0);
	m_kCharacterStats.AddStat("Intelligence"	,0,0);
	m_kCharacterStats.AddStat("Wisdom"			,0,0);
	m_kCharacterStats.AddStat("Charisma"		,0,0);
			
	m_kCharacterStats.AddStat("Load"				,0,0);
	m_kCharacterStats.AddStat("LoadMax"			,0,0);

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

	m_kCharacterStats.AddStat("Attack"			,0,0);	
	m_kCharacterStats.AddStat("Defense"			,0,0);
}

void P_CharacterProperty::UseSkill(const string& strSkill,int iTargetID,bool bAutomatic)
{
	m_bAutomaticSkill = bAutomatic;
	m_iTarget = iTargetID;
	m_strNextSkill = strSkill;

}

void P_CharacterProperty::UpdateSkillQueue()
{
	float fTime = m_pkZeroFps->GetEngineTime();
		
	if(fTime > m_fCombatTimer + 0.5)
	{
		m_fCombatTimer = fTime;
		
		//no skills in queue
		if(!m_strNextSkill.empty())
		{
			//use skill and dont pop queue if skill is still reloading
			int iRes = UseSkill(m_strNextSkill,m_iTarget,Vector3(0,0,0),Vector3(0,0,0));
			
			//dont pop skill if skill not yet reloaded or another skill still being cast
			if(iRes == 1 || iRes == 8)
				return;
			
			if(!m_bAutomaticSkill)
				m_strNextSkill = "";
	
			switch(iRes)
			{
				case -1:
					SendTextToClient("You don't have that skill");break;
				case -2:
					SendTextToClient("You are dead");break;						
				case 2:
					SendTextToClient("You need at least level 1 to use a skill");break;
				case 3:
					SendTextToClient("You need to target an enemy");break;
				case 4:
					SendTextToClient("You need to look at the target");break;							
				case 5:
					SendTextToClient("You need to target an item");break;										
				case 6:
					SendTextToClient("You are too far away");break;										
				case 7:
					SendTextToClient("You need to equip a certain item");break;										
				case 9:
					SendTextToClient("You can't use that skill when not in combat mode");break;										
				case 10:
					SendTextToClient("Not enough mana");break;										
				case 11:
					SendTextToClient("Not enough stamina");break;										
										
			}	
		}
	}

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
	
	DebugSet("Health: ",int(m_kCharacterStats.GetTotal("Health")));
	
	
	//apply fall damage
	float fFallDmg = pkCC->GetFallDamage();
	if(fFallDmg > 0)
	{
		SetLastDamageFrom(-1);					
		m_kCharacterStats.ChangeStat("Health",-fFallDmg);
	}	
	
	// Check if character has run out of air.
	if(m_kCharacterStats.GetTotal("Oxygen") <= 0)
	{
		m_kCharacterStats.SetStat("Health", 0);
	}

	//check if character is dead
	if(m_kCharacterStats.GetTotal("Health") <= 0)
	{
		//cout<<"Character is soooo dead"<<endl;
		OnIncap();	//OnDeath();
		return;
	}
		
	
	float fTime = m_pkZeroFps->GetEngineTime();
	
	//update stats each seccond	
	if(fTime > m_fStatTimer + 1.0)
	{
		m_fStatTimer = fTime;
	
		//check for camp resting
		if(pkCC->GetCharacterState() == eSITTING && InCamp())
		{
 			if(!m_bInCamp)
 				SendTextToClient("Resting");			
 			
 			m_bInCamp = true;			
		}
		else
		{
 			if(m_bInCamp)
 				SendTextToClient("Stopped resting");			
			
			m_bInCamp = false;		
		}
	
		//maximum values
		m_kCharacterStats.SetStat("HealthMax",		m_kCharacterStats.GetTotal("Vitality") * 2.0);
		m_kCharacterStats.SetStat("HealthRegen",	m_kCharacterStats.GetTotal("Vitality") * 0.005);
		
		m_kCharacterStats.SetStat("ManaMax",		m_kCharacterStats.GetTotal("Intelligence") );
		m_kCharacterStats.SetStat("ManaRegen",		m_kCharacterStats.GetTotal("Intelligence") * 0.005 );
 		
 		m_kCharacterStats.SetStat("StaminaMax",	m_kCharacterStats.GetTotal("Vitality") * 5.0 );
 		//m_kCharacterStats.SetStat("StaminaRegen",	m_kCharacterStats.GetTotal("Vitality") * 0.15 );
 		m_kCharacterStats.SetStat("StaminaRegen",	2 );

		//setup basic damage, and attack		
		m_kCharacterStats.SetStat("Attack",	m_kCharacterStats.GetTotal("Dexterity") / 1.0 );
		m_kCharacterStats.SetStat("Defense",m_kCharacterStats.GetTotal("Dexterity") / 2.0 );
			
		
		//stamina	
		
		int iDrain = int(m_kCharacterStats.GetTotal("StaminaRegen"));		
		switch(pkCC->GetCharacterState())
		{
			case eWALKING: iDrain = 0; break;
			case eRUNNING: iDrain -= 3; break;
			case eJUMPING: iDrain -= 8; break;
			case eSWIMMING: iDrain -= 4; break;		
			case eSITTING:	iDrain *= 2; break;		
		}
		
		if(m_bInCamp)
			iDrain *= 10;
				
		string strStamina("Stamina");		
		m_kCharacterStats.ChangeStat(strStamina,iDrain);
		
		if(m_kCharacterStats.GetTotal(strStamina) > m_kCharacterStats.GetTotal("StaminaMax"))
			m_kCharacterStats.SetStat(strStamina,m_kCharacterStats.GetTotal("StaminaMax"));
		if(m_kCharacterStats.GetTotal(strStamina) < 0)
			m_kCharacterStats.SetStat(strStamina,0);
			
			
		//health
		string strHealth("Health");		
 		if(m_bInCamp) 		
 			m_kCharacterStats.ChangeStat(strHealth,m_kCharacterStats.GetTotal("HealthRegen") * 10);
 		else
			m_kCharacterStats.ChangeStat(strHealth,m_kCharacterStats.GetTotal("HealthRegen"));
		
		if(m_kCharacterStats.GetTotal(strHealth) > m_kCharacterStats.GetTotal("HealthMax"))
			m_kCharacterStats.SetStat(strHealth,m_kCharacterStats.GetTotal("HealthMax"));
		if(m_kCharacterStats.GetTotal(strHealth) < 0)
			m_kCharacterStats.SetStat(strHealth,0);

		// Oxygen
		string strOxygen("Oxygen");
		if(pkCC->GetWaterLevel() == WATER_HEAD)
			m_kCharacterStats.ChangeStat(strOxygen, -10);	// Drains 10 pts/sec.
		else
			m_kCharacterStats.ChangeStat(strOxygen, 25);		// Regen 25 pts/sec.
		
		if(m_kCharacterStats.GetTotal(strOxygen) > 100)
			m_kCharacterStats.SetStat(strOxygen,100);
		if(m_kCharacterStats.GetTotal(strOxygen) < 0)
			m_kCharacterStats.SetStat(strOxygen,0);
	
		//mana
		string strMana("Mana");
 		if(m_bInCamp)		
 			m_kCharacterStats.ChangeStat(strMana,m_kCharacterStats.GetTotal("ManaRegen")*10);
 		else
			m_kCharacterStats.ChangeStat(strMana,m_kCharacterStats.GetTotal("ManaRegen"));		
		
		if(m_kCharacterStats.GetTotal(strMana) > m_kCharacterStats.GetTotal("ManaMax"))
			m_kCharacterStats.SetStat(strMana,m_kCharacterStats.GetTotal("ManaMax"));
		if(m_kCharacterStats.GetTotal(strMana) < 0)
			m_kCharacterStats.SetStat(strMana,0);
				

		
		SendStats();
	}

	//setup charactercontrol
	int iSpeed = int(m_kCharacterStats.GetTotal("Speed"));
	int iJump = int(m_kCharacterStats.GetTotal("Jump"));
	
	if(m_kCharacterStats.GetTotal("Stamina") < 10 )
	{
		pkCC->SetForceCrawl(true);
		iJump = 0;
	}
	else
		pkCC->SetForceCrawl(false);
	
	//speed
	pkCC->SetSpeed(iSpeed);
	
	//jump
	pkCC->SetJumpForce(iJump);		
	
	//animation
	if(m_bCombatMode)
		pkCC->SetAnimationSet(1);
	else
		pkCC->SetAnimationSet(0);
	
	
}

bool P_CharacterProperty::InCamp()
{
	vector<Entity*>	kZones;	
	vector<Entity*> 	kEntitys;
		
	bool bHaveFirePlace = false;
	int iEnemy = -1;
	float fRange = 999999999;
	
	Entity* pkZone = NULL;
	
	//does this entity use zones?
	if(m_pkEntity->GetUseZones())
	{
		pkZone = m_pkEntity->GetParent();
	}
	else if(m_pkEntity->GetParent()->GetUseZones())
	{
		pkZone = m_pkEntity->GetParent()->GetParent();
	}
	
	
	//we assume that parent is a zone
	if(pkZone)
	{
		pkZone->GetZoneNeighbours(&kZones);
		
		//get all propertys in zones
		for(int i =0;i<kZones.size();i++)
		{
			kZones[i]->GetAllEntitys(&kEntitys);
		}
		
		for(int i = 0;i<kEntitys.size();i++)
		{
			if(kEntitys[i] == m_pkEntity)
				continue;
		
			//a character?
			if(P_CharacterProperty* pkCP = (P_CharacterProperty*)kEntitys[i]->GetProperty("P_CharacterProperty"))
			{
				if(pkCP->IsDead() || pkCP->IsIncap())
					continue;
				
				//is charater evil against me?
				if(pkCP->IsEnemy(GetEntity()->GetEntityID()))
				{					
					float fDistance = pkCP->GetEntity()->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
					
					//out of range
					if(fDistance < 16)
						return false;
				}
			}
		
		
			//a fireplace here?
			if(kEntitys[i]->GetType() == "fireplace-skillobj.lua")
			{
				//out of range
				if(kEntitys[i]->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV()) < 4)
				{
					bHaveFirePlace = true;
				}
			}
		}
	}
	
	return bHaveFirePlace;
}

bool P_CharacterProperty::CanRest()
{
	vector<Entity*>	kZones;	
	vector<Property*> kPropertys;
		
	int iEnemy = -1;
	float fRange = 999999999;
	
	Entity* pkZone = NULL;
	
	//does this entity use zones?
	if(m_pkEntity->GetUseZones())
	{
		pkZone = m_pkEntity->GetParent();
	}
	else if(m_pkEntity->GetParent()->GetUseZones())
	{
		pkZone = m_pkEntity->GetParent()->GetParent();
	}
	
	
	//we assume that parent is a zone
	if(pkZone)
	{
		pkZone->GetZoneNeighbours(&kZones);
		
		//get all propertys in zones
		for(int i =0;i<kZones.size();i++)
		{
			kZones[i]->GetAllPropertys(&kPropertys,PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_SERVER);
		}
		
		
		//find characters
		static Property* pkProp;
		
		for(int i = 0;i<kPropertys.size();i++)
		{
			pkProp = kPropertys[i];
		
			//skip self
			if(pkProp == this)
				continue;
		
			//we only care about character propertys
 			if(!pkProp->IsType("P_CharacterProperty"))
 				continue;
				
			if(P_CharacterProperty* pkCP = static_cast<P_CharacterProperty*>(pkProp))
			{
				if(pkCP->IsDead() || pkCP->IsIncap())
					continue;
				
				//is charater evil against me?
				if(pkCP->IsEnemy(GetEntity()->GetEntityID()))
				{					
					float fDistance = pkCP->GetEntity()->GetWorldPosV().DistanceTo(m_pkEntity->GetWorldPosV());
					
					//out of range
					if(fDistance < 16)
						return false;
				}
			}
		}
	}
	
	return true;
	
}	


void P_CharacterProperty::MakeAlive()
{		
	m_bIncap = false;
	m_bDead = false;
	ResetAllNetUpdateFlags();

	//make sure character has some life and oxygen.
	if(m_kCharacterStats.GetTotal("Health") <= 0)
		m_kCharacterStats.SetStat("Health",1);
	m_kCharacterStats.SetStat("Oxygen",100);
	
	//set characters XP value to 0
	m_kCharacterStats.SetStat("ExperienceValue",0); 
	
				
	//enable character movement
	if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl"))
	{
		pkCC->SetEnabled(true);	
		pkCC->SetCharacterState(eIDLE_STANDING);		//this will make the character stand up when awakens
	}

	//enable sphere collissions on tcs again
	if(P_Tcs* pkTcs = (P_Tcs*)m_pkEntity->GetProperty("P_Tcs"))
	{
		pkTcs->SetTestType(E_SPHERE);
		pkTcs->SetStatic(false);
	}	
	
	
	//not a player character
	if(m_iConID == -1)
	{
		//enable AI
		if(P_AI* pkAI = (P_AI*)m_pkEntity->GetProperty("P_AI"))
		{
			pkAI->SetState(eAI_STATE_GUARD);
		}				
		
		//set inventory owner again, disables looting
		if(P_Container* pkInv = (P_Container*)m_pkEntityManager->GetPropertyFromEntityID(m_iInventory,"P_Container"))
		{
			pkInv->SetStaticOwner(true);
			pkInv->SetOwnerID(m_pkEntity->GetEntityID());
			pkInv->SetContainerType(eInventory);
		}		
	}
	else
	{
	
		//send im alive info to client, if any
		SendAliveInfo();	
	}	
}

void P_CharacterProperty::OnIncap()
{
	// NPC's always die
	if(m_iConID == -1)
	{
		OnDeath();
		return;
	}

	// 40% chance that we drop dead.
    if(Math::Randomi(100) < 40)
	{
		OnDeath();
		return;
	}

	m_bIncap	= true;
	m_bDead	= false;
	ResetAllNetUpdateFlags();

	SendTextToClient(string("You are incapacitated."));

	m_fDeadTimer = m_pkEntityManager->GetSimTime();
	m_fDecayTime = m_kCharacterStats.GetTotal("Vitality");

	//set death animation
	if(P_Mad* pkMad = (P_Mad*)m_pkEntity->GetProperty("P_Mad"))
	{
		pkMad->SetAnimation("die", 0);
		pkMad->m_bLoop = false;
		pkMad->SetNextAnimation(MAD_NOLOOP);
	}
		
	//disable character movement
	if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl"))
	{
		pkCC->SetEnabled(false);	
	}
	
	//disable TCS collissions
	if(P_Tcs* pkTcs = (P_Tcs*)m_pkEntity->GetProperty("P_Tcs"))
	{
		pkTcs->SetTestType(E_NONE);
		pkTcs->SetAirFriction(15);		//make sure character wont bounce around
	}

	if(m_iConID == -1)
	{
		m_fDeadTimer = m_pkEntityManager->GetSimTime();
	
		//disable AI
		if(P_AI* pkAI = (P_AI*)m_pkEntity->GetProperty("P_AI"))
		{
			pkAI->SetState(eAI_STATE_DEAD);
		}
	}
	else
		SendStats();
}

void P_CharacterProperty::OnDeath()
{
	m_bDead = true;
	ResetAllNetUpdateFlags();
	
	m_pkEntityManager->CallFunction(m_pkEntity, "Death");
	
	
	//give killer some XP
	if(m_iLastDamageFrom != -1)
	{
		if(P_CharacterProperty* pkKiller = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(m_iLastDamageFrom,"P_CharacterProperty"))
		{
			SendTextToClient(string("You were killed by ")+pkKiller->GetName()+". Press backspace to resurrect.");
		
			//only give player characters XP
			if(pkKiller->GetIsPlayerCharacter())
			{
				float fLevelMod = 1.5;
				int iXP =	int(m_kCharacterStats.GetTotal("ExperienceValue") * 
								(m_kCharacterStats.GetTotal("Level") * fLevelMod ));
								
				pkKiller->GiveExperience(iXP);				
			}
		}		
	}				
	
	//set death animation
	P_Mad* pkMad = (P_Mad*)m_pkEntity->GetProperty("P_Mad");
	if(pkMad && !m_bIncap)
	{
		pkMad->SetAnimation("die", 0);
		pkMad->m_bLoop = false;
		pkMad->SetNextAnimation(MAD_NOLOOP);
		m_bIncap = true;
	}
		
	//disable character movement
	if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl"))
	{
		pkCC->SetEnabled(false);	
	}
	
	//disable TCS collissions
	if(P_Tcs* pkTcs = (P_Tcs*)m_pkEntity->GetProperty("P_Tcs"))
	{
		pkTcs->SetTestType(E_NONE);
		pkTcs->SetAirFriction(15);		//make sure character wont bounce around
	}
	
	//not a player character
	if(m_iConID == -1)
	{
		m_fDeadTimer = m_pkEntityManager->GetSimTime();
	
		//disable AI
		if(P_AI* pkAI = (P_AI*)m_pkEntity->GetProperty("P_AI"))
		{
			pkAI->SetState(eAI_STATE_DEAD);
		}
		
		//reset inventory owner, to allow other players to loot
		if(P_Container* pkInv = (P_Container*)m_pkEntityManager->GetPropertyFromEntityID(m_iInventory,"P_Container"))
		{
			//cout<<"disabling static owner of inventory to allow looting"<<endl;
			pkInv->SetStaticOwner(false);
			pkInv->SetOwnerID(-1);
			pkInv->SetContainerType(eNormal);
		}
				
		//create spawner, could be evil if this character is rewived
		CreateSpawner();				
	}
	else
	{		
		//send death info to client
		SendStats();
		SendDeathInfo();
		SendTextToClient(string("You are dead."));
	}
}

void P_CharacterProperty::GiveGroupExperience(int iXP)
{
	const void* apParam[2];
	
	int iEntityId = GetEntity()->GetEntityID();
	int iAddXP = iXP;
	apParam[0] = &iEntityId;
	apParam[1] = &iAddXP;

	Application* m_pkApplication = static_cast<Application*>(g_ZFObjSys.GetObjectPtr("Application"));			
	m_pkApplication->OnSystemMessage("givexp",2,apParam);
}

void P_CharacterProperty::GiveExperience(int iXP)
{
	SendPointText(IntToString(iXP)+" XP",m_pkEntity->GetWorldPosV(),3);
	SendTextToClient("You got "+IntToString(iXP)+" XP");
	
	m_kCharacterStats.ChangeStat("Experience",iXP);
		
	if(m_kCharacterStats.GetTotal("Experience") >= m_kCharacterStats.GetTotal("NextLevel"))
	{
		float fLevelMod = 2.0;	
		m_kCharacterStats.SetStat("PrevLevel",m_kCharacterStats.GetTotal("NextLevel"));
		m_kCharacterStats.SetStat("NextLevel",m_kCharacterStats.GetTotal("NextLevel") * fLevelMod);
		m_kCharacterStats.ChangeStat("Level",1);
		
		OnLevelUP();
	}
}

void P_CharacterProperty::AddStatPoint(int iStat)
{
	if(iStat < 0 || iStat > 5)
		return;
	
	if(m_kCharacterStats.GetStat("StatPoints") <= 0)
		return;

		
	m_kCharacterStats.ChangeStat("StatPoints",-1);
		
	switch(iStat)
	{
		case 0: m_kCharacterStats.ChangeStat("Strength",1);	break;
		case 1: m_kCharacterStats.ChangeStat("Dexterity",1);	break;
		case 2: m_kCharacterStats.ChangeStat("Vitality",1);	break;
		case 3: m_kCharacterStats.ChangeStat("Intelligence",1);break;
		case 4: m_kCharacterStats.ChangeStat("Wisdom",1);		break;
		case 5: m_kCharacterStats.ChangeStat("Charisma",1);	break;	
	}
	
	//send updated stats to client, removes some laggy feeling when adding statpoints
	SendStats();
}

bool P_CharacterProperty::AddSkillPoint(const string& strSkillName)
{
	if(m_kCharacterStats.GetStat("SkillPoints") <= 0)
		return false;
			
			
	if(ChangeSkill(strSkillName,1))
	{
		//remove one skillpoint
		m_kCharacterStats.ChangeStat("SkillPoints",-1);
	
		//send updated skillinfo to client
		SendSkillInfo(strSkillName);	
	
		return true;
	}
		
	return false;
}

void P_CharacterProperty::OnLevelUP()
{
	Vector3 kRandomPos(Math::Randomf(0.5)-0.25,Math::Randomf(0.5)-0.25,Math::Randomf(0.5)-0.25);

 	SendPointText("LEVEL UP!",m_pkEntity->GetWorldPosV()+kRandomPos,3);
	SendTextToClient("You are now level "+IntToString((int)m_kCharacterStats.GetTotal("Level")));	

	m_kCharacterStats.ChangeStat("StatPoints",4);		
	m_kCharacterStats.ChangeStat("SkillPoints",1);	
}

void P_CharacterProperty::SendPointText(const string& strText,const Vector3& kPos,int iType)
{
	const void* apParam[5];

	float fTTL = 3;
	
	apParam[0] = &strText;
	apParam[1] = &kPos;
	apParam[2] = &Vector3(0,0.3,0);
	apParam[3] = &fTTL;
	apParam[4] = &iType;
	
	g_ZFObjSys.SendSystemMessage("Application","PointText",5,apParam);	
}

void P_CharacterProperty::OnHit(int iAttacker,int iDamage)
{
	if(m_bDead)
		return;
	
	//set last attacker
	SetLastDamageFrom(iAttacker);
	
	//reduce health
	m_kCharacterStats.ChangeStat("Health",-iDamage);

	//message to client
	if(m_iConID != -1)
	{	
		int iD = m_iConID;
		const void* pkID[2];
		pkID[0] = &iD;
		pkID[1] = &iDamage;
		m_pkApp->OnSystemMessage("PlayerHit",2,pkID);	
	}		

	//call script function
	vector<ScriptFuncArg> args(1);
	args[0].m_kType.m_eType = tINT;
	args[0].m_pData = &iAttacker;			//owner character id			
	m_pkEntityManager->CallFunction(m_pkEntity,"Hit",&args);	

}

float P_CharacterProperty::Facing(Entity* pkCharacter,const Vector3& kPos)
{
	//check rotation
	Vector3 kOwnerDir = pkCharacter->GetWorldRotM().VectorTransform(Vector3(0,0,1));
	Vector3 kDir = kPos - pkCharacter->GetWorldPosV();		
	
	kOwnerDir.y = 0;
	kDir.y = 0;
	
	kOwnerDir.Normalize();
	kDir.Normalize();
	
	
	return Math::RadToDeg(kOwnerDir.Angle(kDir));
}

void P_CharacterProperty::OnMiss(int iAttacker)
{
	if(m_bDead)
		return;	
		
	DoParryAnimation(iAttacker);	
	
	//calls script fuction
	vector<ScriptFuncArg> args(1);
	args[0].m_kType.m_eType = tINT;
	args[0].m_pData = &iAttacker;			//owner character id			
	m_pkEntityManager->CallFunction(m_pkEntity,"Miss",&args);		
}


void P_CharacterProperty::DoParryAnimation(int iAttacker)
{
	if(iAttacker != -1)
	{
		if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl"))
		{
			if(Entity* pkAttacker = m_pkEntityMan->GetEntityByID(iAttacker))
			{		
				float fFacing = Facing(m_pkEntity,pkAttacker->GetWorldPosV());							
				bool bFront = fFacing <= 90;
				bool bFace = fFacing <= 60;
				bool bHaveShield = 			HaveEqipedBaseType("shield"); 
				bool bHaveOnehandweapon = 	HaveEqipedBaseType("axe") || HaveEqipedBaseType("sword") || HaveEqipedBaseType("mace"); 
				bool bHaveBow = 				HaveEqipedBaseType("bow"); 
						
				vector<int> kAnimations;
				
				if(bFace && bHaveShield) kAnimations.push_back(0);
				if(bFace && bHaveOnehandweapon) kAnimations.push_back(1);
				if(bFace && bHaveBow) kAnimations.push_back(3);
												
				if(!kAnimations.empty())
				{
					pkCC->Lock(0.5);			
					int iAnimation = Math::Randomi(kAnimations.size());
					switch(kAnimations[iAnimation])
					{
						case 0:	pkCC->DoAnimation("parry_shield");break;
						case 1:	pkCC->DoAnimation("parry_weapon");break;
						case 3:	pkCC->DoAnimation("parry_weapon");break;
					}
				}				
			}		
		}
	}
}

void P_CharacterProperty::UpdateSkills()
{
	float fTime = m_pkZeroFps->GetEngineTime();
	
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

void P_CharacterProperty::SetupSpawnPos()
{
	//if we already have a respawn pos and zone, ignore it
 	if(m_iRespawnZone != -1)
 		return;

	m_iRespawnZone = m_pkEntity->GetCurrentZone();
	m_kRespawnPos = m_pkEntity->GetWorldPosV();
	
// 	cout<<"UPDATED SPAWN POSITON: zone "<<m_iRespawnZone<<endl;
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
				pkCon->AddItemType(MLITEM_SHIELDHAND);
				pkCon->SetJoint("equip_left");
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
				pkCon->AddItemType(MLITEM_WEAPONHAND);
				pkCon->SetJoint("equip_right");
				pkCon->SetEquip(true);
				pkCon->SetWeaponHand(true);
				
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

bool P_CharacterProperty::HaveEqipedBaseType(const string& strBaseName)
{
	if(P_Container* pkContainer = (P_Container*)m_pkEntityManager->GetPropertyFromEntityID(m_iLeftHand,"P_Container"))
		if(P_Item* pkItem = (P_Item*)m_pkEntityManager->GetPropertyFromEntityID(*pkContainer->GetItem(0,0),"P_Item"))
			if(pkItem->GetBaseName() == strBaseName)
				return true;
	
	if(P_Container* pkContainer = (P_Container*)m_pkEntityManager->GetPropertyFromEntityID(m_iRightHand,"P_Container"))
		if(P_Item* pkItem = (P_Item*)m_pkEntityManager->GetPropertyFromEntityID(*pkContainer->GetItem(0,0),"P_Item"))
			if(pkItem->GetBaseName() == strBaseName)
				return true;				
				
	return false;
}

P_CharacterProperty::~P_CharacterProperty()
{
	//make sure all sounds are stoped
	m_pkAudioSystem->StopAudio(m_iWalkSoundID);
	m_pkAudioSystem->StopAudio(m_iRunSoundID);
	m_pkAudioSystem->StopAudio(m_iSwimSoundID);

	delete m_pkTextMaterial;
	delete m_pkFont;	
	
	RemoveAllSkills();
}

void P_CharacterProperty::GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState)
{
	if(GetIsPlayerCharacter())
	{
		if(m_bOverHeadText)
		{
			float fDistance = m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(GetEntity()->GetWorldPosV());
			if(fDistance < 20)
			{
				string strText = GetName()+string(" <")+GetOwnedByPlayer()+string("> ") + m_strChatMsg;		
				
								
				float fScale = 0.05 * fDistance;
				
				m_kRenderPackage.m_kMeshData.m_kVertises.clear();
				m_kRenderPackage.m_kMeshData.m_kTexture[0].clear();
				
				m_pkRender->AddTextBillboard(m_kRenderPackage,kRenderState,GetEntity()->GetIWorldPosV()+
								Vector3(0,1.0,0),fScale,strText,m_pkFont,true);
			
			
				m_kRenderPackage.m_pkMaterial = m_pkTextMaterial;
				m_kRenderPackage.m_kMeshData.m_ePolygonMode = QUADS_MODE;			
				m_kRenderPackage.m_kMeshData.m_iNrOfDataElements = m_kRenderPackage.m_kMeshData.m_kVertises.size();
			
			
				kRenderPackages.push_back(&m_kRenderPackage);
			}						
		}
	}	
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
				
				//setup spawn pos
				SetupSpawnPos();		
				
				//send skillbar
				SendSkillbar();
			}
		
			//if not dead
			if(!m_bDead && !m_bIncap)
			{
				//update stats
				UpdateStats();
			
				//update skills
				UpdateSkills();				
			
				//update combat
				UpdateSkillQueue();							
			}
			else
			{
				if(!m_bDead && m_bIncap)
				{
					if(m_kCharacterStats.GetTotal("Health") >= 1)
					{
						MakeAlive();
					}
					else if(m_pkEntityManager->GetSimTime() - m_fDeadTimer > m_fDecayTime)
					{
						//send character to the world beyound
						MakeAlive();						
					}			
				}

				if(m_bDead)
				{
					//not a player character
					if(m_iConID == -1)
					{
						//been dead for a while
						if(m_pkEntityManager->GetSimTime() - m_fDeadTimer > m_fDecayTime)
						{
							//send character to the world beyound
							m_pkEntityManager->Delete(m_pkEntity);						
						}			
					}
				}
			}
		}
			
		//CLIENT
		if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_CLIENT))
			if(m_bWalkSound)
				PlayCharacterMovementSounds();					
		
		//CLIENT AND SERVER
		//reset chat msg
		if(m_pkZeroFps->GetEngineTime() > m_fChatTime + 10.0)
			m_strChatMsg = "";						
	}
	else if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER))
	{
		if(m_pkZeroFps->m_bEditMode)
			DrawEditor();
	}		
}

void P_CharacterProperty::DrawEditor()
{
	if(!m_pkZeroFps->m_kAI_ShowInfo.GetBool())
		return;

	string strText;
	float fDistance;
	fDistance = m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(GetEntity()->GetWorldPosV());

	for(int i=0; i<kDebugVar.size(); i++)
	{
		strText = kDebugVar[i].m_Variable + ": " + kDebugVar[i].m_Value;		
		if(fDistance < 20)
		{
			Vector3 kOffset = Vector3(0,1.0,0) + (0.2 * i);
			
			m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),GetEntity()->GetIWorldPosV()+
							kOffset,0.2,strText,m_pkTextMaterial,m_pkFont,false);													
		}
	}
}


void P_CharacterProperty::CreateSpawner()
{
	if(m_fRespawnTime != -1)
	{	
		//create a spawner
		if(Entity* pkEnt = m_pkEntityManager->CreateEntityFromScriptInZone("data/script/objects/game objects/spawner.lua",m_kRespawnPos,m_iRespawnZone))
		{
			if(P_Spawn* pkSpawn = (P_Spawn*)pkEnt->GetProperty("P_Spawn"))
			{
				if(ZFResourceHandle* pkScript = m_pkEntity->GetEntityScript())
				{			
					pkSpawn->SetSpawnScript(pkScript->GetRes());
					pkSpawn->SetSpawnDelay(m_fRespawnTime);
				}
			}
		}	
		
		//spawner created, disable spawning on this character
		m_fRespawnTime = -1;
	}
}

void P_CharacterProperty::AddChatMsg(const string& strChatMsg)
{
	m_strChatMsg = strChatMsg;
	m_fChatTime = m_pkZeroFps->GetEngineTime();	
}


void P_CharacterProperty::PlayCharacterMovementSounds()
{
	static float fWalkGain = 0.04;
	static Vector3 kOffset(0,0.5,0);

	if(P_CharacterControl* pkCC = (P_CharacterControl*)GetEntity()->GetProperty("P_CharacterControl"))
	{
		//get current character state
		int iState = pkCC->GetCharacterState();
		
		//walk sound
		if(iState == eWALKING)
		{
			if(m_iCurrentCharacterState != eWALKING)
			{
				m_iWalkSoundID = m_pkAudioSystem->PlayAudio(m_strWalkSound,GetEntity()->GetIWorldPosV()+kOffset,Vector3(0,0,0),ZFAUDIO_LOOP,fWalkGain);
			}
			else
			{
				m_pkAudioSystem->MoveAudio(m_iWalkSoundID, GetEntity()->GetIWorldPosV());
			}
		}
		else
		{
			if(m_iCurrentCharacterState == eWALKING)
			{
				m_pkAudioSystem->StopAudio(m_iWalkSoundID);
			}
		}	
		
		//walk watersound
		if(iState == eWALKING_WATER)
		{
			if(m_iCurrentCharacterState != eWALKING_WATER)
				m_iWalkWaterSoundID = m_pkAudioSystem->PlayAudio(m_strWalkWaterSound,GetEntity()->GetIWorldPosV()+kOffset,Vector3(0,0,0),ZFAUDIO_LOOP,0.1);
			else
				m_pkAudioSystem->MoveAudio(m_iWalkWaterSoundID, GetEntity()->GetIWorldPosV());
		}
		else if(m_iCurrentCharacterState == eWALKING_WATER)
			m_pkAudioSystem->StopAudio(m_iWalkWaterSoundID);
		
		//run watersound
		if(iState == eRUNNING_WATER)
		{
			if(m_iCurrentCharacterState != eRUNNING_WATER)
				m_iRunWaterSoundID = m_pkAudioSystem->PlayAudio(m_strRunWaterSound,GetEntity()->GetIWorldPosV()+kOffset,Vector3(0,0,0),ZFAUDIO_LOOP,0.1);
			else
				m_pkAudioSystem->MoveAudio(m_iRunWaterSoundID, GetEntity()->GetIWorldPosV());
		}
		else if(m_iCurrentCharacterState == eRUNNING_WATER)
			m_pkAudioSystem->StopAudio(m_iRunWaterSoundID);		
		
		//run sound
		if(iState == eRUNNING)
		{
			if(m_iCurrentCharacterState != eRUNNING)
			{
				m_iRunSoundID = m_pkAudioSystem->PlayAudio(m_strRunSound,GetEntity()->GetIWorldPosV()+kOffset,Vector3(0,0,0),ZFAUDIO_LOOP,fWalkGain);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iRunSoundID, GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_iCurrentCharacterState == eRUNNING)
				m_pkAudioSystem->StopAudio(m_iRunSoundID);
		}	
		
				
		
		//swim sound
		if(iState == eSWIMMING)
		{
			if(m_iCurrentCharacterState != eSWIMMING)
			{	
				m_iSwimSoundID = m_pkAudioSystem->PlayAudio(m_strSwimSound,GetEntity()->GetIWorldPosV()+kOffset,Vector3(0,0,0),ZFAUDIO_LOOP,0.1);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iSwimSoundID,GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_iCurrentCharacterState == eSWIMMING)
				m_pkAudioSystem->StopAudio(m_iSwimSoundID);
		}			

		//jump sound
		if(iState == eJUMPING)
		{
			if(m_iCurrentCharacterState != eJUMPING)
			{
				m_pkAudioSystem->PlayAudio(m_strJumpSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),0,fWalkGain*2);
			}
		}
															
		//update staes
		m_iCurrentCharacterState =	iState;

	}
}

void P_CharacterProperty::SendAliveInfo()
{
	if(m_iConID == -1)
		return;

	NetPacket kNp;
	kNp.Write((char) MLNM_SC_ALIVE);	
	
	//send package
	kNp.TargetSetClient(m_iConID);
	m_pkApp->SendAppMessage(&kNp);	
}

void P_CharacterProperty::SendDeathInfo()
{
	if(m_iConID == -1)
		return;

	NetPacket kNp;
	kNp.Write((char) MLNM_SC_DEAD);	
	
	//send package
	kNp.TargetSetClient(m_iConID);
	m_pkApp->SendAppMessage(&kNp);		
}

void P_CharacterProperty::SendTextToClient(const string& strText)
{
	if(m_iConID == -1)
		return;

	const void* apParams[2];
	apParams[0] = &strText;
	apParams[1] = &m_iConID;

	g_ZFObjSys.SendSystemMessage("Application","SayToClients",2,apParams);
}

void P_CharacterProperty::SendStats()
{
	if(m_iConID == -1)
		return;

	NetPacket kNp;
	kNp.Write((char) MLNM_SC_CHARACTERSTATS);	

	
	kNp.Write(m_kCharacterStats.GetTotal("Stamina"));
	kNp.Write(m_kCharacterStats.GetTotal("StaminaMax"));
		

	P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntity->GetProperty("P_CharacterControl");
	if(pkCC)
	{
		if(pkCC->GetWaterLevel() != WATER_NONE)
		{
			kNp.Write(m_kCharacterStats.GetTotal("Oxygen"));
			kNp.Write(float(100));
		}
		else
		{
			kNp.Write(m_kCharacterStats.GetTotal("Oxygen"));
			kNp.Write((float)99);
		}
	}

	kNp.Write(m_kCharacterStats.GetTotal("Health"));
	kNp.Write(m_kCharacterStats.GetTotal("HealthMax"));

	kNp.Write(m_kCharacterStats.GetTotal("Mana"));
	kNp.Write(m_kCharacterStats.GetTotal("ManaMax"));
	
	kNp.Write_Str(m_strName);
	kNp.Write(m_kCharacterStats.GetTotal("Level"));	
	kNp.Write(m_kCharacterStats.GetTotal("Experience"));	
	kNp.Write(m_kCharacterStats.GetTotal("PrevLevel"));	
 	kNp.Write(m_kCharacterStats.GetTotal("NextLevel"));	

 	kNp.Write(m_kCharacterStats.GetTotal("Money"));	
 	
 	kNp.Write(m_kCharacterStats.GetTotal("StatPoints"));	 	
 	
 	kNp.Write(m_kCharacterStats.GetTotal("Strength"));	
 	kNp.Write(m_kCharacterStats.GetTotal("Dexterity"));	
 	kNp.Write(m_kCharacterStats.GetTotal("Vitality"));	
 	kNp.Write(m_kCharacterStats.GetTotal("Intelligence"));	
 	kNp.Write(m_kCharacterStats.GetTotal("Wisdom"));	
 	kNp.Write(m_kCharacterStats.GetTotal("Charisma"));	

 	kNp.Write(m_kCharacterStats.GetTotal("Attack"));	
 	kNp.Write(m_kCharacterStats.GetTotal("Defense"));	

			
	float fMinDamage = 0;
	fMinDamage += m_kCharacterStats.GetTotal("DamageSlashingMin");
	fMinDamage += m_kCharacterStats.GetTotal("DamageCrushingMin");
	fMinDamage += m_kCharacterStats.GetTotal("DamagePiercingMin");

	float fMaxDamage = 0;
	fMaxDamage += m_kCharacterStats.GetTotal("DamageSlashingMax");
	fMaxDamage += m_kCharacterStats.GetTotal("DamageCrushingMax");
	fMaxDamage += m_kCharacterStats.GetTotal("DamagePiercingMax");
	
 	kNp.Write(fMinDamage);
 	kNp.Write(fMaxDamage);
	
	
	float fAbsorbTotal = 0;
	fAbsorbTotal += m_kCharacterStats.GetTotal("AbsorbSlashing");
	fAbsorbTotal += m_kCharacterStats.GetTotal("AbsorbCrushing");
	fAbsorbTotal += m_kCharacterStats.GetTotal("AbsorbPiercing");
	fAbsorbTotal += m_kCharacterStats.GetTotal("AbsorbFire");
	fAbsorbTotal += m_kCharacterStats.GetTotal("AbsorbIce");
	fAbsorbTotal += m_kCharacterStats.GetTotal("AbsorbLightning");
	fAbsorbTotal += m_kCharacterStats.GetTotal("AbsorbMagic");
	fAbsorbTotal += m_kCharacterStats.GetTotal("AbsorbPoison");
	
 	kNp.Write(fAbsorbTotal);
		
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
		if(!kProps[i]->IsType("P_Buff"))
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

int P_CharacterProperty::UseSkill(const string& strSkillScript,int iTarget,const Vector3& kPos,const Vector3& kDir)
{
	if(m_bDead)
		return -2;

	//check if character is already using a skill
	if(m_pkZeroFps->GetEngineTime() > m_fSkillLockTime )
	{		
		if(Skill* pkSkill = GetSkillPointer(strSkillScript))
		{	
			//check if its a combat skill, if so are we in combat mode?
			if(pkSkill->GetSkillType() == eOFFENSIVE && !m_bCombatMode)
				return 9;
		
			int iVal = pkSkill->Use(iTarget,kPos,kDir);
			
			//send skillbar
			if(iVal == 0)
				SendSkillbar(strSkillScript);
			
			return iVal;
		}
		else
		{
			cout<<"WARNING: skill "<<strSkillScript<<" not found"<<endl;			
			return -1;
		}
	}
	else
	{
		//cout<<"still waiting for skill to complete"<<endl;
		return 8;
	}
}

void  P_CharacterProperty::LockSkillUsage(float fTime)
{
	//dont lock if new time is shorter
	if(m_pkZeroFps->GetEngineTime() + fTime < m_fSkillLockTime)
		return;

	//lock skill usage
	m_fSkillLockTime = m_pkZeroFps->GetEngineTime() + fTime;
}

Skill* P_CharacterProperty::GetSkillPointer(const string& strSkillName)
{
	if(strSkillName.empty())
		return NULL;

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
					cerr<<"WARNING: Parent "<<pkSkill->GetParent()<<" have to be at least level 1 to level a child "<<strSkillScript<<endl;
					return false;
				}
			}
			else
			{
				cerr<<"WARNING:"<<strSkillScript<<" missing parent skill "<<pkSkill->GetParent()<<endl;
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
	
// 	//try to find parent
// 	if(!strParentSkill.empty())
// 	{
// 		if(!GetSkillPointer(strParentSkill))
// 		{
// // 			cerr<<"WARNING: parent skill "<<strParentSkill<<" not found when adding skill "<<strSkillScript<<endl;
// //			return false;
// 		}
// 	}				
	
	
	//create new skill
	Skill* pkNewSkill = new Skill(strSkillScript,strParentSkill,m_pkEntity->GetEntityID());	
				
	//add skill in skill list
	m_kSkills.push_back(pkNewSkill);	
		
	//do a first update
	pkNewSkill->SetLevel(0);

// 	cout<<"Added skill "<<strSkillScript<<endl;
	return true;
}

P_Buff* P_CharacterProperty::AddBuff(const string& strBuffName)
{
	if(Entity* pkEnt = m_pkEntityManager->CreateEntityFromScript((m_strBuffDir+strBuffName).c_str()))	
	{	
		pkEnt->SetParent(GetEntity());
		pkEnt->SetLocalPosV(Vector3(0,0,0));
		pkEnt->SetRelativeOri(true);
		
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
		if(!kProps[i]->IsType("P_Buff"))
			continue;

		P_Buff* pkBuff = static_cast<P_Buff*>(kProps[i]);

		if(pkBuff->GetEntity()->GetType() == strBuffName)
		{
			cout<<"removing buffs by name: "<<pkBuff->GetEntity()->GetName()<<endl;
			RemoveBuff(pkBuff);
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
	
	pkPackage->Write(m_fRespawnTime);	
	pkPackage->Write(m_iRespawnZone);	
	pkPackage->Write(m_kRespawnPos);	
	
	string test;
	pkPackage->Write_Str(test);
	
	m_kCharacterStats.Save(pkPackage);
	
	
	//save skills
	pkPackage->Write(m_kSkills.size());
	for(int i = 0;i<m_kSkills.size();i++)
	{
		pkPackage->Write_Str(m_kSkills[i]->GetName());
		pkPackage->Write_Str(m_kSkills[i]->GetParent());
		pkPackage->Write(m_kSkills[i]->GetLevel());
		
		pkPackage->Write(m_kSkills[i]->GetTimeLeft());
	}
	
	
	//save skillbar
	pkPackage->Write(m_kSkillBar.size());
	for(int i =0;i<m_kSkillBar.size();i++)
	{
		pkPackage->Write_Str(m_kSkillBar[i]);		
	}
}

void P_CharacterProperty::Load(ZFIoInterface* pkPackage,int iVersion)
{
	string temp;

	switch(iVersion)
	{
		
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
		
		case 9:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
			pkPackage->Read(m_iFaction); 		
			pkPackage->Read(m_bWalkSound); 		
			pkPackage->Read(m_fMarkerSize); 
			pkPackage->Read(m_bDead); 
		
			pkPackage->Read_Str(temp);
			
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
		
		case 10:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
			pkPackage->Read(m_iFaction); 		
			pkPackage->Read(m_bWalkSound); 		
			pkPackage->Read(m_fMarkerSize); 
			pkPackage->Read(m_bDead); 
		
			pkPackage->Read(m_fRespawnTime);	
			pkPackage->Read(m_iRespawnZone);	
			pkPackage->Read(m_kRespawnPos);				
			
			pkPackage->Read_Str(temp);
			
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
		
		case 11:
		{
			pkPackage->Read_Str(m_strName);	
			pkPackage->Read_Str(m_strOwnedByPlayer);	
			pkPackage->Read(m_bIsPlayerCharacter); 		
			pkPackage->Read(m_iFaction); 		
			pkPackage->Read(m_bWalkSound); 		
			pkPackage->Read(m_fMarkerSize); 
			pkPackage->Read(m_bDead); 
		
			pkPackage->Read(m_fRespawnTime);	
			pkPackage->Read(m_iRespawnZone);	
			pkPackage->Read(m_kRespawnPos);				
			
			pkPackage->Read_Str(temp);
			
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

			//load skillbar
			int iSkillbarSize;
			string strSkillName;
			
			pkPackage->Read(iSkillbarSize);
			
			if(iSkillbarSize != m_kSkillBar.size())
				cout<<"WARNING: skillbar size does not match while loading character"<<endl;
			
			m_kSkillBar.clear();			
			for(int i =0;i<iSkillbarSize;i++)
			{
				pkPackage->Read_Str(strSkillName);
				m_kSkillBar.push_back(strSkillName);
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
		
	pkNetPacket->Write(m_bCombatMode);
	pkNetPacket->Write(m_bDead);
	
	pkNetPacket->Write(m_bWalkSound);		
/*	pkNetPacket->Write_Str(m_strWalkSound);
	pkNetPacket->Write_Str(m_strRunSound);
	pkNetPacket->Write_Str(m_strJumpSound);
	pkNetPacket->Write_Str(m_strSwimSound);
	pkNetPacket->Write_Str(m_strWalkWaterSound);		
	pkNetPacket->Write_Str(m_strRunWaterSound);	*/	
	
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
	
	pkNetPacket->Read(m_bCombatMode);	
	pkNetPacket->Read(m_bDead);	
	
	pkNetPacket->Read(m_bWalkSound);	
/*	pkNetPacket->Read_Str(m_strWalkSound);
	pkNetPacket->Read_Str(m_strRunSound);
	pkNetPacket->Read_Str(m_strJumpSound);
	pkNetPacket->Read_Str(m_strSwimSound);		
	pkNetPacket->Read_Str(m_strWalkWaterSound);		
	pkNetPacket->Read_Str(m_strRunWaterSound);	*/	
}

bool P_CharacterProperty::UseMana(float fMana)
{
	if(m_kCharacterStats.GetTotal("Mana") >= fMana)
	{
		m_kCharacterStats.ChangeStat("Mana",-fMana);
		return true;
	}

	return false;
}

bool P_CharacterProperty::UseStamina(float fStamina)
{
	if(m_kCharacterStats.GetTotal("Stamina") >= fStamina)
	{
		m_kCharacterStats.ChangeStat("Stamina",-fStamina);
		return true;
	}

	return false;
}


void P_CharacterProperty::AddSkillToSkillbar(const string& strSkill,int iPos)
{	
	if(iPos < 0  || iPos >= m_kSkillBar.size())
		return;
		
	m_kSkillBar[iPos]  = strSkill;

	SendSkillbar(strSkill);
}

void P_CharacterProperty::RemoveItemFromSkillbar(int iPos)
{
	if(iPos < 0  || iPos >= m_kSkillBar.size())
		return;
	
	m_kSkillBar[iPos] = "";
	
	SendSkillbar();
}

void P_CharacterProperty::ReloadAllSkills()
{
	for(int i = 0;i<m_kSkills.size();i++)
		m_kSkills[i]->SetTimeLeft( 0 );
}


void P_CharacterProperty::SendCloseContainer(int iContainerID)
{
	if(m_iConID == -1)
		return;

	NetPacket kNp;
	kNp.Write((char) MLNM_SC_CLOSECONTAINER);	

	kNp.Write(iContainerID);	
	
	//send package
	kNp.TargetSetClient(m_iConID);				
	m_pkApp->SendAppMessage(&kNp);	
}

void P_CharacterProperty::SendSkillInfo(const string& strSkill)
{
	if(m_iConID == -1)
		return;

	Skill* pkSkill = GetSkillPointer(strSkill);
	
	if(!pkSkill)
	{
		cout << "Skill not found: " << strSkill << endl;   
		return;
	}


	NetPacket kNp;
	kNp.Write((char) MLNM_SC_SKILLINFO);	

	kNp.Write_Str(pkSkill->GetScreenName());
	kNp.Write_Str(pkSkill->GetIcon());
	kNp.Write_Str(pkSkill->GetInfoText());
	kNp.Write(pkSkill->GetLevel());
	kNp.Write(pkSkill->GetSkillType());	
	kNp.Write(pkSkill->GetRange());	
	kNp.Write(pkSkill->GetStaminaUsage());
	kNp.Write(pkSkill->GetManaUsage());
	kNp.Write(pkSkill->GetReloadTime());
	kNp.Write(pkSkill->GetCastTime());
	kNp.Write((int)m_kCharacterStats.GetStat("SkillPoints"));
	

	//send package
	kNp.TargetSetClient(m_iConID);				
	m_pkApp->SendAppMessage(&kNp);	
}

void P_CharacterProperty::SendSkillTree()
{
	if(m_iConID == -1)
		return;

	NetPacket kNp;
	kNp.Write((char) MLNM_SC_SKILLTREE);	
	
	int iSize = m_kSkills.size();
	kNp.Write(iSize);
		
	//walk a
		
		
	for(int i =0;i<iSize;i++)
	{
		kNp.Write_Str(m_kSkills[i]->GetName());
		kNp.Write_Str(m_kSkills[i]->GetParent());		
		kNp.Write_Str(m_kSkills[i]->GetScreenName());					
		kNp.Write_Str(m_kSkills[i]->GetIcon());						
		kNp.Write(m_kSkills[i]->GetLevel());								
	}
	
	//send package
	kNp.TargetSetClient(m_iConID);				
	m_pkApp->SendAppMessage(&kNp);	
}

void P_CharacterProperty::SendSkillbar(const string& strSkill)
{
	if(m_iConID == -1)
		return;

	NetPacket kNp;
	kNp.Write((char) MLNM_SC_SKILLBAR);	
 	
	//default attack skill
// 	kNp.Write_Str(m_strDefaultAttackSkill);	
	

 	int iSize = m_kSkillBar.size();
	for(int i = 0;i<iSize;i++)
	{
		//send only a specific skill?
		if(!strSkill.empty())
			if(m_kSkillBar[i] != strSkill)
				continue;
	
		//is there a skill at this position?
		if(Skill* pkSkill = GetSkillPointer(m_kSkillBar[i]))
		{
			kNp.Write((char)i);						
			kNp.Write_Str(m_kSkillBar[i]);	
			kNp.Write_Str(pkSkill->GetScreenName());
			kNp.Write_Str(pkSkill->GetIcon());
			kNp.Write(pkSkill->GetTimeLeft());
			kNp.Write(pkSkill->GetReloadTime());
			kNp.Write((char)pkSkill->GetSkillType());
			kNp.Write((char)pkSkill->GetTargetType());
		}
 		else
 		{
 			kNp.Write((char)i);
 			kNp.Write_Str(string(""));				
 		}
	}
			
	//end package with -1
	kNp.Write((unsigned char)-1);						
	
	

	//send package
	kNp.TargetSetClient(m_iConID);				
	m_pkApp->SendAppMessage(&kNp);	
		
}

bool P_CharacterProperty::IsEnemy(int iCharacterID)
{
	//im netural, i have no enemy factions
	if(m_iFaction == 0)
		return false;

	P_CharacterProperty* pkOtherCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iCharacterID,"P_CharacterProperty");
	if(!pkOtherCP)
		return false;

	if(pkOtherCP->m_iDMNoTarget)
		return false;

	if(pkOtherCP)
	{
		//im evil , i dont like any other factions
		if(m_iFaction < 0)
			if(pkOtherCP->GetFaction() != m_iFaction)
				return true;
			
		//im good , i dont like evil factions
		if(m_iFaction > 0)
			if(pkOtherCP->GetFaction() < 0)
				return true;

	}
	
	return false;
}

bool P_CharacterProperty::IsFriend(int iCharacterID)
{
	//im netural, i have no friendly factions
	if(m_iFaction == 0)
		return false;

	P_CharacterProperty* pkOtherCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(iCharacterID,"P_CharacterProperty");

	if(pkOtherCP)
	{
		//im evil , im friend with my faction only
		if(m_iFaction < 0)
			if(pkOtherCP->GetFaction() == m_iFaction)
				return true;
			
		//im good , im friend with all good factions
		if(m_iFaction > 0)
			if(pkOtherCP->GetFaction() > 0)
				return true;
	}

	return false;
}

bool P_CharacterProperty::IsNeutral()
{
	return m_iFaction == 0;
}



// SCRIPT INTERFACE FOR P_CharacterProperty
using namespace ObjectManagerLua;

/** \class CharacterProperty 
 *  \ingroup Property
 *  */
namespace SI_P_CharacterProperty
{
/**	\fn AddSkill( EntityID, strSkill, strParent )
		\brief Adds a skill.
		\relates CharacterProperty
*/
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

/**	\fn AddSkill( EntityID, strSkill, iValue )
		\brief Change a skill.
		\relates CharacterProperty
*/
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
	
/**	\fn AddAllSkills( EntityID )
		\brief Adds all skills to a entity skill.
		\relates CharacterProperty
*/
	int AddAllSkillsLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 1)
			return 0;		
			
		int iCharcterID;
		string strSkill;
		string strParent;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty");
		if(!pkCP)
			return 0;

		ZSSVFileSystem* pkFs = dynamic_cast<ZSSVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));
		vector<string>	kFiles;
		pkFs->ListDir(kFiles, "data/script/objects/game objects/skills");
		
		//add all lua files in the directory
		for(int i=0; i<kFiles.size(); i++)
			if(kFiles[i].find(".lua",kFiles[i].length()-4) != -1)
				pkCP->AddSkill(kFiles[i],"");

		return 0;			
	}

	int LockSkillUsageLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		

			
		int iCharcterID;
		double dLockTime;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgNumber(pkLua, 1,&dLockTime);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			pkCP->LockSkillUsage((float)dLockTime);		
	
		return 0;			
	}	
// 	int SetupSkillLua(lua_State* pkLua)
// 	{
// 		if(g_pkScript->GetNumArgs(pkLua) != 9)
// 			return 0;		
// 
// 			
// 		int iCharcterID;
// 		string strSkill;
// 		string strInGameName;
// 		string strSchool;
// 		string strIcon;
// 		double dReloadTime;
// 		int	iTargetType;
// 		int	iSkillType;
// 		double dRange;
// 		
// 		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);		
// 		
// 		g_pkScript->GetArgString(pkLua, 1, strSkill);				
// 		g_pkScript->GetArgString(pkLua, 2, strInGameName);		
// 		g_pkScript->GetArgString(pkLua, 3, strSchool);		
// 		g_pkScript->GetArgString(pkLua, 4, strIcon);		
// 		g_pkScript->GetArgNumber(pkLua, 5, &dReloadTime);		
// 		g_pkScript->GetArgInt(pkLua, 6, &iTargetType);		
// 		g_pkScript->GetArgInt(pkLua, 7, &iSkillType);		
// 		g_pkScript->GetArgNumber(pkLua, 8, &dRange);		
// 		
// 		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
// 		{
// 			if(Skill* pkSkill = pkCP->GetSkillPointer(strSkill))
// 			{
// 				pkSkill->m_strInGameName = strInGameName;
// 				pkSkill->m_strSchool = 		strSchool;
// 				pkSkill->m_strIcon = 		strIcon;
// 				pkSkill->m_fReloadTime = 	float(dReloadTime);
// 				pkSkill->m_iTargetType =	iTargetType;
// 				pkSkill->m_iSkillType =		iSkillType;
// 				pkSkill->m_fRange =			float(dRange);
// 				
// 				//cout<<"skill setup complete"<<endl;
// 			}
// 			else
// 			{
// 				cout<<"ERROR: setupskill called , but character does not have the skill "<<strSkill<<endl;			
// 			}
// 		}
// 	
// 		return 0;			
// 	}	
	
/**	\fn GetCharacterContainerID( EntityID, strContainerName )
		\brief Unknown
		\relates CharacterProperty
*/
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
			if(strContainerName == "Inventory") g_pkScript->AddReturnValue(pkLua,pkCP->m_iInventory);
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
	
/**	\fn MakeAlive( EntityID )
		\brief Return a dead character to life
		\relates CharacterProperty
*/
	int MakeAliveLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 1)
		{
			cout<<"WARNING: MakeAliveLua - wrong number of arguments"<<endl;
			return 0;		
		}

		int iCharcterID;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			pkCP->MakeAlive();

		return 0;
	}

/**	\fn SetCombatMode( EntityID, bMode )
		\brief Set if a character is in combat mode.
		\relates CharacterProperty
*/
	//set combat mode ,0 = false, 1 = true
	int SetCombatModeLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
		{
			cout<<"WARNING: SetDefaultAttackSkill - wrong number of arguments"<<endl;
			return 0;		
		}
					
		int iCharcterID;
		int iMode;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgInt(pkLua, 1, &iMode);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			pkCP->SetCombatMode(iMode);
		}
	
		return 0;	
	
	}
	
/**	\fn SetDefaultAttackSkill( EntityID, strSkill )
		\brief Unkown
		\relates CharacterProperty
*/
// 	//set default attack skill on character
// 	int SetDefaultAttackSkillLua(lua_State* pkLua)
// 	{
// 		if(g_pkScript->GetNumArgs(pkLua) != 2)
// 		{
// 			cout<<"WARNING: SetDefaultAttackSkill - wrong number of arguments"<<endl;
// 			return 0;		
// 		}
// 					
// 		int iCharcterID;
// 		string strSkill;
// 		
// 		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
// 		g_pkScript->GetArgString(pkLua, 1,strSkill);
// 		
// 		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
// 		{
// 			pkCP->SetDefaultAttackSkill(strSkill);
// 		}
// 	
// 		return 0;				
// 	}	

/**	\fn GiveXp( EntityID, iXp )
		\brief Sets the faction that the character will be a member of.
		\relates CharacterProperty
*/
	int GiveXpLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
		{
			cout<<"WARNING: GiveXp - wrong number of arguments"<<endl;
			return 0;		
		}
					
		int iCharcterID;
		int iFaction;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgInt(pkLua, 1, &iFaction);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			pkCP->GiveGroupExperience(iFaction);
		}
	
		return 0;				
	}

/**	\fn SetFaction( EntityID, iFaction )
		\brief Sets the faction that the character will be a member of.
		\relates CharacterProperty
*/
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
	

/**	\fn ChangeStat( EntityID, strStat, dValue )
		\brief Unkown
		\relates CharacterProperty
*/
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

		//cout<<"blub "<<strStat<<" "<<dValue<<endl;
		
				
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			pkCP->m_kCharacterStats.ChangeStat(strStat,float(dValue));		
		}
	
		return 0;				
	}
		
/**	\fn ChangeStatMod( EntityID, strStat, dValue )
		\brief Unkown
		\relates CharacterProperty
*/
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
	
/**	\fn SetStat( EntityID, strStat, dValue )
		\brief Set the stat of a character to the choosen value.
		\relates CharacterProperty
*/
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
	
	
	int GetTotalStatLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
		{
			cout<<"WARNING: SetStat - wrong number of arguments"<<endl;
			return 0;		
		}
			
		int iCharcterID;
		string strStat;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strStat);
		
		
		double dRet = 0;
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			dRet = pkCP->m_kCharacterStats.GetTotal(strStat);		
		}
	
		g_pkScript->AddReturnValue(pkLua, dRet);
	
		return 1;				
	}		
	
/**	\fn AddBuff( EntityID, strBuff )
		\brief Unkown
		\relates CharacterProperty
*/
	//buffs
	int AddBuffLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		

		
		int iCharcterID;
		double dBuffID;
		string strBuff;
		P_Buff* pkBuff = 0;

		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,strBuff);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			pkBuff = pkCP->AddBuff(strBuff);		

		if (pkBuff)
			dBuffID = pkBuff->GetEntity()->GetEntityID();
		else 
			dBuffID = 0;
	
		g_pkScript->AddReturnValue(pkLua, dBuffID);

		return 1;			
	}

/**	\fn RemoveBuff( EntityID, strBuff )
		\brief Unkown
		\relates CharacterProperty
*/
	int RemoveBuffLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		

			
		int iCharacterID;
		string strBuff;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharacterID);
		g_pkScript->GetArgString(pkLua, 1,strBuff);
		
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharacterID,"P_CharacterProperty"))
		{
			pkCP->RemoveBuff(strBuff);		
		}

		return 0;			
	}
		
	
/**	\fn PickupItem( ??? )
		\brief Unkown
		\relates CharacterProperty
*/
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
	
	int HaveEqipedBaseTypeLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,2))
			return 0;		
	
		int iCharcterID;
		string strTypeName;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);		
		g_pkScript->GetArgString(pkLua, 1,strTypeName);
		
		int iRet = 0;
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			if(pkCP->HaveEqipedBaseType(strTypeName))
				iRet = 1;		
		}
		
		g_pkScript->AddReturnValue(pkLua, iRet);							
		return 1;			
	}
	
/**	\fn HaveItem( ??? )
		\brief Unkown
		\relates CharacterProperty
*/
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
	
	int IsPcLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,1))
			return 0;		
		
		int iCharcterID;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		
		int iRet = 0;	
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			if(pkCP->GetIsPlayerCharacter())
				iRet = 1;
			
		g_pkScript->AddReturnValue(pkLua, double(iRet) );							
		return 1;		
	}		
	
	int IsEnemyLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,2))
			return 0;		
		
		int iCharcterID;
		int iTargetID;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgInt(pkLua, 1, &iTargetID);
		
		int iRet = 0;	
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			if(pkCP->IsEnemy(iTargetID))
				iRet = 1;
			
		g_pkScript->AddReturnValue(pkLua, double(iRet) );							
		return 1;		
	}		
	
	int IsFriendLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,2))
			return 0;		
		
		int iCharcterID;
		int iTargetID;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgInt(pkLua, 1, &iTargetID);
		
		int iRet = 0;	
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			if(pkCP->IsFriend(iTargetID))
				iRet = 1;
		}
		else
			return 0;
			
		g_pkScript->AddReturnValue(pkLua, double(iRet) );							
		return 1;		
	}			
	
	int IsNeutralLua(lua_State* pkLua)
	{
		if(!g_pkScript->VerifyArg(pkLua,1))
			return 0;		
		
		int iCharcterID;
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		
		int iRet = 0;	
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			if(pkCP->IsNeutral())
				iRet = 1;
			
		g_pkScript->AddReturnValue(pkLua, double(iRet) );							
		return 1;		
	}		
}




Property* Create_P_CharacterProperty()
{
	return new P_CharacterProperty;
}


void Register_P_CharacterProperty(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_CharacterProperty", Create_P_CharacterProperty);					

	// Register Property Script Interface
	
	//item handling
	g_pkScript->ExposeFunction("PickupItem",		SI_P_CharacterProperty::PickupItemLua);
	g_pkScript->ExposeFunction("HaveItem",			SI_P_CharacterProperty::HaveItemLua);
	g_pkScript->ExposeFunction("HaveEqipedBaseType",			SI_P_CharacterProperty::HaveEqipedBaseTypeLua);	
	g_pkScript->ExposeFunction("GetCharacterContainerID",		SI_P_CharacterProperty::GetCharacterContainerIDLua);

	
	//stats
	g_pkScript->ExposeFunction("SetStat",			SI_P_CharacterProperty::SetStatLua);
	g_pkScript->ExposeFunction("GetTotalStat",	SI_P_CharacterProperty::GetTotalStatLua);
	g_pkScript->ExposeFunction("ChangeStat",		SI_P_CharacterProperty::ChangeStatLua);
	g_pkScript->ExposeFunction("ChangeStatMod",	SI_P_CharacterProperty::ChangeStatModLua);
	
	// life
	g_pkScript->ExposeFunction("MakeAlive",	SI_P_CharacterProperty::MakeAliveLua);

	//buffs
	g_pkScript->ExposeFunction("AddBuff",			SI_P_CharacterProperty::AddBuffLua);
	g_pkScript->ExposeFunction("DeleteBuff",		SI_P_CharacterProperty::RemoveBuffLua);
	
	//skills
	g_pkScript->ExposeFunction("AddSkill",			SI_P_CharacterProperty::AddSkillLua);
	g_pkScript->ExposeFunction("AddAllSkills",	SI_P_CharacterProperty::AddAllSkillsLua);
	g_pkScript->ExposeFunction("ChangeSkill",		SI_P_CharacterProperty::ChangeSkillLua);
	g_pkScript->ExposeFunction("LockSkillUsage",	SI_P_CharacterProperty::LockSkillUsageLua);
	
// 	g_pkScript->ExposeFunction("SetupSkill",		SI_P_CharacterProperty::SetupSkillLua);
// 	g_pkScript->ExposeFunction("Needs",		SI_P_CharacterProperty::SetupSkillLua);
	
	//faction
	g_pkScript->ExposeFunction("SetFaction",		SI_P_CharacterProperty::SetFactionLua);
	
	//basic character handling
	g_pkScript->ExposeFunction("IsPc",				SI_P_CharacterProperty::IsPcLua);
	g_pkScript->ExposeFunction("IsEnemy",			SI_P_CharacterProperty::IsEnemyLua);
	g_pkScript->ExposeFunction("IsFriend",			SI_P_CharacterProperty::IsFriendLua);
	g_pkScript->ExposeFunction("IsNeutral",		SI_P_CharacterProperty::IsNeutralLua);
	
	//combat
// 	g_pkScript->ExposeFunction("SetDefaultAttackSkill",	SI_P_CharacterProperty::SetDefaultAttackSkillLua);
	g_pkScript->ExposeFunction("SetCombatMode",				SI_P_CharacterProperty::SetCombatModeLua);
	g_pkScript->ExposeFunction("GiveXp",						SI_P_CharacterProperty::GiveXpLua);

	// Register Constants.
		// Skill Types
		g_pkScript->RegisterConstant("SKILLTYPE_MISC", eMISC);
		g_pkScript->RegisterConstant("SKILLTYPE_OFFENSIVE", eOFFENSIVE);
		g_pkScript->RegisterConstant("SKILLTYPE_DEFENSIVE", eDEFENSIVE);
		g_pkScript->RegisterConstant("SKILLTYPE_TRADE", eTRADE);

		// Skill Targets
		g_pkScript->RegisterConstant("SKILLTARGET_PASSIVE", ePASSIVE);
		g_pkScript->RegisterConstant("SKILLTARGET_CHARACTER",eCHARACTER_TARGET);
		g_pkScript->RegisterConstant("SKILLTARGET_GROUND",eGROUND_TARGET);
		g_pkScript->RegisterConstant("SKILLTARGET_DIR",eDIRECTION);
		g_pkScript->RegisterConstant("SKILLTARGET_ITEM",eITEM_TARGET);
		g_pkScript->RegisterConstant("SKILLTARGET_SELF",eSELF);
}





// ------------ GRAVEYARD ---------------

// 		case 1:
// 		{
// 			pkPackage->Read_Str(m_strName);	
// 			pkPackage->Read_Str(m_strOwnedByPlayer);	
// 			pkPackage->Read(m_bIsPlayerCharacter); 
// 		
// 			break;		
// 		}
// 		
// 		case 2:
// 		{
// 			pkPackage->Read_Str(m_strName);	
// 			pkPackage->Read_Str(m_strOwnedByPlayer);	
// 			pkPackage->Read(m_bIsPlayerCharacter); 		
// 		
// 			m_kCharacterStats.Load(pkPackage);
// 			break;
// 		}
// 	
// 		case 3:
// 		{
// 			pkPackage->Read_Str(m_strName);	
// 			pkPackage->Read_Str(m_strOwnedByPlayer);	
// 			pkPackage->Read(m_bIsPlayerCharacter); 		
// 			pkPackage->Read(m_iFaction); 		
// 		
// 			m_kCharacterStats.Load(pkPackage);
// 			break;
// 		}		
// 		
// 		case 4:
// 		{
// 			pkPackage->Read_Str(m_strName);	
// 			pkPackage->Read_Str(m_strOwnedByPlayer);	
// 			pkPackage->Read(m_bIsPlayerCharacter); 		
// 			pkPackage->Read(m_iFaction); 		
// 			pkPackage->Read(m_bWalkSound); 		
// 		
// 			m_kCharacterStats.Load(pkPackage);
// 			break;
// 		}		
// 		
// 		case 5:
// 		{
// 			pkPackage->Read_Str(m_strName);	
// 			pkPackage->Read_Str(m_strOwnedByPlayer);	
// 			pkPackage->Read(m_bIsPlayerCharacter); 		
// 			pkPackage->Read(m_iFaction); 		
// 			pkPackage->Read(m_bWalkSound); 		
// 			pkPackage->Read(m_fMarkerSize); 
// 			
// 			m_kCharacterStats.Load(pkPackage);
// 			break;
// 		}			
// 		
// 		
// 		case 6:
// 		{
// 			pkPackage->Read_Str(m_strName);	
// 			pkPackage->Read_Str(m_strOwnedByPlayer);	
// 			pkPackage->Read(m_bIsPlayerCharacter); 		
// 			pkPackage->Read(m_iFaction); 		
// 			pkPackage->Read(m_bWalkSound); 		
// 			pkPackage->Read(m_fMarkerSize); 
// 			
// 			m_kCharacterStats.Load(pkPackage);
// 			
// 			//load skills
// 			RemoveAllSkills();
// 			
// 			int		iNrOfSkills;
// 			string	strSkill;
// 			string	strParent;
// 			int 		iLevel;
// 			
// 			pkPackage->Read(iNrOfSkills);			
// 			for(int i=0;i<iNrOfSkills;i++)
// 			{
// 				pkPackage->Read_Str(strSkill);
// 				pkPackage->Read_Str(strParent);
// 				pkPackage->Read(iLevel);
// 				
// 				AddSkill(strSkill,strParent);
// 				SetSkill(strSkill,iLevel);
// 			}
// 			
// 			break;
// 		}			













// vector<PropertyValues> P_CharacterProperty::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn(5);
// 
// 	kReturn[0].kValueName = "walksound";
// 	kReturn[0].iValueType = VALUETYPE_BOOL; 
// 	kReturn[0].pkValue    = (void*)&m_bWalkSound;	
// 
// 	kReturn[1].kValueName = "markersize";
// 	kReturn[1].iValueType = VALUETYPE_FLOAT; 
// 	kReturn[1].pkValue    = (void*)&m_fMarkerSize;	
// 
// 	kReturn[2].kValueName = "name";
// 	kReturn[2].iValueType = VALUETYPE_STRING; 
// 	kReturn[2].pkValue    = (void*)&m_strName;		
// 	
// 	kReturn[3].kValueName = "faction";
// 	kReturn[3].iValueType = VALUETYPE_INT; 
// 	kReturn[3].pkValue    = (void*)&m_iFaction;		
// 			
// 	kReturn[4].kValueName = "respawntime";
// 	kReturn[4].iValueType = VALUETYPE_FLOAT; 
// 	kReturn[4].pkValue    = (void*)&m_fRespawnTime;		
// 	
// 	
// 	return kReturn;	
// }