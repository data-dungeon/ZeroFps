// constructionmanager.h: interface for the ConstructionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTRUCTIONMANAGER_H__7CB50AE5_958B_4A02_AA3A_E2F7CD829953__INCLUDED_)
#define AFX_CONSTRUCTIONMANAGER_H__7CB50AE5_958B_4A02_AA3A_E2F7CD829953__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

struct StructureUpgrade
{
	char szName[64];			// vad uppgraderingen kallas
	float fCostMultipel;		// ökningen i procent jämfört med förra byggnanden
	float fArmorMultipel;		// ökningen i procent jämfört med förra byggnanden
	float fHitPointMultipel;	// ökningen i procent jämfört med förra byggnanden
	
	int iResarchCost;			// kostnad för att forska fram byggnaden
};

struct Structure
{
	char szName[64];	// vad den heter
	vector<StructureUpgrade*> akUpgrades;
	int iCost;			// vad den kostar att bygga
	int iArmor;			// vad den skyddar
	int iHitPoints;		// vad den tål innan den rasar
};

class ConstructionManager  
{
public:
	ConstructionManager(int iNumTeams);
	virtual ~ConstructionManager();

	void GetPossibleBuildings(unsigned char iTeam, vector<Structure>& akStructures);

	const int c_iNumTeams;

private:
	vector<Structure> m_kAllStructures;
	vector<Structure> *m_paTechTree;
};

#endif // !defined(AFX_CONSTRUCTIONMANAGER_H__7CB50AE5_958B_4A02_AA3A_E2F7CD829953__INCLUDED_)
