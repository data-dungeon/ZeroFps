#include "render_plugins.h"
#include "zerofps.h"


ExposureCalculator::ExposureCalculator() : PostRenderPlugin("HdrExposure")
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

bool ExposureCalculator::Call(const RenderState& kRenderState)
{			
	m_fExposureFactor = GetExposureFactor(kRenderState);
	m_pkZShaderSystem->SetExposure(m_fExposureFactor);
	
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

DebugRenderPlugin::DebugRenderPlugin() : RenderPlugin("DebugRender")
{
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkLight			= static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));
	m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));

	
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


bool DebugRenderPlugin::Call(RenderPackage& kRenderPackage, const RenderState& kRenderState)
{	

	//push matrises	
	m_pkZShaderSystem->MatrixPush();
	
	//apply model matrix
	m_pkZShaderSystem->MatrixMult(kRenderPackage.m_kModelMatrix);
	
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
						
	//setup and draw normals
	bool bUDGS = m_pkZShaderSystem->GetUseDefaultGLSLProgram();
	m_pkZShaderSystem->UseDefaultGLSLProgram(false);
	
	m_pkZShaderSystem->ResetPointers();	
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&kNormals[0]);
	m_pkZShaderSystem->SetPointer(COLOR_POINTER,&kNormalColors[0]);
	m_pkZShaderSystem->SetNrOfVertexs(kNormals.size());
	m_pkZShaderSystem->BindMaterial(m_pkLineMaterial);
	m_pkZShaderSystem->DrawArray(LINES_MODE);
	m_pkZShaderSystem->UseDefaultGLSLProgram(bUDGS);
				
	m_pkZShaderSystem->MatrixPop();
	
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



// ---------DefaultRenderPlugin

bool DefaultRenderPlugin::Call(RenderPackage& kRenderPackage, const RenderState& kRenderState)
{
	//do occulusion test
	if(m_pkZeroFps->GetOcculusionCulling() && kRenderPackage.m_bOcculusionTest)
	{
		//if we have an occulusion parent test parent
		if(kRenderPackage.m_pkOcculusionParent)
		{
			if(!kRenderPackage.m_pkOcculusionParent->m_kOcculusionTest[kRenderState.GetStateID()].GetCurrentResult())
			{	
				return true;									
			}			
		}
		
		//else test this object
		else if(!kRenderPackage.m_kOcculusionTest[kRenderState.GetStateID()].Visible(	kRenderPackage.m_kAABBMin + kRenderPackage.m_kCenter,
																												kRenderPackage.m_kAABBMax + kRenderPackage.m_kCenter))
		{
			m_pkZeroFps->m_iOcculedObjects++;
			return true;					
		}
		else
			m_pkZeroFps->m_iNotOcculedObjects++;
	}

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
		
		//check if if theres an index list
		if(kMesh.m_kDataPointers[i].m_iType == INDEX_POINTER)
			bIndexed = true;
			
		//check for bone matrix pointer
		if(kMesh.m_kDataPointers[i].m_iType == BONEMATRIX_POINTER)
			pkBoneMatrixPointer = &kMesh.m_kDataPointers[i];
		
		//check for bone index pointer
		if(kMesh.m_kDataPointers[i].m_iType == BONEINDEX_POINTER)
			pkBoneIndexPointer = &kMesh.m_kDataPointers[i];
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
	if(pkBoneMatrixPointer && pkBoneIndexPointer)
	{
		DoBoneTransformation(kRenderPackage,*pkBoneMatrixPointer, *pkBoneIndexPointer);
	}
	
	//draw
	m_pkZShaderSystem->DrawArray(kMesh.m_ePolygonMode);		
				
	m_pkZShaderSystem->MatrixPop();
	
	return true;
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
	

	//find nr of vertiess (HACK)
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

DefaultPreRenderPlugin::DefaultPreRenderPlugin() : PreRenderPlugin("PreRender")
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

BloomPostPlugin::BloomPostPlugin() : PostRenderPlugin("Bloom")
{
	m_pkZShaderSystem = 	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));	
	m_pkRender= 			static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));	
		

	//BLOOM 
	m_pkBloomMaterial1 = new ZMaterial;
	m_pkBloomMaterial1->GetPass(0)->m_pkSLP->SetRes("#bloom1.frag.glsl");
	m_pkBloomMaterial1->GetPass(0)->m_pkTUs[0]->SetRes("notex.bmp");
	m_pkBloomMaterial1->GetPass(0)->m_bLighting = false;
	m_pkBloomMaterial1->GetPass(0)->m_bDepthTest = false;
	
	m_pkBloomMaterial2 = new ZMaterial;
	m_pkBloomMaterial2->GetPass(0)->m_pkSLP->SetRes("#bloom2.frag.glsl");
	m_pkBloomMaterial2->GetPass(0)->m_pkTUs[0]->SetRes("notex.bmp");
	m_pkBloomMaterial2->GetPass(0)->m_pkTUs[1]->SetRes("notex.bmp");
	m_pkBloomMaterial2->GetPass(0)->m_bLighting = false;
	m_pkBloomMaterial2->GetPass(0)->m_bDepthTest = false;
	
		
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

BloomPostPlugin::~BloomPostPlugin()
{
	delete m_pkBloomMaterial1;
	delete m_pkBloomMaterial2;
}

bool BloomPostPlugin::Call(const RenderState& kRenderState)
{
	static float data[]= {	-1,-1,-1,
									 1,-1,-1,
							 		 1, 1,-1,
									-1, 1,-1};

	static float uvdata[]= {0,0,
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
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,data);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,uvdata);
	m_pkZShaderSystem->SetNrOfVertexs(4);
	
		
  	//we dont wan to write any depth data
  	m_pkZShaderSystem->SetDepthMask(false);
  	
  	//ugly haxk to force another texture then the one specified in the material
	m_pkZShaderSystem->BindTexture(&m_kBloomTexture);
  	
  	
	//draw scree surface
 	m_pkZShaderSystem->DrawArray(QUADS_MODE);  	
  	
  	
	//save image, and do another draw	
	m_kBloomTexture2.CopyFrameBuffer(	kRenderState.m_kViewPortPos.x,kRenderState.m_kViewPortPos.y,
												kRenderState.m_kViewPortSize.x, kRenderState.m_kViewPortSize.y);
	
	m_pkZShaderSystem->BindMaterial(m_pkBloomMaterial2);	
	m_pkZShaderSystem->BindTexture(&m_kBloomTexture2);		
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	m_pkZShaderSystem->BindTexture(&m_kBloomTexture);
	glActiveTextureARB(GL_TEXTURE0_ARB);
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
InterfaceRender::InterfaceRender() : PreRenderPlugin("InterfaceRender")
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
