#include "p_mad.h"
#include "../../engine/entity.h"
#include "../../render/render.h"
#include "../../basic/zfsystem.h"
#include "../script_interfaces/si_objectmanager.h" 

using namespace ObjectManagerLua;


extern int		g_iMadLODLock;
extern float	g_fMadLODScale;
 
P_Mad::P_Mad()
{

	m_pkZeroFps =		static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkRender	=		static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render")); 
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem")); 
	m_pkLight=		static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light")); 
		
	strcpy(m_acName,"P_Mad");
	m_bNetwork	 = true;
	m_iVersion = 4;
	
	m_iSortPlace = 0;
	
	m_iType = PROPERTY_TYPE_RENDER | PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;
	
	SetVisible(true);
	m_bCanBeInvisible = false;
	m_iShadowGroup = 0;


	
	m_fScale	 = 1.0;
	m_kOffset.Set(0,0,0);
	
  	m_fLastAnimationUpdateTime = m_pkZeroFps->GetEngineTime();
//  	m_fLastAnimationUpdateTime = m_pkEntityManager->GetSimTime();
	m_iLastAnimationUpdateFrame = -1;

	
	//bounding box
	m_bHaveAABB		=false;
	m_AABBMax		=Vector3::ZERO;
	m_AABBMin		=Vector3::ZERO;
	m_fOldScale		=0;
	m_fLastAABBTest=0;	
	m_kLastRot.Zero();
	
}

void P_Mad::Update()
{
	Mad_Core* pkCore = static_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;
		
		
// TODO: When MAD2.0 is finished, search for mesh with name lowpoly instead of always using mesh2
// find lowpoly mesh, if exist

// and MOVE to somewhere else..shouldn't need to be every update.. = ugly
	if (pkCore->NumOfMeshes() > 1)
		m_iCollisionMeshID = 1;
	else
		m_iCollisionMeshID = 0;
/*	
	for (int i = 0; i < pkCore->NumOfMeshes(); i++)
	{
		if ( strcmp(pkCore->GetMeshByID(i)->m_acName, "lowpoly") == 0 )
			m_iCollisionMeshID = i;
	}
*/	
	 
	//if no rendering is done, update animation in normal updates
	if( m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL) && ( !m_pkZeroFps->GetRenderOn() || m_pkZeroFps->GetMinimized() ) )
	{
		DoAnimationUpdate();
	}

	//do render update
	if( m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER) ) 
	{		
		StartProfileTimer("r___mad");				
				
		DoAnimationUpdate();
		
		
		//Cull against sphere
		if(!m_pkZeroFps->GetCam()->GetFrustum()->SphereInFrustum(m_pkEntity->GetWorldPosV(),GetRadius()))
		{
			StopProfileTimer("r___mad");
			return;
		}

		//get entity rotation
		static Matrix3 kRot;
		kRot = m_pkEntity->GetWorldRotM();		
		
		//passed sphere, do AABB test
		if(m_bHaveAABB)
		{
			if(GetCurrentAnimation() == MAD_NOANIMINDEX &&
				m_fScale == m_fOldScale &&
				m_kLastRot == kRot)		
			{
				//have up to date AABB
				if(!m_pkZeroFps->GetCam()->GetFrustum()->CubeInFrustum(m_AABBMin + m_pkEntity->GetIWorldPosV(),m_AABBMax + m_pkEntity->GetIWorldPosV()))
				{
					StopProfileTimer("r___mad");
					return;
				}								
			}
			else
			{
				//AABB is not up to date
				//cout<<"aabb not up do date,removing"<<endl;
				m_bHaveAABB = false;
			}
		}
			
		
		//always update bones
		UpdateBones();
		
		if(m_bIsVisible)
		{			
			//update AABB 
			if(!m_bHaveAABB)
			{
				if(m_pkZeroFps->GetEngineTime() - m_fLastAABBTest > 2)
				{
					if(	GetCurrentAnimation() == MAD_NOANIMINDEX &&
							m_fScale == m_fOldScale &&
							m_kLastRot == kRot)
					{
						//create AABB
						m_bHaveAABB = true;
						
						//cout<<"creating new AABB"<<endl;
						CreateAABB();
					}
					else
					{
						//update test information
						m_fLastAABBTest = m_pkZeroFps->GetEngineTime();
						m_fOldScale = m_fScale;
						m_kLastRot = kRot;
					}
				}
			}
			
			//update lighting
			m_pkLight->Update(&m_kLightProfile,GetEntity()->GetWorldPosV());						
		
			m_pkZShaderSystem->MatrixPush();
				m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetIWorldPosV() + m_kOffset);
				m_pkZShaderSystem->MatrixMult(Matrix4(kRot));
				m_pkZShaderSystem->MatrixScale(m_fScale);
	
				
				
				Draw_All(m_pkZeroFps->m_iMadDraw);
			m_pkZShaderSystem->MatrixPop();
		}

		if(m_pkZeroFps->m_iMadDraw & MAD_DRAW_SPHERE) 
		{
			m_pkZShaderSystem->MatrixPush();
				
				if(m_bHaveAABB)
				{
					m_pkRender->DrawAABB(m_AABBMin + m_pkEntity->GetIWorldPosV(),m_AABBMax + m_pkEntity->GetIWorldPosV(),Vector3(1,1,1));				
				}
				else
				{
					m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetIWorldPosV() + m_kOffset);					
					m_pkRender->Sphere(Vector3::ZERO, GetRadius(), 2, Vector3(1,1,1),false);				
				}
				
			m_pkZShaderSystem->MatrixPop();
			
			
		}
				
		//increse mad counter
		m_pkZeroFps->m_iNumOfMadRender++;
				
		StopProfileTimer("r___mad");			
	}	
}

