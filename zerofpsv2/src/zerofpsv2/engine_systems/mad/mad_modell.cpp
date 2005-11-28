#include "mad_modell.h"
#include "../../render/render.h"
#include "../../basic/zfsystem.h"
#include "../../render/res_texture.h"
#include "../../basic/zfassert.h"
#include "../../render/zvertexbuffer.h"

char szFullTexName[256];
extern int g_iNumOfMadSurfaces;

ConVar g_kMadTrans("r_madtrans","0");
ConVar g_kMadLODScale("r_madlodscale","1.0");

Mad_Modell::Mad_Modell()
{
	m_pkTex		= static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkShader	= static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));	
	
	fCurrentTime		= 0;
	iActiveAnimation	= MAD_NOANIMINDEX;
	m_iNextAnimation	= MAD_NOANIMINDEX;
	m_kLastAnim.m_iAnimationIndex = MAD_NOANIMINDEX;

	m_bLoop				= true;
	m_bActive			= true;
	m_fScale				= 1.0;
	
	m_iFirstMaterialID= -1;
	m_bBlended = false;

	m_pkVBO = NULL;
	

	m_pkRawMesh = NULL;

	for(int i = 0;i<256;i++)
		m_akReplaceTexturesHandles[i] = NULL;

}

Mad_Modell::~Mad_Modell()
{
	if(m_pkVBO)
		delete m_pkVBO;
	
	for(int i = 0;i<256;i++)
		if(m_akReplaceTexturesHandles[i] != NULL)
			delete m_akReplaceTexturesHandles[i];
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
	m_kActiveMesh.clear();
	AddMesh(0);
//	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(kMadHandle.GetResourcePtr()); 
//	Create_GLList(pkCore->GetMeshByID(0));
}

void Mad_Modell::PlayAnimation(int iAnimNum, float fStartTime)
{
	if(kMadHandle.IsValid() == false)
		return;
	if(iAnimNum < 0)	iAnimNum = MAD_NOANIMINDEX;
	Mad_Core* pkCore = (Mad_Core*)kMadHandle.GetResourcePtr(); 
	if(pkCore) {
		if(iAnimNum >= pkCore->GetNumOfAnimations())
			iAnimNum = MAD_NOANIMINDEX;
		}

	// Save data for old animation.
	m_kLastAnim.m_iAnimationIndex   = iActiveAnimation;
	m_kLastAnim.m_fAnimationTime    = fCurrentTime;
	m_kLastAnim.m_fAnimationLength  = pkCore->GetAnimationLengthInS(iActiveAnimation);
	m_fAnimTrans = 0.0;

	iActiveAnimation	=	iAnimNum;
	fCurrentTime		=	fStartTime;
	m_bActive = true;
//	SetLoopedStatus(true);
}

void Mad_Modell::PlayAnimation(const char* szName, float fStartTime)
{
	if(kMadHandle.IsValid() == false)
		return;
	Mad_Core* pkCore = (Mad_Core*)kMadHandle.GetResourcePtr(); 
	if(!pkCore)
		return;

	int iAnimNum = pkCore->GetAnimIndex(szName);

//	if(iAnimNum == -1)
//		return;

	// Save data for old animation.
	m_kLastAnim.m_iAnimationIndex   = iActiveAnimation;
	m_kLastAnim.m_fAnimationTime    = fCurrentTime;
	m_kLastAnim.m_fAnimationLength  = pkCore->GetAnimationLengthInS(iActiveAnimation);
	m_fAnimTrans = 0.0;

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
	Mad_Core* pkCore = (Mad_Core*)kMadHandle.GetResourcePtr(); 
	if(!pkCore)
		return;

	if(m_kLastAnim.m_iAnimationIndex != MAD_NOANIMINDEX)
	{
		m_kLastAnim.m_fAnimationTime += fDelta;
		if(m_kLastAnim.m_fAnimationTime >= m_kLastAnim.m_fAnimationLength) 
			m_kLastAnim.m_fAnimationTime = m_kLastAnim.m_fAnimationLength;
		m_fAnimTrans += (fDelta * 10);
	}

	// Move Anim forward.
	fCurrentTime += fDelta;
	float fAnimLength = pkCore->GetAnimationLengthInS(iActiveAnimation);

	// If we pass the end of the anim
	if(fCurrentTime >= fAnimLength) 
	{
		// If this is a looping anim we simply wrap around.
		if(m_bLoop)
		{
			while(fCurrentTime >= fAnimLength)
				fCurrentTime -= fAnimLength;
		}
		else 
		{
			fCurrentTime = fAnimLength;
			m_bActive = false;

			// We shall not loop so lets try to move on to the next anim if any.
			if( (m_iNextAnimation != MAD_NOANIMINDEX) && (m_iNextAnimation != MAD_NOLOOP) ) 
			{
				PlayAnimation(m_iNextAnimation, 0);
				//m_iNextAnimation = MAD_NOANIMINDEX;
				SetNextAnimation(MAD_NOANIMINDEX);
			}
		}
	}
}

