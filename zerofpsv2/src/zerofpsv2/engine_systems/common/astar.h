#ifndef _ENGINE_ASTAR_H_
#define _ENGINE_ASTAR_H_

#include "../../basic/zfassert.h"
#include "../engine_systems_x.h"
#include "../../engine/console.h"
#include "../../engine/entitymanager.h"
#include "../propertys/p_pfmesh.h"

class NaviMeshCell;


/** \brief	A Node used by the ZSSAStar algo.
	 \ingroup Engine
*/
class AStarCellNode
{
public:
	float				m_fGValue;			// cost of this node + it's predecessors
	float				m_fHValue;			// heuristic estimate of distance to goal
	float				m_fFValue;			// sum of cumulative cost of predecessors and self and heuristic

	P_PfMesh*		m_pkNaviMesh;
	NaviMeshCell*	pkNaviCell;
	ZoneData*		m_pkZoneData;
	AStarCellNode*	m_pParent;

	AStarCellNode(ZoneData*	pkZoneData, P_PfMesh* pkNaviMesh, NaviMeshCell* pkCell)
	{
		m_pkZoneData	= pkZoneData;
		m_pkNaviMesh	= pkNaviMesh;
		pkNaviCell		= pkCell;
		m_fGValue		= 0;
		m_fHValue		= 0;
		m_fFValue		= 0;
		m_pParent		= NULL;
	}
};

typedef AStarCellNode* AStarCellNodePtr;

// Klass som STL anv�nder f�r att sortera noderna i nodlistorna
class HeapCellComp
{
	public:
		bool operator() ( const AStarCellNodePtr a, const AStarCellNodePtr b ) const
		{
			return a->m_fFValue > b->m_fFValue;
		}
};


/** \brief		One step in a path.
	 \ingroup Engine
*/
class PathNode
{	
public:
	Vector3			kPosition;
	P_PfMesh*		pkStartMesh;
	NaviMeshCell*	pkStartCell;	
};


/** \brief	Subsystem used for pathfinding
	 \ingroup Engine
*/
class ENGINE_API ZSSAStar : public ZFSubSystem 
{
private:
	ZSSEntityManager*	m_pkEntityManager;

	Vector3			m_kStart;
	Vector3			m_kGoal;
	int				m_iStartZone;
	int				m_iEndZone;

	vector<AStarCellNode*>	kOpenList;
	vector<AStarCellNode*>	kClosedList;
	
	ConVar				m_kbDrawNaviMesh;
	ConVar				m_kbDrawPaths;

public:
	ZSSAStar();
	virtual ~ZSSAStar() {}

	bool GetFullPath(Vector3 kStart, Vector3 kEnd, vector<PathNode>& kPath);
	void Reset();
	void CalcCoset(AStarCellNode* pkNode);
	void MakePath(AStarCellNode* pkNode, vector<PathNode>& kPath);

	AStarCellNode* GetConnectedZone(ZoneData*	pkZoneData, Vector3 kA, Vector3 kB);

	vector<Vector3> OptimizePath(vector<PathNode>& kInPath);
	
	P_PfMesh*	GetPathFindMesh(Vector3 kPos);

	bool StartUp();
	bool ShutDown()	{ return true;		}
	bool IsValid()		{ return true;		}

	friend class P_PfMesh;
	friend class P_PfPath;
};

//	AStarNode* FindNodeInList(vector<AStarNodePtr>& List, int iID);
//	bool GetPath(Vector3 kStart, Vector3 kEnd, vector<Vector3>& kPath);
//	void CalcCoset(AStarNode* pkNode);
//	void MakePath(AStarNode* pkNode, vector<Vector3>& kPath);

/*
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

// Klass som STL anv�nder f�r att sortera noderna i nodlistorna
class HeapComp
{
	public:
		bool operator() ( const AStarNodePtr a, const AStarNodePtr b ) const
		{
			return a->m_fFValue > b->m_fFValue;
		}
};*/


#endif