void P_Mad::CreateAABB()
{
	static Vector3 kVert;
	
	m_AABBMax.Set(-99999999,-99999999,-99999999);
	m_AABBMin.Set(99999999,99999999,99999999);
	Matrix3 kRot = m_pkEntity->GetWorldRotM();
		
	Mad_Core* pkCore = static_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	
	int iNumOfMesh = m_kActiveMesh.size();	//GetNumOfMesh();	
	for(int iM = 0; iM <iNumOfMesh; iM++) 
	{
		SelectMesh(m_kActiveMesh[iM]);		//SelectMesh(iM);
		pkCore->PrepareMesh(pkCore->GetMeshByID(m_kActiveMesh[iM]));
		
		Vector3* kVertexPointer = GetVerticesPtr();
		
		int iVerts = GetNumVertices();		
		for(int i = 0;i<iVerts;i++)
		{
			kVert = kRot.VectorTransform(kVertexPointer[i]) * m_fScale;
		
			
			//max
			if(kVert.x > m_AABBMax.x)
				m_AABBMax.x = kVert.x;

			if(kVert.y > m_AABBMax.y)
				m_AABBMax.y = kVert.y;
			
			if(kVert.z > m_AABBMax.z)
				m_AABBMax.z = kVert.z;	
				
			//min	
			if(kVert.x < m_AABBMin.x)
				m_AABBMin.x = kVert.x;

			if(kVert.y < m_AABBMin.y)
				m_AABBMin.y = kVert.y;
			
			if(kVert.z < m_AABBMin.z)
				m_AABBMin.z = kVert.z;				
		}
					
	}		
	
}

void P_Mad::DoAnimationUpdate()
{
	if(m_iLastAnimationUpdateFrame != m_pkZeroFps->GetCurrentFrame())
	{
//    		float fCurrentTime = m_pkEntityManager->GetSimTime();		
  		float fCurrentTime = m_pkZeroFps->GetEngineTime();
				
		UpdateAnimation(fCurrentTime - m_fLastAnimationUpdateTime);
	
		m_iLastAnimationUpdateFrame = m_pkZeroFps->GetCurrentFrame();
		m_fLastAnimationUpdateTime = fCurrentTime;
	}
}

void P_Mad::SetBase(const char* acName)
{
	SetBasePtr(string(acName));		
	m_pkEntity->SetRadius(GetRadius());
	SetNetUpdateFlag(true);
}

void P_Mad::SetScale(float fScale)
{
	Mad_Modell::SetScale(fScale);
	m_pkEntity->SetRadius(GetRadius());	
	
	SetNetUpdateFlag(true);

}


int P_Mad::GetCurrentAnimation()
{
	return Mad_Modell::GetCurrentAnimation();
}

string P_Mad::GetCurrentAnimationName()
{
	return Mad_Modell::GetCurrentAnimationName();
}

void P_Mad::SetAnimation(const char* szName, float fStartTime)
{
	PlayAnimation(szName,fStartTime);
	Mad_Modell::SetNextAnimation(-1);
	SetNetUpdateFlag(true);	
}

