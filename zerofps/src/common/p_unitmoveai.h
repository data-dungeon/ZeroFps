// p_unitmoveai.h: interface for the p_unitmoveai class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P_UNITMOVEAI_H__A1B57C3C_7DAD_418C_9BB8_05BF11DD6271__INCLUDED_)
#define AFX_P_UNITMOVEAI_H__A1B57C3C_7DAD_418C_9BB8_05BF11DD6271__INCLUDED_

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

#include "p_serverunit.h"
#include "unitcommandinfo.h"

using namespace std;

#include "pathfind.h"
#include "pathbuilder.h"


class COMMON_API P_UnitMoveAI : public Property, public AIBase
{
private: 
	static HeightMap* m_pkMap;
	ZeroFps*				m_pkFps;
	PathFind* 			m_pkPathFind;
	Point 				m_kStartPoint, m_kEndPoint;
	Vector3				m_kStartPos,m_kEndPos;
	
	Vector3				m_kCurretDestination;	
	float					m_fSpeedMod;
	
	P_ServerUnit*		m_pkUnit;
	ExternalCommand*	m_pkMoveUnitCommand;
	int					m_iCurrentState;
	bool					m_bTemp;
	
	enum { 
		UNIT_MOVE, 
		TEMP 
	};



public:
	P_UnitMoveAI();
	bool RegisterExternalCommands();
	virtual ~P_UnitMoveAI();
	AIBase* UpdateAI();
	AIBase* RunUnitCommand(int iCommandID, int iXDestinaton, int iYDestinaton, int iTarget);
	void CloneOf(Property* pkProperty) { }
	void Init();
	vector<PropertyValues> GetPropertyValues();
	
	bool MoveTo(Vector3 kPos);
	bool DoPathFind(Vector3 kStart,Vector3 kStop);
};

COMMON_API Property* Create_P_UnitMoveAI();

#endif // !defined(AFX_P_UNITMOVEAI_H__A1B57C3C_7DAD_418C_9BB8_05BF11DD6271__INCLUDED_)



