// constructionmanager.cpp: implementation of the ConstructionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "constructionmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConstructionManager::ConstructionManager(int iNumTeams)
	: c_iNumTeams(iNumTeams)
{
	m_paTeamTechTree = new vector<Structure>(iNumTeams);
}

ConstructionManager::~ConstructionManager()
{
	delete[] m_paTeamTechTree;
}

int ConstructionManager::GetPossibleBuildings(unsigned int uiTechLevel, 
											  vector<Structure>& akStructures)
{
	return 0;
}
