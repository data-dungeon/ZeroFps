#include "render_plugins.h"
#include "zerofps.h"

// --------- SKY RENDERER

SkyRender::SkyRender() : PreRenderPlugin("SkyRender",-100)
{
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkEntityManager = static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));			
	m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));	

	

//  	AddTexture("skybox/winter/winter");
}

SkyRender::~SkyRender()
{
}

void SkyRender::Clear()
{	
	for(int i = 0;i<m_kMaterials.size();i++)
	{
		delete m_kMaterials[i];
	}
	
	m_kMaterials.clear();	
}

void SkyRender::AddTexture(const string& strName,SKY_HDR eHDR,const Vector3& kColor,bool bOnlySides)
{
	static string strNames[6] = {	"_north.tga",
											"_east.tga",
											"_south.tga",
											"_west.tga",
											"_up.tga",
											"_down.tga"};	
	
	static string strNameshi[6] = {	"_north_hi.tga",
												"_east_hi.tga",
												"_south_hi.tga",
												"_west_hi.tga",
												"_up_hi.tga",
												"_down_hi.tga"};	
	
	
	bool bBottom = m_kMaterials.empty();
	m_kMaterials.push_back(	new SkyLayer);
	
	
	SkyLayer& kLayer = *m_kMaterials[m_kMaterials.size()-1];
	
	kLayer.m_bSidesOnly = bOnlySides;


	for(int i = 0;i<6;i++)
	{	
		ZMaterial& kMaterial = kLayer.m_kMaterials[i];

		
		//setup material	
		kMaterial.GetPass(0)->m_iTUTexCords[1] = 		CORDS_FROM_ARRAY_0;	
		kMaterial.GetPass(0)->m_pkTUs[0]->SetRes(strName + strNames[i]);	
		kMaterial.GetPass(0)->m_iPolygonModeFront =	FILL_POLYGON;
		kMaterial.GetPass(0)->m_iCullFace = 			CULL_FACE_BACK;		
		kMaterial.GetPass(0)->m_bLighting = 			false;			
		kMaterial.GetPass(0)->m_bFog = 					false;		
		kMaterial.GetPass(0)->m_bDepthTest = 			false;		

		kMaterial.GetPass(0)->m_kVertexColor = 		Vector4(kColor.x,kColor.y,kColor.z,1);

		if(eHDR != NO_HDR && m_pkZShaderSystem->SupportGLSLProgram())
		{
			
			if(eHDR == LINEAR_HDR)
			{
				kMaterial.GetPass(0)->m_pkSLP->SetRes("#hdrsky_linear.frag.glsl");								
			}
			else if(eHDR == SQUARE_HDR)
			{
		 		kMaterial.GetPass(0)->m_pkSLP->SetRes("#hdrsky_square.frag.glsl");								
			}
			else
			{
				kMaterial.GetPass(0)->m_pkTUs[1]->SetRes(strName + strNameshi[i]);	
		 		kMaterial.GetPass(0)->m_pkSLP->SetRes("#hdrsky.frag.glsl");					
			}	
		}
		
		if(!bBottom)
		{		
			kMaterial.GetPass(0)->m_bBlend = 			true;		
			kMaterial.GetPass(0)->m_iBlendSrc =			SRC_ALPHA_BLEND_SRC;
			kMaterial.GetPass(0)->m_iBlendDst =			ONE_MINUS_SRC_ALPHA_BLEND_DST;			
		}
		
		
		//setup renderpackage
		kLayer.m_kRenderPackage[i].m_pkMaterial = &kLayer.m_kMaterials[i]; 
		
		switch(i)
		{
			case 0://north
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,1,1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,1,1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,-1,1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,-1,1));							
				break;					
			case 1://east
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,1,1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,1,-1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,-1,-1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,-1,1));							
				break;	
			case 2://south
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,1,-1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,1,-1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,-1,-1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,-1,-1));							
				break;	
			case 3://west
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,1,-1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,1,1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,-1,1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,-1,-1));							
				break;																	
			case 4://up
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,1,1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,1,1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,1,-1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,1,-1));											
				break;	
			case 5://down
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,-1,-1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,-1,-1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(-1,-1,1));
				kLayer.m_kRenderPackage[i].m_kMeshData.m_kVertises.push_back(Vector3(1,-1,1));							
				break;																					
		}
		
		kLayer.m_kRenderPackage[i].m_kMeshData.m_kTexture[0].push_back(Vector2(0,1));							
		kLayer.m_kRenderPackage[i].m_kMeshData.m_kTexture[0].push_back(Vector2(1,1));							
		kLayer.m_kRenderPackage[i].m_kMeshData.m_kTexture[0].push_back(Vector2(1,0));							
		kLayer.m_kRenderPackage[i].m_kMeshData.m_kTexture[0].push_back(Vector2(0,0));							
		
		
		kLayer.m_kRenderPackage[i].m_kMeshData.m_iNrOfDataElements = 4;
		kLayer.m_kRenderPackage[i].m_kMeshData.m_ePolygonMode = QUADS_MODE;
	}		
	
}

bool SkyRender::Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState)
{		
	if(m_kMaterials.empty())
	{
 		AddTexture("cp#skybox/rusted/rusted",SQUARE_HDR,Vector3(4,4,4));
//   		AddTexture("cp#skybox/sahara/sahara",LINEAR_HDR,Vector3(8,8,8)); 		
// 		AddTexture("cp#skybox/foglayer/foglayer",NO_HDR,Vector3(0.7,0.65,0.54),true);
	
	}

	//push matrises
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPush();	

	//load projection matrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixLoad(&kRenderState.m_kCameraProjectionMatrix);
	
	//reset modelview matrix and setup the newone
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
  	m_pkZShaderSystem->MatrixLoad(&kRenderState.m_kCameraRotation);		

	vector<RenderPackage*> kRenderPackages;
	
	//add renderpackages
	for(int i = 0;i<m_kMaterials.size();i++)
		for(int j = 0;j<5;j++)
		{
			if(j == 4 && m_kMaterials[i]->m_bSidesOnly) continue;			
			kRenderPackages.push_back(&m_kMaterials[i]->m_kRenderPackage[j]);
		}
	
	//render call to render engine
	kRenderEngine.DoRender(kRenderPackages,kRenderState);
	
			
	//pop matrises
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPop();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPop();			
	
	return true;
}




// ---------DepthMap renderer

DepthMapRenderer::DepthMapRenderer() : RenderPlugin("DepthMapRender",0)
{
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkLight			= static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));
	m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkEntityManager = static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));			

	m_pkDepthMaterial = new ZMaterial;

	m_pkDepthMaterial->GetPass(0)->m_iPolygonModeFront = FILL_POLYGON;
	m_pkDepthMaterial->GetPass(0)->m_iCullFace = CULL_FACE_BACK;		
	m_pkDepthMaterial->GetPass(0)->m_bLighting = false;
	m_pkDepthMaterial->GetPass(0)->m_bColorMaterial = false;
	m_pkDepthMaterial->GetPass(0)->m_bFog = false;

}

DepthMapRenderer::~DepthMapRenderer()
{
	delete m_pkDepthMaterial;
}


