#include "mad_modell.h"
#include "../../render/render.h"
#include "../../basic/zfsystem.h"
#include "../../engine/res_texture.h"

char szFullTexName[256];
extern int g_iNumOfMadSurfaces;
extern float g_fMadLODScale;

Mad_Modell::Mad_Modell()
{
	m_pkTex		= static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkShader	= static_cast<ZShader*>(g_ZFObjSys.GetObjectPtr("ZShader"));	
	
	fCurrentTime		= 0;
	iActiveAnimation	= MAD_NOANIMINDEX;
	m_iNextAnimation	= MAD_NOANIMINDEX;

	m_bLoop				= true;
	m_bActive			= true;
	m_fScale			= 1.0;
}

void Mad_Modell::SetBasePtr(string strResName)
{
	if(m_kMadFile == strResName)
		return;

	m_kMadFile = strResName;
	if(!kMadHandle.SetRes(strResName))
		return;

	m_bActive = true;
	LoadTextures();
	AddMesh(0);
//	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
//	Create_GLList(pkCore->GetMeshByID(0));
}

void Mad_Modell::PlayAnimation(int iAnimNum, float fStartTime)
{
	if(kMadHandle.IsValid() == false)
		return;
	if(iAnimNum < 0)	iAnimNum = MAD_NOANIMINDEX;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(pkCore) {
		if(iAnimNum >= pkCore->GetNumOfAnimations())
			iAnimNum = MAD_NOANIMINDEX;
		}

	iActiveAnimation	=	iAnimNum;
	fCurrentTime		=	fStartTime;
	m_bActive = true;
//	SetLoopedStatus(true);
}

void Mad_Modell::PlayAnimation(char* szName, float fStartTime)
{
	if(kMadHandle.IsValid() == false)
		return;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;

	int iAnimNum = pkCore->GetAnimIndex(szName);

//	if(iAnimNum == -1)
//		return;

	iActiveAnimation	=	iAnimNum;
	fCurrentTime		=	fStartTime;
//	fLastUpdate			=	fGameTime; 
	m_bActive = true;
//	SetLoopedStatus(true);
}

void Mad_Modell::UpdateAnimation(float fDelta)
{
	if(!m_bActive)
		return;
	if(iActiveAnimation == MAD_NOANIMINDEX)
		return;

	if(kMadHandle.IsValid() == false)
		return;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;

	// Move Anim forward.
	fCurrentTime += fDelta;
	float fAnimLength = pkCore->GetAnimationLengthInS(iActiveAnimation);

	// If we pass the end of the anim
	if(fCurrentTime >= fAnimLength) {
		// If this is a looping anim we simply wrap around.
		if(m_bLoop)
			fCurrentTime -= fAnimLength;
		else {
			fCurrentTime = fAnimLength;
			m_bActive = false;

			// We shall not loop so lets try to move on to the next anim if any.
			if(m_iNextAnimation != MAD_NOANIMINDEX) {
				PlayAnimation(m_iNextAnimation, 0);
				m_iNextAnimation = MAD_NOANIMINDEX;
				}
			//else {
				//
			//	}
			}
		}

//	printf("Current time %f\n", fCurrentTime);
}

void Mad_Modell::SetScale(float fScale)
{
	m_fScale = fScale;
}


void	Mad_Modell::SetNextAnimation(int iAnimNum)
{
	m_iNextAnimation = iAnimNum;
	
	if(iAnimNum != -1)
		SetLoopedStatus(false);
	else
		SetLoopedStatus(true);
	
}

void Mad_Modell::SetNextAnimation(char* szName)
{
	if(kMadHandle.IsValid() == false)
		return;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;

	int iAnimNum = pkCore->GetAnimIndex(szName);
//	printf("Playing Anim %d", iAnimNum);

	if(iAnimNum == -1)
		return;
	m_iNextAnimation = iAnimNum;
	SetLoopedStatus(false);
}


void	Mad_Modell::PlayNextAnimations(void)
{

}

int		Mad_Modell::GetNextAnimation()
{
	return m_iNextAnimation;
}


int		Mad_Modell::GetCurrentAnimation()
{
	return iActiveAnimation;
}

void	Mad_Modell::SetLoopedStatus(bool bLoop)
{
	m_bLoop = bLoop;
}

bool	Mad_Modell::IsLooped()
{
	return m_bLoop;
}

void Mad_Modell::SetAnimationActive(bool bPause) {
	m_bActive = bPause;
}
bool Mad_Modell::GetAnimationActive() {
	return m_bActive;
}


