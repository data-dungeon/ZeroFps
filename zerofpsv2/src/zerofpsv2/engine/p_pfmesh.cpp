#include "p_pfmesh.h"
#include "entity.h"
#include "astar.h"
#include "../engine_systems/propertys/p_mad.h"
#include "../engine_systems/propertys/p_hmrp2.h"
#include "../engine_systems/mad/mad_core.h"
#include "../render/render.h"

Vector3 CalcNormal(Vector3 VertA, Vector3 VertB, Vector3 VertC)
{
	Vector3 kV1 = VertB - VertA;
	Vector3 kV2 = VertC - VertA;

	Vector3 kNorm = kV1.Cross(kV2);
	kNorm.Normalize();
	return kNorm;
}

bool NaviMeshCell::IsConnected(NaviMeshCell* pkOther, Vector3 kVertexA, Vector3 kVertexB)
{
	if( m_kVertex[VERT_A].NearlyEquals(kVertexA,			0.01 )) {
		if( m_kVertex[VERT_B].NearlyEquals(kVertexB,		0.01 ) )		return true;
		if( m_kVertex[VERT_C].NearlyEquals(kVertexB,		0.01 )  )	return true;
		}

	else if( m_kVertex[VERT_B].NearlyEquals(kVertexA,	0.01 ) ) {
		if( m_kVertex[VERT_A].NearlyEquals(kVertexB,		0.01 ) )		return true;
		if( m_kVertex[VERT_C].NearlyEquals(kVertexB,		0.01 ) )		return true;
		}

	else if( m_kVertex[VERT_C].NearlyEquals(kVertexA,	0.01 ) ) {
		if( m_kVertex[VERT_A].NearlyEquals(kVertexB,		0.01 ) )		return true;
		if( m_kVertex[VERT_B].NearlyEquals(kVertexB,		0.01 ) )		return true;
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

void NaviMeshCell::RefreshData()
{
	m_bNonWalkable = false;
	m_kEdges[0].SetPoints( m_kVertex[0].x,m_kVertex[0].z, m_kVertex[1].x,m_kVertex[1].z );
	m_kEdges[1].SetPoints( m_kVertex[1].x,m_kVertex[1].z, m_kVertex[2].x,m_kVertex[2].z );
	m_kEdges[2].SetPoints( m_kVertex[2].x,m_kVertex[2].z, m_kVertex[0].x,m_kVertex[0].z );
	if( m_kVertex[0].x == m_kVertex[1].x && m_kVertex[0].z == m_kVertex[1].z)	m_bNonWalkable = true;
	if( m_kVertex[1].x == m_kVertex[2].x && m_kVertex[1].z == m_kVertex[2].z)	m_bNonWalkable = true;
	if( m_kVertex[2].x == m_kVertex[0].x && m_kVertex[2].z == m_kVertex[0].z)	m_bNonWalkable = true;
}


bool NaviMeshCell::IsPointInCell( Vector2& kPoint)
{
	int iInsideCount = 0;
	
	for(int i=0; i<3; i++) {
		Line2D::POINT_CLASSIFICATION eSideResult = m_kEdges[i].ClassifyPoint( kPoint );
		
		if(eSideResult != Line2D::LEFT_SIDE) 
			iInsideCount++;
		}

	return (iInsideCount == 3);

}

Vector3 NaviMeshCell::GetEdgeCenter(int iSide)
{
	Vector3 kCenter(0,0,0);
	Vector3 kDiff;

	Vector3 kA, kB;
	GetEdgeVertex(iSide, kA, kB);
	kDiff = kB - kA;
	kDiff *= 0.5;
	kCenter = kA + kDiff; 

/*	switch(eSide) {
		case SIDE_AB:
			kDiff = ( m_kVertex[VERT_B] - m_kVertex[VERT_A] ) * 0.5;
			kCenter = m_kVertex[VERT_A] + kDiff; 
			break;
		case SIDE_BC:
			kDiff = ( m_kVertex[VERT_C] - m_kVertex[VERT_B] ) * 0.5;
			kCenter = m_kVertex[VERT_B] + kDiff; 
			break;
		case SIDE_CA:
			kDiff = ( m_kVertex[VERT_C] - m_kVertex[VERT_A] ) * 0.5;
			kCenter = m_kVertex[VERT_C] + kDiff; 
			break;
	}*/

	return kCenter;
}

Vector3 NaviMeshCell::MapToCellHeight(Vector3 kIn)
{
	Plane kPlane;
	kPlane.Set(m_kVertex[VERT_A], m_kVertex[VERT_B], m_kVertex[VERT_C]);
	kIn.y = kPlane.SolveY(kIn.x,kIn.z);
	return kIn;
}

NaviMeshCell::PATH_CLASSIFICATION NaviMeshCell::ClassifyPath(Line2D& kPath, int& iNextCell, CELL_SIDE& eSide, Vector2* pkIntersection)
{
	if(m_bNonWalkable) 
	{
		iNextCell = -1;
		return CELL_EXIT;
	}



	int iInside = 0;

	for(int i=0; i<3; i++)
	{
		if( m_kEdges[i].ClassifyPoint(kPath.EndPointB()) != Line2D::RIGHT_SIDE) {
			if( m_kEdges[i].ClassifyPoint(kPath.EndPointA()) != Line2D::LEFT_SIDE) {
				Line2D::LINE_CLASSIFICATION iInterSect = kPath.Intersection(m_kEdges[i],pkIntersection);

				if(iInterSect == Line2D::SEGMENTS_INTERSECT || iInterSect == Line2D::A_BISECTS_B) {
					iNextCell = m_aiLinks[i];
					eSide = (CELL_SIDE)i;
					return CELL_EXIT;
					}
				}
			}

		else {
			iInside++;
			}
	}

	if( iInside == 3)
		return CELL_END;

	return CELL_NONE;
}




P_PfMesh::P_PfMesh()
{
	strcpy(m_acName,"P_PfMesh");
	m_iType = PROPERTY_TYPE_RENDER | PROPERTY_TYPE_NORMAL;
//	m_iType = PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;

	m_pkMad			= NULL;
	m_pkSelected	= NULL;

	m_pkAStar = static_cast<AStar*>(g_ZFObjSys.GetObjectPtr("AStar"));	
}

P_PfMesh::~P_PfMesh()
{

}

void P_PfMesh::Update()
{
	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_NORMAL) ) 
	{
		//cout << "P_PfMesh::Update" << endl;
	
		if(m_NaviMesh.size() == 0)
		{
			m_pkMad = (P_Mad*)m_pkObject->GetProperty("P_Mad");
			P_HMRP2* pkHmap = (P_HMRP2*)m_pkObject->GetProperty("P_HMRP2");

			if(m_pkMad)
				SetMad(m_pkMad);
			if(pkHmap)
				SetHmap(pkHmap);
		}
	}

	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER) ) {
		if(m_pkAStar->m_bDrawNaviMesh)
			DrawNaviMesh();
		}
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

