// constructionmanager.cpp: implementation of the ConstructionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "constructionmanager.h"
#include "../zerofps/basic/zfini.h"

ConstructionManager* ConstructionManager::m_pkInstance = new ConstructionManager(4);	

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

void ConstructionManager::Init()
{
	ZFIni* pkIni=static_cast<ZFIni*>(g_ZFObjSys.GetObjectPtr("ZFIni"));	
	
	if(pkIni)
	{
		const char szFileName[] = "techtree.txt";

		if(!pkIni->Open(szFileName, false))
		{
			printf("Failed to open file %s\n", szFileName);
			return;
		}

		vector<string> kSections;
		pkIni->GetSectionNames(kSections);

		printf("SIZE = %i\n\n\n\n", kSections.size());
		
		for(unsigned int i=0; i<kSections.size(); i++)
		{
			Structure kNewStructure;
			strcpy(kNewStructure.szName, pkIni->GetValue(kSections[i].c_str(), "name"));
			kNewStructure.ucID = atoi(pkIni->GetValue(kSections[i].c_str(), "id"));
			kNewStructure.ucGiver = (unsigned char) atoi(pkIni->GetValue(kSections[i].c_str(), "giver"));
			kNewStructure.ucTechLevel = (unsigned char) atoi(pkIni->GetValue(kSections[i].c_str(), "level"));
			kNewStructure.iCost = atoi(pkIni->GetValue(kSections[i].c_str(), "cost"));

			unsigned char structur_id = kNewStructure.ucID;
			for(unsigned int j=0; j<kSections.size(); j++)
			{
				if( i == j )
					continue;

				// Resistrera möjliga uppgraderingar.
				if(atoi(pkIni->GetValue(kSections[j].c_str(), "giver")) == structur_id)
					kNewStructure.akUpgrades.push_back( 
						atoi(pkIni->GetValue(kSections[j].c_str(), "id")) );
			}

			m_kNameMap.insert( map<int, string>::value_type(
				kNewStructure.ucID, kNewStructure.szName ) );

			m_kAllStructures.push_back(kNewStructure);
		}
	}
}

void ConstructionManager::PrintAll()
{
	printf("--ConstructionManager::PrintAll---------------------------------\n");
	for(int i=0; i<m_kAllStructures.size(); i++)
	{
		printf("Name: %s\nID: %i\nGiver: %i\nTechLevel: %i\nCost: %i\n",
			m_kAllStructures[i].szName, (int)m_kAllStructures[i].ucID,
			(int)m_kAllStructures[i].ucGiver, (int)m_kAllStructures[i].ucTechLevel, 
			(int)m_kAllStructures[i].iCost);

		for(int j=0; j<m_kAllStructures[i].akUpgrades.size(); j++)
		{
			printf("Upgrade %i = %s\n", j, 
				m_kNameMap.find(m_kAllStructures[i].akUpgrades[j])->second.c_str());
		}

		printf("\n");
	}
}