void P_Mad::SetNextAnimation(const char* szName)
{
	Mad_Modell::SetNextAnimation(szName);
	SetNetUpdateFlag(true);	
}

void P_Mad::SetNextAnimation(int iAnimID)
{
	Mad_Modell::SetNextAnimation(iAnimID);
	SetNetUpdateFlag(true);	
}


void P_Mad::Save(ZFIoInterface* pkPackage)
{	
	char temp[128];
	strcpy(temp,m_kMadFile.c_str());

	pkPackage->Write((void*)temp,128,1);
	pkPackage->Write((void*)&m_fScale,4,1);
	pkPackage->Write((void*)&m_bCanBeInvisible,sizeof(m_bCanBeInvisible),1);
	pkPackage->Write((void*)&m_iShadowGroup,sizeof(m_iShadowGroup),1);
	pkPackage->Write((void*)&m_kOffset,sizeof(m_kOffset),1);
	pkPackage->Write((void*)&m_iSortPlace,sizeof(m_iSortPlace),1);
	
	
	pkPackage->Write( iActiveAnimation );
	pkPackage->Write( m_iNextAnimation );	
}

void P_Mad::Load(ZFIoInterface* pkPackage,int iVersion)
{
	switch(iVersion)
	{
		case 1:
		{
			char temp[128];
			pkPackage->Read((void*)temp,128,1);	
			SetBase(temp);
			
			float scale;
			pkPackage->Read((void*)&scale,4,1);
			SetScale(scale);
			
			pkPackage->Read((void*)&m_bCanBeInvisible,sizeof(m_bCanBeInvisible),1);
			pkPackage->Read((void*)&m_iShadowGroup,sizeof(m_iShadowGroup),1);
			break;
		}
		
		case 2:
		{
			char temp[128];
			pkPackage->Read((void*)temp,128,1);	
			SetBase(temp);
			
			float scale;
			pkPackage->Read((void*)&scale,4,1);
			SetScale(scale);
			
			pkPackage->Read((void*)&m_bCanBeInvisible,sizeof(m_bCanBeInvisible),1);
			pkPackage->Read((void*)&m_iShadowGroup,sizeof(m_iShadowGroup),1);
			pkPackage->Read((void*)&m_kOffset,sizeof(m_kOffset),1);
			break;			
		}
		case 3:
		{
			char temp[128];
			pkPackage->Read((void*)temp,128,1);	
			SetBase(temp);
			
			float scale;
			pkPackage->Read((void*)&scale,4,1);
			SetScale(scale);
			
			pkPackage->Read((void*)&m_bCanBeInvisible,sizeof(m_bCanBeInvisible),1);
			pkPackage->Read((void*)&m_iShadowGroup,sizeof(m_iShadowGroup),1);
			pkPackage->Read((void*)&m_kOffset,sizeof(m_kOffset),1);
			pkPackage->Read((void*)&m_iSortPlace,sizeof(m_iSortPlace),1);
			break;			
		}
		
		case 4:
		{
			char temp[128];
			pkPackage->Read((void*)temp,128,1);	
			SetBase(temp);
			
			float scale;
			pkPackage->Read((void*)&scale,4,1);
			SetScale(scale);
			
			pkPackage->Read((void*)&m_bCanBeInvisible,sizeof(m_bCanBeInvisible),1);
			pkPackage->Read((void*)&m_iShadowGroup,sizeof(m_iShadowGroup),1);
			pkPackage->Read((void*)&m_kOffset,sizeof(m_kOffset),1);
			pkPackage->Read((void*)&m_iSortPlace,sizeof(m_iSortPlace),1);
			
			pkPackage->Read( iActiveAnimation );
			pkPackage->Read( m_iNextAnimation );				
			break;			
		}
		
		
		default:
		{
			cout<<"Tried to load unkown version of p_mad"<<endl;
			break;
		}
	}
	
	SetNetUpdateFlag(true);	
}