void Mad_Modell::NextCoreAnimation(void)
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 

	iActiveAnimation++;
	if(iActiveAnimation >= pkCore->GetNumOfAnimations())
		iActiveAnimation = 0;
}

void Mad_Modell::SetReplaceTexture(char* szName)
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	pkCore->SetReplaceTexture(szName);
}

/*
MadRender*	MadInstans::GetRender()
{
	return &kMadRender;
}*/

void Mad_Modell::Begin() { }
void Mad_Modell::End() {}

int Mad_Modell::GetNumOfMesh() 
{
	if(kMadHandle.IsValid() == false)
		return 0;

	ZFResource* pkres = kMadHandle.GetResourcePtr();
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkres); 
 	return pkCore->NumOfMeshes();
}

int Mad_Modell::GetNumOfSubMesh(int iMeshID)
{
	if(kMadHandle.IsValid() == false)
		return 0;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	return pkCore->GetMeshByID(iMeshID)->GetLODMesh(0)->kHead.iNumOfSubMeshes;
}

void Mad_Modell::SelectMesh(int iMeshID)
{
	if(kMadHandle.IsValid() == false)
		return;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	m_pkMesh = pkCore->GetMeshByID(iMeshID);
}

void Mad_Modell::SelectSubMesh(int iSubMeshID)
{
	if(kMadHandle.IsValid() == false)
		return;
	m_iSubMesh =  iSubMeshID;
	m_pkSubMesh = m_pkMesh->GetLODMesh(0)->GetSubMesh(iSubMeshID);
}


int Mad_Modell::GetNumVertices()
{
	return m_pkMesh->GetLODMesh(0)->kHead.iNumOfVertex;
}

int Mad_Modell::GetNumFaces()
{
	return m_pkSubMesh->iNumOfTriangles;
}

Vector3*  Mad_Modell::GetVerticesPtr()
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	return	pkCore->GetVerticesPtr();
}

int* Mad_Modell::GetFacesPtr()
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	return	pkCore->GetFacesPtr(m_pkMesh, m_pkSubMesh);
}

Mad_TextureCoo* Mad_Modell::GetTextureCooPtr()
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	return pkCore->GetTextureCooPtr(m_pkMesh);
}

Vector3* Mad_Modell::GetNormalsPtr()
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	return pkCore->GetNormalsPtr();
}

int Mad_Modell::GetTextureID()
{
	return 0;
}

char* Mad_Modell::GetTextureName()
{
	return m_pkMesh->GetLODMesh(0)->GetTextureName(m_pkSubMesh->iTextureIndex);
}

Mad_CoreTexture* Mad_Modell::GetTextureInfo()
{
	return m_pkMesh->GetLODMesh(0)->GetTextureInfo(m_pkSubMesh->iTextureIndex);

}

void Mad_Modell::Create_GLList(Mad_CoreMesh* pkMesh)
{
/*	if(pkMesh->bNotAnimated == false)
		return;
	
	int iListID;
	iListID = pkMesh->GetDisplayID();
	if(iListID != -1) {
		return;
		}

	iListID = glGenLists(1);

	iActiveAnimation = 0;
	fCurrentTime = 0;

	GLenum iError = glGetError();
	if(iError != GL_NO_ERROR) {
		cout << "Failed to create GLList in Mad_Modell::Create_GLList. Err = " << GetOpenGLErrorName(iError) << endl;
		return;
		}

	glNewList(iListID,GL_COMPILE );
	Draw_All();
	glEndList();

	iError = glGetError();
	if(iError != GL_NO_ERROR) {
		cout << "Failed to create GLList in Mad_Modell::Create_GLList. Err = " << GetOpenGLErrorName(iError) << endl;
		return;
		}

	cout << "List Created: " << iListID << endl;
	pkMesh->SetDisplayID(iListID);
*/
}

void Mad_Modell::LoadTextures()
{
	TextureManager*	pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));

	int iNumOfMesh = GetNumOfMesh();
	int iNumOfSubMesh;

	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 

	for(int iM = 0; iM <iNumOfMesh; iM++) {
		SelectMesh(iM);

		iNumOfSubMesh = GetNumOfSubMesh(iM);
		pkCore->PrepareMesh(pkCore->GetMeshByID(iM));

		for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) {
			SelectSubMesh(iSubM);

			Mad_CoreTexture* pkTexInfo = GetTextureInfo();
			
			sprintf(szFullTexName,"data/material/%s.zmt",pkTexInfo->ucTextureName);

			 m_pkMesh->GetLODMesh(0)->SetTextureHandle(m_pkSubMesh->iTextureIndex,szFullTexName);			
			}
		}
}


