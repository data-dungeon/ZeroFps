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
	m_paTechTree = new vector<Structure>(iNumTeams);
}

ConstructionManager::~ConstructionManager()
{
	delete[] m_paTechTree;
}

void ConstructionManager::GetPossibleBuildings(unsigned char iTeam, 
											   vector<Structure>& akStructures)
{

}
