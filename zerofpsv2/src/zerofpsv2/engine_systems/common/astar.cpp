#include "astar.h"
#include "../propertys/p_pfmesh.h"

ZSSAStar::ZSSAStar()
: ZFSubSystem("ZSSAStar")
{
	// Set Default values
	//m_bDrawNaviMesh	= false;
	//m_bDrawPaths		= false;

	// Register Variables
	//RegisterVariable("ai_shownavmesh",	&m_bDrawNaviMesh,	CSYS_BOOL);	
	//RegisterVariable("ai_showpath",		&m_bDrawPaths,		CSYS_BOOL);	
	m_kbDrawNaviMesh.Register(this, "ai_shownavmesh", "0");
	m_kbDrawPaths.Register(this, "ai_showpath", "0");
}


bool ZSSAStar::StartUp()
{
	m_pkEntityManager	= static_cast<ZSSEntityManager*>(GetSystem().GetObjectPtr("ZSSEntityManager"));

	return true;
}


AStarCellNode* FindNodeInList( vector< AStarCellNode*>& List, NaviMeshCell* pkCell)
{
	for(unsigned int i=0; i<List.size(); i++) 
	{
		if(List[i]->pkNaviCell == pkCell)
			return List[i];
	}

	return NULL;
}


void ZSSAStar::MakePath(AStarCellNode* pkNode, vector<PathNode>& kPath)
{
	PathNode kNode;

	do 
	{
		//if(pkNode->pkNaviCell->m_bNonWalkable == false)
		//{
			kNode.kPosition	= pkNode->pkNaviCell->m_kCenter;
			kNode.pkStartMesh = pkNode->m_pkNaviMesh;
			kNode.pkStartCell = pkNode->pkNaviCell;
			kPath.push_back( kNode );
		//}

		pkNode = pkNode->m_pParent;
	} while(pkNode);
}


void ZSSAStar::CalcCoset(AStarCellNode* pkNode)
{
//	ZoneData* pkZone;
//	pkZone = m_pkEntityManagerger->GetZoneData(pkNode->m_iZoneID);
	
	pkNode->m_fGValue = pkNode->m_pParent->m_fFValue + 1;
	Vector3 kDist		= m_kStart - pkNode->pkNaviCell->m_kCenter;
	pkNode->m_fHValue = kDist.Length();
	pkNode->m_fFValue = pkNode->m_fGValue + pkNode->m_fHValue;
}


AStarCellNode* ZSSAStar::GetConnectedZone(ZoneData* pkZoneData, Vector3 kA, Vector3 kB)
{
	ZoneData*		pkOtherZone;
	P_PfMesh*		pkMesh;
	NaviMeshCell*  pkCell;

	Vector3 kEdgeCenter = (kA + kB) / 2;

	// loop all connected zones
	for(unsigned int i=0; i<pkZoneData->m_iZoneLinks.size(); i++) 
	{
		pkOtherZone = m_pkEntityManager->GetZoneData(pkZoneData->m_iZoneLinks[i]);
		if(pkOtherZone == NULL)					continue;
		if(pkOtherZone->m_pkZone == NULL)	continue;	

		pkMesh = (P_PfMesh*)pkOtherZone->m_pkZone->GetProperty("P_PfMesh");
		if(pkMesh == NULL)						continue;	

		pkCell = pkMesh->GetCell(kA,kB);
		if(pkCell) 
		{
			return new AStarCellNode(pkOtherZone, pkMesh, pkCell);
		}
	}

	return NULL;
}


void ZSSAStar::Reset()
{
	unsigned int i;

	if(kOpenList.size()) 
	{
		for(i=0; i<kOpenList.size(); i++) 
		{
			delete kOpenList[i];
		}
		kOpenList.clear();
	}

	if(kClosedList.size())
	{
		for(i=0; i<kClosedList.size(); i++) 
		{
			delete kClosedList[i];
		}

		kClosedList.clear();
	}
}


