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

class TileEngine;

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
	unsigned char ucParent;				// vilken byggnad i samma tech level som tar en till nästa nivå.
};

class COMMON_API ConstructionManager  
{
public:
	bool Build(char* szStructureName, Point kSquare);
	void PrintUpgrades(int iTechLevel);
	void PrintAll();
	ConstructionManager(int iNumTeams);
	virtual ~ConstructionManager();

	// Funktion som returnerar en ny tech level.
	int GetPossibleBuildings(unsigned int uiTechLevel, vector<Structure>& akStructures);
	void Init(char* szTechTreeINIFile);

	const int c_iNumTeams;

	static ConstructionManager*	m_pkInstance;
	static ConstructionManager* GetInstance() { return m_pkInstance; }

private:
	vector<Structure> m_kAllStructures;
	map<int, Structure> m_kNameMap;
	char m_cMaxTechLevel;

	ObjectManager* m_pkObjectMan;
	TileEngine* m_pkTileEngine;
};

#endif // !defined(AFX_CONSTRUCTIONMANAGER_H__7CB50AE5_958B_4A02_AA3A_E2F7CD829953__INCLUDED_)
