#include "p_pfpath.h"
#include "entity.h"
#include "astar.h"
#include "zerofps.h"
#include "../engine_systems/propertys/p_mad.h"

P_PfPath::P_PfPath()
{
	strcpy(m_acName,"P_PfPath");
	m_iType = PROPERTY_TYPE_NORMAL;// | PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;

	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkAStar=static_cast<AStar*>(g_ZFObjSys.GetObjectPtr("AStar"));	

	m_fSpeed = 3;
	m_iNavMeshCell = 0;
}

P_PfPath::~P_PfPath()
{

}

void P_PfPath::Update()
{
	Vector3 kPos = m_pkObject->GetWorldPosV();

	ZoneData* pkZone;
	P_PfMesh* pkMesh;
	NaviMeshCell* pkEndCell;

	

	int iStartZone	= m_pkObjMan->GetZoneIndex(kPos,-1, false);
	pkZone = m_pkObjMan->GetZoneData(iStartZone);
	if(pkZone->m_pkZone == NULL)
		return ;
	pkMesh = (P_PfMesh*)pkZone->m_pkZone->GetProperty("P_PfMesh");
	if(pkMesh == NULL)
		return ;

	NaviMeshCell* pkStartCell = pkMesh->GetCurrentCell( kPos );
	if(pkStartCell == NULL) {
		cout << "No StartCell Found at current position" << endl;
		return ;
		}
	
	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER) ) {
		Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render")); 

		for(int i=0; i<m_kPath.size(); i++) {
			pkRender->Draw_MarkerCross(m_kPath[i], Vector3(1,1,0), 1);
			}	

		//cout << "Render Path"<< endl;
		return;
		}

	if(m_kPath.size() == 0) {
/*		Vector3 kStart;
		Vector3 kEnd;
		kStart = m_pkObject->GetWorldPosV();
		kEnd = kStart + Vector3(100 - rand()%200,0,100 - rand()%200);
		vector<Vector3>	kNewPath;

		if(m_pkZeroFps->m_pkAStar->GetFullPath(kStart,kEnd,kNewPath)) {
			reverse(kNewPath.begin(), kNewPath.end());
			SetPath(kNewPath);
			}*/

		return;
		}

	static Vector3 kOffset = Vector3(0,0,0);;
	

	// Get Distance to next goal.
	Vector3 kGoal = m_kPath[m_iNextGoal] + kOffset;

	Vector3 kdiff = kGoal - kPos;
	float fdist = kdiff.Length();
	if(fdist < 0.2) {
		m_pkObject->SetWorldPosV(kGoal);
		m_iNextGoal++;
		if(m_iNextGoal == m_kPath.size()) 
		{
			m_kPath.clear();
			
			
			//play idle
			P_Mad* pm = (P_Mad*)m_pkObject->GetProperty("P_Mad");
			if(pm)
				pm->SetAnimation("idle",0);
		}
		else
		{
			//play run animation
			P_Mad* pm = (P_Mad*)m_pkObject->GetProperty("P_Mad");
			if(pm)
				pm->SetAnimation("run",0);

			

		}

		return;
		}

	kdiff.Normalize();
	kPos += (kdiff * m_fSpeed) * m_pkFps->GetFrameTime();
	m_pkObject->SetWorldPosV(kPos);

}

void P_PfPath::Save(ZFIoInterface* pkFile)
{

}

void P_PfPath::Load(ZFIoInterface* pkFile)
{

}

void P_PfPath::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{

}

void P_PfPath::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
}

vector<PropertyValues> P_PfPath::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
	return kReturn;
}

void P_PfPath::SetPath(vector<Vector3> kPath)
{
	m_kPath = kPath;
	m_iNextGoal = 0;
}

bool P_PfPath::MakePathFind(Vector3 kDestination)
{
	vector<Vector3> kPath;

	/* Vim Test Path*/
	Vector3 kPathStart = m_pkObject->GetWorldPosV();
	Vector3 kPathEnd   = kDestination;
	kPath.clear();
	bool bres = m_pkAStar->GetFullPath(kPathStart,kPathEnd,kPath);

	if(bres) 
	{
		reverse(kPath.begin(), kPath.end());
		kPath.push_back(kPathEnd);
		SetPath(kPath);
		return true;
	}
	
	return false;
}



Property* Create_P_PfPath()
{
	return new P_PfPath;
}

