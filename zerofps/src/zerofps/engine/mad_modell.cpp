#include "mad_modell.h"

float fGameTime;

void SetGameTime(void)
{
	fGameTime = SDL_GetTicks() / 1000.0;
}
 
Mad_Modell::Mad_Modell()
{
//	kMadRender.m_pkMadInstans = this;
}

Mad_Modell::Mad_Modell(Mad_Core* pkModell) {
//	strcpy(m_acName,"MadProperty");

	pkCore = pkModell;
	
	PlayAnimation(0, 0.0);
	m_fScale = 1.0;
	m_bActive = true;
	pkCore->ClearReplaceTexture();
	fGameTime = 0;

}

void Mad_Modell::SetBase(Mad_Core* pkModell)
{
	pkCore = pkModell;
	
	PlayAnimation(0, 0.0);
	m_fScale = 1.0;
	m_bActive = true;
	pkCore->ClearReplaceTexture();
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
	iActiveAnimation++;
	if(iActiveAnimation >= pkCore->GetNumOfAnimations())
		iActiveAnimation = 0;


}

void Mad_Modell::SetReplaceTexture(char* szName)
{
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
	return 1;
}

int Mad_Modell::GetNumOfSubMesh(int iMeshID)
{
	return pkCore->GetMeshByID(iMeshID)->kHead.iNumOfSubMeshes;
}

void Mad_Modell::SelectMeshSubMesh(int iMeshID, int iSubMeshID)
{
	m_pkMesh = pkCore->GetMeshByID(iMeshID);
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
	return	pkCore->GetVerticesPtr();
}

int* Mad_Modell::GetFacesPtr()
{
	return	pkCore->GetFacesPtr(m_pkMesh, m_pkSubMesh);
}

Mad_TextureCoo* Mad_Modell::GetTextureCooPtr()
{
	return pkCore->GetTextureCooPtr(m_pkMesh);
}

Vector3* Mad_Modell::GetNormalsPtr()
{
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

char szFullTexName[256];

void Mad_Modell::LoadTextures()
{
	TextureManager*	pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));

	int iNumOfMesh = GetNumOfMesh();
	int iNumOfSubMesh;
	int iTexID;


	for(int iM = 0; iM <iNumOfMesh; iM++) {
		iNumOfSubMesh = GetNumOfSubMesh(iM);
		pkCore->PrepareMesh(pkCore->GetMeshByID(iM));
	
		for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) {
			SelectMeshSubMesh(iM, iSubM);

			Mad_CoreTexture* pkTexInfo = GetTextureInfo();
			pkTexInfo->bClampTexture	= false;
			pkTexInfo->bIsAlphaTest		= false;
			pkTexInfo->bTwoSided		= false;
			sprintf(szFullTexName, "../data/textures/%s.tga", pkTexInfo->ucTextureName);

			if(strcmp("../data/textures/pine_branch_color02.tga",szFullTexName) == 0) {
				pkTexInfo->bClampTexture	= true;
				pkTexInfo->bIsAlphaTest		= true;
				pkTexInfo->bTwoSided		= true;
				}

			if(pkTexInfo->bClampTexture)
				iTexID = pkTex->Load(szFullTexName,T_CLAMP);
			else
				iTexID = pkTex->Load(szFullTexName,0);
			
			m_pkMesh->SetTextureID(m_pkSubMesh->iTextureIndex, iTexID);
			}
		}
}

void Mad_Modell::Draw_All(void)
{
	LoadTextures();

	// Refresh Skelleton Pose.
 	pkCore->SetBoneAnimationTime(iActiveAnimation, fCurrentTime);
	pkCore->SetupBonePose();

	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );

	glColor3f(1,1,1);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D );

	int iNumOfMesh = GetNumOfMesh();
	int iNumOfFaces;
	int iNumOfSubMesh;
	
	TextureManager*	pkTex = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));

	for(int iM = 0; iM <iNumOfMesh; iM++) {
		iNumOfSubMesh = GetNumOfSubMesh(iM);
		pkCore->PrepareMesh(pkCore->GetMeshByID(iM));
	
		for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) {
			SelectMeshSubMesh(iM, iSubM);

			glTexCoordPointer(2,GL_FLOAT,0,GetTextureCooPtr());
			glVertexPointer(3,GL_FLOAT,0,GetVerticesPtr());
			glNormalPointer(GL_FLOAT,0,GetNormalsPtr());

			iNumOfFaces = GetNumFaces();

			Mad_CoreTexture* pkTexInfo = GetTextureInfo();
			pkTex->BindTexture( m_pkMesh->GetTextureID(m_pkSubMesh->iTextureIndex));
			if(pkTexInfo->bIsAlphaTest) {
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GEQUAL, 0.5);
				}

			glDrawElements(GL_TRIANGLES,
				iNumOfFaces * 3,
				GL_UNSIGNED_INT,
				GetFacesPtr());

//			DrawNormal(GetVerticesPtr(), GetNormalsPtr());
			}
		}

	glDisable(GL_ALPHA_TEST);

//	DrawSkelleton();
	glPopAttrib();

	glEnable(GL_CULL_FACE);
}
/*

void Mad_ModellRender::DrawNormal(Vector3* pkVertex, Vector3* pkNormals)
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );

	glColor3f(1,1,1);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D );

	glBegin(GL_LINES);

	for(int i=0; i<m_pkMesh->kHead.iNumOfVertex; i++) {
		Vector3 Vert = pkVertex[i];
		Vector3 Norm = pkNormals[i];
		glVertex3f(Vert.x, Vert.y, Vert.z);
		Vert += Norm * 1;
		glVertex3f(Vert.x, Vert.y, Vert.z);		

		}
	glEnd();

	glPopAttrib();
}

void Mad_ModellRender::DrawSkelleton()
{
/*	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	glDisable (GL_DEPTH_TEST);
	
//	SetupBonePose();
	Vector3 Position;

	glColor3f(1,1,1);
	for(unsigned int i=0; i<pkCore->GetNumOfBones(); i++) {
		if (pkCore->GetBoneParent(i) >= 0) {
//			DrawBone(g_Madkbonetransform[m_kSkelleton[i].m_iParent].GetPos(),
//				g_Madkbonetransform[i].GetPos(),
			DrawBone(pkCore->GetBonePosition(pkCore->GetBoneParent(i)),
				pkCore->GetBonePosition(i),Vector3(1, 0.7f, 0));
				//g_Madkbonetransform[m_kSkelleton[i].m_iParent].GetPos(),
				//g_Madkbonetransform[i].GetPos(),
				//Vector3(1, 0.7f, 0));
			}
		else {
			// Draw marker for parent bone.
			glPointSize (5.0f);
			glColor3f (0.8f, 0, 0);
			glBegin (GL_POINTS);
//			Position = g_Madkbonetransform[i].GetPos();
			Position = pkCore->GetBonePosition(i);
				glVertex3f(Position.x,Position.y,Position.z);
			glEnd ();

			}
		}
 
	glEnable(GL_DEPTH_TEST);
	glPopAttrib();
}
*/