bool DepthMapRenderer::Call(ZSSRenderEngine& kRenderEngine,RenderPackage& kRenderPackage, const RenderState& kRenderState)
{
	//do this material cast shadows?
	if(!kRenderPackage.m_pkMaterial->m_bCastShadow || (!kRenderPackage.m_bStatic && !m_pkZeroFps->GetShadowMapRealtime()) )
		return true;

	//do occulusion test
	if(!kRenderEngine.OcculusionTest(kRenderPackage,kRenderState))
		return true;

	
	//push matrises	
	m_pkZShaderSystem->MatrixPush();
	
	//apply model matrix
	m_pkZShaderSystem->MatrixMult(kRenderPackage.m_kModelMatrix);
	
	//bind material
	if(kRenderPackage.m_pkMaterial->GetPass(0)->m_bAlphaTest)
	 	m_pkZShaderSystem->BindMaterial(kRenderPackage.m_pkMaterial);
	else
		m_pkZShaderSystem->BindMaterial(m_pkDepthMaterial);
		
	//SETUP MESH --------------
	MeshData& kMesh = kRenderPackage.m_kMeshData;
	
	//reset all data pointers	
	m_pkZShaderSystem->ResetPointers();


	bool bIndexed = false;
	const DataPointer* pkBoneMatrixPointer = NULL;
	const DataPointer* pkBoneIndexPointer = NULL;
	
	//setup new pointers from pointer array
	int iPointers = kMesh.m_kDataPointers.size();
	for(int i = 0;i<iPointers;i++)
	{
		//setup pointer
		m_pkZShaderSystem->SetPointer((POINTER_TYPE)kMesh.m_kDataPointers[i].m_iType,kMesh.m_kDataPointers[i].m_pkPointer);										
				
		switch(kMesh.m_kDataPointers[i].m_iType)
		{
			case INDEX_POINTER: 
				bIndexed = true;
				break;
			case BONEMATRIX_POINTER:
				pkBoneMatrixPointer = &kMesh.m_kDataPointers[i];
				break;
		
			case BONEINDEX_POINTER:
				pkBoneIndexPointer = &kMesh.m_kDataPointers[i];
				break;
		}
	}
	
	// setup new pointers from none pointer data, this will override any privius set pointers
	if(!kMesh.m_kVertises2D.empty()) 	m_pkZShaderSystem->SetPointer(VERTEX2D_POINTER,	&(kMesh.m_kVertises2D[0]));
	if(!kMesh.m_kVertises.empty()) 		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,	&(kMesh.m_kVertises[0]));
	if(!kMesh.m_kNormals.empty()) 		m_pkZShaderSystem->SetPointer(NORMAL_POINTER,	&(kMesh.m_kNormals[0]));
	if(!kMesh.m_kTexture[0].empty()) 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,	&(kMesh.m_kTexture[0][0]));
	if(!kMesh.m_kTexture[1].empty()) 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,	&(kMesh.m_kTexture[1][0]));
	if(!kMesh.m_kTexture[2].empty()) 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER2,	&(kMesh.m_kTexture[2][0]));
	if(!kMesh.m_kTexture[3].empty()) 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER3,	&(kMesh.m_kTexture[3][0]));
	if(!kMesh.m_kColors.empty()) 			m_pkZShaderSystem->SetPointer(COLOR_POINTER,		&(kMesh.m_kColors[0]));
	if(!kMesh.m_kTangents.empty()) 		m_pkZShaderSystem->SetPointer(TANGENT_POINTER,	&(kMesh.m_kTangents[0]));
	if(!kMesh.m_kBiTangents.empty()) 	m_pkZShaderSystem->SetPointer(BITANGENT_POINTER,&(kMesh.m_kBiTangents[0]));
	if(!kMesh.m_kIndexs.empty()) 			{m_pkZShaderSystem->SetPointer(INDEX_POINTER,	&(kMesh.m_kIndexs[0]));	bIndexed = true;	};
						
	//set number of elements
	if(bIndexed)
		m_pkZShaderSystem->SetNrOfIndexes(kMesh.m_iNrOfDataElements);
	else
		m_pkZShaderSystem->SetNrOfVertexs(kMesh.m_iNrOfDataElements);
	
	//---------------------
	
	
	//bone transformation
	if(kRenderPackage.m_pkMaterial->m_bBoneTransform && pkBoneMatrixPointer && pkBoneIndexPointer)
	{
		DoBoneTransformation(kRenderPackage,*pkBoneMatrixPointer, *pkBoneIndexPointer);
	}
	
	//draw
	m_pkZShaderSystem->DrawArray(kMesh.m_ePolygonMode);		
				
	m_pkZShaderSystem->MatrixPop();
	
	return true;
}

void DepthMapRenderer::DoBoneTransformation(const RenderPackage& kRenderPackage,const DataPointer& kBoneMatrises,const DataPointer& kBoneIndexes)
{
	static vector<Vector3>	kVertises;
	static vector<Vector3>	kNormals;
			
	const Vector3* pkOldVertises 			= (const Vector3*) m_pkZShaderSystem->GetPointer(VERTEX_POINTER);
	const Vector3* pkOldNormals 			= (const Vector3*) m_pkZShaderSystem->GetPointer(NORMAL_POINTER);	
	const int*	pkIndexes		 			= (const int*)	m_pkZShaderSystem->GetPointer(INDEX_POINTER);
	const int* pkBoneIndexes 				= (const int*) kBoneIndexes.m_pkPointer;	
	const Matrix4*	pkMatrix					= (const Matrix4*) kBoneMatrises.m_pkPointer;
	

	//find nr of vertises
	int iNrOfVertises = 0;	
	if(pkIndexes)
	{
		iNrOfVertises = kBoneIndexes.m_iElements;	
		if(iNrOfVertises == 0)
		{
			cerr<<"ERROR: DepthMapRenderer::DoBoneTransformation, indexed mesh needs nr of elements in bone index list"<<endl;
			return;				
		}
	}
	else
	 	iNrOfVertises = kRenderPackage.m_kMeshData.m_iNrOfDataElements;
	 	
	 	
	//resize arrays if they are to small
	if(kVertises.size() < iNrOfVertises)
	{
		kVertises.resize(iNrOfVertises);
		kNormals.resize(iNrOfVertises);
	}	 	
		
	//loop all elements and transform them
	for(int i = 0;i<iNrOfVertises;i++)
	{
		kVertises[i] = pkMatrix[ pkBoneIndexes[i] ].VectorTransform( pkOldVertises[i] );
		kNormals[i]  = pkMatrix[ pkBoneIndexes[i] ].VectorRotate( pkOldNormals[i] ); 
	}
	
 	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&kVertises[0]);
 	m_pkZShaderSystem->SetPointer(NORMAL_POINTER,&kNormals[0]);
}


