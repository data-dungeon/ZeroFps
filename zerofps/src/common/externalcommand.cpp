// externalcommand.cpp: implementation of the externalcommand class.
//
//////////////////////////////////////////////////////////////////////

#include "externalcommand.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ExternalCommand::ExternalCommand(AIBase* pkAIOwner, int iCommandID) : 
m_pkOwner(pkAIOwner), m_iCommandID(iCommandID) 
{

}

ExternalCommand::~ExternalCommand()
{

}

AIBase* ExternalCommand::RunUnitCommand(int iXDestinaton, int iYDestinaton, int iTarget)
{
	return m_pkOwner->RunUnitCommand(m_iCommandID, iXDestinaton, iYDestinaton, iTarget);
}