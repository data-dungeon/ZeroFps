// constructionmanager.h: interface for the ConstructionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTRUCTIONMANAGER_H__7CB50AE5_958B_4A02_AA3A_E2F7CD829953__INCLUDED_)
#define AFX_CONSTRUCTIONMANAGER_H__7CB50AE5_958B_4A02_AA3A_E2F7CD829953__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include "common_x.h"
using namespace std;

struct Structure
{
	char szName[64];					// vad byggnaden heter
	vector<int> akUpgrades;				// vektor med id nummer på möjliga byggnader 
										// som går att bygga härnäst.
	int iCost;							// vad byggnaden kostar att konstruera
	unsigned char ucID;					// vilket id byggnaden har
	unsigned char ucGiver;				// vilken struktur som producerad denna struktur
	unsigned char ucTechLevel;			// vilken teknisk nivå denna ligger på.
};

class COMMON_API ConstructionManager  
{
public:
	void PrintAll();
	ConstructionManager(int iNumTeams);
	virtual ~ConstructionManager();

	// Funktion som returnerar en ny tech level.
	int GetPossibleBuildings(unsigned int uiTechLevel, vector<Structure>& akStructures);
	void Init();

	const int c_iNumTeams;

	static ConstructionManager*	m_pkInstance;
	static ConstructionManager* GetInstance() { return m_pkInstance; }

private:
	vector<Structure> m_kAllStructures;
	vector<Structure> *m_paTeamTechTree;
	map<int, string> m_kNameMap;
};

#endif // !defined(AFX_CONSTRUCTIONMANAGER_H__7CB50AE5_958B_4A02_AA3A_E2F7CD829953__INCLUDED_)