void Mad_Modell::SetScale(float fScale)
{
	m_fScale = fScale;
}

float Mad_Modell::GetScale()
{
	return m_fScale;
}

void	Mad_Modell::SetNextAnimation(int iAnimNum)
{
	m_iNextAnimation = iAnimNum;
	 
	if( (iAnimNum == MAD_NOANIMINDEX))
		SetLoopedStatus(true);
	else 
	 	SetLoopedStatus(false);
	
}

void Mad_Modell::SetNextAnimation(const char* szName)
{
	if(kMadHandle.IsValid() == false)
		return;
		
	Mad_Core* pkCore = (Mad_Core*)kMadHandle.GetResourcePtr(); 
	if(!pkCore)
		return;

	int iAnimNum = pkCore->GetAnimIndex(szName);

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

string Mad_Modell::GetCurrentAnimationName()
{
	if(kMadHandle.IsValid() == false)
		return "error";
	
	Mad_Core* pkCore = (Mad_Core*)kMadHandle.GetResourcePtr(); 
	if(!pkCore)
		return "error";

	return pkCore->GetAnimationName(iActiveAnimation);

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
	Mad_Core* pkCore = (Mad_Core*)kMadHandle.GetResourcePtr(); 

	iActiveAnimation++;
	if(iActiveAnimation >= pkCore->GetNumOfAnimations())
		iActiveAnimation = 0;
}

void Mad_Modell::SetReplaceTexture(char* szName)
{
	Mad_Core* pkCore = (Mad_Core*)kMadHandle.GetResourcePtr(); 
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
	Mad_Core* pkCore = (Mad_Core*)pkres; 
 	return pkCore->NumOfMeshes();
}

int Mad_Modell::GetNumOfSubMesh(int iMeshID)
{
	if(kMadHandle.IsValid() == false)
		return 0;
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 
	return m_pkRawMesh->kHead.iNumOfSubMeshes;
}

void Mad_Modell::SelectMesh(int iMeshID)
{
	if(kMadHandle.IsValid() == false)
		return;
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 
	m_pkMesh = pkCore->GetMeshByID(iMeshID);

}

void Mad_Modell::SelectSubMesh(int iSubMeshID)
{
	if(kMadHandle.IsValid() == false)
		return;
	m_iSubMesh =  iSubMeshID;
	m_pkSubMesh = m_pkRawMesh->GetSubMesh(iSubMeshID);
}


int Mad_Modell::GetNumVertices()
{
	if(!m_pkRawMesh) return 0;

	return m_pkRawMesh->kHead.iNumOfVertex;
}

int Mad_Modell::GetNumFaces()
{
	return m_pkSubMesh->iNumOfTriangles;
}

Vector3*  Mad_Modell::GetVerticesPtr()
{
	Mad_Core* pkCore =(Mad_Core*)(kMadHandle.GetResourcePtr()); 
	return	pkCore->GetVerticesPtr();
}

int* Mad_Modell::GetFacesPtr()
{
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 
	return	pkCore->GetFacesPtr(m_pkMesh, m_pkSubMesh);
}

Mad_TextureCoo* Mad_Modell::GetTextureCooPtr()
{
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 
	return pkCore->GetTextureCooPtr(m_pkMesh);
}

Vector3* Mad_Modell::GetNormalsPtr()
{
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 
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
	return m_pkRawMesh->GetTextureInfo(m_pkSubMesh->iTextureIndex);
	//return m_pkMesh->GetLODMesh(0)->GetTextureInfo(m_pkSubMesh->iTextureIndex);
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
	int iNumOfMesh = GetNumOfMesh();
	int iNumOfSubMesh;

	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 

	// Loop all meshes.
	for(int iM = 0; iM <iNumOfMesh; iM++) 
	{
		SelectMesh(iM);

		// Loop all rawmeshes.
		for(int iRaw=0; iRaw<m_pkMesh->m_kLodMesh.size(); iRaw++)
		{
			m_pkRawMesh = m_pkMesh->GetLODMesh( iRaw );

			iNumOfSubMesh = GetNumOfSubMesh(iM);
			pkCore->PrepareMesh(pkCore->GetMeshByID(iM), pkCore->GetMeshByID(iM)->GetLODMesh( iRaw ));

			for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) 
			{
				SelectSubMesh(iSubM);

				Mad_CoreTexture* pkTexInfo = GetTextureInfo();
				
				sprintf(szFullTexName,"%s.zmt",pkTexInfo->ucTextureName);
				m_pkMesh->GetLODMesh( iRaw )->SetTextureHandle(m_pkSubMesh->iTextureIndex,szFullTexName);
			}
		}
	}
}