void P_Mad::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write_Str(m_kMadFile.c_str());
	pkNetPacket->Write( m_fScale );
	pkNetPacket->Write( m_bCanBeInvisible );	
	pkNetPacket->Write( iActiveAnimation );
	pkNetPacket->Write( m_iNextAnimation );
	pkNetPacket->Write( m_kOffset );
	pkNetPacket->Write( m_iShadowGroup );
	
	
	unsigned char ucNumOfMesh = m_kActiveMesh.size();
	int iMesh;

	pkNetPacket->Write( ucNumOfMesh );
	for(int i=0; i<ucNumOfMesh; i++) {
		iMesh = m_kActiveMesh[i] ;
		pkNetPacket->Write( iMesh );
		}

	SetNetUpdateFlag(iConnectionID,false);
}
 
void P_Mad::PackFrom(NetPacket* pkNetPacket, int iConnectionID )
{
	char temp[128];
	pkNetPacket->Read_Str(temp);
	SetBase(temp);
	pkNetPacket->Read(m_fScale);

	pkNetPacket->Read(m_bCanBeInvisible);

	int iNewAnim;
	pkNetPacket->Read( iNewAnim );
	if(iNewAnim != iActiveAnimation)
		PlayAnimation(iNewAnim, 0);

	int iNewNextAnim;
	pkNetPacket->Read( iNewNextAnim );	
	Mad_Modell::SetNextAnimation(iNewNextAnim);

	pkNetPacket->Read( m_kOffset );
	pkNetPacket->Read( m_iShadowGroup );
	
	unsigned char ucNumOfMesh;
	int iMesh;

	ClearAllMesh();
	pkNetPacket->Read( ucNumOfMesh );
	for(int i=0; i<ucNumOfMesh; i++) 
	{
		pkNetPacket->Read( iMesh );
		AddMesh(iMesh);
	}
}

bool P_Mad::AddMesh(int iSId)
{
	SetNetUpdateFlag(true);	
	return Mad_Modell::AddMesh(iSId);
}

vector<PropertyValues> P_Mad::GetPropertyValues()
{
	vector<PropertyValues> kReturn(6);
	
	kReturn[0].kValueName = "m_fScale";
	kReturn[0].iValueType = VALUETYPE_FLOAT;
	kReturn[0].pkValue    = (void*)&m_fScale;

	kReturn[1].kValueName = "m_kMadFile";
	kReturn[1].iValueType = VALUETYPE_STRING;
	kReturn[1].pkValue    = (void*)&m_kMadFile;

	kReturn[2].kValueName = "m_bCanBeInvisible";
	kReturn[2].iValueType = VALUETYPE_BOOL;
	kReturn[2].pkValue    = (void*)&m_bCanBeInvisible;

	kReturn[3].kValueName = "m_iShadowGroup";
	kReturn[3].iValueType = VALUETYPE_INT;
	kReturn[3].pkValue    = (void*)&m_iShadowGroup;

	kReturn[4].kValueName = "m_iSortPlace";
	kReturn[4].iValueType = VALUETYPE_INT;
	kReturn[4].pkValue    = (void*)&m_iSortPlace;

	kReturn[5].kValueName = "m_kOffset";
	kReturn[5].iValueType = VALUETYPE_VECTOR3;
	kReturn[5].pkValue    = (void*)&m_kOffset;
		
	return kReturn;
}

bool P_Mad::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_kMadFile") == 0) {
		SetBase(kValue.c_str());
		return true;
	}
	
	if(strcmp(kValueName.c_str(), "m_fScale") == 0) {
		m_fScale = float( atof(kValue.c_str()) );
		m_pkEntity->SetRadius(GetRadius());
				
		SetNetUpdateFlag(true);
		return true;
	}

	return false;
}

bool P_Mad::TestLine(Vector3 kPos,Vector3 kDir,bool bSphereOnly ,bool bIgnoreY )
{
	if(bIgnoreY)
	{
		kPos.y = 0;
		kDir.y = 0;
	}

	if(LineVSSphere(kPos,kDir,bIgnoreY))
	{
		if(bSphereOnly)
			return true;
	
		return LineVSMesh(kPos,kDir);
	}

	return false;
}

bool P_Mad::LineVSSphere(const Vector3& kPos,Vector3 &kDir,bool bIgnoreY )
{
	static Plane P;
	static Vector3 k;

	
	P.Set(kDir,kPos);	
	if(!P.SphereInside( m_pkEntity->GetWorldPosV(),GetRadius()))
	{
		return false;
	}

	kDir.Normalize();					
	Vector3 c = m_pkEntity->GetWorldPosV() - kPos;
	
	if(bIgnoreY)
		c.y = 0;
			
	k = kDir.Proj(c);			
	float cdis=c.Length();
	float kdis=k.Length();
	float Distance = float( sqrt((cdis*cdis)-(kdis*kdis)) );
	
	//set col pos
	m_kColPos = m_pkEntity->GetWorldPosV();
		
	return (Distance < GetRadius());	
}