float GetStepHojd(Vector3 kV1,Vector3 kV2,Vector3 kV3 )
{
	float fMaxHojd = 0.0;
	float fHojd = 0.0;

	fHojd = fabs( kV1.y - kV2.y );
	if(fHojd > fMaxHojd)
		fMaxHojd = fHojd;
	fHojd = fabs( kV2.y - kV3.y );
	if(fHojd > fMaxHojd)
		fMaxHojd = fHojd;
	fHojd = fabs( kV3.y - kV1.y );
	if(fHojd > fMaxHojd)
		fMaxHojd = fHojd;
	
	return fMaxHojd;
}

void P_PfMesh::BuildNavMesh(bool bWorldCoo, vector<Mad_Face>* pkFace, vector<Vector3>* pkVertex, vector<Vector3>* pkNormal)
{
	m_NaviMesh.clear();
	NaviMeshCell kNaviMesh;

	kNaviMesh.m_kVertex[0].Set(0,0,0);
	kNaviMesh.m_kVertex[1].Set(0,0,0);
	kNaviMesh.m_kVertex[2].Set(0,0,0);
	kNaviMesh.m_kCenter.Set(0,0,0);
	kNaviMesh.m_aiLinks[0] = 0;
	kNaviMesh.m_aiLinks[1] = 0;
	kNaviMesh.m_aiLinks[2] = 0;
	m_NaviMesh.push_back( kNaviMesh );

	Vector3 kNormal;

	Matrix4 kMat = m_pkObject->GetWorldOriM();

	//cout << "BuildNavMesh: Create Faces" << endl;
	for(unsigned int i=0; i<pkFace->size(); i++) 
	{
			kNaviMesh.m_kVertex[0] = (*pkVertex)[ (*pkFace)[i].iIndex[0] ];
			kNaviMesh.m_kVertex[1] = (*pkVertex)[ (*pkFace)[i].iIndex[1] ];
			kNaviMesh.m_kVertex[2] = (*pkVertex)[ (*pkFace)[i].iIndex[2] ];

		if(!bWorldCoo) 
		{
			kNaviMesh.m_kVertex[0] = kMat.VectorTransform(kNaviMesh.m_kVertex[0]);
			kNaviMesh.m_kVertex[1] = kMat.VectorTransform(kNaviMesh.m_kVertex[1]);
			kNaviMesh.m_kVertex[2] = kMat.VectorTransform(kNaviMesh.m_kVertex[2]);
		}

		kNormal = CalcNormal(kNaviMesh.m_kVertex[0], kNaviMesh.m_kVertex[1], kNaviMesh.m_kVertex[2]);
		
		float fMaxStepSize = 0.2;
		float fStepSize = GetStepHojd(kNaviMesh.m_kVertex[0], kNaviMesh.m_kVertex[1], kNaviMesh.m_kVertex[2]);

		kNaviMesh.m_kCenter = (	kNaviMesh.m_kVertex[0] + kNaviMesh.m_kVertex[1] + kNaviMesh.m_kVertex[2]) / 3;
		kNaviMesh.m_aiLinks[0] = 0;
		kNaviMesh.m_aiLinks[1] = 0;
		kNaviMesh.m_aiLinks[2] = 0;
		kNaviMesh.RefreshData();

		if(fStepSize < fMaxStepSize)
			m_NaviMesh.push_back( kNaviMesh );
/*		else
		{
			if(kNormal.y > 0.9)	
				m_NaviMesh.push_back( kNaviMesh );
		}*/
	}

	LinkCells();
}