void Mad_Modell::SetRawMesh(Mad_CoreMesh* pkMesh)
{
	m_pkMesh = pkMesh;
	m_pkRawMesh = m_pkMesh->GetLODMesh(0);
}

void Mad_Modell::SetReplaceTexture(char* szOrgName, char* szNew)
{
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 

	int iNumOfMesh = m_kActiveMesh.size();	//GetNumOfMesh();
	int iNumOfSubMesh;

	for(int iM = 0; iM <iNumOfMesh; iM++) {
		SelectMesh(m_kActiveMesh[iM]);		//SelectMesh(iM);
		m_pkRawMesh = m_pkMesh->GetLODMesh(0);

		pkCore->PrepareMesh(pkCore->GetMeshByID(m_kActiveMesh[iM]), pkCore->GetMeshByID(m_kActiveMesh[iM])->GetLODMesh(0));

		iNumOfSubMesh = GetNumOfSubMesh(m_kActiveMesh[iM]);
		
		for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) {
			SelectSubMesh(iSubM);
			Mad_CoreTexture* pkTexInfo = GetTextureInfo();

			if(strcmp(pkTexInfo->ucTextureName, szOrgName) == 0)
			{
				sprintf(szFullTexName, "%s.bmp", szNew);
				
				//create resource handle if it does not exist
				if(m_akReplaceTexturesHandles[ m_pkSubMesh->iTextureIndex ] == NULL)
					m_akReplaceTexturesHandles[ m_pkSubMesh->iTextureIndex ] = new ZFResourceHandle;
				
				m_akReplaceTexturesHandles[ m_pkSubMesh->iTextureIndex ]->SetRes(szFullTexName);
			}
		}
	}
}

BoneTransform	kAnim1[MAX_BONES];
BoneTransform	kAnim2[MAX_BONES];
BoneTransform	kFinalBoneTransform[MAX_BONES];

