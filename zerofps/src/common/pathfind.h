// pathfind.h: interface for the PathFind class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHFIND_H__0D886B99_D642_4951_A610_AA69537A7DBC__INCLUDED_)
#define AFX_PATHFIND_H__0D886B99_D642_4951_A610_AA69537A7DBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common_x.h"
#include "../zerofps/basic/basic.pkg"

#include <vector>
#include <queue>
using namespace std;

class PathBuilder;

enum TerrainType
{
	GRASS=0,
	ROAD,
	SWAMP,
	WOOD,
	WATER,
};

const int BLOCKED = 999;	// Unpassable square

class COMMON_API PathFind : public ZFObject 
{
private:

	// A node represents a possible state in the search
	// The user provided state type is included inside this type
	class Node
	{
		public:
			Node *m_pkParent; // used during the search to record the parent of successor nodes
			Node *m_pkChild; // used after the search for the application to view the search in reverse
			
			float m_fGValue; // cost of this node + it's predecessors
			float m_fHValue; // heuristic estimate of distance to goal
			float m_fFValue; // sum of cumulative cost of predecessors and self and heuristic

			Node(int x=0, int y=0) : m_pkParent( 0 ), m_pkChild( 0 ), 
				m_fGValue( 0.0f ), m_fHValue( 0.0f ), m_fFValue( 0.0f )
			{		
				m_kSqrPos.x = x;
				m_kSqrPos.y = y;
			}

			float GetHeuristicCost( Point nodeGoal );

			///////////////////////////////////////////////////////////////////
			// Node data
			///////////////////////////////////////////////////////////////////

			Point m_kSqrPos;
	};

	typedef Node* NodePtr;

	enum SEARCH_STATE
	{
		ACTIVE = 0,
		FOUND_DESTINATION,
		CANT_REACH_DESTINATION
	};

	// Klass som STL använder för att sortera noderna i nodlistorna
	class HeapComp
	{
		public:
			bool operator() ( const NodePtr a, const NodePtr b ) const
			{
				return a->m_fFValue > b->m_fFValue;
			}
	};

	vector<NodePtr> m_kvOpenList;
	vector<NodePtr> m_kvClosedList; 
	vector<NodePtr> m_kvNeighbours;
	queue<Point> m_kqPath;
	bool m_bPathIsReversed;

	// State
	SEARCH_STATE m_eState;

	NodePtr m_pkStartNode;
	NodePtr m_pkDestNode;
	NodePtr m_pkSearchNode;
	
	int* m_piMapTerrain;
	const unsigned int BLOCKED_VALUE;

	// Advances search one step 
	SEARCH_STATE SearchStep();

	int GetTerrainCost(int x, int y);
	bool FillQueue();
	void FreeAllNodes();
	void DeleteUnusedNodes();

	// User calls this to add a successor to a list of successors
	// when expanding the search frontier
	bool AddNeighbour( Point &kNewSquare );

	// Free the solution nodes
	// This is done to clean up all used Node memory when you are done with the
	// search
	void FreeSolutionNodes();

	NodePtr GetSolutionStart();
	NodePtr GetSolutionNext();
	NodePtr GetNextNextDiagonalStep(NodePtr pkNode);
	void ReversePath();
	bool ImpossibleToReach(int iStartPosX, int iStartPosY, int iDestPosX, int iDestPosY);

public: 
	
	PathFind(int* piMapTerrain, int m_iMapWidth, unsigned int uiBlockedValue);
	~PathFind();

	void Reset();
	bool GetNextStep(int &riSquareX, int &riSquareY);
	bool Rebuild(int startx, int starty, int endx, int endy);

	static int m_siMapWidth;
	friend class PathBuilder;
};

#endif // !defined(AFX_PATHFIND_H__0D886B99_D642_4951_A610_AA69537A7DBC__INCLUDED_)
