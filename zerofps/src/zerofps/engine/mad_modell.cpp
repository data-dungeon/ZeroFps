#include "mad_modell.h"
#include "../render/render.h"

float fGameTime;
char szFullTexName[256];
extern int g_iNumOfMadSurfaces;
extern float g_fMadLODScale;


/*
void SetGameTime(void)
{
	fGameTime = SDL_GetTicks() / 1000.0;
}*/
 
Mad_Modell::Mad_Modell()
{
	m_pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	fCurrentTime = 0;
	iActiveAnimation = 0;
	fLastUpdate = 0;
	m_bLoop		= 0;
	m_fScale	= 1.0;
	m_bActive	= true;
//	pkCore = NULL;
}

Mad_Modell::Mad_Modell(string strResName)
{
	m_kMadFile = strResName;
	if(!kMadHandle.SetRes(strResName))
		return;

	// pkCore = pkModell;
	// m_kMadFile = pkCore->Name;

	PlayAnimation(0, 0.0);
	m_fScale = 1.0;
	m_bActive = true;
	//pkCore->ClearReplaceTexture();
	fGameTime = 0;

	TextureManager*	m_pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	LoadTextures();
	AddMesh(0);
}

/*Mad_Modell::Mad_Modell(Mad_Core* pkModell) 
{
	pkCore = pkModell;
	m_kMadFile = pkCore->Name;

	PlayAnimation(0, 0.0);
	m_fScale = 1.0;
	m_bActive = true;
	pkCore->ClearReplaceTexture();
	fGameTime = 0;

	TextureManager*	m_pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	LoadTextures();
	AddMesh(0);
}*/

void Mad_Modell::SetBasePtr(string strResName)
{
	m_kMadFile = strResName;
	if(!kMadHandle.SetRes(strResName))
		return;

	PlayAnimation(0, 0.0);
	m_fScale = 1.0;
	m_bActive = true;
//	pkCore->ClearReplaceTexture();
	TextureManager*	m_pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	LoadTextures();

//	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
//	Create_GLList(pkCore->GetMeshByID(0));
	AddMesh(0);

/*	m_fScale = 1.0;
	m_bActive = true;
	
	TextureManager*	m_pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));

	if(pkModell) {
		pkCore = pkModell;
		m_kMadFile = pkCore->Name;
		PlayAnimation(0, 0.0);
		pkCore->ClearReplaceTexture();
		LoadTextures();
		Create_GLList(pkCore->GetMeshByID(0));

		AddMesh(0);
		}*/
}

void Mad_Modell::PlayAnimation(int iAnimNum, float fStartTime)
{
//	SetGameTime();

	iActiveAnimation	=	iAnimNum;
	fCurrentTime		=	fStartTime;
	fLastUpdate			=	fGameTime; 
}

void Mad_Modell::UpdateAnimation(float fDelta)
{
	if(!m_bActive)
		return;

	if(kMadHandle.IsValid() == false)
		return;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return;

//	float fDelta = fGameTime - fLastUpdate;

	fCurrentTime += fDelta;

	float fAnimLength = pkCore->GetAnimationLengthInS(iActiveAnimation);

	if(fCurrentTime >= fAnimLength)

		fCurrentTime -= fAnimLength;

	fLastUpdate = fGameTime; 
}

void Mad_Modell::SetScale(float fScale)
{
	m_fScale = fScale;
}


void	Mad_Modell::SetNextAnimation(int iAnimNum)
{
	m_iNextAnimation = iAnimNum;
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
	return pkCore->GetMeshByID(iMeshID)->kHead.iNumOfSubMeshes;
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
	m_pkSubMesh = m_pkMesh->GetSubMesh(iSubMeshID);
}


int Mad_Modell::GetNumVertices()
{
	return m_pkMesh->kHead.iNumOfVertex;
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
	return m_pkMesh->GetTextureName(m_pkSubMesh->iTextureIndex);
}

Mad_CoreTexture* Mad_Modell::GetTextureInfo()
{
	return m_pkMesh->GetTextureInfo(m_pkSubMesh->iTextureIndex);

}

void Mad_Modell::Create_GLList(Mad_CoreMesh* pkMesh)
{
	if(pkMesh->bNotAnimated == false)
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

}