void Mad_Modell::UpdateBones()
{
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 


	// Refresh Skelleton Pose.
	/*
	pkCore->SetBoneAnimationTime(iActiveAnimation, fCurrentTime, m_bLoop);
	pkCore->SetupBonePose(kBoneTransform);*/


	// Check if there is a prev anim to blend from
	if(m_kLastAnim.m_iAnimationIndex != MAD_NOANIMINDEX)
	{
		if(g_kMadTrans.GetBool() == false)
			m_kLastAnim.m_iAnimationIndex = MAD_NOANIMINDEX;

		if(m_fAnimTrans >= 1.0)
			m_kLastAnim.m_iAnimationIndex = MAD_NOANIMINDEX;
		else
		{
			pkCore->SetBoneAnimationTime(m_kLastAnim.m_iAnimationIndex, m_kLastAnim.m_fAnimationTime, m_bLoop);
			pkCore->SetupBonePose(kAnim1);
		}
	}

	// Refresh Skelleton Pose.
 	pkCore->SetBoneAnimationTime(iActiveAnimation, fCurrentTime, m_bLoop);
	pkCore->SetupBonePose(kAnim2);

	if(m_kLastAnim.m_iAnimationIndex <= MAD_NOANIMINDEX)
	{
		pkCore->GenerateBoneMatris(kAnim2);
	}
	else
	{
		pkCore->InterPolTransforms(kFinalBoneTransform,kAnim1,kAnim2, m_fAnimTrans);
		pkCore->GenerateBoneMatris(kFinalBoneTransform);
	}
}

