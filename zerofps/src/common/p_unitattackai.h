// p_unitattackai.h: interface for the p_unitattackai class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P_UNITATTACKAI_H__C50CB863_E4AC_4E2F_AB86_7CB1E33AEAF9__INCLUDED_)
#define AFX_P_UNITATTACKAI_H__C50CB863_E4AC_4E2F_AB86_7CB1E33AEAF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"
#include "p_serverunit.h"
#include "externalcommand.h"
#include "aibase.h"
#include "p_unitsystem.h"

#include "p_serverunit.h"
#include "unitcommandinfo.h"

using namespace std;

class COMMON_API P_UnitAttackAI : public Property, public AIBase
{
public:
	Vector3 m_kTargetPrevPos;
	bool RegisterExternalCommands();
	void Init();
	AIBase* UpdateAI();
	AIBase* RunUnitCommand(int iCommandID, int iXDestinaton, int iYDestinaton, int iTarget);
	void CloneOf(Property* pkProperty) { }
	P_UnitAttackAI();
	virtual ~P_UnitAttackAI();
private:
	HeightMap*			m_pkMap;
	P_ServerUnit*		m_pkUnit;
	P_UnitSystem*		m_pkUnitSystem;
	ZeroFps*				m_pkFps;	
	ExternalCommand*	m_pkAttackCommand;
	
	int m_iCurrentState;
	Object* m_pkTargetObject;
	int m_iTargetID;
	float m_fRange;
	float m_fDistance;
	float m_fLastFireTime;
	AIBase* m_pkAi;
	enum 
	{ UNIT_ATTACK, TEMP };

};
COMMON_API Property* Create_P_UnitAttackAI();

#endif // !defined(AFX_P_UNITATTACKAI_H__C50CB863_E4AC_4E2F_AB86_7CB1E33AEAF9__INCLUDED_)
