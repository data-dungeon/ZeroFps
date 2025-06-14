#include "p_pfpath.h"
#include "../../engine/entity.h"
#include "../common/astar.h"
#include "../../engine/zerofps.h"
#include "p_mad.h"
#include "p_tcs.h"


P_PfPath::P_PfPath()  : Property("P_PfPath")
{

//	m_iType = PROPERTY_TYPE_NORMAL | PROPERTY_TYPE_RENDER;
//	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;

// 	m_iType = PROPERTY_TYPE_NORMAL | PROPERTY_TYPE_RENDER;
	m_iType = PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER;// | PROPERTY_SIDE_CLIENT;

	m_pkFps		=	static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkAStar	=	static_cast<ZSSAStar*>(g_ZFObjSys.GetObjectPtr("ZSSAStar"));	
	m_pkRender	=	static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender")); 

	m_fSpeed			= 5;
	m_bTilt			= false;
	m_iNavMeshCell = 0;
	
	m_kOffset.Set(0,0.1,0);
	m_bHaveOffset	= false;

	m_kRunAnim = "run";
	m_kIdleAnim = "idle";


	m_kPropertyValues.push_back(PropertyValues("Tilt",VALUETYPE_BOOL,(void*)&m_bTilt));

}


P_PfPath::~P_PfPath()		{	}


void P_PfPath::Init()		{	}

/**	\brief	Renders the path.
*/
void P_PfPath::RenderPath()
{
	if(!m_pkAStar->m_kbDrawPaths.GetBool()) 
		return;

	unsigned int i;
	Vector3 kColor;
	Vector3 kPathOffset(0,0,0);

	// Render the raw path.
	kColor = m_pkRender->GetEditColor("ai/rawpath");
	glColor3f(kColor.x,kColor.y,kColor.z);
	if(m_kRawPath.size() >= 2) 
	{
		for(i=0; i<m_kRawPath.size() - 1; i++) 
		{
			m_pkRender->Line(m_kRawPath[i].kPosition + kPathOffset, m_kRawPath[i+1].kPosition + kPathOffset);
		}	
	}
	
	// Render the optimized path.
	kColor = m_pkRender->GetEditColor("ai/path");
	glColor3f(kColor.x,kColor.y,kColor.z);
	if(m_kPath.size() >= 2) 
	{
		for(i=0; i < (m_kPath.size() - 1); i++) 
		{
			m_pkRender->Line(m_kPath[i] + kPathOffset, m_kPath[i+1] + kPathOffset);
		}	
	}
}