// ---------- SHADOWMAP RENDER PLUGIN
ShadowmapPlugin::ShadowmapPlugin() : PreRenderPlugin("Shadowmap",-10)
{
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));	
	m_pkRender= 			static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));	
	m_pkZeroFps = 			static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));	
	m_pkEntityManager = 	static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));			
	m_pkLight = 			static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));			
		
	m_fShadowArea 		= 50;
	m_iShadowFBO 		= 0;
	m_iShadowRBOcolor = 0;
	m_bEnabled 			= true;
	
	m_kShadowRenderState.AddPlugin("DepthMapRender");
		
	//create shadowmap texture	
	if(	(!m_pkZShaderSystem->HaveExtension("GL_ARB_shadow")) || 
			(!m_pkZShaderSystem->HaveExtension("GL_ARB_depth_texture")) )
	{
		cerr<<"WARNING: GL_ARB_shadow not supported shadows disabled"<<endl;
		m_bEnabled = false;
	}
	else	
	{
		//SHADOW HACK
		
		//find shadowtexture size
		int iQuality = m_pkZeroFps->GetShadowMapQuality();
 		m_iShadowTexWidth = iQuality;
 		m_iShadowTexHeight = iQuality; 		 		
		
		if(m_pkZShaderSystem->SupportFBO())
		{
			int iMaxFBO;
			glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT,&iMaxFBO);
			
			g_Logf("CAMERA: Using FBO's for shadows , max resulution %dx%d", iMaxFBO, iMaxFBO);
			//cout<<"CAMERA: Using FBO's for shadows , max resulution "<<iMaxFBO<<"x"<<iMaxFBO<<endl;
	 		m_iShadowTexWidth = (int)Min(iQuality,iMaxFBO);
 			m_iShadowTexHeight = (int)Min(iQuality,iMaxFBO);;							
		}
		else
		{
			int iMaxWidth = GetMaxSize(m_pkRender->GetWidth());
			int iMaxHeight = GetMaxSize(m_pkRender->GetHeight());
		
			g_Logf("CAMERA: No FBO support, falling back to glCopyTexSubImage2D, max resulution %dx%d.\n",iMaxWidth,iMaxHeight);
			//cout<<"CAMERA: No FBO support, falling back to glCopyTexSubImage2D, max resulution "<<iMaxWidth<<"x"<<iMaxHeight<<endl;
	 		m_iShadowTexWidth = (int)Min(iQuality,iMaxWidth);
 			m_iShadowTexHeight = (int)Min(iQuality,iMaxHeight);;					
		}
			
		g_Logf("CAMERA: Using shadow texture size: %dx%d.\n", m_iShadowTexWidth, m_iShadowTexHeight);
		//cout<<"CAMERA: Using shadow texture size:"<<	m_iShadowTexWidth<<" "<<m_iShadowTexHeight<<endl;
				
		m_kShadowmap.m_kTexture.CreateEmptyTexture(m_iShadowTexWidth,m_iShadowTexHeight,T_DEPTH|T_CLAMPTOBORDER|T_NOCOMPRESSION|T_NOMIPMAPPING);
		m_kShadowmap.m_kTexture.SetBorderColor(Vector4(1,1,1,1));
 		
		
		//USE FBO ?
		if(m_pkZShaderSystem->SupportFBO())
		{
			//create FBO and color buffer
			glGetError();
			glGenFramebuffersEXT(1, &m_iShadowFBO);
			glGenRenderbuffersEXT(1, &m_iShadowRBOcolor);
			if(glGetError() != GL_NO_ERROR) cerr<<"ERROR generating FBO"<<endl;
			
			//bind frame buffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iShadowRBOcolor);			
			if(glGetError() != GL_NO_ERROR) cerr<<"ERROR binding FBO"<<endl;
			
			
			//setup color buffer
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_iShadowRBOcolor);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
												GL_RGBA, m_iShadowTexWidth, m_iShadowTexHeight);			
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
													GL_COLOR_ATTACHMENT0_EXT,
													GL_RENDERBUFFER_EXT, m_iShadowRBOcolor);			
			if(glGetError() != GL_NO_ERROR) cerr<<"ERROR color buffer"<<endl;
			
			
			// attach depth buffer texture                                 		
			glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT,
												GL_DEPTH_ATTACHMENT_EXT,
												GL_TEXTURE_2D, m_kShadowmap.m_kTexture.GetOpenGLTextureID(), 0);			
												
			if(glGetError() != GL_NO_ERROR) cerr<<"ERROR binding depth "<<endl;
				
			//check framebuffer status
			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			switch(status)
			{
				case GL_FRAMEBUFFER_COMPLETE_EXT:					
					break;
					
				case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
					cerr<<"ERROR unsupported FBO format"<<endl;
					break;
					
				default:
					cerr<<"ERROR fbo totaly failed"<<endl;					
					break;
			}
					
			//return to normal window buffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);	
		}
	
		//Light projection matrix
		m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);		
		m_pkZShaderSystem->MatrixGenerateOrtho(-m_fShadowArea,m_fShadowArea,-m_fShadowArea,m_fShadowArea,1,200);
		m_pkZShaderSystem->MatrixSave(&m_kLightProjMatrix);			
		m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);		
		
		
	}	
}

ShadowmapPlugin::~ShadowmapPlugin()
{
	if(m_iShadowFBO != 0)
		glDeleteFramebuffersEXT(1,&m_iShadowFBO);
	
	if(m_iShadowRBOcolor != 0)
		glDeleteRenderbuffersEXT(1,&m_iShadowRBOcolor);
}

void ShadowmapPlugin::MakeShadowTexture(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState,const Vector3& kLightPos,const Vector3& kCenter)
{
	//push matrises
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPush();	


	//load matrises
 	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
 	m_pkZShaderSystem->MatrixLoad(&m_kLightProjMatrix);
	
	//modelview matrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);	
	m_pkZShaderSystem->MatrixIdentity();
	gluLookAt( kLightPos.x, kLightPos.y, kLightPos.z,
					kCenter.x,kCenter.y,kCenter.z,
					0.0f, 1.0f, 0.0f);
					
	m_pkZShaderSystem->MatrixSave(&m_kLightViewMatrix);													

	//seutp frustum
	m_kShadowRenderState.m_kFrustum.GetFrustum(m_kLightProjMatrix,m_kLightViewMatrix);	
 	m_kShadowRenderState.m_kCameraPosition = kRenderState.m_kCameraPosition;

	//setup FBO if supported
	if(m_pkZShaderSystem->SupportFBO())
	{
		//enable FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iShadowFBO);
	}
	
	//setup viewport
	glViewport(0, 0, m_iShadowTexWidth, m_iShadowTexHeight);
	glScissor(0, 0, m_iShadowTexWidth, m_iShadowTexHeight);
	
	//clear viewport
	m_pkZShaderSystem->ClearBuffer(COLOR_BUFFER|DEPTH_BUFFER);
			
	//Disable color writes, and use flat shading for speed
  	m_pkZShaderSystem->ForceColorMask(FORCE_DISABLE);

 	//polygon offset
 	glEnable(GL_POLYGON_OFFSET_FILL);
  	glPolygonOffset(2,0);

	//reload last material
	m_pkZShaderSystem->ReloadMaterial();		
	
	//shading stuff
  	m_pkLight->SetLighting(false);
	m_pkZShaderSystem->ForceLighting(LIGHT_ALWAYS_OFF);
 	glShadeModel(GL_FLAT);
	
	//disable all shaders
	bool bFDGS = m_pkZShaderSystem->GetForceDisableGLSL();
	m_pkZShaderSystem->SetForceDisableGLSL(true);

	//depending on root entity
	vector<Property*>	kPropertys;	
	if(kRenderState.m_pkRoot)
		kRenderState.m_pkRoot->GetAllPropertys(&kPropertys, PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT);
	else
		m_pkEntityManager->m_pkSceneAABBTree->GetRenderPropertysInFrustum(&kPropertys,&m_kShadowRenderState.m_kFrustum);
	
	//get render packages from propertys
	vector<RenderPackage*>	kRenderPackages;
	int iPropertys = kPropertys.size();
	for(int i = 0;i<iPropertys;i++)
		kPropertys[i]->GetRenderPackages(kRenderPackages,m_kShadowRenderState);		

	//do render call
	kRenderEngine.DoRender(kRenderPackages,m_kShadowRenderState);
	
	//save texture	
	if(m_pkZShaderSystem->SupportFBO())
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	else
		m_kShadowmap.m_kTexture.CopyFrameBuffer(0,0,m_iShadowTexWidth,m_iShadowTexHeight);	
	
	
	//enable shaders again
	m_pkZShaderSystem->SetForceDisableGLSL(bFDGS);
	
  	glDepthRange (0.0, 1.0);
 	glDisable(GL_POLYGON_OFFSET_FILL);
  	glPolygonOffset(0,0);
	
  	m_pkLight->SetLighting(true);
	m_pkZShaderSystem->ForceLighting(LIGHT_MATERIAL);
 	glShadeModel(GL_SMOOTH);
	m_pkZShaderSystem->ForceColorMask(FORCE_DEFAULT);
		
		
	//return viewport settings
	glScissor(	GLint(kRenderState.m_kViewPortPos.x), 		GLint(kRenderState.m_kViewPortPos.y),	
					GLsizei(kRenderState.m_kViewPortSize.x), 	GLsizei(kRenderState.m_kViewPortSize.y) );					
	glViewport(	GLint(kRenderState.m_kViewPortPos.x), 		GLint(kRenderState.m_kViewPortPos.y),	
					GLsizei(kRenderState.m_kViewPortSize.x), 	GLsizei(kRenderState.m_kViewPortSize.y) );					
	
	m_pkZShaderSystem->ClearBuffer(COLOR_BUFFER|DEPTH_BUFFER);
	
	
	//reload last material
	m_pkZShaderSystem->ReloadMaterial();		
	
	//setup matrix
	static Matrix4 biasMatrix(	0.5f, 0.0f, 0.0f, 0.0f,
										0.0f, 0.5f, 0.0f, 0.0f,
										0.0f, 0.0f, 0.5f, 0.0f,
										0.5f, 0.5f, 0.5f, 1.0f); //bias from [-1, 1] to [0, 1]

	static Matrix4 Proj;
	static Matrix4 View;
	static Matrix4 Bias;
	
	Proj = m_kLightProjMatrix;
	View = m_kLightViewMatrix;
	Bias = biasMatrix;	

	View.Transponse();
	Proj.Transponse();
	Bias.Transponse();	
	
	m_kShadowmap.m_kTextureMatrix=Bias*(Proj*View);			
	
	
	
	//pop matrises
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPop();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPop();		
}