void Mad_Modell::SetReplaceTexture(char* szOrgName, char* szNew)
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 

	int iNumOfMesh = m_kActiveMesh.size();	//GetNumOfMesh();
	int iNumOfSubMesh;

	for(int iM = 0; iM <iNumOfMesh; iM++) {
		SelectMesh(m_kActiveMesh[iM]);		//SelectMesh(iM);

		pkCore->PrepareMesh(pkCore->GetMeshByID(m_kActiveMesh[iM]));

		iNumOfSubMesh = GetNumOfSubMesh(m_kActiveMesh[iM]);
		
		for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) {
			SelectSubMesh(iSubM);
			Mad_CoreTexture* pkTexInfo = GetTextureInfo();

			if(strcmp(pkTexInfo->ucTextureName, szOrgName) == 0) {
				sprintf(szFullTexName, "data/textures/%s.bmp", szNew);
				m_akReplaceTexturesHandles[ m_pkSubMesh->iTextureIndex ].SetRes(szFullTexName);
				}
		}
	}
}

void Mad_Modell::Draw_All(int iDrawFlags)
{
	if(iDrawFlags == 0)
		return;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 

	/*	
	int iListID = pkCore->GetMeshByID(0)->GetDisplayID();
	if(iListID != -1) {
		glCallList(iListID);
		g_iNumOfMadSurfaces += pkCore->GetMeshByID(0)->kHead.iNumOfFaces;
		//cout << "Rendering display list " << iListID << endl;
		return;
		}*/

	// Refresh Skelleton Pose.
 	pkCore->SetBoneAnimationTime(iActiveAnimation, fCurrentTime, m_bLoop);
	pkCore->SetupBonePose();


	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );

	glColor3f(1,1,1);
	if(iDrawFlags & MAD_DRAW_NOLIGHT)
		glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D );

	int iNumOfMesh = m_kActiveMesh.size();	//GetNumOfMesh();
	int iNumOfFaces;
	int iNumOfSubMesh;

	if(iDrawFlags &	MAD_DRAW_LINES)
		glPolygonMode(GL_FRONT, GL_LINE);

	for(int iM = 0; iM <iNumOfMesh; iM++) {
		SelectMesh(m_kActiveMesh[iM]);		//SelectMesh(iM);

		//if(g_fMadLODScale == 1)
			pkCore->PrepareMesh(pkCore->GetMeshByID(m_kActiveMesh[iM]));

		m_pkShader->Reset();
		m_pkShader->SetPointer(TEXTURE_POINTER0,GetTextureCooPtr());
		m_pkShader->SetPointer(VERTEX_POINTER,GetVerticesPtr());		
		m_pkShader->SetPointer(NORMAL_POINTER,GetNormalsPtr());						
		m_pkShader->SetDrawMode(TRIANGLES_MODE);
		m_pkShader->SetNrOfVertexs(GetNumVertices());
	
		iNumOfSubMesh = GetNumOfSubMesh(m_kActiveMesh[iM]);
		
		for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) {
			
			
			SelectSubMesh(iSubM);

			if(iDrawFlags & MAD_DRAW_MESH) {
				iNumOfFaces = GetNumFaces();	// * g_fMadLODScale;

				Mad_CoreTexture* pkTexInfo = GetTextureInfo();

				ZFResourceHandle* pkRes;
				if(m_akReplaceTexturesHandles[ m_pkSubMesh->iTextureIndex ].IsValid()) {
					pkRes = &m_akReplaceTexturesHandles[m_pkSubMesh->iTextureIndex];
					}
				else {
					pkRes = m_pkMesh->GetLODMesh(0)->GetTextureHandle(m_pkSubMesh->iTextureIndex);
					}
				
				ZMaterial* pkMaterial = (ZMaterial*)(pkRes->GetResourcePtr());				
				m_pkShader->SetMaterial(pkMaterial);				
				m_pkShader->SetPointer(INDEX_POINTER,GetFacesPtr());				
				m_pkShader->SetNrOfIndexes(iNumOfFaces * 3);
				
				m_pkShader->Draw();
				g_iNumOfMadSurfaces += iNumOfFaces;
				}

			if(iDrawFlags & MAD_DRAW_NORMAL)
				DrawNormal(GetVerticesPtr(), GetNormalsPtr());
			}
		}

	glDisable(GL_ALPHA_TEST);
	if(iDrawFlags & MAD_DRAW_BONES)
		DrawSkelleton();
	if(iDrawFlags &	MAD_DRAW_LINES)
	glPolygonMode(GL_FRONT, GL_FILL );

	glPopAttrib();
}


