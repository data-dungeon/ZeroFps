#include "astar.h"
#include "p_pfmesh.h"



AStar::AStar()
: ZFSubSystem("AStar")
{

}

bool AStar::StartUp()
{
	m_pkObjectManger	= static_cast<EntityManager*>(GetSystem().GetObjectPtr("EntityManager"));

	return true;
}


AStarNode* FindNodeInList(vector<AStarNodePtr>& List, int iID)
{
	for(unsigned int i=0; i<List.size(); i++) {
		if(List[i]->m_iZoneID == iID)
			return List[i];
		}

	return NULL;
}

bool AStar::GetPath(Vector3 kStart, Vector3 kEnd, vector<Vector3>& kPath)
{
	m_kStart		= kStart;
	m_kGoal			= kEnd;
	m_iStartZone	= m_pkObjectManger->GetZoneIndex(m_kStart,-1, false);
	m_iEndZone		= m_pkObjectManger->GetZoneIndex(m_kGoal,-1, false);

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

		pkZone = m_pkObjectManger->GetZoneData(pkNode->m_iZoneID);

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
}

void AStar::CalcCoset(AStarNode* pkNode)
{
	ZoneData* pkZone;
	pkZone = m_pkObjectManger->GetZoneData(pkNode->m_iZoneID);

	pkNode->m_fGValue = pkNode->m_pParent->m_fFValue + 1;
	Vector3 kDist = m_kStart - pkZone->m_kPos;
	pkNode->m_fHValue = kDist.Length();
	pkNode->m_fFValue = pkNode->m_fGValue + pkNode->m_fHValue;

}

void AStar::MakePath(AStarNode* pkNode, vector<Vector3>& kPath)
{
	do {
		kPath.push_back(m_pkObjectManger->GetZoneData(pkNode->m_iZoneID)->m_kPos);
		pkNode = pkNode->m_pParent;
	} while(pkNode);
}

bool AStar::GetFullPath(Vector3 kStart, Vector3 kEnd, vector<Vector3>& kPath)
{
	printf("Creating full path");

	kPath.clear();

	m_kStart			= kStart;
	m_kGoal			= kEnd;
	m_iStartZone	= m_pkObjectManger->GetZoneIndex(m_kStart,-1, false);
	m_iEndZone		= m_pkObjectManger->GetZoneIndex(m_kGoal,-1, false);

	if(m_iStartZone != m_iEndZone)
		return false;

	ZoneData* pkZone;
	pkZone = m_pkObjectManger->GetZoneData(m_iEndZone);
	if(pkZone->m_pkZone == NULL)
		return false;

	P_PfMesh* pkMesh = (P_PfMesh*)pkZone->m_pkZone->GetProperty("P_PfMesh");
	if(pkMesh) {
		pkMesh->GetCell(m_kGoal);
		}

	return false;
}