bool ShadowmapPlugin::Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState)
{
	if(!m_bEnabled) return true;
	if(!m_pkZShaderSystem->SupportGLSLProgram()) return true;
	
	//setup light
	LightSource* pkLight = m_pkLight->GetSunPointer();		
	Vector3 kLightPos = (kRenderState.m_kCameraPosition + (pkLight->kRot.Unit() * 100));

	
	//create shadow map	realtime or not
	if(m_pkZeroFps->GetShadowMapRealtime())
	{
		MakeShadowTexture(kRenderEngine,kRenderState,kLightPos,kRenderState.m_kCameraPosition);
	}
	else if(m_kLastShadowPos.DistanceTo(kRenderState.m_kCameraPosition) > m_fShadowArea/6.0 || m_kLastShadowRot != pkLight->kRot)
	{
		m_kLastShadowRot = pkLight->kRot;
		m_kLastShadowPos = kRenderState.m_kCameraPosition;
		
		MakeShadowTexture(kRenderEngine,kRenderState,kLightPos,kRenderState.m_kCameraPosition);
	}
	
	//set shadowmap width in shadersystem	
	m_pkZShaderSystem->SetShadowMapSize(m_iShadowTexWidth,m_iShadowTexHeight);
	
	//set shadowmap parameter
	kRenderEngine.SetParameter("shadowmap",&m_kShadowmap);
	
	
	return true;
}

int ShadowmapPlugin::GetMaxSize(int iRes)
{
	if(iRes >= 2048 )
		return(2048);
	else if(iRes >= 1024 )
		return(1024);
	else if(iRes >= 512 )
		return(512);
	else if(iRes >= 256 )
		return(256);
	
	return 128;
}

// ---------- HDR EXPOSURE CALCULATOR PLUGIN

ExposureCalculator::ExposureCalculator() : PostRenderPlugin("HdrExposure",10)
{
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));	
	m_pkRender= 			static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));	
	m_pkZeroFps = 			static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));	
	m_fExposureFactor	= 1.0;
		
  	m_iFSSTextureWidth = GetMinSize(m_pkRender->GetWidth());
 	m_iFSSTextureHeight = GetMinSize(m_pkRender->GetHeight());

	m_pkTex = new ResTexture;
	m_pkTex->CreateEmptyTexture(m_iFSSTextureWidth,m_iFSSTextureHeight,T_RGBA|T_CLAMP|T_NOCOMPRESSION);		
}

ExposureCalculator::~ExposureCalculator()
{
	delete m_pkTex;
}

bool ExposureCalculator::Call(RenderState& kRenderState)
{			
	if(!m_pkZShaderSystem->UseHDR())
		return true;
		
	m_fExposureFactor = GetExposureFactor(kRenderState);
// 	m_pkZShaderSystem->SetExposure(m_fExposureFactor);
	kRenderState.m_fExposure = m_fExposureFactor;
	
	return true;
}

float ExposureCalculator::GetExposureFactor(const RenderState& kRenderState)
{

	//get framebuffer
	m_pkTex->CopyFrameBuffer(	kRenderState.m_kViewPortPos.x,	kRenderState.m_kViewPortPos.y,
									kRenderState.m_kViewPortSize.x, 	kRenderState.m_kViewPortSize.y);
	m_pkTex->RegenerateMipmaps();
	
	float fx= float(kRenderState.m_kViewPortSize.x)/float(m_iFSSTextureWidth);
	float fy= float(kRenderState.m_kViewPortSize.y)/float(m_iFSSTextureHeight);
	
	Image* pkImage = m_pkTex->GetTextureImage(4);

	color_rgba	kColor;	
	float fTotal = 0;
	int iSamples = 0;
	
	for(int y = 0;y<pkImage->m_iHeight*fy;y++)
	{			
		for(int x = 0;x<pkImage->m_iWidth*fx;x++)
		{
			pkImage->get_pixel(x,y,kColor);	
			fTotal += (kColor.r*0.3+kColor.g*0.59+kColor.b*0.11);
			iSamples++;
		}			
	}
	
	delete pkImage;
	
	//
	//luminance = 0.3*R + 0.59 * G + 0.11*B
	//
 	static const float fTargetBrightness = 0.45;
	
	//avrage	
	float fExp = fTotal/ (iSamples * (255*0.3 + 255*0.59 + 255*0.11));
	
	//get current factor 	
 	float fEF = m_fExposureFactor;
 	
 	//adjust exposure factor
  	fEF += (fTargetBrightness - fExp)*m_pkZeroFps->GetFrameTime()*0.5;
	
	//min and max exposure
	if(fEF < 0.1) fEF = 0.1; 	
 	if(fEF > 1.0) fEF = 1.0;	
 	
	
	return fEF;
}

int ExposureCalculator::GetMinSize(int iRes)
{
	if(iRes <= 256 )
		return 256;
	else if(iRes <= 512)
		return 512;
	else if(iRes <= 1024)
		return 1024;
	else if(iRes <= 2048)
		return 2048;
		
	return 256;
}

// --------- debug render

DebugRenderPlugin::DebugRenderPlugin() : RenderPlugin("DebugRender",0)
{
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkLight			= static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));
	m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkRender			= static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));
	
	m_pkLineMaterial = new ZMaterial;
	m_pkLineMaterial->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
	m_pkLineMaterial->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
	m_pkLineMaterial->GetPass(0)->m_iCullFace = CULL_FACE_NONE;		
	m_pkLineMaterial->GetPass(0)->m_bLighting = false;
	m_pkLineMaterial->GetPass(0)->m_bColorMaterial = true;
}

DebugRenderPlugin::~DebugRenderPlugin()
{
	delete m_pkLineMaterial;
}


