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

void P_DMClickMe::Update()
{
	for ( list<Visiter>::iterator kIte = m_kVisiters.begin(); kIte != m_kVisiters.end(); kIte++ )
	{
		(*kIte).m_fVisitTime -= m_pkObject->m_pkZeroFps->GetFrameTime();

		// visit time is over, get out of the house
		if ( (*kIte).m_fVisitTime <= 0 )
		{
			Entity* pkVisiter = m_pkObjMan->GetObjectByNetWorkID ( (*kIte).m_iVisiter );

			if ( pkVisiter == 0 )
			{
				cout << "Error! P_DMClickMe::Update: Visited doesn't exist anymore!" << endl;
				return;
			}
			else
			{
				pkVisiter->GetParent()->RemoveChild (pkVisiter);
				pkVisiter->SetUpdateStatus(UPDATE_ALL);
			}

			// remove from list
			//m_kVisiters.remove ( kIte );

		}
	}
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

void P_DMClickMe::Click(int iObjID)
{
	if ( m_pkScript == 0 )
		m_pkScript = (P_ScriptInterface*)m_pkObject->GetProperty("P_ScriptInterface");

	if ( m_pkScript == 0 )
	{
		cout << "Error! P_DMClickMe::Click: Object has P_DMClickMe but not P_ScriptInterface." << endl;
		return;
	}

	vector<ARG_DATA> kParams;
	
	int iValue = iObjID;
	
	ARG_DATA kData;
	kData.eType = tINT;
	kData.pData = &iValue;

	kParams.push_back (kData);

	((P_ScriptInterface*)m_pkScript)->CallFunction ( "Click", &kParams );
}

// ------------------------------------------------------------------------------------------

void P_DMClickMe::AddVisiter( int iObjID, float fVisitTime )
{
	Visiter kNewVisit = { iObjID, fVisitTime };

	m_kVisiters.push_back ( kNewVisit );
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



