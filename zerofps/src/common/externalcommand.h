// externalcommand.h: interface for the externalcommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTERNALCOMMAND_H__AC3B92E3_7ACA_4D37_B095_8AF2AFCD98FB__INCLUDED_)
#define AFX_EXTERNALCOMMAND_H__AC3B92E3_7ACA_4D37_B095_8AF2AFCD98FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "unitcommandinfo.h"  
#include "aibase.h"

class ExternalCommand  
{
public:
	ExternalCommand(AIBase* pkAIOwner, int iCommandID);
	virtual ~ExternalCommand();
	//virtual operator()(int iDestinaton)
	UnitCommandInfo m_kUnitCommandInfo;
	AIBase* RunUnitCommand(int iXDestinaton, int iYDestinaton, int iTarget);
private:
	AIBase* m_pkOwner;
	int m_iCommandID;
};

#endif // !defined(AFX_EXTERNALCOMMAND_H__AC3B92E3_7ACA_4D37_B095_8AF2AFCD98FB__INCLUDED_)
