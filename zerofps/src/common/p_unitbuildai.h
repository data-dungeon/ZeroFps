// p_unitbuildai.h: interface for the p_unitbuildai class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_P_UNITBUILDAI_H__785E2C51_8B9E_4F78_B019_C8C1EAA500EF__INCLUDED_)
#define AFX_P_UNITBUILDAI_H__785E2C51_8B9E_4F78_B019_C8C1EAA500EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <vector>

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include "common_x.h"
#include "aibase.h"
#include "externalcommand.h"
#include "p_serverunit.h"
#include "constructionmanager.h"

using namespace std;

class COMMON_API P_UnitBuildAI  : public Property, public AIBase
{
public:
	P_UnitBuildAI();
	virtual ~P_UnitBuildAI();
	bool RegisterExternalCommands();
	void Init();
	void CloneOf(Property* pkProperty) { };
	AIBase* UpdateAI();
	AIBase* RunUnitCommand(int iCommandID, int iXDestinaton, int iYDestinaton, int iTarget);
private:
	P_ServerUnit* m_pkUnit;
	vector<PropertyValues> GetPropertyValues();
	vector<Structure> m_kStructures;
	vector<ExternalCommand*> m_kExternalCommands;
	int m_uiTechLevel;
	unsigned int m_uiCommandID;
//	unsigned int m_uiNrOfBuildings;
	void Save(ZFMemPackage* pkPackage);
	void Load(ZFMemPackage* pkPackage);
};
	COMMON_API Property* Create_P_UnitBuildAI();
#endif // !defined(AFX_P_UNITBUILDAI_H__785E2C51_8B9E_4F78_B019_C8C1EAA500EF__INCLUDED_)