bool DebugRenderPlugin::Call(ZSSRenderEngine& kRenderEngine,RenderPackage& kRenderPackage, const RenderState& kRenderState)
{		
	//bind material
	m_pkZShaderSystem->BindMaterial(kRenderPackage.m_pkMaterial);
	
	
	//SETUP MESH --------------
	MeshData& kMesh = kRenderPackage.m_kMeshData;
	
	
	const DataPointer* pkBoneMatrixPointer = NULL;
	const DataPointer* pkBoneIndexPointer = NULL;
	
	const Vector3* 		pkVertexPointer = NULL;
	const Vector3* 		pkNormalPointer = NULL;
	const unsigned int*	pkIndexPointer = NULL;
	int iVertises = 0;
	
	//setup new pointers from pointer array
	int iPointers = kMesh.m_kDataPointers.size();
	for(int i = 0;i<iPointers;i++)
	{
		switch(kMesh.m_kDataPointers[i].m_iType)
		{
			case VERTEX_POINTER: 
			{
				pkVertexPointer = (const Vector3*)kMesh.m_kDataPointers[i].m_pkPointer;
				iVertises = kMesh.m_kDataPointers[i].m_iElements;
				break;
			}
			case NORMAL_POINTER: 
				pkNormalPointer = (const Vector3*)kMesh.m_kDataPointers[i].m_pkPointer;
				break;		
			case INDEX_POINTER: 
				pkIndexPointer = (const unsigned int*)kMesh.m_kDataPointers[i].m_pkPointer;
				break;					
		
			case BONEMATRIX_POINTER: 
				pkBoneMatrixPointer = &kMesh.m_kDataPointers[i];
				break;					
			case BONEINDEX_POINTER: 
				pkBoneIndexPointer = &kMesh.m_kDataPointers[i];
				break;					
		
		}
	}	
	
	if(!kMesh.m_kVertises.empty())
	{ 	
		pkVertexPointer = &(kMesh.m_kVertises[0]);
		iVertises = kMesh.m_kVertises.size();
	}
	if(!kMesh.m_kNormals.empty()) 	pkNormalPointer =	&(kMesh.m_kNormals[0]);
	if(!kMesh.m_kIndexs.empty()) 	pkIndexPointer =	&(kMesh.m_kIndexs[0]);
						
	if(!pkVertexPointer || !pkNormalPointer)
		return true;
						
	if(!pkIndexPointer)
		iVertises = kMesh.m_iNrOfDataElements;				
						
 	if(iVertises == 0)
 	{
 		cout<<"WARNING: number of vertises is 0"<<endl;
 		return true;
 	}
						
						
	//bone transformation
	if(pkBoneMatrixPointer && pkBoneIndexPointer)
	{
		DoBoneTransformation(iVertises,*pkBoneMatrixPointer, *pkBoneIndexPointer,&pkVertexPointer,&pkNormalPointer);
	}							
						
	vector<Vector3>	kNormals;
	vector<Vector4>	kNormalColors;
	
	//loop all vertises and build normal array
	for(int i=0;i<iVertises;i++)
	{
		kNormals.push_back(pkVertexPointer[i]);
		kNormals.push_back(pkVertexPointer[i] + pkNormalPointer[i]*0.2 );			
		kNormalColors.push_back(Vector4(1,0,0,1));
		kNormalColors.push_back(Vector4(0,1,0,1));
	}
					
					
	//push matrises	
	m_pkZShaderSystem->MatrixPush();
	
	//apply model matrix
	m_pkZShaderSystem->MatrixMult(kRenderPackage.m_kModelMatrix);
						
						
	//setup and draw normals
	bool bUDGS = m_pkZShaderSystem->GetUseDefaultGLSLProgram();
	m_pkZShaderSystem->UseDefaultGLSLProgram(false);
	
	m_pkZShaderSystem->ResetPointers();	
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&kNormals[0]);
	m_pkZShaderSystem->SetPointer(COLOR_POINTER,&kNormalColors[0]);
	m_pkZShaderSystem->SetNrOfVertexs(kNormals.size());
	m_pkZShaderSystem->BindMaterial(m_pkLineMaterial);
	m_pkZShaderSystem->DrawArray(LINES_MODE);
				
				
	m_pkZShaderSystem->MatrixPop();
				
				
	//DRAW RENDERPACKAGES
	m_pkRender->DrawAABB(kRenderPackage.m_kCenter - Vector3(0.25,0.25,0.25),
								kRenderPackage.m_kCenter + Vector3(0.25,0.25,0.25),
								Vector3(1,1,1),2);
				
	
	if(kRenderPackage.m_bOcculusionTest && !kRenderPackage.m_pkOcculusionParent)
	{
		//DRAW 
		m_pkRender->DrawAABB(kRenderPackage.m_kCenter + kRenderPackage.m_kAABBMin,
									kRenderPackage.m_kCenter + kRenderPackage.m_kAABBMax,
									Vector3(0,1,0),1);				
	}
					
					
	m_pkZShaderSystem->UseDefaultGLSLProgram(bUDGS);
	
	return true;
}

void DebugRenderPlugin::DoBoneTransformation(int iVertises,const DataPointer& kBoneMatrises,const DataPointer& kBoneIndexes,const Vector3** pkVertises,const Vector3** pkNormals)
{
	static vector<Vector3>	kVertises;
	static vector<Vector3>	kNormals;
			
	const Vector3* pkOldVertises 			= *pkVertises;
	const Vector3* pkOldNormals 			= *pkNormals;
	const int* pkBoneIndexes 				= (const int*) kBoneIndexes.m_pkPointer;	
	const Matrix4*	pkMatrix					= ( Matrix4*) kBoneMatrises.m_pkPointer;
		 	
	 	
	//resize arrays if they are to small
	if(kVertises.size() < iVertises)
	{
		kVertises.resize(iVertises);
		kNormals.resize(iVertises);
	}	 	
		
	//loop all elements and transform them
	for(int i = 0;i<iVertises;i++)
	{
		kVertises[i] = pkMatrix[ pkBoneIndexes[i] ].VectorTransform( pkOldVertises[i] );
		kNormals[i]  = pkMatrix[ pkBoneIndexes[i] ].VectorRotate( pkOldNormals[i] ); 
	}
	
		
	*pkVertises = &kVertises[0];
	*pkNormals = &kNormals[0]; 
}

// --------AttachToJoint
bool AttachToJoint::Call(ZSSRenderEngine& kRenderEngine,RenderPackage& kRenderPackage, const RenderState& kRenderState)
{
	if(kRenderPackage.m_iAttachToEntityID == -1)
		return true;
		
		
	if(P_Mad* pkMad = (P_Mad*)m_pkEntityManager->GetPropertyFromEntityID( kRenderPackage.m_iAttachToEntityID,"P_Mad"))
	{
		//find scale
		static Vector3 kUnit(1,0,0);			
		float fScale = kRenderPackage.m_kModelMatrix.VectorRotate(kUnit).Length();
		
		//apply rotation and set position
		kRenderPackage.m_kModelMatrix = pkMad->GetJointRotation(kRenderPackage.m_strAttachToJointName);
		kRenderPackage.m_kModelMatrix.Scale(fScale);
		kRenderPackage.m_kCenter = pkMad->GetJointPosition(kRenderPackage.m_strAttachToJointName) + pkMad->GetEntity()->GetIWorldPosV();
		kRenderPackage.m_kModelMatrix.SetPos(kRenderPackage.m_kCenter);		
	}

	
	return true;
}


// ---------DefaultRenderPlugin