void Mad_Modell::DrawNormal(Vector3* pkVertex, Vector3* pkNormals)
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );

	glColor3f(1,1,1);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D );

	int i;

	// Draw All Normals.
	glBegin(GL_LINES);
	for(i=0; i<m_pkMesh->GetLODMesh(0)->kHead.iNumOfVertex; i++) {
		Vector3 Vert = pkVertex[i];
		Vector3 Norm = pkNormals[i];
		glVertex3f(Vert.x, Vert.y, Vert.z);
		Vert += Norm * 1;
		glVertex3f(Vert.x, Vert.y, Vert.z);		

		}
	glEnd();

	// Mark All Vertices.
	glPointSize (3.0f);
	glBegin(GL_POINTS);
	glColor3f (0, 0.8, 0);	
	for(i=0; i<m_pkMesh->GetLODMesh(0)->kHead.iNumOfVertex; i++) {
		Vector3 Vert = pkVertex[i];
		glVertex3f(Vert.x, Vert.y, Vert.z);
		}
	glEnd();
	glPointSize (1.0f);


	glPopAttrib();
}


void DrawBone(Vector3 From, Vector3 To, Vector3 Color)
{
	glDisable (GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	
	glPointSize (3.0f);
	glColor3f (Color.x,Color.y,Color.z);
	glBegin (GL_LINES);
		glVertex3f(From.x,From.y,From.z);
		glVertex3f(To.x,To.y,To.z);
	glEnd ();

	glColor3f (0, 0, 0.8f);	
	glBegin (GL_POINTS);
	glVertex3f(From.x,From.y,From.z);
	glVertex3f(To.x,To.y,To.z);
	glEnd ();

	glPointSize (1.0f);
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
}

extern Matrix4		g_FullBoneTransform[MAX_BONES];		// Bone transformation matrix.
extern Matrix4		g_Madkbonetransform[MAX_BONES];		// Bone transformation matrix.


void Mad_Modell::DrawSkelleton()
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	
	Vector3 Position;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 

	Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));

	glColor3f(1,1,1);
	for(unsigned int i=0; i<pkCore->GetNumOfBones(); i++) {
		if (pkCore->GetBoneParent(i) >= 0) {
			DrawBone(pkCore->GetBonePosition(pkCore->GetBoneParent(i)),
				pkCore->GetBonePosition(i),Vector3(1, 0.7f, 0));
			
			glMatrixMode(GL_MODELVIEW );
			glPushMatrix();
				glMultMatrixf( (float*)&g_Madkbonetransform[i] );
				pkRender->Draw_AxisIcon(0.1 );
			glPopMatrix();
			}
		else {
			// Draw marker for parent bone.
			glPointSize (5.0f);
			glColor3f (0.8f, 0, 0);
			glBegin (GL_POINTS);
			Position = pkCore->GetBonePosition(i);
				glVertex3f(Position.x,Position.y,Position.z);
			glEnd ();

			}
		}
 
	glEnable(GL_DEPTH_TEST);
	glPopAttrib();
}

float Mad_Modell::GetRadius()
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return 0;

	return pkCore->GetRadius() * m_fScale;	
}

void Mad_Modell::ClearAllMesh()
{
	m_kActiveMesh.clear();
}

bool Mad_Modell::AddMesh(int iSubId)
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(pkCore->GetMeshByID(iSubId) == NULL)
		return false;

	m_kActiveMesh.push_back(iSubId);
	return true;
}

bool Mad_Modell::AddMesh(char* szName)		
{ 
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	int iSubId = pkCore->GetMeshIDByName( szName );

	cout << "Mesh: " << szName << ", Id: " << iSubId << endl;

	if(iSubId == -1)
		return false;

	return AddMesh(iSubId);
}

bool Mad_Modell::RemoveMesh(char* szName)	
{ 
	return false; 
}

bool Mad_Modell::IsMeshActive(int iId)	
{
	return false; 
}

bool Mad_Modell::IsMeshActive(char* szName) 
{ 
	return false; 
}