void P_PfMesh::SetMad(P_Mad* pkMad)
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 
	if(pkCore == NULL)
		return;
	Mad_CoreMesh* pkCoreMesh = pkCore->GetMeshByID(0);
	if(pkCoreMesh == NULL)
		return;
	
	vector<Mad_Face>*		pkFace	= pkCoreMesh->GetLODMesh(0)->GetFacesPointer();
	vector<Vector3>*		pkVertex = (*pkCoreMesh->GetLODMesh(0)->GetVertexFramePointer())[0].GetVertexPointer();
	vector<Vector3>*		pkNormal = (*pkCoreMesh->GetLODMesh(0)->GetVertexFramePointer())[0].GetNormalPointer();

	BuildNavMesh(false, pkFace, pkVertex, pkNormal);
//	m_pkSelected = &m_NaviMesh[0];
}

void P_PfMesh::SetHmap(P_HMRP2* pkHmap)
{
	vector<Mad_Face>		kFace;
	vector<Vector3>		kVertex;
	vector<Vector3>		kNormal;
	
	pkHmap->m_pkHeightMap->GetCollData(&kFace,&kVertex,&kNormal);

	// Transform all vertex from Local to World.
	for(unsigned int i=0; i<kVertex.size(); i++) 
		kVertex[i] += pkHmap->m_pkHeightMap->m_kCornerPos;;

	BuildNavMesh(true, &kFace, &kVertex, &kNormal);
	

	cout << "P_PfMesh::SetHmap " << endl;
}

void P_PfMesh::AutoMakeNaviMesh()
{
	// Try to find a Hmap or Mad to build mesh from.
	P_HMRP2* pkHmap = (P_HMRP2*)m_pkObject->GetProperty("P_HMRP2");
	if(pkHmap)
		SetHmap(pkHmap);
	P_Mad* pkMad = (P_Mad*)m_pkObject->GetProperty("P_Mad");
	if(pkMad)
		SetMad(pkMad);

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

	glPushAttrib(GL_DEPTH_BUFFER_BIT );
	glDisable(GL_LIGHTING );
	glDisable(GL_TEXTURE_2D);
	glDepthFunc(GL_EQUAL);
	glColor3f(1,1,1);
	Vector3 kColor;

	Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render")); 

	for(unsigned int i=1; i<m_NaviMesh.size(); i++) {
//		if(&m_NaviMesh[i] == m_pkSelected)	kColor.Set(0,0,1);
//			else										kColor.Set(1,1,1);
//		pkRender->Draw_MarkerCross(m_NaviMesh[i].m_kCenter, kColor, 0.1);

		glBegin(GL_TRIANGLES);
			glColor3f(0,1,0);
			glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[0] );
			glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[1] );
			glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[2] );
		glEnd();


/*		glLineWidth(3.0);
		glBegin(GL_LINES);
			SetEdgeColor(m_NaviMesh[i].m_aiLinks[0]);
			glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[0] );		glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[1] );
			
			SetEdgeColor(m_NaviMesh[i].m_aiLinks[1]);
			glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[1] );		glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[2] );
			
			SetEdgeColor(m_NaviMesh[i].m_aiLinks[2]);
			glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[2] );		glVertex3fv( (float*) &m_NaviMesh[i].m_kVertex[0] );
		glEnd();
		glLineWidth(1.0);
*/