bool ZSSAStar::GetFullPath(Vector3 kStart, Vector3 kEnd, vector<PathNode>& kPath)
{
	Reset();

	kPath.clear();

	m_kStart			= kStart;
	m_kGoal			= kEnd;

	m_iStartZone	= m_pkEntityManager->GetZoneIndex(m_kStart,-1, false);
	m_iEndZone		= m_pkEntityManager->GetZoneIndex(m_kGoal,-1, false);

	Vector3 kA, kB;

	if(m_iStartZone < 0) 
	{	// No path found.
		printf("No Start Zone Was Found\n");
		return false;
	}

	if(m_iEndZone < 0) 
	{
		printf("No Start Zone Was Found\n");
		return false;
	}

	if(m_pkEntityManager->GetZoneData(m_iStartZone) == NULL) 
	{
		printf("Failed to get data for start zone\n");
		return false;
	}

	if(m_pkEntityManager->GetZoneData(m_iEndZone) == NULL)	
	{
		printf("Failed to get data for end zone\n");
		return false;
	}

	AStarCellNode* pkNewNode;
	NaviMeshCell* pkEndCell;
	ZoneData* pkZone;
	P_PfMesh* pkMesh;

	P_PfMesh* pkPfStartMesh;
	P_PfMesh* pkPfEndMesh;

	// Get Ptr to End Cell.
	pkZone = m_pkEntityManager->GetZoneData(m_iEndZone);
	if(pkZone->m_pkZone == NULL)
		return false;
	pkMesh = (P_PfMesh*)pkZone->m_pkZone->GetProperty("P_PfMesh");
	if(pkMesh == NULL)
		return false;
	pkPfEndMesh = pkMesh;
	//pkEndCell = pkMesh->GetCell(m_kGoal);
	pkEndCell = pkMesh->GetCurrentCell(m_kGoal);
	if(pkEndCell == NULL) {
		printf("No End Cell Was Found :(\n");
		return false;
		}
	Vector3 kRealEnd = pkEndCell->MapToCellHeight( m_kGoal );

	// 1: Let P = the starting point.
	pkZone = m_pkEntityManager->GetZoneData(m_iStartZone);
	if(pkZone->m_pkZone == NULL)
		return false;
	pkMesh = (P_PfMesh*)pkZone->m_pkZone->GetProperty("P_PfMesh");
	if(pkMesh == NULL)
		return false;
	pkPfStartMesh = pkMesh;

	NaviMeshCell* pkStartCell = pkMesh->GetCurrentCell( kStart );
	if(pkStartCell == NULL) {
		cout << "No StartCell Found at current position" << endl;
		return false;
		}
	
	pkNewNode = new AStarCellNode(pkZone, pkMesh, pkStartCell);	// pkMesh->GetCell(kStart)

	// 2: Assign f,g and h values to P.
	pkNewNode->m_pParent = NULL;
	pkNewNode->m_fGValue = 0;
	pkNewNode->m_fHValue = 0;
	pkNewNode->m_fFValue = 0;

	// 3: Add P to open list.
	kOpenList.push_back(pkNewNode);
	push_heap( kOpenList.begin(), kOpenList.end(), HeapCellComp() );

	NaviMeshCell* pkCell;

	// 4: Let B be the best node from the open list.
	while(kOpenList.size()) {
//		cout << "kOpenList " << kOpenList.size() << endl;

		// Get best node from open list
		AStarCellNode* pkNode = kOpenList.front();
		pop_heap( kOpenList.begin(), kOpenList.end(), HeapCellComp() );
		kOpenList.pop_back();
		kClosedList.push_back(pkNode);

		// 4:a	If B is the goal node.
		if(pkNode->pkNaviCell == pkEndCell) {
			MakePath(pkNode, kPath);
			PathNode kNode;
			kNode.kPosition = kStart;
			kNode.pkStartMesh = pkPfStartMesh;
			kNode.pkStartCell = 	pkStartCell;
			kPath.push_back( kNode );

			reverse(kPath.begin(), kPath.end());

			kNode.kPosition = kRealEnd;
			kNode.pkStartMesh = pkPfEndMesh;
			kNode.pkStartCell = 	pkEndCell;
			kPath.push_back( kNode );
			return true;
			}

		//pkZone = m_pkEntityManagerger->GetZoneData(pkNode->m_iZoneID);
		pkCell = pkNode->pkNaviCell;

		// 5: Let C be a node connected to B.
		for(unsigned int i=0; i<3; i++) {
			pkNewNode = NULL;

			if(pkCell->m_aiLinks[i] <= 0) 
			{
				pkCell->GetEdgeVertex(i, kA, kB);
				pkNewNode = GetConnectedZone( pkNode->m_pkZoneData, kA, kB);

				if(pkCell->m_bNonWalkable &&
					kA.x == kB.x && kA.z == kB.z)
				{
					pkNewNode = NULL;
				}
			}
			else {
				if(pkCell->m_aiLinks[i] > 0)	
					pkNewNode = new AStarCellNode( pkNode->m_pkZoneData, pkNode->m_pkNaviMesh, &pkNode->m_pkNaviMesh->m_NaviMesh [ pkCell->m_aiLinks[i] ]);
			}

			//if(pkCell->m_aiLinks[i] > 0)	
			//	pkNewNode = new AStarCellNode( pkNode->m_pkZoneData, pkNode->m_pkNaviMesh, &pkNode->m_pkNaviMesh->m_NaviMesh [ pkCell->m_aiLinks[i] ]);
			if(pkNewNode == NULL)	continue;

			// 5:a Assign f,g and h values to C.
			pkNewNode->m_pParent = pkNode;
			CalcCoset(pkNewNode);

			// 5:b Check if C is in the open or closed list
			AStarCellNode* pkInOpen   = FindNodeInList(kOpenList,		pkNewNode->pkNaviCell);
			AStarCellNode* pkInClosed = FindNodeInList(kClosedList,	pkNewNode->pkNaviCell);
			
			if(pkInOpen || pkInClosed) {
				if(pkInOpen) {
					if(pkNewNode->m_fGValue < pkInOpen->m_fGValue) {
						*pkInOpen = *pkNewNode;
						delete pkNewNode;
						push_heap( kOpenList.begin(), kOpenList.end(), HeapCellComp() );
						}
					}
				if(pkInClosed) {
					// Ignore step 5bi1
					delete pkNewNode;
					}
				}
			else {
				// 5bii
				kOpenList.push_back(pkNewNode);
				push_heap( kOpenList.begin(), kOpenList.end(), HeapCellComp() );
				}
			}
		}

	return false;
}

