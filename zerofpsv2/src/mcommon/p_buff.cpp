#include "p_buff.h"
#include "p_characterproperty.h"


P_Buff::P_Buff()
{
	strcpy(m_acName,"P_Buff");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_bNetwork = false;
	m_iVersion = 1;	
	
	m_pkCharacter = NULL;
	
	
	
	m_strName	= "Unkown";
	m_strIcon	= "default";
	m_fTimeOut	= -1;
	m_cType		= 0;
	m_bShow		= true;
}

P_Buff::~P_Buff()
{
	Disable();
}

void P_Buff::Init()
{
	//dont save buff entitys
	GetEntity()->SetSave(false);

}


void P_Buff::Update()
{
	//dont do anything if character is already set
	if(m_pkCharacter)
		return;
	
	//check if parent or parent parent is a character
	if(P_CharacterProperty* pkCP = (P_CharacterProperty*)GetEntity()->GetProperty("P_CharacterProperty"))
	{		
		cout<<"parent is a character, applying buff"<<endl;		
		
		Enable(pkCP);		
		return;
	}

	
	if(Entity* pkGrandParent = GetEntity()->GetParent())
	{
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkGrandParent->GetProperty("P_CharacterProperty"))
		{
			cout<<"grandparent is a character , applying buff"<<endl;
			Enable(pkCP);
			return;
		}
	}
}


void P_Buff::Enable(P_CharacterProperty* pkCP)
{
	m_pkCharacter = pkCP;
	
	
	vector<ScriptFuncArg> kParams;
	int iID = m_pkCharacter->GetEntity()->GetEntityID();
	kParams.push_back(ScriptFuncArg( &iID,tINT));	
	
	m_pkEntityManager->CallFunction(GetEntity(),"AddBuff",&kParams);
}

void P_Buff::Disable()
{
	if(m_pkCharacter)
	{	
		vector<ScriptFuncArg> kParams;
		
		int iID = m_pkCharacter->GetEntity()->GetEntityID();
		kParams.push_back(ScriptFuncArg( &iID,tINT));	
		
		m_pkEntityManager->CallFunction(GetEntity(),"RemoveBuff",&kParams);				
	
		
		m_pkCharacter = NULL;	
	}
	
}


Property* Create_P_Buff()
{
	return new P_Buff;
}

void Register_P_Buff(ZeroFps* pkZeroFps)
{
	pkZeroFps->m_pkPropertyFactory->Register("P_Buff", Create_P_Buff);					
}