void Mad_Modell::Draw_All(int iDrawFlags)
{
	if(iDrawFlags == 0)
		return;
	
	static ZMaterial* pkLineMaterial = NULL;
	if(!pkLineMaterial)
	{
		pkLineMaterial = new ZMaterial();
		pkLineMaterial->GetPass(0)->m_iCullFace = CULL_FACE_NONE;
		pkLineMaterial->GetPass(0)->m_bLighting = false;
		pkLineMaterial->GetPass(0)->m_bFog = true;
		pkLineMaterial->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkLineMaterial->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
	}

	static ZMaterial* pkLod = NULL;
	if(!pkLod)
	{
		pkLod = new ZMaterial();
		pkLod->GetPass(0)->m_iCullFace = CULL_FACE_NONE;
		pkLod->GetPass(0)->m_bLighting = false;
		pkLod->GetPass(0)->m_bFog = true;
		pkLod->GetPass(0)->m_bColorMaterial = true;
		pkLod->GetPass(0)->m_kVertexColor.Set(1,1,0,1);
	}

	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 

	int iNumOfMesh = m_kActiveMesh.size();	//GetNumOfMesh();
	int iNumOfFaces;
	int iNumOfSubMesh;


	pkCore->fRenderDistance = fRenderDistance;
	for(int iM = 0; iM <iNumOfMesh; iM++) 
	{
		SelectMesh(m_kActiveMesh[iM]);		//SelectMesh(iM);

		m_pkRawMesh		= m_pkMesh->GetLODRender(fRenderDistance * g_kMadLODScale.GetFloat());
		int iLodIndex	= m_pkMesh->GetLODRenderIndex(fRenderDistance * g_kMadLODScale.GetFloat());
		if(!m_pkRawMesh)
			continue;
		if(iLodIndex == -1)
			continue;

		pkCore->PrepareMesh(pkCore->GetMeshByID(m_kActiveMesh[iM]), m_pkRawMesh);
		
			
	
		iNumOfSubMesh = GetNumOfSubMesh(m_kActiveMesh[iM]);
		
		for(int iSubM = 0; iSubM < iNumOfSubMesh; iSubM++) 
		{
			//setup all texture pointers
			m_pkShader->ResetPointers();		
			m_pkShader->SetPointer(TEXTURE_POINTER0,GetTextureCooPtr());	
			m_pkShader->SetPointer(VERTEX_POINTER,GetVerticesPtr());		
			m_pkShader->SetPointer(NORMAL_POINTER,GetNormalsPtr());						
			m_pkShader->SetDrawMode(TRIANGLES_MODE);
			m_pkShader->SetNrOfVertexs(GetNumVertices());
			
			
			SelectSubMesh(iSubM);

			if(iDrawFlags & MAD_DRAW_MESH || iDrawFlags & MAD_DRAW_LINES) 
			{
				iNumOfFaces = GetNumFaces();

				ZFResourceHandle* pkRes;
				
				//denna IF sats kan vara väldigt EVIL men hoppas den fungerar. 
				if(m_akReplaceTexturesHandles[ m_pkSubMesh->iTextureIndex ] != NULL &&
					m_akReplaceTexturesHandles[ m_pkSubMesh->iTextureIndex ]->IsValid())
				{
					pkRes = m_akReplaceTexturesHandles[m_pkSubMesh->iTextureIndex];
				}				
				else 
				{
					// m_pkMesh->GetLODMesh(0)
					pkRes = m_pkMesh->GetLODMesh(iLodIndex)->GetTextureHandle(m_pkSubMesh->iTextureIndex);
				}
				
				//setup material
				ZMaterial* pkMaterial = (ZMaterial*)(pkRes->GetResourcePtr());		
				
				
				if(iNumOfSubMesh == 1)
				{
					m_iFirstMaterialID = pkMaterial->GetID();
					m_bBlended = pkMaterial->GetPass(0)->m_bBlend;					
				}
				else
				{
					m_iFirstMaterialID = -1;
					m_bBlended = false;
				}
				
				if(iDrawFlags & MAD_DRAW_LINES)
					m_pkShader->BindMaterial(pkLineMaterial);				
				else if (iDrawFlags & MAD_DRAW_LODCODE)
				{
					if(iLodIndex == 0 )	pkLod->GetPass(0)->m_kVertexColor.Set(1,0,0,0);
					if(iLodIndex == 1 )	pkLod->GetPass(0)->m_kVertexColor.Set(0,1,0,0);
					if(iLodIndex == 2 )	pkLod->GetPass(0)->m_kVertexColor.Set(0,0,1,0);
					if(iLodIndex == 3 )	pkLod->GetPass(0)->m_kVertexColor.Set(1,1,0,0);
					if(iLodIndex > 3 )	pkLod->GetPass(0)->m_kVertexColor.Set(1,1,1,0);

					m_pkShader->BindMaterial(NULL);				
					m_pkShader->BindMaterial(pkLod);				
				}
				else				
					m_pkShader->BindMaterial(pkMaterial);				
				
				m_pkShader->SetPointer(INDEX_POINTER,GetFacesPtr());				
				m_pkShader->SetNrOfIndexes(iNumOfFaces * 3);
				
				
				//sutible for VBO ?
				if( iNumOfMesh == 1 && iNumOfSubMesh == 1 && 	iActiveAnimation == MAD_NOANIMINDEX && GetNumVertices() >500 )
				{
					if(!m_pkVBO)
  						m_pkVBO = m_pkShader->CreateVertexBuffer(); 				
				}
				else
				{
					if(m_pkVBO)
					{
						delete m_pkVBO;
						m_pkVBO = NULL;
					}
				}
					
				//VBO or normal rendering
//  				if(m_pkVBO)
//  					m_pkShader->DrawVertexBuffer(m_pkVBO);
//  				else
// 					m_pkShader->DrawArray();


				//TEMPORARY HACK FOR NORMALMAPPING
 				//m_kTangents.clear();
 				//m_kBiTangents.clear();
				if(m_kTangents.empty())
				{
					Math::GenerateTangents(	GetVerticesPtr(),
													GetNormalsPtr(),
													(Vector2*)GetTextureCooPtr(),
													GetFacesPtr(),
													m_kTangents,
													m_kBiTangents,
													iNumOfFaces * 3);												
				}								
				m_pkShader->SetPointer(TANGENT_POINTER,&m_kTangents[0]);
				m_pkShader->SetPointer(BITANGENT_POINTER,&m_kBiTangents[0]);
				m_pkShader->DrawArray();
				// END OF HACK
				
				
				g_iNumOfMadSurfaces += iNumOfFaces;
			}

			if(iDrawFlags & MAD_DRAW_NORMAL)
				DrawNormal(GetVerticesPtr(), GetNormalsPtr());
		}
	}

	
	if(iDrawFlags & MAD_DRAW_BONES)
		DrawSkelleton();

	m_pkRawMesh = NULL;
}

