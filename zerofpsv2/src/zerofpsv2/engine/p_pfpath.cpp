#include "p_pfpath.h"
#include "entity.h"
#include "astar.h"
#include "zerofps.h"
#include "../engine_systems/propertys/p_mad.h"

P_PfPath::P_PfPath()
{
	strcpy(m_acName,"P_PfPath");

//	m_iType = PROPERTY_TYPE_NORMAL | PROPERTY_TYPE_RENDER;
//	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;

	m_iType = PROPERTY_TYPE_NORMAL | PROPERTY_TYPE_RENDER;
	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;

	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkAStar=static_cast<AStar*>(g_ZFObjSys.GetObjectPtr("AStar"));	

	m_fSpeed = 5;
	m_bTilt = false;
	m_iNavMeshCell = 0;
	
	m_kOffset.Set(0,0,0);
	m_bHaveOffset = false;
}

P_PfPath::~P_PfPath()
{

}

void P_PfPath::Init()
{

}

void P_PfPath::RenderPath()
{
	if(!m_pkAStar->m_bDrawPaths) 
		return;

	Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render")); 
	unsigned int i;

	glColor3f(1,0,0);
	if(m_kRawPath.size() >= 2) {
		for(i=0; i<m_kRawPath.size() - 1; i++) {
			pkRender->Line(m_kRawPath[i].kPosition + Vector3(0,0.1,0), m_kRawPath[i+1].kPosition + Vector3(0,0.1,0));
			}	
		}

	glColor3f(0,1,0);
	if(m_kPath.size() >= 2) {
		for(i=0; i < (m_kPath.size() - 1); i++) {
			pkRender->Line(m_kPath[i] + Vector3(0,0.1,0), m_kPath[i+1] + Vector3(0,0.1,0));
			}	
		}
}


void P_PfPath::Update()
{
	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER) && m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER)) {
		RenderPath();
		return;
		}

	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_NORMAL) && m_pkObjMan->IsUpdate(PROPERTY_SIDE_CLIENT))
		return;

	Vector3 kPos = m_pkObject->GetWorldPosV();

	ZoneData* pkZone;
	P_PfMesh* pkMesh;
//	NaviMeshCell* pkEndCell;

	int iStartZone	= m_pkObjMan->GetZoneIndex(kPos,-1, false);
	pkZone = m_pkObjMan->GetZoneData(iStartZone);
	if(!pkZone)
		return;
	
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
	

	if(m_kPath.size() == 0)
		return;

	// Get Distance to next goal.
	Vector3 kGoal = m_kPath[m_iNextGoal] + m_kOffset;

	Vector3 kdiff = kGoal - kPos;
	float fdist = kdiff.Length();
	if(fdist < (m_fSpeed) * m_pkFps->m_pkObjectMan->GetSimDelta() ) 
	{
		m_pkObject->SetWorldPosV(kGoal);
		m_iNextGoal++;
		if(m_iNextGoal == m_kPath.size()) 
		{
			m_kPath.clear();
			m_kRawPath.clear();
			
			
			//play idle
			P_Mad* pm = (P_Mad*)m_pkObject->GetProperty("P_Mad");
			if(pm)
				pm->SetAnimation("idle",0);
		}
		else
		{
		
		}

		return;
	}

	kdiff.Normalize();
	kPos += (kdiff * m_fSpeed) * m_pkFps->m_pkObjectMan->GetSimDelta();
	m_pkObject->SetWorldPosV(kPos);


	//setup character orientation
	if(!m_bTilt)
		kdiff.y = 0;
	
	Matrix4 kRotM;
	kRotM.LookDir(kdiff.Unit(),Vector3(0,1,0));
	kRotM.Transponse();
		
	m_pkObject->SetLocalRotM(kRotM);
		


}

void P_PfPath::Save(ZFIoInterface* pkFile)
{
	pkFile->Write((void*)&m_bTilt,sizeof(m_bTilt),1);
	pkFile->Write((void*)&m_fSpeed,sizeof(m_fSpeed),1);	
	pkFile->Write((void*)&m_iNextGoal,sizeof(m_iNextGoal),1);	
	
	int nodes = m_kPath.size();
	pkFile->Write((void*)&nodes,sizeof(nodes),1);	
	for(int i = 0;i<nodes;i++)
	{
		pkFile->Write((void*)&(m_kPath[i]),sizeof(m_kPath[i]),1);	
	}
}

void P_PfPath::Load(ZFIoInterface* pkFile)
{
	pkFile->Read((void*)&m_bTilt,sizeof(m_bTilt),1);
	pkFile->Read((void*)&m_fSpeed,sizeof(m_fSpeed),1);	
	pkFile->Read((void*)&m_iNextGoal,sizeof(m_iNextGoal),1);		

	m_kPath.clear();
	int nodes;
	pkFile->Read((void*)&nodes,sizeof(nodes),1);			
	for(int i = 0;i<nodes;i++)
	{
		Vector3 pos;
		pkFile->Read((void*)&pos,sizeof(pos),1);	
		m_kPath.push_back(pos);
	}
}

void P_PfPath::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{

}

void P_PfPath::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{


}

vector<PropertyValues> P_PfPath::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);
	
	kReturn[0].kValueName="Tilt";
	kReturn[0].iValueType=VALUETYPE_BOOL;
	kReturn[0].pkValue=(void*)&m_bTilt;
	
	
	
	return kReturn;
}

void P_PfPath::SetPath(vector<Vector3> kPath)
{
	m_kPath = kPath;
	m_iNextGoal = 0;
}

bool P_PfPath::MakePathFind(Vector3 kDestination)
{
	vector<PathNode> kPath;

	/* Vim Test Path*/
	Vector3 kPathStart = m_pkObject->GetWorldPosV();
	Vector3 kPathEnd   = kDestination;
	cout << "Making PathFind: ";
	kPathStart.Print();
	kPathEnd.Print();
	cout << endl;

	kPath.clear();
	bool bres = m_pkAStar->GetFullPath(kPathStart,kPathEnd,kPath);

	if(bres) 
	{
		//reverse(kPath.begin(), kPath.end());
		//kPath.push_back(kPathEnd);
		m_kRawPath = kPath;
		SetPath( m_pkAStar->OptimizePath(kPath) );
		
		//play run animation
		P_Mad* pm = (P_Mad*)m_pkObject->GetProperty("P_Mad");
		if(pm)
			pm->SetAnimation("run",0);

		return true;
	}
	
	return false;
}


void P_PfPath::SetupOffset()
{
	m_bHaveOffset = true;
	return;

	P_Mad* pm = (P_Mad*)m_pkObject->GetProperty("P_Mad");
	if(pm)
	{	
		//m_kOffset = pm->GetJointPosition("fem_run_c_root");
		m_kOffset = Vector3(0,1.1,0);
		m_bHaveOffset = true;
		
		cout<<"Got offset: "<<m_kOffset.x<<" "<<m_kOffset.y<<" "<<m_kOffset.z<<endl;
	}
}


bool P_PfPath::HavePath()
{
	if(m_kPath.size() == 0 )
		return false;
	else
		return true;
}

/*
vector<Vector3> P_PfPath::OptimizePath(vector<Vector3> kInPath)
{
	
}*/


Property* Create_P_PfPath()
{
	return new P_PfPath;
}