bool P_Mad::LineVSMesh(const Vector3& kPos,const Vector3& kDir)
{

	vector<Mad_Face>*			pkFaces;			// Faces in mesh.
	vector<Vector3>*			pkVertex;			// Vertex frames for mesh.
	vector<Vector3>*			pkNormal;

	Mad_Core*					pkCore;
	Mad_CoreMesh*				pkCoreMesh;

	//setup all pointers
	pkCore = (Mad_Core*)kMadHandle.GetResourcePtr(); 
	if(!pkCore)
	{
		return false;
	}
		
	pkCoreMesh = pkCore->GetMeshByID(m_iCollisionMeshID);		
	if(!pkCoreMesh)
	{
		return false;
	}
		
	pkCore->PrepareMesh(	pkCoreMesh );	
		
	pkFaces =	pkCoreMesh->GetLODMesh(0)->GetFacesPointer();
	pkVertex = (*pkCoreMesh->GetLODMesh(0)->GetVertexFramePointer())[0].GetVertexPointer();
	pkNormal = (*pkCoreMesh->GetLODMesh(0)->GetVertexFramePointer())[0].GetNormalPointer();
	
	GenerateModelMatrix();
	
	
	float		fDist = 9999999;
	Vector3	kClosestColPos;
	m_iColFace = -1;
	
	//setup a virtual point in space
	Vector3 Point2 = kPos+(kDir*1000);
	
	Vector3 data[3];
	float d;
	for(unsigned int i=0;i<pkFaces->size();i++)
	{
		
		//get and transform vertexes		
		for(int j=0;j<3;j++)
			data[j] = m_kModelMatrix.VectorTransform((*pkVertex)[ (*pkFaces)[i].iIndex[j]]);
		
		
		
		if(TestPolygon(data,kPos,Point2))
		{	
			d = m_kColPos.DistanceTo(kPos); 
			
			//if this point is closer than the last one, set it as closest
			if(d < fDist)
			{	
				m_iColFace = i;
				kClosestColPos=m_kColPos;
				fDist = d;
			}
		}
	}		
	
	//if we colided whit something, set colpos 
	if(m_iColFace != -1)
	{
		m_kColPos=kClosestColPos; 
		//m_pkZeroFps->m_pkRender->Sphere(m_kColPos,0.1,5,Vector3(1,0,0),true);					
		
		return true;
	}
	
	return false;
}

bool P_Mad::TestPolygon(Vector3* kVerts,Vector3 kPos1,Vector3 kPos2)
{		
	static Plane P;
	static Vector3 V1,V2,Normal;

	V1 = kVerts[1] - kVerts[0];
	V2 = kVerts[2] - kVerts[0];		
	Normal= V1.Cross(V2);
	
	if(Normal.Length() == 0)
		return false;
	
	Normal.Normalize();
	P.m_fD = -Normal.Dot(kVerts[0]);	
	P.m_kNormal = Normal;

	if(P.LineTest(kPos1 , kPos2 ,&m_kColPos))
		if(TestSides(kVerts,&Normal,m_kColPos))
			return true;
	
	return false;
}