void Mad_Modell::LoadTextures()
{
	TextureManager*	pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));

	int iNumOfMesh = GetNumOfMesh();
	int iNumOfSubMesh;
	int iTexID;

	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 

	for(int iM = 0; iM <iNumOfMesh; iM++) {
		SelectMesh(iM);

		iNumOfSubMesh = GetNumOfSubMesh(iM);
		pkCore->PrepareMesh(pkCore->GetMeshByID(iM));

		for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) {
			SelectSubMesh(iSubM);

			Mad_CoreTexture* pkTexInfo = GetTextureInfo();
			sprintf(szFullTexName, "../data/textures/%s.tga", pkTexInfo->ucTextureName);
			if(pkTexInfo->bIsAlphaTest)
				sprintf(szFullTexName, "../data/textures/%s.tga", pkTexInfo->ucTextureName);
			else 
				sprintf(szFullTexName, "../data/textures/%s.bmp", pkTexInfo->ucTextureName);


			/*pkTexInfo->bClampTexture	= false;
			pkTexInfo->bIsAlphaTest		= false;
			pkTexInfo->bTwoSided		= false;
			cout << szFullTexName;
			cout << " bClampTexture: " << pkTexInfo->bClampTexture;
			cout << " bIsAlphaTest: " << pkTexInfo->bIsAlphaTest;
			cout << " bTwoSided: " << pkTexInfo->bTwoSided;
			cout << endl;
			//cout << iM << "/" << iSubM << ": " << szFullTexName << endl;
			if(strcmp("../data/textures/pine_branch_color02.tga",szFullTexName) == 0) {
				pkTexInfo->bClampTexture	= true;
				pkTexInfo->bIsAlphaTest		= true;
				pkTexInfo->bTwoSided		= true;
				}*/

			if(pkTexInfo->bClampTexture)
				iTexID = pkTex->Load(szFullTexName,T_CLAMP);
			else
				iTexID = pkTex->Load(szFullTexName,0);
			
			m_pkMesh->SetTextureID(m_pkSubMesh->iTextureIndex, iTexID);
			}
		}
}

Mad_CoreMesh* g_pkLastMesh;

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
 	pkCore->SetBoneAnimationTime(iActiveAnimation, fCurrentTime);
	pkCore->SetupBonePose();

	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );

	glColor3f(1,1,1);
//	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D );

	int iNumOfMesh = m_kActiveMesh.size();	//GetNumOfMesh();
	int iNumOfFaces;
	int iNumOfSubMesh;

//	Render* m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		
//	m_pkRender->DumpGLState();

	if(iDrawFlags &	MAD_DRAW_LINES)
		glPolygonMode(GL_FRONT, GL_LINE);

	for(int iM = 0; iM <iNumOfMesh; iM++) {
		SelectMesh(m_kActiveMesh[iM]);		//SelectMesh(iM);

		//if(g_fMadLODScale == 1)
			pkCore->PrepareMesh(pkCore->GetMeshByID(m_kActiveMesh[iM]));

		glTexCoordPointer(2,GL_FLOAT,0,GetTextureCooPtr());
		glVertexPointer(3,GL_FLOAT,0,GetVerticesPtr());
		glNormalPointer(GL_FLOAT,0,GetNormalsPtr());

		iNumOfSubMesh = GetNumOfSubMesh(m_kActiveMesh[iM]);
		

		for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) {
			SelectSubMesh(iSubM);

			if(iDrawFlags & MAD_DRAW_MESH) {
				iNumOfFaces = GetNumFaces();	// * g_fMadLODScale;
				//iNumOfFaces = 1;

				Mad_CoreTexture* pkTexInfo = GetTextureInfo();
				m_pkTex->BindTexture( m_pkMesh->GetTextureID(m_pkSubMesh->iTextureIndex));
				//m_pkTex->BindTexture("../data/textures/c_red.tga",0);
				if(pkTexInfo->bIsAlphaTest) {
					glEnable(GL_ALPHA_TEST);
					glAlphaFunc(GL_GEQUAL, 0.5);
					}

				glDrawElements(GL_TRIANGLES,
					iNumOfFaces * 3,
					GL_UNSIGNED_INT,
					GetFacesPtr());
				
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
	for(i=0; i<m_pkMesh->kHead.iNumOfVertex; i++) {
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
	for(i=0; i<m_pkMesh->kHead.iNumOfVertex; i++) {
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

void Mad_Modell::DrawSkelleton()
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	
	Vector3 Position;
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 

	glColor3f(1,1,1);
	for(unsigned int i=0; i<pkCore->GetNumOfBones(); i++) {
		if (pkCore->GetBoneParent(i) >= 0) {
			DrawBone(pkCore->GetBonePosition(pkCore->GetBoneParent(i)),
				pkCore->GetBonePosition(i),Vector3(1, 0.7f, 0));
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

bool Mad_Modell::AddMesh(int iSubId)
{
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
	if(pkCore->GetMeshByID(iSubId) == NULL)
		return false;

	m_kActiveMesh.push_back(iSubId);
	return true;
}