void P_PfPath::Update()
{
	if( m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER)) 
	{
		RenderPath();
		return;
	}

	if( m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL) && m_pkEntityManager->IsUpdate(PROPERTY_SIDE_CLIENT))
		return;

	//TCS TEST
	P_Tcs* pkTcs = NULL;//(P_Tcs*)m_pkObject->GetProperty("P_Tcs");
		
		
	Vector3 kPos = m_pkEntity->GetWorldPosV();

	// This section are about to be removed, i think, could be, don't know what it is doing... - Vim
				ZoneData* pkZone;
				P_PfMesh* pkMesh;

				int iStartZone	= m_pkEntityManager->GetZoneIndex(kPos,-1, false);
				pkZone = m_pkEntityManager->GetZoneData(iStartZone);
				if(!pkZone)
					return;
				
				if(pkZone->m_pkZone == NULL)
					return ;
				pkMesh = (P_PfMesh*)pkZone->m_pkZone->GetProperty("P_PfMesh");
				if(pkMesh == NULL)
					return ;

				NaviMeshCell* pkStartCell = pkMesh->GetCurrentCell( kPos );
				if(pkStartCell == NULL) 
				{
					cout << "No StartCell Found at current position - Lets Ignore it and see what happens" << endl;
					return;
				}
	

	if(m_kPath.size() == 0)
		return;
	if(m_iNextGoal == (int) m_kPath.size())
		return;

	// Get Distance to next goal.
	Vector3 kGoal = m_kPath[m_iNextGoal] + m_kOffset;
	Vector3 kdiff = kGoal - kPos;
	kdiff.y = 0;
	
	// Check if we are so close to the goal that we will reach it in this frame. If so move to it and go for the next one.
	float fdist = kdiff.Length();
	if(fdist < (m_fSpeed) * m_pkFps->m_pkEntityManager->GetSimDelta() ) 
	{
		if(!pkTcs)
			m_pkEntity->SetWorldPosV(kGoal);
		
		m_iNextGoal++;
		if(m_iNextGoal == (int) m_kPath.size()) 
		{
			if(!m_pkAStar->m_kbDrawPaths.GetBool())
			{
				m_kPath.clear(); // added again by zerom, havepath didn't work else
				m_kRawPath.clear();
			}

			//play idle
			P_Mad* pm = (P_Mad*)m_pkEntity->GetProperty("P_Mad");
			if(pm)
				pm->SetAnimation((char*)m_kIdleAnim.c_str(),0);
					
		
			if(pkTcs)
				pkTcs->ApplyForce(Vector3(0,0,0),Vector3(0,0,0));
		
		}
		else
		{
		}

		return;
	}

	// Move towards current goal.
	if(kdiff.Length() != 0)
		kdiff.Normalize();
		
	kPos += (kdiff * m_fSpeed) * m_pkFps->m_pkEntityManager->GetSimDelta();
	
	//TCS TEST
	if(pkTcs)
	{
		pkTcs->ApplyForce(Vector3(0,0,0),kdiff*20);
	}
	else
		m_pkEntity->SetWorldPosV(kPos);
	
		
	//setup character orientation
	if(!m_bTilt)
		kdiff.y = 0;
	
	// Zerom bug fixes, on low speed goal and pos is same and craches on kdiff.unit
	if ( kGoal.x != kPos.x || kGoal.z != kPos.z )
	{
		Matrix4 kRotM;
		kRotM.LookDir(kdiff.Unit(),Vector3(0,1,0));
		kRotM.Transponse();
		
		m_pkEntity->SetLocalRotM(kRotM);
	}
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


void P_PfPath::Load(ZFIoInterface* pkFile,int iVersion)
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





void P_PfPath::SetPath(vector<Vector3> kPath)
{
	m_kPath = kPath;
	m_iNextGoal = 1;			//dvoid...satte till 1 s� inte gubben rycker n�r man klickar m�nga g�nger
}

/**	\brief	Makes a pathfind to a choosen postition.
*/
bool P_PfPath::MakePathFind(Vector3 kDestination)
{
	vector<PathNode> kPath;

	Vector3 kPathStart = m_pkEntity->GetWorldPosV();
	Vector3 kPathEnd   = kDestination;
	
	kPath.clear();
	bool bPathFound = m_pkAStar->GetFullPath(kPathStart,kPathEnd,kPath);

	if(bPathFound) 
	{
		m_kRawPath = kPath;
		SetPath( m_pkAStar->OptimizePath(kPath) );
		
		
		
		//play run animation
		P_Mad* pm = (P_Mad*)m_pkEntity->GetProperty("P_Mad");
		if(pm)
			if(pm->GetCurrentAnimationName() != m_kRunAnim)
				pm->SetAnimation((char*)m_kRunAnim.c_str(),0);
	}
	
	return bPathFound;
}


void P_PfPath::SetupOffset()
{
	m_bHaveOffset = true;
	return;

/*	P_Mad* pm = (P_Mad*)m_pkObject->GetProperty("P_Mad");
	if(pm)
	{	
		//m_kOffset = pm->GetJointPosition("fem_run_c_root");
		m_kOffset = Vector3(0,1.1,0);
		m_bHaveOffset = true;
		//	cout<<"Got offset: "<<m_kOffset.x<<" "<<m_kOffset.y<<" "<<m_kOffset.z<<endl;
	}*/
}


bool P_PfPath::HavePath()
{
	if(m_kPath.size() == 0 )
		return false;
	else
		return true;
}


Property* Create_P_PfPath()
{
	return new P_PfPath;
}





// vector<PropertyValues> P_PfPath::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn(1);
// 	
// 	kReturn[0].kValueName="Tilt";
// 	kReturn[0].iValueType=VALUETYPE_BOOL;
// 	kReturn[0].pkValue=(void*)&m_bTilt;
// 	
// 	return kReturn;
// }