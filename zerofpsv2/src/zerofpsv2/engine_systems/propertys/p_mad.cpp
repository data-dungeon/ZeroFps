#include "p_mad.h"
#include "../../engine/entity.h"
#include "../../render/render.h"
#include "../../basic/zfsystem.h"
 
extern int g_iMadLODLock;
extern float g_fMadLODScale;
 
P_Mad::P_Mad()
{
	m_pkZeroFps =		static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkRender	=		static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render")); 
	m_pkZShader = 		static_cast<ZShader*>(g_ZFObjSys.GetObjectPtr("ZShader")); 
	
	strcpy(m_acName,"P_Mad");
	bNetwork	 = true;

	m_iType = PROPERTY_TYPE_RENDER | PROPERTY_TYPE_NORMAL;
	m_iSide = PROPERTY_SIDE_SERVER | PROPERTY_SIDE_CLIENT;
	
	SetVisible(true);
	m_bCanBeInvisible = false;
	m_fScale	 = 1.0;
}

void P_Mad::Update() 
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;

	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_NORMAL ) /* &  m_pkObjMan->IsUpdate(PROPERTY_SIDE_SERVER)*/ ) {
		UpdateAnimation(m_pkZeroFps->GetFrameTime());
		return;
		}
	
	if( m_pkObjMan->IsUpdate(PROPERTY_TYPE_RENDER) ) {

		
		if(!m_pkZeroFps->GetCam()->m_kFrustum.SphereInFrustum(m_pkObject->GetWorldPosV(),GetRadius()))
			return;
		
/*		// Set Object LOD.
		if(g_iMadLODLock == 0) {
			Vector3 kDiff = m_pkZeroFps->GetCam()->GetPos() - m_pkObject->GetWorldPosV();
			float fDist = float( fabs(kDiff.Length()) );
			m_fLod = 1 - (fDist / 300);
			//cout << "fDist: " << fDist << " / " << "m_fLod: " << m_fLod << endl;
			
			
			//dvoid yber loding deluxe
			float blub = GetRadius() / fDist;		
			if(blub < 0.010)
				return;
		}

		g_fMadLODScale = m_fLod;
*/		
		//set force transparent if not visible
		if(!m_bIsVisible)
			m_pkZShader->SetForceBlending(BLEND_FORCE_TRANSPARENT);
		

		glPushMatrix();
			Vector3 pos = m_pkObject->GetIWorldPosV();
			glTranslatef(pos.x,pos.y,pos.z);
		
			Matrix4 ori = m_pkObject->GetWorldRotM();
			glMultMatrixf(&ori[0]);
			glScalef(m_fScale, m_fScale, m_fScale);
			Draw_All(m_pkZeroFps->m_iMadDraw);
		glPopMatrix();

		if(m_pkZeroFps->m_iMadDraw & MAD_DRAW_SPHERE) {
			glPushMatrix();
				glTranslatef(m_pkObject->GetWorldPosV().x,m_pkObject->GetWorldPosV().y,m_pkObject->GetWorldPosV().z);
				glRotatef(90 ,1,0,0);
				m_pkRender->DrawBoundSphere(GetRadius(),Vector3::ZERO);		
			glPopMatrix();
		}

		m_pkZeroFps->m_iNumOfMadRender++;
	
		//reset blend
		if(!m_bIsVisible)
			m_pkZShader->SetForceBlending(BLEND_MATERIAL);
	
	
		return;
		}
}

void P_Mad::SetBase(const char* acName)
{
	SetBasePtr(string(acName));
	SetNetUpdateFlag(true);
}

void P_Mad::SetAnimation(char* szName, float fStartTime)
{
	PlayAnimation(szName,fStartTime);
	SetNetUpdateFlag(true);	
}

void P_Mad::Save(ZFIoInterface* pkPackage)
{	
	char temp[128];
	strcpy(temp,m_kMadFile.c_str());

	pkPackage->Write((void*)temp,128,1);
	pkPackage->Write((void*)&m_fScale,4,1);
	pkPackage->Write((void*)&m_bCanBeInvisible,sizeof(m_bCanBeInvisible),1);
}