P_PfMesh* ZSSAStar::GetPathFindMesh(Vector3 kPos)
{
	m_pkEntityManager->GetZoneIndex(kPos,-1, false);
	if(m_iStartZone < 0)	return NULL;

	ZoneData* pkZone = m_pkEntityManager->GetZoneData(m_iStartZone);
	if(pkZone->m_pkZone == NULL)
		return false;

	P_PfMesh* pkMesh = (P_PfMesh*)pkZone->m_pkZone->GetProperty("P_PfMesh");
	return pkMesh;
}

vector<Vector3> ZSSAStar::OptimizePath(vector<PathNode>& kInPath)
{
	unsigned int i;
	vector<Vector3> kResult;
/*	for( i=0; i<kInPath.size(); i++) 
		kResult.push_back( kInPath[i].kPosition );
	return kResult;*/

	kResult.push_back( kInPath[0].kPosition );

	Vector3			kStart;
	P_PfMesh*		pkStartMesh;
	NaviMeshCell*	pkStartCell;	

	Vector3			kEnd;
	P_PfMesh*		pkEndMesh;
	NaviMeshCell*	pkEndCell;	

	kStart = kInPath[0].kPosition;
	pkStartMesh = kInPath[0].pkStartMesh;	//GetPathFindMesh( kStart );
	pkStartCell = kInPath[0].pkStartCell;	//pkStartMesh->GetCurrentCell( kStart );

	for(i=1; i<kInPath.size(); i++) 
	{
		kEnd		= kInPath[i].kPosition;
		pkEndMesh = kInPath[i].pkStartMesh;	//GetPathFindMesh( kEnd );
		pkEndCell = kInPath[i].pkStartCell;	//pkEndMesh->GetCurrentCell( kEnd );
		
		assert(pkStartMesh);
		assert(pkStartCell);
		assert(pkEndMesh);
		assert(pkEndCell);

		if(pkStartMesh != pkEndMesh) 
		{
			kResult.push_back( kInPath[ i - 1 ].kPosition );
			kResult.push_back( kInPath[ i ].kPosition );
			kStart = kInPath[ i ].kPosition;
			pkStartMesh = kInPath[i].pkStartMesh;	//GetPathFindMesh( kStart );
			pkStartCell = kInPath[i].pkStartCell;	//pkStartMesh->GetCurrentCell( kStart );
			i++;
		}
		else 
		{
			if(pkStartMesh->LineOfSightTest(pkStartCell, kStart, pkEndCell, kEnd) == false && pkEndCell->m_bNonWalkable == false) 
			{
				kResult.push_back( kInPath[ i - 1].kPosition );
				kStart = kInPath[ i - 1 ].kPosition;
				pkStartMesh = kInPath[ i - 1 ].pkStartMesh;	//GetPathFindMesh( kStart );
				pkStartCell = kInPath[ i - 1 ].pkStartCell;	//pkStartMesh->GetCurrentCell( kStart );
			}
			//else 
			//	cout << "Skipping Path step " << i << endl;
		}
	}

	kResult.push_back( kInPath[kInPath.size() - 1].kPosition );
	return kResult;
}

