#include "../zerofpsv2/engine_systems/propertys/p_scriptinterface.h"
#include "../zerofpsv2/engine/entity.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "p_dmclickme.h"
#include <iostream>
   using namespace std;

// ------------------------------------------------------------------------------------------

vector<PropertyValues> P_DMClickMe::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	return kReturn;
}

// ------------------------------------------------------------------------------------------

void P_DMClickMe::Init()
{
	m_pkScript = (P_ScriptInterface*)m_pkObject->GetProperty("P_ScriptInterface");

}

// ------------------------------------------------------------------------------------------

P_DMClickMe::P_DMClickMe()
{
	strcpy(m_acName,"P_DMClickMe");

	bNetwork = true;
}

// ------------------------------------------------------------------------------------------

void P_DMClickMe::Save(ZFIoInterface* pkPackage)
{
}

// ------------------------------------------------------------------------------------------

void P_DMClickMe::Load(ZFIoInterface* pkPackage)
{
}

// ------------------------------------------------------------------------------------------

void P_DMClickMe::Click()
{
	if ( m_pkScript == 0 )
		m_pkScript = (P_ScriptInterface*)m_pkObject->GetProperty("P_ScriptInterface");

	if ( m_pkScript == 0 )
	{
		cout << "Error! P_DMClickMe::Click: Object has P_DMClickMe but not P_ScriptInterface." << endl;
		return;
	}

	((P_ScriptInterface*)m_pkScript)->CallFunction ("Click");
}

// ------------------------------------------------------------------------------------------

P_DMClickMe::~P_DMClickMe()
{
}

// ------------------------------------------------------------------------------------------

Property* Create_P_DMClickMe()
{
	return new P_DMClickMe;
}