/*		kColor.Set(1,0,0);
		for(int e=0; e<3; e++) {
			//Vector2 kCen2 = m_NaviMesh[i].m_kEdges[e].GetMidPoint( );
			//Vector3 kCenter(kCen2.x,-1,kCen2.y);
			Vector3 kCenter = m_NaviMesh[i].GetEdgeCenter(e);
			kCenter.y += 0.1;
			pkRender->Draw_MarkerCross(kCenter, kColor, 0.1);
			Vector2 kNormal = m_NaviMesh[i].m_kEdges[e].GetNormal();
			kNormal = kNormal * 0.4;

			glBegin(GL_LINES);
				glVertex3fv( (float*) &kCenter );	
				glVertex3f( kCenter.x + kNormal.x, kCenter.y, kCenter.z + kNormal.y);
			glEnd();
			}*/
		}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();

	glPopMatrix();
}


void P_PfMesh::LinkToConnectedCells(NaviMeshCell* pkNavCell)
{
	pkNavCell->m_aiLinks[0] = NULL;
	pkNavCell->m_aiLinks[1] = NULL;
	pkNavCell->m_aiLinks[2] = NULL;

	for(unsigned int i=1; i<m_NaviMesh.size(); i++) {
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
	//cout << "LinkCells: " << m_NaviMesh.size() << endl;
	int iTest = 0;

	for(unsigned int i=1; i<m_NaviMesh.size(); i++, iTest++) 
	{
		if(iTest >= 1000) {
         cout << ".";
			iTest = 0;
			}
		LinkToConnectedCells( &m_NaviMesh[i] );
		
	}

//	FlagExternalLinks();
}

NaviMeshCell* P_PfMesh::GetCurrentCell(Vector3 kPos)
{
	float fClosest = 100000000;
	int iIndex = 0;

	Vector2 kStartPos(kPos.x,kPos.z);

	for(unsigned int i=1; i<m_NaviMesh.size(); i++) {
		if(m_NaviMesh[i].m_bNonWalkable)
			continue;

		if(m_NaviMesh[i].IsPointInCell(kStartPos) == false )
			continue;

		Vector3 kDiff = m_NaviMesh[i].m_kCenter - kPos;
		float fDist = kDiff.Length();

		if(fDist < fClosest) {
			iIndex = i;
			fClosest = fDist;
			}
		}

	if(iIndex == 0)
		return NULL;
	
	m_pkSelected = &m_NaviMesh[iIndex];
	return &m_NaviMesh[iIndex];
}

NaviMeshCell* P_PfMesh::GetCell(Vector3 kPos)
{
//	kPos -= m_pkObject->GetWorldPosV();

	float fClosest = 100000000;
	int iIndex = 0;

	for(unsigned int i=1; i<m_NaviMesh.size(); i++) {
		Vector3 kDiff = m_NaviMesh[i].m_kCenter - kPos;
		float fDist = kDiff.Length();

		if(fDist < fClosest) {
			iIndex = i;
			fClosest = fDist;
			}
		}

	//m_pkSelected = &m_NaviMesh[iIndex];
	return &m_NaviMesh[iIndex];
}

void P_PfMesh::FlagExternalLinks()
{
	NaviMeshCell* pkNavCell;

	// Loop all cells
	for(unsigned int iNavMesh=1; iNavMesh<m_NaviMesh.size(); iNavMesh++) {
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
	NaviMeshCell* pkNavCell;

	// Loop all cells
	for(unsigned int iNavMesh=1; iNavMesh<m_NaviMesh.size(); iNavMesh++) {
		pkNavCell = &m_NaviMesh[iNavMesh];

		// Loop all edges in cell
		for(int i=0; i<3; i++) {
			if(pkNavCell->m_aiLinks[i] > 0)		continue;

			if( pkNavCell->IsConnected(NULL, kA, kB)) {
				pkNavCell->m_aiLinks[i] = -1;
				return pkNavCell;
				}
			}
		}

	return NULL;
}

bool P_PfMesh::LineOfSightTest(NaviMeshCell* pkStartCell, Vector3& kStartPos, NaviMeshCell* pkEndCell, Vector3& kEndPos)
{
	Line2D kPath(Vector2(kStartPos.x,kStartPos.z), Vector2(kEndPos.x, kEndPos.z));
	NaviMeshCell* pkNextCell = pkStartCell;
	int iNextCell;
	NaviMeshCell::CELL_SIDE					eSideNumber;
	NaviMeshCell::PATH_CLASSIFICATION	eResult;

	while((eResult = pkNextCell->ClassifyPath(kPath, iNextCell, eSideNumber, 0)) == NaviMeshCell::CELL_EXIT)
	{
		if(iNextCell <= 0)	return false;
		pkNextCell = &m_NaviMesh[iNextCell];
	}

	return (eResult == NaviMeshCell::CELL_END);
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