//bool P_PfMesh::LineOfSightTest(NaviMeshCell* pkStartCell, Vector3& kStartPos, NaviMeshCell* pkEndCell, Vector3& kEndPos)


/*
AStarNode* FindNodeInList(vector<AStarNodePtr>& List, int iID)
{
	for(unsigned int i=0; i<List.size(); i++) {
		if(List[i]->m_iZoneID == iID)
			return List[i];
		}

	return NULL;
}

bool ZSSAStar::GetPath(Vector3 kStart, Vector3 kEnd, vector<Vector3>& kPath)
{
	m_kStart		= kStart;
	m_kGoal			= kEnd;
	m_iStartZone	= m_pkEntityManagerger->GetZoneIndex(m_kStart,-1, false);
	m_iEndZone		= m_pkEntityManagerger->GetZoneIndex(m_kGoal,-1, false);

	kPath.clear();

	AStarNode* pkNewNode;
	ZoneData* pkZone;

	vector<AStarNodePtr>	kOpenList;
	vector<AStarNodePtr>	kClosedList;

	if(m_iStartZone < 0)	// No path found.
		return false;
	if(m_iEndZone < 0)		// No path found.
		return false;

	// 1: Let P = the starting point.
	pkNewNode = new AStarNode(m_iStartZone);

	// 2: Assign f,g and h values to P.
	pkNewNode->m_pParent = NULL;
	pkNewNode->m_fGValue = 0;
	pkNewNode->m_fHValue = 0;
	pkNewNode->m_fFValue = 0;

	// 3: Add P to open list.
	kOpenList.push_back(pkNewNode);
	push_heap( kOpenList.begin(), kOpenList.end(), HeapComp() );

	// 4: Let B be the best node from the open list.
	while(kOpenList.size()) {
		// Get best node from open list
		AStarNode* pkNode = kOpenList.front();
		pop_heap( kOpenList.begin(), kOpenList.end(), HeapComp() );
		kOpenList.pop_back();

		// 4:a	If B is the goal node.
		if(pkNode->m_iZoneID == m_iEndZone) {
			MakePath(pkNode, kPath);
			return true;
			}

		pkZone = m_pkEntityManagerger->GetZoneData(pkNode->m_iZoneID);

		// 5: Let C be a node connected to B.
		for(unsigned int i=0; i<pkZone->m_iZoneLinks.size(); i++) {
			// 5:a Assign f,g and h values to C.
			pkNewNode = new AStarNode( pkZone->m_iZoneLinks[i] );
			pkNewNode->m_pParent = pkNode;
			CalcCoset(pkNewNode);

			// 5:b Check if C is in the open or closed list
			AStarNode* pkInOpen   = FindNodeInList(kOpenList, pkNewNode->m_iZoneID);
			AStarNode* pkInClosed = FindNodeInList(kClosedList, pkNewNode->m_iZoneID);
			
			if(pkInOpen || pkInClosed) {
				if(pkInOpen) {
					if(pkNewNode->m_fGValue < pkInOpen->m_fGValue) {
						*pkInOpen = *pkNewNode;
						delete pkNewNode;
						push_heap( kOpenList.begin(), kOpenList.end(), HeapComp() );
						}
					}
				if(pkInClosed) {
					// Ignore step 5bi1
					delete pkNewNode;
					}
				}
			else {
				// 5bii
				kOpenList.push_back(pkNewNode);
				push_heap( kOpenList.begin(), kOpenList.end(), HeapComp() );
				}
			}
		}

	return false;
}*/



/*
void ZSSAStar::CalcCoset(AStarNode* pkNode)
{
	ZoneData* pkZone;
	pkZone = m_pkEntityManagerger->GetZoneData(pkNode->m_iZoneID);

	pkNode->m_fGValue = pkNode->m_pParent->m_fFValue + 1;
	Vector3 kDist = m_kStart - pkZone->m_kPos;
	pkNode->m_fHValue = kDist.Length();
	pkNode->m_fFValue = pkNode->m_fGValue + pkNode->m_fHValue;

}

void ZSSAStar::MakePath(AStarNode* pkNode, vector<Vector3>& kPath)
{
	do {
		kPath.push_back(m_pkEntityManagerger->GetZoneData(pkNode->m_iZoneID)->m_kPos);
		pkNode = pkNode->m_pParent;
	} while(pkNode);
}



*/


/*
AStarNode* FindNodeInList(vector<AStarNodePtr>& List, int iID)
{
	for(unsigned int i=0; i<List.size(); i++) {
		if(List[i]->m_iZoneID == iID)
			return List[i];
		}

	return NULL;
}
*/