void P_Mad::Load(ZFIoInterface* pkPackage)
{
	char temp[128];
	pkPackage->Read((void*)temp,128,1);	
	SetBase(temp);
	
	float scale;
	pkPackage->Read((void*)&scale,4,1);
	SetScale(scale);
	
	pkPackage->Read((void*)&m_bCanBeInvisible,sizeof(m_bCanBeInvisible),1);	
	
	//update object radius
	m_pkObject->SetRadius(GetRadius());
	
	SetNetUpdateFlag(true);	
}

void P_Mad::PackTo(NetPacket* pkNetPacket, int iConnectionID )
{
	pkNetPacket->Write_Str(m_kMadFile.c_str());
	pkNetPacket->Write( m_fScale );
	pkNetPacket->Write( m_bCanBeInvisible );	
	pkNetPacket->Write( iActiveAnimation );
	
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

	unsigned char ucNumOfMesh;
	int iMesh;

	ClearAllMesh();
	pkNetPacket->Read( ucNumOfMesh );
	for(int i=0; i<ucNumOfMesh; i++) {
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
	vector<PropertyValues> kReturn(3);
	
	kReturn[0].kValueName = "m_fScale";
	kReturn[0].iValueType = VALUETYPE_FLOAT;
	kReturn[0].pkValue    = (void*)&m_fScale;

	kReturn[1].kValueName = "m_kMadFile";
	kReturn[1].iValueType = VALUETYPE_STRING;
	kReturn[1].pkValue    = (void*)&m_kMadFile;

	kReturn[2].kValueName = "m_bCanBeInvisible";
	kReturn[2].iValueType = VALUETYPE_BOOL;
	kReturn[2].pkValue    = (void*)&m_bCanBeInvisible;


	return kReturn;
}

bool P_Mad::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "m_kMadFile") == 0) {
		SetBase(kValue.c_str());
		return true;
	}
	
	if(strcmp(kValueName.c_str(), "m_fScale") == 0) {
		m_fScale = atof(kValue.c_str());
		m_pkObject->SetRadius(GetRadius());
				
		SetNetUpdateFlag(true);
		return true;
	}

	return false;
}

bool P_Mad::TestLine(Vector3 kPos,Vector3 kDir)
{
	if(LineVSSphere(kPos,kDir))
	{
		return LineVSMesh(kPos,kDir);
	}

	return false;
}

bool P_Mad::LineVSSphere(Vector3 &kPos,Vector3 &kDir)
{
	Vector3 c=m_pkObject->GetWorldPosV() - kPos;		
	kDir.Normalize();		
	Vector3 k=kDir.Proj(c);		
	
	float cdis=c.Length();
	float kdis=k.Length();
	float Distance = sqrt((cdis*cdis)-(kdis*kdis));
	
	if(Distance < GetRadius())
		return true;

	return false;
}

