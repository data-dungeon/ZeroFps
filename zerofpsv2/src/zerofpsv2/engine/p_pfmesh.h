#ifndef _ENGINE_P_PFMESH_H_
#define _ENGINE_P_PFMESH_H_

#include "property.h"
#include "engine_x.h"

class P_Mad;

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

	enum CELL_SIDE
	{
		SIDE_AB = 0,
		SIDE_BC,
		SIDE_CA
	};

public:
	
	Vector3			m_kVertex[3];		// Vertex the made up the cell.
	Vector3			m_kCenter;			// Center of the Cell.
	//NaviMeshCell*	m_apkLinks[3];		// Links to cells on each side. NULL if none.
	int				m_aiLinks[3];		// Links to cells on each side. 0 if none.

	void GetEdgeVertex(int iEdge, Vector3& kA, Vector3& kB);

	bool	IsConnected(NaviMeshCell* pkOther, Vector3 kVertexA, Vector3 kVertexB);
};


class ENGINE_API P_PfMesh : public Property 
{
	private:
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

		void SetMad(P_Mad* pkMad);
		void CalcNaviMesh();

		void DrawNaviMesh();
		void LinkCells();
		void LinkToConnectedCells(NaviMeshCell* pkNavCell);
		
		void FlagExternalLinks();

		NaviMeshCell* GetCell(Vector3 kPos);

		NaviMeshCell* GetCell(Vector3 kA, Vector3 kB);

	protected:
		vector<PropertyValues> GetPropertyValues();

	friend class  AStar;
};

Property* Create_P_PfMesh();



#endif