bool DefaultRenderPlugin::Call(ZSSRenderEngine& kRenderEngine,RenderPackage& kRenderPackage, const RenderState& kRenderState)
{

	//HACK
	if(kRenderPackage.m_pkProperty)
	{
		m_pkEntityManager->SetCurrentUpdateFlags(PROPERTY_TYPE_RENDER|PROPERTY_SIDE_CLIENT);
		kRenderPackage.m_pkProperty->Update();
		return true;
	}

	//do occulusion test
	if(!kRenderEngine.OcculusionTest(kRenderPackage,kRenderState))
		return true;
 	

	//setup shadowmaping
	ShadowMap* pkShadowmap = (ShadowMap*)kRenderEngine.GetParameter("shadowmap");	
	if(pkShadowmap && kRenderPackage.m_pkMaterial->m_bReceiveShadow)
		SetupShadowmap(pkShadowmap);	


	//setup lighting if any
	if(kRenderPackage.m_pkLightProfile)
	{
		m_pkLight->Update(kRenderPackage.m_pkLightProfile,kRenderPackage.m_kCenter,kRenderPackage.m_fRadius);
	}		
	
	//push matrises	
	m_pkZShaderSystem->MatrixPush();
	
	//apply model matrix
	m_pkZShaderSystem->MatrixMult(kRenderPackage.m_kModelMatrix);
	
	//bind material
	m_pkZShaderSystem->BindMaterial(kRenderPackage.m_pkMaterial);
	
	
	//SETUP MESH --------------
	MeshData& kMesh = kRenderPackage.m_kMeshData;
	
	//reset all data pointers	
	m_pkZShaderSystem->ResetPointers();



	bool bIndexed = false;
	const DataPointer* pkBoneMatrixPointer = NULL;
	const DataPointer* pkBoneIndexPointer = NULL;
	
	//setup new pointers from pointer array
	int iPointers = kMesh.m_kDataPointers.size();
	for(int i = 0;i<iPointers;i++)
	{
		//setup pointer
		m_pkZShaderSystem->SetPointer((POINTER_TYPE)kMesh.m_kDataPointers[i].m_iType,kMesh.m_kDataPointers[i].m_pkPointer);										
				
		switch(kMesh.m_kDataPointers[i].m_iType)
		{
			case INDEX_POINTER: 
				bIndexed = true;
				break;
			case BONEMATRIX_POINTER:
				pkBoneMatrixPointer = &kMesh.m_kDataPointers[i];
				break;
		
			case BONEINDEX_POINTER:
				pkBoneIndexPointer = &kMesh.m_kDataPointers[i];
				break;
		}
	}
	
	// setup new pointers from none pointer data, this will override any privius set pointers
	if(!kMesh.m_kVertises2D.empty()) 	m_pkZShaderSystem->SetPointer(VERTEX2D_POINTER,	&(kMesh.m_kVertises2D[0]));
	if(!kMesh.m_kVertises.empty()) 		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,	&(kMesh.m_kVertises[0]));
	if(!kMesh.m_kNormals.empty()) 		m_pkZShaderSystem->SetPointer(NORMAL_POINTER,	&(kMesh.m_kNormals[0]));
	if(!kMesh.m_kTexture[0].empty()) 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,	&(kMesh.m_kTexture[0][0]));
	if(!kMesh.m_kTexture[1].empty()) 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,	&(kMesh.m_kTexture[1][0]));
	if(!kMesh.m_kTexture[2].empty()) 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER2,	&(kMesh.m_kTexture[2][0]));
	if(!kMesh.m_kTexture[3].empty()) 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER3,	&(kMesh.m_kTexture[3][0]));
	if(!kMesh.m_kColors.empty()) 			m_pkZShaderSystem->SetPointer(COLOR_POINTER,		&(kMesh.m_kColors[0]));
	if(!kMesh.m_kTangents.empty()) 		m_pkZShaderSystem->SetPointer(TANGENT_POINTER,	&(kMesh.m_kTangents[0]));
	if(!kMesh.m_kBiTangents.empty()) 	m_pkZShaderSystem->SetPointer(BITANGENT_POINTER,&(kMesh.m_kBiTangents[0]));
	if(!kMesh.m_kIndexs.empty()) 			{m_pkZShaderSystem->SetPointer(INDEX_POINTER,	&(kMesh.m_kIndexs[0]));	bIndexed = true;	};
						
	//set number of elements
	if(bIndexed)
		m_pkZShaderSystem->SetNrOfIndexes(kMesh.m_iNrOfDataElements);
	else
		m_pkZShaderSystem->SetNrOfVertexs(kMesh.m_iNrOfDataElements);
	
	//---------------------
	
	
	//bone transformation
	if(kRenderPackage.m_pkMaterial->m_bBoneTransform && pkBoneMatrixPointer && pkBoneIndexPointer)
	{
		DoBoneTransformation(kRenderPackage,*pkBoneMatrixPointer, *pkBoneIndexPointer);
	}
	

	//draw
	m_pkZShaderSystem->DrawArray(kMesh.m_ePolygonMode);		
				
	//disable shadowmap
	if(pkShadowmap && kRenderPackage.m_pkMaterial->m_bReceiveShadow)
		SetupShadowmap(NULL);	
				
				
	m_pkZShaderSystem->MatrixPop();
	
	return true;
}


void DefaultRenderPlugin::SetupShadowmap(ShadowMap* pkShadowmap)
{
	if(pkShadowmap)
	{
		m_pkZShaderSystem->ForceTU3Disabled(true);
	
		//setup some textures
		glActiveTextureARB(GL_TEXTURE3_ARB);						
		
		glEnable(GL_TEXTURE_2D);
		
		//Bind & enable shadow map texture
		m_pkZShaderSystem->BindTexture(&pkShadowmap->m_kTexture);
		
		
		
		//Set up texture coordinate generation.
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_S, GL_EYE_PLANE, &(pkShadowmap->m_kTextureMatrix.RowCol[0][0]));
		glEnable(GL_TEXTURE_GEN_S);
		
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_T, GL_EYE_PLANE, &(pkShadowmap->m_kTextureMatrix.RowCol[1][0]));
		glEnable(GL_TEXTURE_GEN_T);
		
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_R, GL_EYE_PLANE, &(pkShadowmap->m_kTextureMatrix.RowCol[2][0]));
		glEnable(GL_TEXTURE_GEN_R);
		
		glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_Q, GL_EYE_PLANE, &(pkShadowmap->m_kTextureMatrix.RowCol[3][0]));
		glEnable(GL_TEXTURE_GEN_Q);
	
		
		//Enable shadow comparison
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
		
		//Shadow comparison should be true (ie not in shadow) if r<=texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_GEQUAL);
		
		//Shadow comparison should generate an INTENSITY result
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_ALPHA);
		
		//Set alpha test to discard false comparisons
		//if((!m_pkZeroFps->GetShadowMapMode()) || (!m_pkZShaderSystem->SupportGLSLProgram()))
		//	m_pkZShaderSystem->ForceAlphaTest(FORCE_OTHER);
	}
	else
	{
		
		glActiveTextureARB(GL_TEXTURE3_ARB);
		
		//m_pkZShaderSystem->ForceAlphaTest(FORCE_DEFAULT);
		
		//Disable textures and texgen
		glDisable(GL_TEXTURE_2D);
		
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		
		//Restore other states
		glActiveTextureARB(GL_TEXTURE0_ARB);
		
		m_pkZShaderSystem->ForceTU3Disabled(false);	
	}
}

void DefaultRenderPlugin::DoBoneTransformation(const RenderPackage& kRenderPackage,const DataPointer& kBoneMatrises,const DataPointer& kBoneIndexes)
{
	static vector<Vector3>	kVertises;
	static vector<Vector3>	kNormals;
			
	const Vector3* pkOldVertises 			= (const Vector3*) m_pkZShaderSystem->GetPointer(VERTEX_POINTER);
	const Vector3* pkOldNormals 			= (const Vector3*) m_pkZShaderSystem->GetPointer(NORMAL_POINTER);	
	const int*	pkIndexes		 			= (const int*)	m_pkZShaderSystem->GetPointer(INDEX_POINTER);
	const int* pkBoneIndexes 				= (const int*) kBoneIndexes.m_pkPointer;	
	const Matrix4*	pkMatrix					= (const Matrix4*) kBoneMatrises.m_pkPointer;
	

	//find nr of vertises
	int iNrOfVertises = 0;	
	if(pkIndexes)
	{
		iNrOfVertises = kBoneIndexes.m_iElements;	
		if(iNrOfVertises == 0)
		{
			cerr<<"ERROR: DefaultRenderPlugin::DoBoneTransformation, indexed mesh needs nr of elements in bone index list"<<endl;
			return;				
		}
	}
	else
	 	iNrOfVertises = kRenderPackage.m_kMeshData.m_iNrOfDataElements;
	 	
	 	
	//resize arrays if they are to small
	if(kVertises.size() < iNrOfVertises)
	{
		kVertises.resize(iNrOfVertises);
		kNormals.resize(iNrOfVertises);
	}	 	
		
	//loop all elements and transform them
	for(int i = 0;i<iNrOfVertises;i++)
	{
		kVertises[i] = pkMatrix[ pkBoneIndexes[i] ].VectorTransform( pkOldVertises[i] );
		kNormals[i]  = pkMatrix[ pkBoneIndexes[i] ].VectorRotate( pkOldNormals[i] ); 
	}
	
 	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&kVertises[0]);
 	m_pkZShaderSystem->SetPointer(NORMAL_POINTER,&kNormals[0]);
}