ZMaterial* Mad_Modell::GetMaterial(int iMesh,int iSubMesh)
{
	if(iMesh < 0 || iMesh >= m_kActiveMesh.size())
		return NULL;
	
	SelectMesh(m_kActiveMesh[iMesh]);
	m_pkRawMesh = m_pkMesh->GetLODMesh(0);
			
	if(iSubMesh < 0 || iSubMesh >= GetNumOfSubMesh(m_kActiveMesh[iMesh]))
		return NULL;
	
	SelectSubMesh(iSubMesh);
	ZFResourceHandle* pkRes = m_pkMesh->GetLODMesh(0)->GetTextureHandle(m_pkSubMesh->iTextureIndex);
		
	return (ZMaterial*)(pkRes->GetResourcePtr());		
}

/**
 * 
 * @param pkVertex 
 * @param pkNormals 
 */
void Mad_Modell::DrawNormal(Vector3* pkVertex, Vector3* pkNormals)
{
	static ZMaterial* pkLineMaterial = NULL;
	if(!pkLineMaterial)
	{
		pkLineMaterial = new ZMaterial();
		pkLineMaterial->GetPass(0)->m_iCullFace = CULL_FACE_NONE;
		pkLineMaterial->GetPass(0)->m_bLighting = false;
		pkLineMaterial->GetPass(0)->m_bFog = true;
		pkLineMaterial->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkLineMaterial->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
	}
	
	static ZMaterial* pkPointMaterial = NULL;
	if(!pkPointMaterial)
	{
		pkPointMaterial = new ZMaterial();
		pkPointMaterial->GetPass(0)->m_iCullFace = CULL_FACE_NONE;
		pkPointMaterial->GetPass(0)->m_bLighting = false;
		pkPointMaterial->GetPass(0)->m_bFog = true;
		pkPointMaterial->GetPass(0)->m_iPolygonModeFront = GL_POINT;
		pkPointMaterial->GetPass(0)->m_iPolygonModeBack = GL_POINT;
		pkPointMaterial->GetPass(0)->m_fLineWidth = 5;
		pkPointMaterial->GetPass(0)->m_bColorMaterial = true;
		pkPointMaterial->GetPass(0)->m_kVertexColor.Set(0,1,0,0);
	}

	bool bOld = m_pkShader->GetForceDisableGLSL();
	m_pkShader->SetForceDisableGLSL(true);

	//draw lines
	m_pkShader->ClearGeometry();	
	for(int i=0; i<GetNumVertices(); i++) 
	{	
		m_pkShader->AddLineV(pkVertex[i],pkVertex[i]+pkNormals[i].Unit() * 0.2);
	}

	m_pkShader->BindMaterial(pkLineMaterial);
	m_pkShader->DrawGeometry(LINES_MODE);

	
	//draw points
	m_pkShader->ClearGeometry();		
	for(int i=0; i<GetNumVertices(); i++) 
	{	
		m_pkShader->AddPointV(pkVertex[i]);
	}

	m_pkShader->BindMaterial(pkPointMaterial);	
	m_pkShader->DrawGeometry(POINTS_MODE);

	m_pkShader->SetForceDisableGLSL(bOld);

}


