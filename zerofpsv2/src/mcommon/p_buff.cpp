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
	
	
	m_fAddTime	= 0;
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
	//have character
	if(m_pkCharacter)
	{
		if(m_fTimeOut != -1)
		{
			if(m_pkZeroFps->GetTicks() > m_fAddTime + m_fTimeOut)
			{
				m_pkCharacter->RemoveBuff(this);
			}
		}
	}
	else
	{
		
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
}


void P_Buff::Enable(P_CharacterProperty* pkCP)
{
	m_pkCharacter = pkCP;
	
	
	vector<ScriptFuncArg> kParams;
	int iID = m_pkCharacter->GetEntity()->GetEntityID();
	kParams.push_back(ScriptFuncArg( &iID,tINT));	
	
	m_pkEntityManager->CallFunction(GetEntity(),"AddBuff",&kParams);
	
	m_fAddTime = m_pkZeroFps->GetTicks();
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

vector<PropertyValues> P_Buff::GetPropertyValues()
{
	vector<PropertyValues> kReturn(5);
	
	kReturn[0].kValueName = "name";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = &m_strName;	

	kReturn[1].kValueName = "icon";
	kReturn[1].iValueType = VALUETYPE_STRING;
	kReturn[1].pkValue    = &m_strIcon;	
	
	kReturn[2].kValueName = "type";
	kReturn[2].iValueType = VALUETYPE_INT;
	kReturn[2].pkValue    = &m_cType;	

	kReturn[3].kValueName = "show";
	kReturn[3].iValueType = VALUETYPE_BOOL;
	kReturn[3].pkValue    = &m_bShow;	

	kReturn[4].kValueName = "timeout";
	kReturn[4].iValueType = VALUETYPE_FLOAT;
	kReturn[4].pkValue    = &m_fTimeOut;		
				
	
	return kReturn;
}



Property* Create_P_Buff()
{
	return new P_Buff;
}

void Register_P_Buff(ZeroFps* pkZeroFps)
{
	pkZeroFps->m_pkPropertyFactory->Register("P_Buff", Create_P_Buff);					
}