//---------- PRE RENDER

DefaultPreRenderPlugin::DefaultPreRenderPlugin() : PreRenderPlugin("PreRender",0)
{
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkEntityManager = static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));			
	m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));	

	//setup default shaders
	m_pkDefaultShader = new ZFResourceHandle;
 	m_pkDefaultShader->SetRes("shadowmap.vert#shadowmap.frag.glsl"); 				
	m_pkDefaultShaderPPL= new ZFResourceHandle;
 	m_pkDefaultShaderPPL->SetRes("pplshadowmap.vert#pplshadowmap.frag.glsl"); 				
	m_pkDefaultFastShadowmapShader = new ZFResourceHandle;
	m_pkDefaultFastShadowmapShader->SetRes("#fastshadowmap.frag.glsl");
}

DefaultPreRenderPlugin::~DefaultPreRenderPlugin()
{
	delete m_pkDefaultShader;
	delete m_pkDefaultShaderPPL;
	delete m_pkDefaultFastShadowmapShader;
}

bool DefaultPreRenderPlugin::Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState)
{		
	//push matrises
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPush();	

	kRenderEngine.SetupView(kRenderState);

	//get all propertys			
	vector<Property*>	kPropertys;
	
	//depending on root entity
	if(kRenderState.m_pkRoot)
		kRenderState.m_pkRoot->GetAllPropertys(&kPropertys, PROPERTY_TYPE_RENDER,PROPERTY_SIDE_CLIENT);
	else
		m_pkEntityManager->m_pkSceneAABBTree->GetRenderPropertysInFrustum(&kPropertys,&kRenderState.m_kFrustum);

	
	//get render packages from propertys
	vector<RenderPackage*>	kRenderPackages;
	for(int i = 0;i<kPropertys.size();i++)
	{		
		kPropertys[i]->GetRenderPackages(kRenderPackages,kRenderState);		
	}
		
	//sort
	int iPackages = kRenderPackages.size();
	for(int i = 0 ;i<iPackages;i++)
	{
		kRenderPackages[i]->m_fDistanceToCamera = kRenderState.m_kCameraPosition.DistanceTo(kRenderPackages[i]->m_kCenter)  - kRenderPackages[i]->m_fRadius;
	}	
	stable_sort(kRenderPackages.begin(),kRenderPackages.end(),Less_RenderPackage);
	
	//set default glsl shader
	switch(m_pkZeroFps->GetShadowMapMode())
	{
		case 0:
			m_pkZShaderSystem->UseDefaultGLSLProgram(false);
			break;
	
		case 1:
			m_pkZShaderSystem->UseDefaultGLSLProgram(true);
			m_pkZShaderSystem->SetDefaultGLSLProgram(m_pkDefaultShader);
			break;
		case 2:
			m_pkZShaderSystem->UseDefaultGLSLProgram(true);
			m_pkZShaderSystem->SetDefaultGLSLProgram(m_pkDefaultFastShadowmapShader);
			break;
		case 3:
			m_pkZShaderSystem->UseDefaultGLSLProgram(true);
			m_pkZShaderSystem->SetDefaultGLSLProgram(m_pkDefaultShaderPPL);
			break;
	}	
	
	

	
	
	//render call to render engine
	kRenderEngine.DoRender(kRenderPackages,kRenderState);
	
	
	//disable default glslprogram
	m_pkZShaderSystem->UseDefaultGLSLProgram(false);
		
	//pop matrises
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPop();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPop();			
	
	return true;
}



//------ BLOOM

BloomPostPlugin::BloomPostPlugin() : PostRenderPlugin("Bloom",0)
{
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));	
	m_pkRender= 			static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));	
		

	//BLOOM 
	m_pkBloomMaterial1 = new ZMaterial;
	m_pkBloomMaterial1->GetPass(0)->m_pkSLP->SetRes("#bloom1.frag.glsl");
	m_pkBloomMaterial1->GetPass(0)->m_pkTUs[0]->SetRes("notex.bmp");
	m_pkBloomMaterial1->GetPass(0)->m_bLighting = false;
	m_pkBloomMaterial1->GetPass(0)->m_bDepthTest = false;
	
	//BLOOM 
	m_pkBloomMaterial2 = new ZMaterial;
	m_pkBloomMaterial2->GetPass(0)->m_pkSLP->SetRes("#bloom2.frag.glsl");
	m_pkBloomMaterial2->GetPass(0)->m_pkTUs[0]->SetRes("notex.bmp");
	m_pkBloomMaterial2->GetPass(0)->m_bLighting = false;
	m_pkBloomMaterial2->GetPass(0)->m_bDepthTest = false;
	
	
	m_pkBloomMaterial3 = new ZMaterial;
	m_pkBloomMaterial3->GetPass(0)->m_pkSLP->SetRes("#bloom3.frag.glsl");
	m_pkBloomMaterial3->GetPass(0)->m_pkTUs[0]->SetRes("notex.bmp");
	m_pkBloomMaterial3->GetPass(0)->m_pkTUs[1]->SetRes("notex.bmp");
	m_pkBloomMaterial3->GetPass(0)->m_iTUTexCords[1] = CORDS_FROM_ARRAY_1;
	m_pkBloomMaterial3->GetPass(0)->m_bLighting = false;
	m_pkBloomMaterial3->GetPass(0)->m_bDepthTest = false;
	
  	m_iFSSTextureWidth = GetMinSize(m_pkRender->GetWidth());
 	m_iFSSTextureHeight = GetMinSize(m_pkRender->GetHeight());
	m_kBloomTexture.CreateEmptyTexture(m_iFSSTextureWidth,m_iFSSTextureHeight,T_RGBA|T_CLAMP|T_NOCOMPRESSION|T_NOMIPMAPPING);
	m_kBloomTexture2.CreateEmptyTexture(m_iFSSTextureWidth,m_iFSSTextureHeight,T_RGBA|T_CLAMP|T_NOCOMPRESSION|T_NOMIPMAPPING);

	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);		
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixGenerateOrtho(-1,1,-1,1,0,2);
	m_pkZShaderSystem->MatrixSave(&m_kFSSProjMatrix);			
	m_pkZShaderSystem->MatrixPop();
}

bool BloomPostPlugin::Validate(const RenderState& kRenderState)
{
	if(!m_pkZShaderSystem->SupportGLSLProgram())
	{
		cerr<<"WARNING: BloomPostPlugin requires glsl support"<<endl;
		return false;
	}
	
	return true;
}

BloomPostPlugin::~BloomPostPlugin()
{
	delete m_pkBloomMaterial1;
	delete m_pkBloomMaterial2;
}