void Mad_Modell::DrawBone(const Vector3& From,const Vector3& To, bool bRoot)
{
	static ZMaterial* pkLineMaterial = NULL;
	if(!pkLineMaterial)
	{
		pkLineMaterial = new ZMaterial();
		pkLineMaterial->GetPass(0)->m_iCullFace = CULL_FACE_NONE;
		pkLineMaterial->GetPass(0)->m_bLighting = false;
		pkLineMaterial->GetPass(0)->m_bFog = true;
		pkLineMaterial->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkLineMaterial->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
	}
	
	static ZMaterial* pkPointMaterial = NULL;
	if(!pkPointMaterial)
	{
		pkPointMaterial = new ZMaterial();
		pkPointMaterial->GetPass(0)->m_iCullFace = CULL_FACE_NONE;
		pkPointMaterial->GetPass(0)->m_bLighting = false;
		pkPointMaterial->GetPass(0)->m_bFog = true;
		pkPointMaterial->GetPass(0)->m_iPolygonModeFront = GL_POINT;
		pkPointMaterial->GetPass(0)->m_iPolygonModeBack = GL_POINT;
		pkPointMaterial->GetPass(0)->m_fLineWidth = 5;
		pkPointMaterial->GetPass(0)->m_bColorMaterial = true;
		pkPointMaterial->GetPass(0)->m_kVertexColor.Set(1,1,0,0);
	}	
	
	static ZMaterial* pkPointMaterial2 = NULL;
	if(!pkPointMaterial2)
	{
		pkPointMaterial2 = new ZMaterial();
		pkPointMaterial2->GetPass(0)->m_iCullFace = CULL_FACE_NONE;
		pkPointMaterial2->GetPass(0)->m_bLighting = false;
		pkPointMaterial2->GetPass(0)->m_bFog = true;
		pkPointMaterial2->GetPass(0)->m_iPolygonModeFront = GL_POINT;
		pkPointMaterial2->GetPass(0)->m_iPolygonModeBack = GL_POINT;
		pkPointMaterial2->GetPass(0)->m_fLineWidth = 8;
		pkPointMaterial2->GetPass(0)->m_bColorMaterial = true;
		pkPointMaterial2->GetPass(0)->m_kVertexColor.Set(1,0,0,0);
	}		
	
	bool bOld = m_pkShader->GetForceDisableGLSL();
	m_pkShader->SetForceDisableGLSL(true);
	
	
	//draw line,if not root
	if(!bRoot)
	{
		m_pkShader->ClearGeometry();	
		m_pkShader->AddLineV(From,To);
		m_pkShader->BindMaterial(pkLineMaterial);
		m_pkShader->DrawGeometry(LINES_MODE);	
	}	
		
	//draw points,different color depending on bone type
	if(bRoot)
		m_pkShader->BindMaterial(pkPointMaterial2);	
	else
		m_pkShader->BindMaterial(pkPointMaterial);	
	
	
	m_pkShader->ClearGeometry();		
	m_pkShader->AddPointV(From);	
	m_pkShader->DrawGeometry(POINTS_MODE);	
	
	m_pkShader->SetForceDisableGLSL(bOld);

}

extern Matrix4		g_FullBoneTransform[MAX_BONES];		// Bone transformation matrix.
extern Matrix4		g_Madkbonetransform[MAX_BONES];		// Bone transformation matrix.


void Mad_Modell::DrawSkelleton()
{
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 

	for(unsigned int i=0; i<pkCore->GetNumOfBones(); i++) {
		if (pkCore->GetBoneParent(i) >= 0) 
			DrawBone(pkCore->GetBonePosition(pkCore->GetBoneParent(i)),	pkCore->GetBonePosition(i),false);
		else
			DrawBone(pkCore->GetBonePosition(i),pkCore->GetBonePosition(i),true);
	}
}

float Mad_Modell::GetSize()
{
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 
	if(!pkCore)
		return 0;

	return pkCore->GetSize() * m_fScale;	
}

float Mad_Modell::GetRadius()
{
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 
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
	if(Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr())) 
	{	
		if(pkCore->GetMeshByID(iSubId) == NULL)
			return false;
	
		m_kActiveMesh.push_back(iSubId);
		return true;
	}
	else
		return false;
	
}

bool Mad_Modell::AddMesh(char* szName)		
{ 
	Mad_Core* pkCore = (Mad_Core*)(kMadHandle.GetResourcePtr()); 
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