bool P_Mad::TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos)
{
  Vector3 e10=kVerts[1]-kVerts[0];
  Vector3 e20=kVerts[2]-kVerts[0];
  float a = e10.Dot(e10);
  float b = e10.Dot(e20);
  float c = e20.Dot(e20);
  float ac_bb=(a*c)-(b*b);
  Vector3 vp(kPos.x-kVerts[0].x, kPos.y-kVerts[0].y, kPos.z-kVerts[0].z);
  float d = vp.Dot(e10);
  float e = vp.Dot(e20);
  float x = (d*c)-(e*b);
  float y = (e*a)-(d*b);
  float z = x+y-ac_bb;
  return (( ((unsigned int &)z) & ~(((unsigned int &)x)|((unsigned int &)y)) ) & 0x80000000) != 0;

/*	


//	Plane side[6];
	static Plane side[3]; 
	static Vector3 V1,V2,V3;
	
	V1 = kVerts[1] - kVerts[0];
	V2 = kVerts[2] - kVerts[1];	
	V3 = kVerts[0] - kVerts[2];	
	
	side[0].m_kNormal = pkNormal->Cross(V1).Unit();
	side[1].m_kNormal = pkNormal->Cross(V2).Unit();	
	side[2].m_kNormal = pkNormal->Cross(V3).Unit();

//	side[3].m_kNormal = (side[0].m_kNormal + side[2].m_kNormal).Unit();
//	side[4].m_kNormal = (side[0].m_kNormal + side[1].m_kNormal).Unit();
//	side[5].m_kNormal = (side[1].m_kNormal + side[2].m_kNormal).Unit();

	side[0].m_fD = -side[0].m_kNormal.Dot(kVerts[0]);
	side[1].m_fD = -side[1].m_kNormal.Dot(kVerts[1]);	
	side[2].m_fD = -side[2].m_kNormal.Dot(kVerts[2]);	

//	side[3].m_fD = -side[3].m_kNormal.Dot(kVerts[0]);
//	side[4].m_fD = -side[4].m_kNormal.Dot(kVerts[1]);	
//	side[5].m_fD = -side[5].m_kNormal.Dot(kVerts[2]);	
	
	
	//check that the point is inside all planes	
	for(int i=0;i<3;i++)
		if(!side[i].PointInside(kPos))
			return false;
	
	
	return true;	*/
}


void P_Mad::GenerateModelMatrix()
{
	static Matrix4 kMat;

	m_kModelMatrix.Identity();
	m_kModelMatrix.Scale(m_fScale,m_fScale,m_fScale);
	kMat = m_pkEntity->GetWorldRotM();
	m_kModelMatrix *= kMat;	
	m_kModelMatrix.Translate(m_pkEntity->GetWorldPosV());		
	m_kModelMatrix.Translate(m_kOffset);		

}


void P_Mad::SetVisible(bool bVisible)
{
	m_bIsVisible = bVisible;
	
// 	if(m_bIsVisible)
// 		m_iSortPlace = 9;
// 	else
// 		m_iSortPlace = 10;
}


Vector3 P_Mad::GetJointPosition(const char* szJointName)
{
	Mad_Core* pkMc = (Mad_Core*)kMadHandle.GetResourcePtr();
	
	if(pkMc)
	{
	
		//update joint positions
		UpdateBones();
	
		if( pkMc->GetJointID(szJointName) == -1)
			cout<<"Joint "<<szJointName<<" not found"<<endl;
		
		Matrix4 kMat;
		Vector3 kPos;
		
		kMat = pkMc->GetBoneTransform(pkMc->GetJointID(szJointName));
		kPos = kMat.GetPos() * m_fScale;
		
		//rotate joint with entity rotation
		kPos = m_pkEntity->GetWorldRotM().VectorTransform(kPos);
		
		//kPos = -pkMc->GetJointPosition(szJointName);
		
		return kPos + m_kOffset;	
	}
	cout<<"Error: No Model loaded when trying to get joint position"<<endl;
	
	return m_kOffset;
}

Matrix4 P_Mad::GetJointRotation(const char* szJointName)
{
	if(Mad_Core* pkMc = (Mad_Core*)kMadHandle.GetResourcePtr())
	{
	
		//update joint positions
		UpdateBones();
	
		if( pkMc->GetJointID(szJointName) == -1)
			cout<<"Joint "<<szJointName<<" not found"<<endl;
		
		Matrix4 kMat;
		Vector3 kPos;
		
		kMat = pkMc->GetBoneTransform(pkMc->GetJointID(szJointName));
		kMat*=m_pkEntity->GetWorldRotM();
		kMat.SetPos(Vector3(0,0,0));
		
		return kMat;
		//kPos = kMat.GetPos() * m_fScale;
		
		//rotate joint with entity rotation
		//kPos = m_pkEntity->GetWorldRotM().VectorTransform(kPos);
		
		//kPos = -pkMc->GetJointPosition(szJointName);
		
		//return kPos + m_kOffset;	
	}
	cout<<"Error: No Model loaded when trying to get joint position"<<endl;
	
	return Matrix4();
}


