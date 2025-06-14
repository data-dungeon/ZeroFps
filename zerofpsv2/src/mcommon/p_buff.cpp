#include "p_buff.h"
#include "p_characterproperty.h"


P_Buff::P_Buff() : Property("P_Buff")
{
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	m_bNetwork = false;
	m_iVersion = 1;	
	
	m_pkCharacter = NULL;
	
	
	
	m_strName	= "Unkown";
	m_strIcon	= "default";
	m_fTimeOut	= -1;				//buff total time, if no timout -1
	m_fTimeLeft = -1;				//remainng buff time
	m_cType		= 0;
	m_bShow		= true;	
	m_fLastTime = 0;				//ticks on last update



	m_kPropertyValues.push_back(PropertyValues("name",VALUETYPE_STRING,(void*)&m_strName));
	m_kPropertyValues.push_back(PropertyValues("icon",VALUETYPE_STRING,(void*)&m_strIcon));
	m_kPropertyValues.push_back(PropertyValues("type",VALUETYPE_INT,(void*)&m_cType));
	m_kPropertyValues.push_back(PropertyValues("show",VALUETYPE_BOOL,(void*)&m_bShow));
	m_kPropertyValues.push_back(PropertyValues("timeout",VALUETYPE_FLOAT,(void*)&m_fTimeOut));
	

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
			//decrese timeout
			m_fTimeLeft -= m_pkZeroFps->GetEngineTime() - m_fLastTime;
			m_fLastTime = m_pkZeroFps->GetEngineTime();
		
			//time has expired,remove buff
			if(m_fTimeLeft <= 0)
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
	
	m_fLastTime = m_pkZeroFps->GetEngineTime();
	m_fTimeLeft = m_fTimeOut;
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

void Register_P_Buff(ZSSZeroFps* pkZeroFps)
{
	pkZeroFps->m_pkPropertyFactory->Register("P_Buff", Create_P_Buff);					
}







// vector<PropertyValues> P_Buff::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn(5);
// 	
// 	kReturn[0].kValueName = "name";
// 	kReturn[0].iValueType = VALUETYPE_STRING;
// 	kReturn[0].pkValue    = &m_strName;	
// 
// 	kReturn[1].kValueName = "icon";
// 	kReturn[1].iValueType = VALUETYPE_STRING;
// 	kReturn[1].pkValue    = &m_strIcon;	
// 	
// 	kReturn[2].kValueName = "type";
// 	kReturn[2].iValueType = VALUETYPE_INT;
// 	kReturn[2].pkValue    = &m_cType;	
// 
// 	kReturn[3].kValueName = "show";
// 	kReturn[3].iValueType = VALUETYPE_BOOL;
// 	kReturn[3].pkValue    = &m_bShow;	
// 
// 	kReturn[4].kValueName = "timeout";
// 	kReturn[4].iValueType = VALUETYPE_FLOAT;
// 	kReturn[4].pkValue    = &m_fTimeOut;		
// 				
// 	
// 	return kReturn;
// }