bool BloomPostPlugin::Call(RenderState& kRenderState)
{
	float fSize = 0.25;

	static float data[]= {	-1,-1,-1,
									 1,-1,-1,
							 		 1, 1,-1,
									-1, 1,-1};

	static float smalldata[]= {	-1				,-1			,-1,
											-1+2*fSize	,-1			,-1,
							 				-1+2*fSize	,-1+2*fSize	,-1,
											-1				,-1+2*fSize	,-1};

	static float uvdata[]= {0,0,
									1,0,
							 		1,1,
									0,1};
									
	static float smalluvdata[]= {	0,0,
											1,0,
							 				1,1,
											0,1};
									
									
	//calculate how to stretch the texture									
 	float xs = 	float(kRenderState.m_kViewPortSize.x) / float(m_iFSSTextureWidth);								
 	float ys = 	float(kRenderState.m_kViewPortSize.y) / float(m_iFSSTextureHeight);

 	uvdata[2] = xs;
 	uvdata[4] = xs;
 	uvdata[5] = ys;
 	uvdata[7] = ys;
	
 	smalluvdata[2] = xs*fSize;
 	smalluvdata[4] = xs*fSize;
 	smalluvdata[5] = ys*fSize;
 	smalluvdata[7] = ys*fSize;	
	
	
	
	m_pkZShaderSystem->SetShadowMapSize( m_iFSSTextureWidth*xs*fSize,m_iFSSTextureHeight*ys*fSize);
	
	
	//save screen surface to fss texture									
	m_kBloomTexture.CopyFrameBuffer(kRenderState.m_kViewPortPos.x,kRenderState.m_kViewPortPos.y,kRenderState.m_kViewPortSize.x, kRenderState.m_kViewPortSize.y);
			
	//setup fss orthogonal projection matrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixLoad(&m_kFSSProjMatrix);
	
	//identity
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixIdentity();
	
	//bind bloom material	1
	m_pkZShaderSystem->BindMaterial(m_pkBloomMaterial1);
	
	//setup vertex data
	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,smalldata);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,uvdata);
	m_pkZShaderSystem->SetNrOfVertexs(4);
	
		
  	//we dont wan to write any depth data
  	m_pkZShaderSystem->SetDepthMask(false);
  	
  	//ugly haxk to force another texture then the one specified in the material
	m_pkZShaderSystem->BindTexture(&m_kBloomTexture);
  	
  	
	//draw first pass X blurr
 	m_pkZShaderSystem->DrawArray(QUADS_MODE);  	
  	
  	
	//save image, and do another draw	
	m_kBloomTexture2.CopyFrameBuffer(	kRenderState.m_kViewPortPos.x,kRenderState.m_kViewPortPos.y,
													kRenderState.m_kViewPortSize.x*fSize, kRenderState.m_kViewPortSize.y*fSize);
	
	m_pkZShaderSystem->BindMaterial(m_pkBloomMaterial2);	
	m_pkZShaderSystem->BindTexture(&m_kBloomTexture2);		
	
	//draw seccond pass Y blurr
 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,smalluvdata); 	
 	m_pkZShaderSystem->DrawArray(QUADS_MODE);  	
	
	m_kBloomTexture2.CopyFrameBuffer(	kRenderState.m_kViewPortPos.x,kRenderState.m_kViewPortPos.y,
													kRenderState.m_kViewPortSize.x*fSize, kRenderState.m_kViewPortSize.y*fSize);
	
	m_pkZShaderSystem->BindMaterial(m_pkBloomMaterial3);	
	m_pkZShaderSystem->BindTexture(&m_kBloomTexture2);		
		
 	//bind original texture
 	glActiveTextureARB(GL_TEXTURE1_ARB);
 	m_pkZShaderSystem->BindTexture(&m_kBloomTexture);
 	glActiveTextureARB(GL_TEXTURE0_ARB);
 	
 	//draw final combining pass
 	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,data);	
 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,smalluvdata);
 	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,uvdata);	
 	m_pkZShaderSystem->DrawArray(QUADS_MODE); 
	
		
  	//reenable depth mask
	m_pkZShaderSystem->SetDepthMask(true);
  	
  	
  	//pop matrices
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);	
	m_pkZShaderSystem->MatrixPop();	
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);	
	m_pkZShaderSystem->MatrixPop();		

	return true;
}

int BloomPostPlugin::GetMinSize(int iRes)
{
	if(iRes <= 256 )
		return 256;
	else if(iRes <= 512)
		return 512;
	else if(iRes <= 1024)
		return 1024;
	else if(iRes <= 2048)
		return 2048;
		
	return 256;
}

// -------- InterfaceRender
InterfaceRender::InterfaceRender() : PreRenderPlugin("InterfaceRender",0)
{
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkLight			= static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));
	m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
	m_pkApplication	= static_cast<Application*>(g_ZFObjSys.GetObjectPtr("Application"));
	m_pkEntityManager = static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));			

	//line material
	m_pkLine = new ZMaterial;
	m_pkLine->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
	m_pkLine->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
	m_pkLine->GetPass(0)->m_iCullFace = CULL_FACE_NONE;		
	m_pkLine->GetPass(0)->m_bLighting = false;
	m_pkLine->GetPass(0)->m_bColorMaterial = true;
	m_pkLine->GetPass(0)->m_bFog = false;
		
		
	//axes icon
	static float afVertexData[] ={0,0,0,
											2,0,0,
											0,0,0,
											0,2,0,
											0,0,0,
											0,0,2};	
		
	static float afColorData[] ={	1,0,0,1,
											1,0,0,1,
											0,1,0,1,
											0,1,0,1,
											0,0,1,1,
											0,0,1,1};	
		
	m_kAxesIcon.m_pkMaterial = m_pkLine;
	m_kAxesIcon.m_kMeshData.m_ePolygonMode = LINES_MODE;
	m_kAxesIcon.m_kMeshData.m_kDataPointers.push_back(DataPointer(VERTEX_POINTER,afVertexData));	
	m_kAxesIcon.m_kMeshData.m_kDataPointers.push_back(DataPointer(COLOR_POINTER,afColorData));	
	m_kAxesIcon.m_kMeshData.m_iNrOfDataElements = 6;
	
	
}

InterfaceRender::~InterfaceRender()
{
	delete m_pkLine;
}

bool InterfaceRender::Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState)
{
	//push matrises
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPush();	

	kRenderEngine.SetupView(kRenderState);
	
	vector<RenderPackage*>	kRenderPackages;
	
	//AXIS ICON
	if(m_pkZeroFps->GetDrawAxesIcon())
 		kRenderPackages.push_back(&m_kAxesIcon);	

	//ZONE LIST
	m_pkEntityManager->GetZonesDebugRenderPackages(kRenderPackages,kRenderState,NULL);

	//AABB entity tree
	m_pkEntityManager->GetAABBTreeRenderPackages(kRenderPackages,kRenderState);

	//APPLICATION RENDER DATA
	m_pkApplication->GetRenderPackages(kRenderPackages,kRenderState);

	//render packages
	kRenderEngine.DoRender(kRenderPackages,kRenderState);

	//hax
	m_pkApplication->RenderInterface();


	//pop matrises
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPop();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPop();		

	return true;
}

//---- create functions

Plugin* Create_AttachToJointPlugin()
{
	return (Plugin*)new AttachToJoint();
};

Plugin* Create_SkyRender()
{
	return (Plugin*)new SkyRender();
};

Plugin* Create_DepthMapRendererPlugin()
{
	return (Plugin*)new DepthMapRenderer();
};

Plugin* Create_ShadowmapPlugin()
{
	return (Plugin*)new ShadowmapPlugin();
};

Plugin* Create_DebugRenderPlugin()
{
	return (Plugin*)new DebugRenderPlugin();
};

Plugin* Create_InterfaceRender()
{
	return (Plugin*)new InterfaceRender();
};


Plugin* Create_DefaultRenderPlugin()
{
	return (Plugin*)new DefaultRenderPlugin();
};

Plugin* Create_DefaultPreRenderPlugin()
{
	return (Plugin*)new DefaultPreRenderPlugin();
};

Plugin* Create_ExposureCalculator()
{
	return (Plugin*)new ExposureCalculator();
};

Plugin* Create_BloomPostPlugin()
{
	return (Plugin*)new BloomPostPlugin();
};