bool P_Mad::operator<(Property& kOther)
{

	if(m_iSortPlace < kOther.m_iSortPlace)
	{		
		return true;		
	}else 
	{
		if(m_iSortPlace == kOther.m_iSortPlace)
		{	
			if(kOther.IsPropertyType("P_Mad"))			
				if(P_Mad* pkMad = static_cast<P_Mad*>(&kOther))
					return (m_iFirstMaterialID < pkMad->m_iFirstMaterialID);			
		}
	}
	
	return false;
}


/* ********************************** SCRIPT INTERFACE ****************************************/
/**	\brief Script functions for Mad
	\ingroup si
*/
class SIMad { };

namespace SI_PMad
{

/**	\fn SetNextAnim( Entity, AnimName)
 	\relates SIMad
	\brief Sets the next animation for a object to play.

	This function sets the next animation that the object will play after completing the current one
	(if any). If the current playing animation is set to loop it will stop loop so AnimName can play after it.
	The AnimName animation will be set to loop.
*/
int SetNextAnim(lua_State* pkLua)
{
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);
//	printf("Next Anim to play is '%s' on object %d", acName,  iId1);

	Entity* o1 = g_pkObjMan->GetEntityByID(iId1);
	P_Mad* mp = static_cast<P_Mad*>(o1->GetProperty("P_Mad"));
	mp->SetNextAnimation(acName);
	return 1;
}

/**	\fn PlayAnim( Entity, AnimName)
 	\relates SIMad
	\brief Sets the playing animation.

	Plays animation on entity. Aborts the current animation if any. The animation will loop.
*/
int PlayAnim(lua_State* pkLua)
{
	double dTemp; 
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iEntityId = (int)dTemp;

	Entity* pkEntity = g_pkObjMan->GetEntityByID(iEntityId);
	if(!pkEntity)
		return 0;

	char acAnimationName[100];
	g_pkScript->GetArg(pkLua, 1, acAnimationName);

	P_Mad* pkMad = static_cast<P_Mad*>(pkEntity->GetProperty("P_Mad"));
	pkMad->SetAnimation(acAnimationName,0);
	
	return 1;
}

/**	\fn AddMesh( Entity, MeshIndex)
 	\relates SIMad
   \brief Adds the Mesh to be displayed on the entity.
*/
int AddMesh(lua_State* pkLua)
{
	// Get ObjectID ID
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	// Get MeshName
	g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
	int iId2 = (int)dTemp;

	Entity* o1 = g_pkObjMan->GetEntityByID(iId1);
	P_Mad* mp = static_cast<P_Mad*>(o1->GetProperty("P_Mad"));

	mp->AddMesh( iId2 );
	return 1;
}

// p_mad: changes MadFile: Takes ObjectID, filename
int SetMadfileLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dId;		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
		
		if (Entity* pkEnt = g_pkObjMan->GetEntityByID((int)dId))
		{
			if ( P_Mad* pkMad = ((P_Mad*)pkEnt->GetProperty("P_Mad")) )
			{
				char acModel[100];
				g_pkScript->GetArg(pkLua, 1, acModel);
				pkMad->SetBase(acModel);
			}
		}
		
	}
	return 0;
}

int SetDrawingOrderLua(lua_State* pkLua)
{
	if( g_pkScript->GetNumArgs(pkLua) == 1 )
	{

      // caster
      double dOrder;
      g_pkScript->GetArgNumber(pkLua, 0, &dOrder);

      Entity* pkEntity = g_pkObjMan->GetEntityByID(ObjectManagerLua::g_kScriptState.g_iCurrentObjectID);

  		P_Mad* pkMAD = (P_Mad*)pkEntity->GetProperty("P_Mad");

      if ( pkMAD )
         pkMAD->m_iSortPlace = (int) dOrder;
      else
         cout << "Warning! Tried to set drawing order on a non-MAD object." << endl;
 }

   return 0;
}
}


Property* Create_MadProperty()
{
	return new P_Mad;
}

void Register_MadProperty(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Mad", Create_MadProperty);				

	// Register Property Script Interface
	g_pkScript->ExposeFunction("SetNextAnim",			SI_PMad::SetNextAnim);
	g_pkScript->ExposeFunction("PlayAnim",				SI_PMad::PlayAnim);
	g_pkScript->ExposeFunction("AddMesh",				SI_PMad::AddMesh);
	g_pkScript->ExposeFunction("SetMadfile",			SI_PMad::SetMadfileLua);
	g_pkScript->ExposeFunction("SetDrawingOrder",	SI_PMad::SetMadfileLua);
}


