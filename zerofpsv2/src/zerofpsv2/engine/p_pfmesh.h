#ifndef _ENGINE_P_PFMESH_H_
#define _ENGINE_P_PFMESH_H_

#include "property.h"
#include "engine_x.h"
#include "../basic/line2d.h"

class P_Mad;
class P_HMRP2;

using namespace std;

class NaviMeshCell
{
private:
	
	enum CELL_VERT
	{
		VERT_A = 0,
		VERT_B,
		VERT_C
	};


public:
	enum CELL_SIDE
	{
		SIDE_AB = 0,
		SIDE_BC,
		SIDE_CA
	};

	enum PATH_CLASSIFICATION
	{
		CELL_NONE,				// Path do not cross this cell.
		CELL_END,				// Path ends in cell.
		CELL_EXIT,				// Path exits this cell.
	};
	
	Vector3			m_kVertex[3];		// Vertex the made up the cell.
	Vector3			m_kCenter;			// Center of the Cell.
	int				m_aiLinks[3];		// Links to cells on each side. 0 if none.

	Line2D			m_kEdges[3];

	bool				m_bNonWalkable;	// Non walkable, like a step.

	void	GetEdgeVertex(int iEdge, Vector3& kA, Vector3& kB);
	bool	IsConnected(NaviMeshCell* pkOther, Vector3 kVertexA, Vector3 kVertexB);

	bool IsPointInCell( Vector2& kPoint);

	void RefreshData();
	Vector3 GetEdgeCenter(int iSide);

	Vector3 MapToCellHeight(Vector3 kIn);

	NaviMeshCell::PATH_CLASSIFICATION ClassifyPath(Line2D& kPath, int& iNextCell, CELL_SIDE& eSide, Vector2* pkIntersection);

};

class AStar;
struct Mad_Face;

/** \brief	Gives a entity ability to have a mesh used for pathfinding on it. 
	 \ingroup Engine
*/
class ENGINE_API P_PfMesh : public Property 
{
	private:
		AStar*						m_pkAStar;

		vector<NaviMeshCell>		m_NaviMesh;
		P_Mad*						m_pkMad;
		NaviMeshCell*				m_pkSelected;

	public:
		P_PfMesh();
		~P_PfMesh();

		void CloneOf(Property* pkProperty) { }
		void Update();

		void Save(ZFIoInterface* pkFile);
		void Load(ZFIoInterface* pkFile);

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		void BuildNavMesh(bool bWorldCoo, vector<Mad_Face>* pkFace, vector<Vector3>* pkVertex, vector<Vector3>* pkNormal);
		void SetMad(P_Mad* pkMad);
		void SetHmap(P_HMRP2* pkHmap);
		void AutoMakeNaviMesh();
		
		void CalcNaviMesh();

		void DrawNaviMesh();
		void LinkCells();
		void LinkToConnectedCells(NaviMeshCell* pkNavCell);
		
		void FlagExternalLinks();

		NaviMeshCell* GetCell(Vector3 kPos);
		NaviMeshCell* GetCell(Vector3 kA, Vector3 kB);
		NaviMeshCell* GetCurrentCell(Vector3 kPos);
		bool LineOfSightTest(NaviMeshCell* pkStartCell, Vector3& kStartPos, NaviMeshCell* pkEndCell, Vector3& kEndPos);


	protected:
		vector<PropertyValues> GetPropertyValues();

	friend class  AStar;
};

Property* Create_P_PfMesh();



#endif










