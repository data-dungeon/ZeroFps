#include "p_pfmesh.h"
#include "entity.h"
#include "../engine_systems/propertys/p_mad.h"
#include "../engine_systems/mad/mad_core.h"
#include "../render/render.h"




bool NaviMeshCell::IsConnected(NaviMeshCell* pkOther, Vector3 kVertexA, Vector3 kVertexB)
{
//	printf("-Found a edge to test \n");
//	printf(" 1 <%f,%f,%f> \n", m_kVertex[VERT_A].x, m_kVertex[VERT_A].y,m_kVertex[VERT_A].z);
//	printf(" 2 <%f,%f,%f> \n", m_kVertex[VERT_B].x, m_kVertex[VERT_B].y,m_kVertex[VERT_B].z);
//	printf(" 3 <%f,%f,%f> \n", m_kVertex[VERT_C].x, m_kVertex[VERT_C].y,m_kVertex[VERT_C].z);

	
	
	if( m_kVertex[VERT_A].NearlyEquals(kVertexA,0.3 )) {
		if( m_kVertex[VERT_B].NearlyEquals(kVertexB,0.3 ) )	return true;
		if( m_kVertex[VERT_C].NearlyEquals(kVertexB,0.3 )  )	return true;
		}

	else if( m_kVertex[VERT_B].NearlyEquals(kVertexA,0.3 ) ) {
		if( m_kVertex[VERT_A].NearlyEquals(kVertexB,0.3 ) )	return true;
		if( m_kVertex[VERT_C].NearlyEquals(kVertexB,0.3 ) )	return true;
		}

	else if( m_kVertex[VERT_C].NearlyEquals(kVertexA,0.3 ) ) {
		if( m_kVertex[VERT_A].NearlyEquals(kVertexB,0.3 ) )	return true;
		if( m_kVertex[VERT_B].NearlyEquals(kVertexB,0.3 ) )	return true;
		}


	return false;
}

void NaviMeshCell::GetEdgeVertex(int iEdge, Vector3& kA, Vector3& kB)
{
	if(iEdge == 0) {
		kA = m_kVertex[0];
		kB = m_kVertex[1];
		}
		
	if(iEdge == 1) {
		kA = m_kVertex[1];
		kB = m_kVertex[2];
		}

	if(iEdge == 2) {
		kA = m_kVertex[2];
		kB = m_kVertex[0];
		}
}





































P_PfMesh::P_PfMesh()
{
	strcpy(m_acName,"P_PfMesh");
	m_iType = PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;

	m_pkMad			= NULL;
	m_pkSelected	= NULL;
}

P_PfMesh::~P_PfMesh()
{

}

void P_PfMesh::Update()
{
	if(m_pkMad == NULL) {
		m_pkMad = (P_Mad*)m_pkObject->GetProperty("P_Mad");
		if(m_pkMad)
			SetMad(m_pkMad);
		}

	DrawNaviMesh();
}

void P_PfMesh::Save(ZFIoInterface* pkFile)
{

}

void P_PfMesh::Load(ZFIoInterface* pkFile)
{
		
}

void P_PfMesh::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{

}

void P_PfMesh::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{

}

void P_PfMesh::SetMad(P_Mad* pkMad)
{
	printf("Refreshing Mesh\n");

	m_NaviMesh.clear();

	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 
	if(pkCore == NULL)
		return;
	Mad_CoreMesh* pkCoreMesh = pkCore->GetMeshByID(0);
	if(pkCoreMesh == NULL)
		return;
	
	vector<Mad_Face>*		pkFace	= pkCoreMesh->GetFacesPointer();
	vector<Vector3>*		pkVertex = (*pkCoreMesh->GetVertexFramePointer())[0].GetVertexPointer();
	vector<Vector3>*		pkNormal = (*pkCoreMesh->GetVertexFramePointer())[0].GetNormalPointer();

	NaviMeshCell kNaviMesh;

	kNaviMesh.m_kVertex[0].Set(0,0,0);
	kNaviMesh.m_kVertex[1].Set(0,0,0);
	kNaviMesh.m_kVertex[2].Set(0,0,0);
	kNaviMesh.m_kCenter.Set(0,0,0);
	kNaviMesh.m_aiLinks[0] = 0;
	kNaviMesh.m_aiLinks[1] = 0;
	kNaviMesh.m_aiLinks[2] = 0;
	m_NaviMesh.push_back( kNaviMesh );

	printf("Vertex/Normals: %d,%d\n",(*pkVertex).size(),(*pkNormal).size());
	Vector3 kNormal;

	Matrix4 kMat = m_pkObject->GetWorldOriM();

	for(int i=0; i<pkFace->size(); i++) {
		kNormal.Set(0,0,0);
		kNormal += (*pkNormal)[ (*pkFace)[i].iIndex[0] ];
		kNormal += (*pkNormal)[ (*pkFace)[i].iIndex[1] ];
		kNormal += (*pkNormal)[ (*pkFace)[i].iIndex[2] ];
		kNormal.Normalize();
		if(kNormal.y <= 0.9)	continue;
			
		kNaviMesh.m_kVertex[0] = (*pkVertex)[ (*pkFace)[i].iIndex[0] ];
		kNaviMesh.m_kVertex[1] = (*pkVertex)[ (*pkFace)[i].iIndex[1] ];
		kNaviMesh.m_kVertex[2] = (*pkVertex)[ (*pkFace)[i].iIndex[2] ];

		kNaviMesh.m_kVertex[0] = kMat.VectorTransform(kNaviMesh.m_kVertex[0]);
		kNaviMesh.m_kVertex[1] = kMat.VectorTransform(kNaviMesh.m_kVertex[1]);
		kNaviMesh.m_kVertex[2] = kMat.VectorTransform(kNaviMesh.m_kVertex[2]);

		kNaviMesh.m_kCenter = (kNaviMesh.m_kVertex[0] + kNaviMesh.m_kVertex[1] + kNaviMesh.m_kVertex[2]) / 3;
		kNaviMesh.m_aiLinks[0] = 0;
		kNaviMesh.m_aiLinks[1] = 0;
		kNaviMesh.m_aiLinks[2] = 0;

		m_NaviMesh.push_back( kNaviMesh );
		}

	LinkCells();
//	printf("NaviMesh Size: %d \n", m_NaviMesh.size());
	m_pkSelected = &m_NaviMesh[0];
}

