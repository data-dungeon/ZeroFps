// constructionmanager.cpp: implementation of the ConstructionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "constructionmanager.h"
#include "../zerofps/basic/zfini.h"
#include "tileengine.h"

ConstructionManager* ConstructionManager::m_pkInstance = new ConstructionManager(4);	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConstructionManager::ConstructionManager(int iNumTeams)
	: c_iNumTeams(iNumTeams)
{
	m_cMaxTechLevel = -1;
}

ConstructionManager::~ConstructionManager()
{
//	delete[] m_paTeamTechTree;
}

int ConstructionManager::GetPossibleBuildings(unsigned int uiTechLevel, 
											  vector<Structure>& akStructures)
{
	// Fel inparameter
	if((unsigned char)m_cMaxTechLevel < uiTechLevel)
		return -1;

	for(unsigned int i=0; i<m_kAllStructures.size(); i++)
	{
		if(m_kAllStructures[i].ucTechLevel == uiTechLevel)
		{
			// Leta reda på rätt giver..
			unsigned char giver = m_kNameMap.find(m_kAllStructures[i].ucID)->second.ucParent;

			for(unsigned int j=0; j<m_kAllStructures.size(); j++)
			{
				if(m_kAllStructures[j].ucGiver == giver)
				{
					akStructures.push_back( m_kAllStructures[j] );
				}
			}

			break;
		}
	}

	return akStructures.size();
}

void ConstructionManager::Init(char* szTechTreeINIFile)
{
	ZFIni* pkIni=static_cast<ZFIni*>(g_ZFObjSys.GetObjectPtr("ZFIni"));	

	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkTileEngine = TileEngine::GetInstance();

	if(pkIni)
	{
		if(!pkIni->Open(szTechTreeINIFile, false))
		{
			printf("Failed to open file %s\n", szTechTreeINIFile);
			return;
		}

		vector<string> kSections;
		pkIni->GetSectionNames(kSections);
		
		for(unsigned int i=0; i<kSections.size(); i++)
		{
			Structure kNewStructure;
			strcpy(kNewStructure.szName, pkIni->GetValue(kSections[i].c_str(), "name"));
			kNewStructure.ucID = atoi(pkIni->GetValue(kSections[i].c_str(), "id"));
			kNewStructure.ucGiver = (unsigned char) atoi(pkIni->GetValue(kSections[i].c_str(), "giver"));
			kNewStructure.ucTechLevel = (unsigned char) atoi(pkIni->GetValue(kSections[i].c_str(), "level"));
			kNewStructure.ucParent = (unsigned char) atoi(pkIni->GetValue(kSections[i].c_str(), "parent"));
			kNewStructure.iCost = atoi(pkIni->GetValue(kSections[i].c_str(), "cost"));
			
			if(strcmp(pkIni->GetValue(kSections[i].c_str(), "type"), "building") == 0)
				kNewStructure.ucType = 0;
			else
			if(strcmp(pkIni->GetValue(kSections[i].c_str(), "type"), "unit") == 0)
				kNewStructure.ucType = 1;
			else
			if(strcmp(pkIni->GetValue(kSections[i].c_str(), "type"), "weapon") == 0)
				kNewStructure.ucType = 2;
			else
			{
				kNewStructure.ucType = 255;
				printf("SOMETHING IS TERRIBLE WRONG!!\n\n\n\n\n\n\n");
			}
			
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

			m_kNameMap.insert( map<int, Structure>::value_type(
				kNewStructure.ucID, kNewStructure ) );

			if(kNewStructure.ucTechLevel > m_cMaxTechLevel)
				m_cMaxTechLevel = kNewStructure.ucTechLevel ;

			m_kAllStructures.push_back(kNewStructure);
		}
	}
}

void ConstructionManager::PrintAll()
{
	printf("--ConstructionManager::PrintAll---------------------------------\n");
	for(unsigned int i=0; i<m_kAllStructures.size(); i++)
	{
		printf("Name: %s\nID: %i\nGiver: %i\nTechLevel: %i\nParent: %i\nCost: %i\n",
			m_kAllStructures[i].szName, (int)m_kAllStructures[i].ucID,
			(int)m_kAllStructures[i].ucGiver, (int)m_kAllStructures[i].ucTechLevel, 
			(int)m_kAllStructures[i].ucParent, (int)m_kAllStructures[i].iCost);

		for(unsigned int j=0; j<m_kAllStructures[i].akUpgrades.size(); j++)
		{
			printf("Upgrade %i = %s\n", j, 
				m_kNameMap.find(
					m_kAllStructures[i].akUpgrades[j])->second.szName);
		}

		printf("\n");
	}
}

void ConstructionManager::PrintUpgrades(int iTechLevel)
{
	vector<Structure> kStructs;
	GetPossibleBuildings(iTechLevel, kStructs);

	printf("--ConstructionManager::PrintUpgrades---------------------------------\n");
	for(unsigned int k=0; k<kStructs.size(); k++)
		printf("%s\n", kStructs[k].szName);

}

bool ConstructionManager::Build(char *szStructureName, Point kSquare)
{
	const float c_fFailBuildAngle = 20.0f;

	Object* pkNewObject = m_pkObjectMan->CreateObjectByArchType(szStructureName);
	
	if(pkNewObject)
	{
		Vector3 pos = m_pkTileEngine->GetPosFromSqr(kSquare);

		P_ServerUnit* pkServerUnit = (P_ServerUnit*) pkNewObject->GetProperty("P_ServerUnit");
		if(pkServerUnit)
		{
			int width = pkServerUnit->m_kInfo.m_Info2.m_cWidth;
			int height = pkServerUnit->m_kInfo.m_Info2.m_cHeight;

			// Kolla först i tile så att rutorna inte är upptagna.
			bool bOK = true;
			int x_min = kSquare.x-width/2;
			int y_min = kSquare.y-height/2;
			int x_max = kSquare.x+width/2;
			int y_max = kSquare.y+height/2;

			int x,y;
			for(y=y_min; y<y_max; y++)
				for(x=x_min; x<x_max; x++)
				{
					if(m_pkTileEngine->GetTile(x,y)->kUnits.size())
					{
						bOK = false;
						break;
					}

					if(m_pkTileEngine->GetTile(x,y)->fAngle > c_fFailBuildAngle)
					{
						bOK = false;
						break;
					}
				}

			if(bOK == false)
			{
				pkServerUnit->Damage(9999);
				printf("Failed to build stucture\n");
				return false;
			}
		}



		pkNewObject->AttachToClosestZone();
		pkNewObject->SetPos(pos);
		pkNewObject->SetPos(pos); // måste vara 2 anrop til setpos annars fuckar det upp sig.
		return true;
	}

	return false;
}
