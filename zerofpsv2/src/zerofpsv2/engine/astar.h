#ifndef _ENGINE_ASTAR_H_
#define _ENGINE_ASTAR_H_

#include "../basic/zfassert.h"
#include "engine_x.h"
#include "console.h"
#include "entitymanager.h"

class AStarNode
{
public:
	float m_fGValue;		// cost of this node + it's predecessors
	float m_fHValue;		// heuristic estimate of distance to goal
	float m_fFValue;		// sum of cumulative cost of predecessors and self and heuristic

	int	  m_iZoneID;

	AStarNode*	m_pParent;

	AStarNode(int iID)
	{
		m_iZoneID = iID;
		m_fGValue = 0;
		m_fHValue = 0;
		m_fFValue = 0;
		m_pParent = NULL;
	}
};

typedef AStarNode* AStarNodePtr;

// Klass som STL använder för att sortera noderna i nodlistorna
class HeapComp
{
	public:
		bool operator() ( const AStarNodePtr a, const AStarNodePtr b ) const
		{
			return a->m_fFValue > b->m_fFValue;
		}
};




class ENGINE_API AStar : public ZFSubSystem 
{
private:
	EntityManager*	m_pkObjectManger;

	Vector3 m_kStart;
	Vector3 m_kGoal;

	int		m_iStartZone;
	int		m_iEndZone;

public:
	AStar();
	virtual ~AStar() {}

	bool GetPath(Vector3 kStart, Vector3 kEnd, vector<Vector3>& kPath);
	bool GetFullPath(Vector3 kStart, Vector3 kEnd, vector<Vector3>& kPath);
	void CalcCoset(AStarNode* pkNode);
	void MakePath(AStarNode* pkNode, vector<Vector3>& kPath);

	bool StartUp();
	bool ShutDown() { return true; }
	bool IsValid() { return true;  }

};

#endif