bool P_Mad::LineVSMesh(Vector3 &kPos,Vector3 &kDir)
{
	vector<Mad_Face>*			pkFaces;			// Faces in mesh.
	vector<Vector3>*			pkVertex;			// Vertex frames for mesh.
	vector<Vector3>*			pkNormal;

	Mad_Core*					pkCore;
	Mad_CoreMesh*				pkCoreMesh;

	//setup all pointers
	pkCore = (Mad_Core*)kMadHandle.GetResourcePtr(); 
	if(!pkCore)
		return false;
	
	pkCoreMesh = pkCore->GetMeshByID(0);		
	if(!pkCoreMesh)
		return false;
	
	pkFaces =	pkCoreMesh->GetFacesPointer();
	pkVertex = (*pkCoreMesh->GetVertexFramePointer())[0].GetVertexPointer();
	pkNormal = (*pkCoreMesh->GetVertexFramePointer())[0].GetNormalPointer();
	
	GenerateModelMatrix();
	
	
	float		fDist = 9999999;
	Vector3	kClosestColPos;
	m_iColFace = -1;
	
	Vector3 data[3];
	for(unsigned int i=0;i<pkFaces->size();i++)
	{
		
		//get and transform vertexes		
		for(int j=0;j<3;j++)
			data[j] = m_kModelMatrix.VectorTransform((*pkVertex)[ (*pkFaces)[i].iIndex[j]]);
		
		//setup a virtual point in space
		Vector3 Point2 = kPos+(kDir*1000);
		
		
		if(TestPolygon(data,kPos,Point2))
		{	
			float d = (m_kColPos-kPos).Length(); 
			
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
	Plane P;
	


	Vector3 V1 = kVerts[1] - kVerts[0];
	Vector3 V2 = kVerts[2] - kVerts[0];		
	Vector3 Normal= V1.Cross(V2);
	
	
	if(Normal.Length() == 0)
	{
		return false;
	}
	
	Normal.Normalize();
	P.m_fD = -Normal.Dot(kVerts[0]);	
	P.m_kNormal = Normal;

	if(P.LineTest(kPos1 , kPos2 ,&m_kColPos)){
		if(TestSides(kVerts,&Normal,m_kColPos))
		{
			
			return true;
		}
	}
	
	
	return false;
}

bool P_Mad::TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos)
{
	Plane side[6];
	
	
/*	if(kVerts[0] == kVerts[1] || 
		kVerts[2] == kVerts[1] ||
		kVerts[0] == kVerts[2])
		return false;*/
	
	
	Vector3 V1 = kVerts[1] - kVerts[0];
	Vector3 V2 = kVerts[2] - kVerts[1];	
	Vector3 V3 = kVerts[0] - kVerts[2];	
	
	side[0].m_kNormal = pkNormal->Cross(V1).Unit();
	side[1].m_kNormal = pkNormal->Cross(V2).Unit();	
	side[2].m_kNormal = pkNormal->Cross(V3).Unit();

	side[3].m_kNormal = (side[0].m_kNormal + side[2].m_kNormal).Unit();
	side[4].m_kNormal = (side[0].m_kNormal + side[1].m_kNormal).Unit();
	side[5].m_kNormal = (side[1].m_kNormal + side[2].m_kNormal).Unit();


	side[0].m_fD = -side[0].m_kNormal.Dot(kVerts[0]);
	side[1].m_fD = -side[1].m_kNormal.Dot(kVerts[1]);	
	side[2].m_fD = -side[2].m_kNormal.Dot(kVerts[2]);	

	side[3].m_fD = -side[3].m_kNormal.Dot(kVerts[0]);
	side[4].m_fD = -side[4].m_kNormal.Dot(kVerts[1]);	
	side[5].m_fD = -side[5].m_kNormal.Dot(kVerts[2]);	
	
	
	bool inside = true;
	
	for(int i=0;i<6;i++)
	{
		if(!side[i].SphereInside(kPos,0.01)){
			inside=false;
		}
	
	}
	
	
	return inside;	
}


void P_Mad::GenerateModelMatrix()
{
	m_kModelMatrix.Identity();
	m_kModelMatrix.Scale(m_fScale,m_fScale,m_fScale);
	m_kModelMatrix*=m_pkObject->GetWorldRotM();	
	m_kModelMatrix.Translate(m_pkObject->GetWorldPosV());		

}


void P_Mad::SetVisible(bool bVisible)
{
	m_bIsVisible = bVisible;
	
	if(m_bIsVisible)
		m_iSortPlace = 0;
	else
		m_iSortPlace = 10;
}

Vector3 P_Mad::GetJointPosition(char* szJointName)
{
	Mad_Core* pkMc = (Mad_Core*)kMadHandle.GetResourcePtr();
	
	if(pkMc)
	{
	 	//animate object
	 	//cout<<"bla:"<<iActiveAnimation<<" "<<fCurrentTime<<" "<<m_bLoop<<endl;
	 	
	 	//pkMc->SetBoneAnimationTime(iActiveAnimation, fCurrentTime, m_bLoop);
		//pkMc->SetupBonePose();

	
	
		if( pkMc->GetJointID(szJointName) == -1)
			cout<<"Joint "<<szJointName<<" not found"<<endl;
		
		
		Matrix4 kMat;
		Vector3 kPos;
		
		kMat = pkMc->GetBoneTransform(pkMc->GetJointID(szJointName));
		kPos = kMat.GetPos() * m_fScale;
				
		//kPos = -pkMc->GetJointPosition(szJointName);
		
		return kPos;	
	}
	cout<<"Error: No Model loaded when trying to get joint position"<<endl;
	
	return Vector3(0,0,0);
}


Property* Create_MadProperty()
{
	return new P_Mad;
}