void P_PfMesh::CalcNaviMesh()
{
	P_Mad* pkMad = dynamic_cast<P_Mad*>(m_pkObject->GetProperty("P_Mad"));
	if(!pkMad)	
		return;

	SetMad(pkMad);
}

void SetEdgeColor(int iID)
{
	if(iID > 0 )	glColor3f(0,1,0);
	if(iID == 0 )	glColor3f(1,0,0);
	if(iID < 0 )	glColor3f(0,0,1);
}

void P_PfMesh::DrawNaviMesh()
{
	if(m_NaviMesh.size() == 0)
		return;

	glPushMatrix();

	glDisable(GL_LIGHTING );
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3.0);

	glColor3f(1,1,1);
	Vector3 kColor;

	Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render")); 

	for(int i=1; i<m_NaviMesh.size(); i++) {
		if(&m_NaviMesh[i] == m_pkSelected)	kColor.Set(0,0,1);
			else										kColor.Set(1,1,1);
		pkRender->Draw_MarkerCross(m_NaviMesh[i].m_kCenter, kColor, 0.1);

		glBegin(GL_LINES);
			SetEdgeColor(m_NaviMesh[i].m_aiLinks[0]);
			glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[0] );		glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[1] );
			
			SetEdgeColor(m_NaviMesh[i].m_aiLinks[1]);
			glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[1] );		glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[2] );
			
			SetEdgeColor(m_NaviMesh[i].m_aiLinks[2]);
			glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[2] );		glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[0] );
		glEnd();
		}

	glLineWidth(1.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}


void P_PfMesh::LinkToConnectedCells(NaviMeshCell* pkNavCell)
{
	pkNavCell->m_aiLinks[0] = NULL;
	pkNavCell->m_aiLinks[1] = NULL;
	pkNavCell->m_aiLinks[2] = NULL;

	for(int i=1; i<m_NaviMesh.size(); i++) {
		if(&m_NaviMesh[i] == pkNavCell)	continue;

		if(m_NaviMesh[i].IsConnected(pkNavCell, pkNavCell->m_kVertex[0], pkNavCell->m_kVertex[1]) ) {
			pkNavCell->m_aiLinks[0] = i;
			}

		if(m_NaviMesh[i].IsConnected(pkNavCell, pkNavCell->m_kVertex[1], pkNavCell->m_kVertex[2]) ) {
			pkNavCell->m_aiLinks[1] = i;
			}

		if(m_NaviMesh[i].IsConnected(pkNavCell, pkNavCell->m_kVertex[2], pkNavCell->m_kVertex[0]) ) {
			pkNavCell->m_aiLinks[2] = i;
			}

		}
}

void P_PfMesh::LinkCells()
{
	for(int i=1; i<m_NaviMesh.size(); i++) {
		LinkToConnectedCells( &m_NaviMesh[i] );
		}

	FlagExternalLinks();
}

NaviMeshCell* P_PfMesh::GetCell(Vector3 kPos)
{
//	kPos -= m_pkObject->GetWorldPosV();

	float fClosest = 100000000;
	int iIndex = 0;

	for(int i=1; i<m_NaviMesh.size(); i++) {
		Vector3 kDiff = m_NaviMesh[i].m_kCenter - kPos;
		float fDist = kDiff.Length();

		if(fDist < fClosest) {
			iIndex = i;
			fClosest = fDist;
			}
		}

	m_pkSelected = &m_NaviMesh[iIndex];
	return &m_NaviMesh[iIndex];
}

void P_PfMesh::FlagExternalLinks()
{
	NaviMeshCell* pkNavCell;

	// Loop all cells
	for(int iNavMesh=1; iNavMesh<m_NaviMesh.size(); iNavMesh++) {
		pkNavCell = &m_NaviMesh[iNavMesh];
		
		// Loop all edges in cell
		for(int i=0; i<3; i++) {
			if(pkNavCell->m_aiLinks[i] > 0)		continue;
			
			pkNavCell->m_aiLinks[i] = -1;

			}
		}
}



NaviMeshCell* P_PfMesh::GetCell(Vector3 kA, Vector3 kB)
{
//	printf("Scanning for Cell <%f,%f,%f>, <%f,%f,%f>", kA.x, kA.y,kA.z, kB.x, kB.y,kB.z);

	NaviMeshCell* pkNavCell;

	// Loop all cells
	for(int iNavMesh=1; iNavMesh<m_NaviMesh.size(); iNavMesh++) {
		pkNavCell = &m_NaviMesh[iNavMesh];

		// Loop all edges in cell
		for(int i=0; i<3; i++) {
			if(pkNavCell->m_aiLinks[i] >= 0)		continue;

			if( pkNavCell->IsConnected(NULL, kA, kB))
				return pkNavCell;
			}
		}

	return NULL;
}



vector<PropertyValues> P_PfMesh::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
	return kReturn;
}

Property* Create_P_PfMesh()
{
	return new P_PfMesh;
}

