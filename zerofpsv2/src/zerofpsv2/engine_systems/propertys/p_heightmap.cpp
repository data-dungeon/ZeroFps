#include "p_heightmap.h"
#include "../../engine/zerofps.h"



P_Heightmap::P_Heightmap() : Property("P_Heightmap")
{
	m_bNetwork = true;
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	m_iVersion = 4;
	m_iSortPlace = 0;
	m_bSortDistance = true;

	m_pkZShaderSystem =	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkLight=				static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight")); 
	m_pkRender=				static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			

	m_fLastOcculusionTime 	= 0;
	m_bOculled					= false;
	m_bHaveOCTested			= false;

	m_fScale = 2.0;
	m_fMaxValue = 100;
	m_iLod = 0; 	
	
	ZFResourceHandle* pkTempMat = new ZFResourceHandle;
	pkTempMat->SetRes("heightmap/default.zlm");	
	m_kMaterials.push_back(pkTempMat);
	
}

void P_Heightmap::Init()
{
	SetSize(4,4);
}

P_Heightmap::~P_Heightmap()
{
	//clear all materials
	for(int i =0;i<m_kMaterials.size();i++)
		delete m_kMaterials[i];	

	//clear data arrays
	for(int i = 0;i<m_kLodLevels.size();i++)
		for(int j = 0;j<m_kLodLevels[i].size();j++)			
			delete m_kLodLevels[i][j];

}

void P_Heightmap::SetMaxValue(float fMax)							
{
	m_fMaxValue = fMax;	
	m_pkEntity->SetLocalAABB(Vector3(-m_iWidth/2.0,m_fExtremeMin,-m_iWidth/2.0),Vector3(m_iWidth/2.0,m_fExtremeMax,m_iWidth/2.0));
}

void P_Heightmap::GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState)
{
	if(!m_bHaveRebuilt)
		BuildTextureArrays();
	
	if(!kRenderState.m_kFrustum.CubeInFrustum(	m_pkEntity->GetWorldPosV() + Vector3(-m_iWidth/2.0,m_fExtremeMin,-m_iHeight/2.0),
																m_pkEntity->GetWorldPosV() + Vector3(m_iWidth/2.0,m_fExtremeMax,m_iHeight/2.0)))
		return;


	
	//calculate lod level
	float fDistance =kRenderState.m_kCameraPosition.DistanceTo(m_pkEntity->GetWorldPosV()) - m_iWidth/2.0;		
	
	m_iLod = 3;
	if(fDistance < m_pkZeroFps->GetViewDistance()*0.8)
		m_iLod = 2;		
	if(fDistance < m_pkZeroFps->GetViewDistance()*0.5)
		m_iLod = 1;
	if(fDistance < m_pkZeroFps->GetViewDistance()*0.2)
		m_iLod = 0;		

	if(m_iLod < 0) m_iLod = 0;
	if(m_iLod >= m_kLodLevels.size()) m_iLod = m_kLodLevels.size()-1;	
	
	vector<HeightmapArrays*>& kDataArrays = m_kLodLevels[m_iLod];
		
	for(int i =0;i<kDataArrays.size();i++)
	{
		if(kDataArrays[i]->m_kVertexData.empty())
			continue;	
	
		m_kRenderPackages[i].m_kMeshData.m_kDataPointers.clear();
		m_kRenderPackages[i].m_kMeshData.m_kDataPointers.push_back(DataPointer(VERTEX_POINTER,&kDataArrays[i]->m_kVertexData[0]));
		m_kRenderPackages[i].m_kMeshData.m_kDataPointers.push_back(DataPointer(NORMAL_POINTER,&kDataArrays[i]->m_kNormalData[0]));
		m_kRenderPackages[i].m_kMeshData.m_kDataPointers.push_back(DataPointer(TEXTURE_POINTER0,&kDataArrays[i]->m_kTextureData[0]));		
		m_kRenderPackages[i].m_kMeshData.m_kDataPointers.push_back(DataPointer(COLOR_POINTER,&kDataArrays[i]->m_kColorData[0]));		
		m_kRenderPackages[i].m_kMeshData.m_kDataPointers.push_back(DataPointer(TANGENT_POINTER,&kDataArrays[i]->m_kTangentData[0]));
		m_kRenderPackages[i].m_kMeshData.m_kDataPointers.push_back(DataPointer(BITANGENT_POINTER,&kDataArrays[i]->m_kBiTangentData[0]));							
		m_kRenderPackages[i].m_kMeshData.m_iNrOfDataElements = kDataArrays[i]->m_kVertexData.size();		
	
		m_kRenderPackages[i].m_kModelMatrix.Identity();
		m_kRenderPackages[i].m_kModelMatrix.Translate(m_pkEntity->GetWorldPosV());
		m_kRenderPackages[i].m_kModelMatrix*= m_pkEntity->GetWorldRotM();
		m_kRenderPackages[i].m_kModelMatrix.Translate(-Vector3(m_iWidth/2.0,0,m_iHeight/2.0));	
		m_kRenderPackages[i].m_kCenter = m_pkEntity->GetWorldPosV();
		
		
		kRenderPackages.push_back(&m_kRenderPackages[i]);					
	}

}

void P_Heightmap::Update()
{
// 	if(!( (m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_CASTSHADOW ||
// 			 m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_SHADOWED)))
// 		return;

	if(!( m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_SHADOWED))
		return;

	if(!m_pkZeroFps->GetCam()->GetFrustum()->CubeInFrustum(	m_pkEntity->GetWorldPosV() - Vector3(m_iWidth/2.0,m_fMaxValue,m_iHeight/2.0),
																				m_pkEntity->GetWorldPosV() + Vector3(m_iWidth/2.0,m_fMaxValue,m_iHeight/2.0)))
		return;


	
	if(m_pkZShaderSystem->SupportOcculusion() && 
		m_pkZeroFps->GetOcculusionCulling() &&
		m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_SHADOWED)
	{						
		static Vector3 kPos;
		kPos = m_pkEntity->GetWorldPosV();
		//occulusion test
		if(m_kOCTests[m_pkZeroFps->GetCam()].Visible(kPos-Vector3(m_iWidth/2.0,m_fMaxValue,m_iHeight/2.0),
									kPos+Vector3(m_iWidth/2.0,m_fMaxValue,m_iHeight/2.0)))
		{
			//update light					
		 	m_pkLight->Update(&m_kLightProfile,GetEntity()->GetWorldPosV(),GetEntity()->GetRadius());					
			
			DrawTexturedHeightmap();
			
			m_pkZeroFps->m_iNotOcculedObjects++;								
		}
		else
		{
			m_pkZeroFps->m_iOcculedObjects++;		
		}
	}
	else		
	{
		//update light					
	 	m_pkLight->Update(&m_kLightProfile,GetEntity()->GetWorldPosV(),GetEntity()->GetRadius());					
	
		DrawTexturedHeightmap();		
	}	

}

void P_Heightmap::DrawTexturedHeightmap()
{
	if(!m_bHaveRebuilt)
		BuildTextureArrays();

	
	//calculate lod level
	if(m_pkZeroFps->GetCam()->GetCurrentRenderMode() != RENDER_CASTSHADOW)
	{
		float fDistance = m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(m_pkEntity->GetWorldPosV()) - m_iWidth/2.0;		
		//m_iLod = (fDistance/100) * 3;
	
		m_iLod = 3;
		if(fDistance < m_pkZeroFps->GetViewDistance()*0.8)
			m_iLod = 2;		
		if(fDistance < m_pkZeroFps->GetViewDistance()*0.5)
			m_iLod = 1;
		if(fDistance < m_pkZeroFps->GetViewDistance()*0.2)
			m_iLod = 0;		
	}

	if(m_iLod < 0) m_iLod = 0;
	if(m_iLod >= m_kLodLevels.size()) m_iLod = m_kLodLevels.size()-1;
	

	m_pkZShaderSystem->MatrixPush();	
	m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetWorldPosV()  );
	m_pkZShaderSystem->MatrixMult(m_pkEntity->GetWorldRotM());	
	m_pkZShaderSystem->MatrixTranslate(- Vector3(m_iWidth/2.0,0,m_iHeight/2.0));

	vector<HeightmapArrays*>* pkDataArrays = &m_kLodLevels[m_iLod];
	
	for(int i =0;i<pkDataArrays->size();i++)
	{
		if((*pkDataArrays)[i]->m_kVertexData.empty())
			continue;
	
		//always draw the first material without blending
		if(i == 0)
			m_pkZShaderSystem->ForceBlending(BLEND_FORCE_OPAQ);
		
		m_pkZShaderSystem->BindMaterial((ZMaterial*)(m_kMaterials[i]->GetResourcePtr()) );	

// 		if(m_kDataArrays[i]->m_pkVBO)
// 		{
// 			m_pkZShaderSystem->DrawVertexBuffer(m_kDataArrays[i]->m_pkVBO);		
// 		}
// 		else
// 		{
		
			m_pkZShaderSystem->ResetPointers();
			m_pkZShaderSystem->SetPointer(COLOR_POINTER,&( (*pkDataArrays)[i]->m_kColorData[0]));
			m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,&((*pkDataArrays)[i]->m_kTextureData[0]));
			m_pkZShaderSystem->SetPointer(NORMAL_POINTER,&((*pkDataArrays)[i]->m_kNormalData[0]));
			m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&((*pkDataArrays)[i]->m_kVertexData[0]));
			
// 			m_pkZShaderSystem->SetPointer(TEXTURE_POINTER1,&((*pkDataArrays)[i]->m_kTangentData[0]));
// 			m_pkZShaderSystem->SetPointer(TEXTURE_POINTER2,&((*pkDataArrays)[i]->m_kBiTangentData[0]));
			
			m_pkZShaderSystem->SetPointer(TANGENT_POINTER,&((*pkDataArrays)[i]->m_kTangentData[0]));
			m_pkZShaderSystem->SetPointer(BITANGENT_POINTER,&((*pkDataArrays)[i]->m_kBiTangentData[0]));
			
			m_pkZShaderSystem->SetNrOfVertexs((*pkDataArrays)[i]->m_kVertexData.size());
			
			m_pkZShaderSystem->DrawArray(TRIANGLES_MODE);		
// 		}
		
		if(i == 0)
			m_pkZShaderSystem->ForceBlending(BLEND_MATERIAL);
	}


	m_pkZShaderSystem->MatrixPop();	
	
	return;
}

void P_Heightmap::FindExtremeValues(float& fMax,float& fMin)
{
	fMax = -999999999;
	fMin =  999999999;

	int iSize = m_kHeightData.size();
	for(int i = 0;i<iSize;i++)
	{
		if(m_kHeightData[i] < fMin) fMin = m_kHeightData[i];
		if(m_kHeightData[i] > fMax) fMax = m_kHeightData[i];	
	}
	
}

void P_Heightmap::BuildTextureArrays()
{
	m_bHaveRebuilt = true;

	//make a new material with no blending
	m_kBottom = *(ZMaterial*)(m_kMaterials[0]->GetResourcePtr());
	m_kBottom.GetPass(0)->m_bBlend = false;			

	//setup renderpackages array
	if(m_kRenderPackages.size() != m_kMaterials.size())
		m_kRenderPackages.resize(m_kMaterials.size());

	//find extreme values
	FindExtremeValues(m_fExtremeMax,m_fExtremeMin);

	//update AABB
	m_pkEntity->SetLocalAABB(Vector3(-m_iWidth/2.0,m_fExtremeMin,-m_iWidth/2.0),Vector3(m_iWidth/2.0,m_fExtremeMax,m_iWidth/2.0));

	//clear data arrays
	for(int i = 0;i<m_kLodLevels.size();i++)
	{
		for(int j = 0;j<m_kLodLevels[i].size();j++)			
			delete m_kLodLevels[i][j];
	}
	m_kLodLevels.clear(); 
	
	//how many lodlevels to generate
	int iLevels = 4;
	if(m_iRows < 16)
		iLevels = 3;
	if(m_iRows < 8)
		iLevels = 2;

	for(int l = 0;l<iLevels;l++)
	{
	
		int iStep;
		
		switch(l)
		{
			case 0: iStep = 1;break;
			case 1: iStep = 2;break;
			case 2: iStep = 4;break;
			case 3: iStep = 8;break;
		};


		m_kLodLevels.push_back(vector<HeightmapArrays*>() );		
		vector<HeightmapArrays*>* pkNewDataArrays = &m_kLodLevels.back();
	
		for(int i =0;i<m_kMaterials.size();i++)
		{
			HeightmapArrays* pkNewArrays = new HeightmapArrays;
			pkNewDataArrays->push_back(pkNewArrays);									
			
			
			//draw the first material all over the place if we dont have glsl support
// 			if(i == 0 && !m_pkZShaderSystem->SupportGLSLProgram())
			if(i == 0 )
			{
				for(int y = 0;y<m_iCols-iStep;y+=iStep)
				{	
					for(int x = 0;x<m_iRows-iStep;x+=iStep)
					{	
						if(m_kTextureIDs[y*m_iRows + x] == -1 ||
							m_kTextureIDs[(y+iStep)*m_iRows + x] == -1||
							m_kTextureIDs[y*m_iRows + x+iStep] == -1||
							m_kTextureIDs[(y+iStep)*m_iRows + x+iStep] == -1)						
							continue;				
					
						AddPolygon(pkNewArrays,x,y,i,true,iStep);
						AddPolygon(pkNewArrays,x,y,i,false,iStep);			
					}
				}				
			}
			else
			{
				for(int y = 0;y<m_iCols-iStep;y+=iStep)
				{	
					for(int x = 0;x<m_iRows-iStep;x+=iStep)
					{		
						if(m_kTextureIDs[y*m_iRows + x] == -1 ||
							m_kTextureIDs[(y+iStep)*m_iRows + x] == -1||
							m_kTextureIDs[y*m_iRows + x+iStep] == -1||
							m_kTextureIDs[(y+iStep)*m_iRows + x+iStep] == -1)						
							continue;
								
						//TOP POLYGON
						if(m_kTextureIDs[y*m_iRows + x] == i ||
							m_kTextureIDs[(y+iStep)*m_iRows + x] == i||
							m_kTextureIDs[y*m_iRows + x+iStep] == i)
						{
							AddPolygon(pkNewArrays,x,y,i,true,iStep);
						}					
						
						//BOTTOM POLYGON											
						if(m_kTextureIDs[y*m_iRows + x+iStep] == i ||
							m_kTextureIDs[(y+iStep)*m_iRows + x] == i||
							m_kTextureIDs[(y+iStep)*m_iRows + x+iStep] == i)
						{
							AddPolygon(pkNewArrays,x,y,i,false,iStep);
						}
					}
				}				
			}

			//generate tangent data					
			Math::GenerateTangents(	&pkNewArrays->m_kVertexData[0],
											&pkNewArrays->m_kNormalData[0],
											&pkNewArrays->m_kTextureData[0],
											pkNewArrays->m_kTangentData,
											pkNewArrays->m_kBiTangentData,
											pkNewArrays->m_kVertexData.size());
			
			


		}
	}
	
	
	//setup renderpackages
		
	for(int i = 0;i<m_kRenderPackages.size();i++)
	{
		//if this is the bottom material, use special no blending bottom material			
		if(i == 0)
			m_kRenderPackages[i].m_pkMaterial = &m_kBottom;
		else						
			m_kRenderPackages[i].m_pkMaterial = (ZMaterial*)(m_kMaterials[i]->GetResourcePtr());

		m_kRenderPackages[i].m_kMeshData.m_ePolygonMode = TRIANGLES_MODE;
		
		m_kRenderPackages[i].m_pkLightProfile = &m_kLightProfile;
		m_kRenderPackages[i].m_fRadius = GetEntity()->GetRadius();
		
		//disable blend sorting
		m_kRenderPackages[i].m_bBlendSort = false;
		
		//occulusion
		m_kRenderPackages[i].m_kAABBMin = Vector3(-m_iWidth/2.0,m_fExtremeMin,-m_iHeight/2.0);
		m_kRenderPackages[i].m_kAABBMax = Vector3(m_iWidth/2.0,m_fExtremeMax,m_iHeight/2.0);
		m_kRenderPackages[i].m_bOcculusionTest = true;
		
		//set occulusion test parent to bottom material
		if(i != 0)
			m_kRenderPackages[i].m_pkOcculusionParent = &(m_kRenderPackages[i]);	
	}
	
}

void P_Heightmap::AddPolygon(HeightmapArrays* pkNewArrays,int x,int y,int iID,bool bTop,int iStep)
{
	if(bTop)
	{
		//top left corner
		if(iStep > 1 && x == 0 && y == 0)
		{
			for(int i = x;i<x+iStep;i++)
			{		
				AddVertex(pkNewArrays,x,i,iID);												
				AddVertex(pkNewArrays,x,i+1,iID);		
				AddVertex(pkNewArrays,i+1,y,iID);		
												
				if(i != x+iStep-1)
				{
					AddVertex(pkNewArrays,i+1,y,iID);												
					AddVertex(pkNewArrays,x,i+1,iID);		
					AddVertex(pkNewArrays,i+2,y,iID);				
				}												
			}						
		}
		//top edge
		else if(iStep > 1 && y == 0)
		{
			for(int i = x;i<x+iStep;i++)
			{
				AddVertex(pkNewArrays,i,y,iID);												
				AddVertex(pkNewArrays,x,y+iStep,iID);		
				AddVertex(pkNewArrays,i+1,y,iID);																
			}
		}
		//left edge
		else if(iStep > 1 && x == 0)
		{
			for(int i = y;i<y+iStep;i++)
			{
				AddVertex(pkNewArrays,x,i,iID);												
				AddVertex(pkNewArrays,x,i+1,iID);		
				AddVertex(pkNewArrays,x+iStep,y,iID);				
											
			}		
		}		
		//center tiles
		else
		{		
			AddVertex(pkNewArrays,x,y,iID);
			AddVertex(pkNewArrays,x,y+iStep,iID);
			AddVertex(pkNewArrays,x+iStep,y,iID);

		}						
	}
	else
 	{
		//botom right corner
		if(iStep > 1 && x == m_iCols-iStep-1 && y == m_iRows-iStep-1)
		{
			for(int i = x;i<x+iStep;i++)
			{		
				AddVertex(pkNewArrays,x+iStep,i,iID);												
				AddVertex(pkNewArrays,i,y+iStep,iID);		
				AddVertex(pkNewArrays,x+iStep,i+1,iID);		
												
				AddVertex(pkNewArrays,i+1,y+iStep,iID);												
				AddVertex(pkNewArrays,x+iStep,i+1,iID);		
				AddVertex(pkNewArrays,i,y+iStep,iID);											
			}						
		}
		//botom edge
		else if(iStep > 1 && y == m_iRows-iStep-1)
		{
			for(int i = x;i<x+iStep;i++)
			{
				AddVertex(pkNewArrays,x+iStep,y,iID);												
				AddVertex(pkNewArrays,i,y+iStep,iID);		
				AddVertex(pkNewArrays,i+1,y+iStep,iID);				

												
			}
		}
		//right edge
		else if(iStep > 1 && x == m_iCols-iStep-1)
		{
			for(int i = y;i<y+iStep;i++)
			{
				AddVertex(pkNewArrays,x+iStep,i,iID);												
				AddVertex(pkNewArrays,x,y+iStep,iID);		
				AddVertex(pkNewArrays,x+iStep,i+1,iID);				
											
			}		
		}		
		//center tiles
		else
		{
			AddVertex(pkNewArrays,x+iStep,y,iID);
			AddVertex(pkNewArrays,x,y+iStep,iID);
			AddVertex(pkNewArrays,x+iStep,y+iStep,iID);
		}	
	}
	
	
}

void P_Heightmap::AddVertex(HeightmapArrays* pkNewArrays,int x,int y,int iID)
{
	static float fTexMod = 0.5;
	static Vector4 kColor;	
	
	float fBrightness = (float(m_kBrightness[y*m_iRows + x]) / 255.0);
	
	if(iID == 0)
	{
		kColor.Set(fBrightness,fBrightness,fBrightness,1);
	}
	else
	{
		if(m_kTextureIDs[y*m_iRows + x] == iID)
			kColor.Set(fBrightness,fBrightness,fBrightness,2);
		else
			kColor.Set(fBrightness,fBrightness,fBrightness,0);
	}		

	//color
	pkNewArrays->m_kColorData.push_back(kColor);		
		
	//UV's
	pkNewArrays->m_kTextureData.push_back(Vector2(-x*fTexMod,y*fTexMod));
	
	//normals
	pkNewArrays->m_kNormalData.push_back( GenerateNormal(x,y).Unit());


	//vertex
	pkNewArrays->m_kVertexData.push_back(Vector3(x*m_fScale,m_kHeightData[y*m_iRows + x],y*m_fScale));
	
}


Vector3 P_Heightmap::GenerateNormal(int x,int y)
{
	if(x == 0 || y == 0 || x == m_iRows-1 || y == m_iCols-1)
		return Vector3(0,1,0);


	static Vector3 V1,V2;
	
	V1 = Vector3(x+1,m_kHeightData[(y)*m_iRows+x+1],y) - Vector3(x,m_kHeightData[(y)*m_iRows+x],y);
	V2 = Vector3(x,m_kHeightData[(y+1)*m_iRows+x],y+1) - Vector3(x,m_kHeightData[(y)*m_iRows+x],y);

	return V2.Cross(V1);
}

void P_Heightmap::GetCollData(vector<Mad_Face>* pkFace,vector<Vector3>* pkVertex , vector<Vector3>* pkNormal)
{
	int iIndex = 0;
	static Mad_Face kFace;
	static Vector3 kNorm;
	static Vector3 V1,V2,V3;
	static Vector3 kDir1,kDir2;
	
	
	for(int y=0; y<m_iCols-1; y++) 
	{
		for(int x=0; x<m_iRows-1; x++) 
		{
			//dont add transparent polygons
			if(m_kTextureIDs[y*m_iRows + x] == -1 ||
				m_kTextureIDs[(y+1)*m_iRows + x] == -1||
				m_kTextureIDs[y*m_iRows + x+1] == -1||
				m_kTextureIDs[(y+1)*m_iRows + x+1] == -1)						
				continue;		
		
			//top polygon
			V1 = Vector3((float)x*m_fScale, 			m_kHeightData[y*m_iRows+x], 			(float)y*m_fScale);
			V2 = Vector3((float)x*m_fScale, 	m_kHeightData[(y+1)*m_iRows+x], 				(float)(y + 1.0f)*m_fScale);
			V3 = Vector3((float)(x + 1)*m_fScale, 	m_kHeightData[(y)*m_iRows+(x+1)], 	(float)y*m_fScale);
			
			pkVertex->push_back( V1 );			
			pkVertex->push_back( V2 );		
			pkVertex->push_back( V3 );		
			
			//face
			kFace.iIndex[0] = iIndex++;	
			kFace.iIndex[1] = iIndex++;		
			kFace.iIndex[2] = iIndex++;	
			pkFace->push_back(kFace);
			
			//normal
			kDir1 = V1 - V2;
			kDir2 = V3 - V2;
			kNorm = kDir2.Cross(kDir1);
			kNorm.Normalize();
			pkNormal->push_back( kNorm );




			//botom polygon
			V1 = Vector3((float)(x+1)*m_fScale, 	m_kHeightData[y*m_iRows+x+1], 		(float)y*m_fScale); 
			V2 = Vector3((float)x*m_fScale , 		m_kHeightData[(y+1)*m_iRows+x], 		(float)(y + 1.0f)*m_fScale);
			V3 = Vector3((float)(x + 1)*m_fScale, 	m_kHeightData[(y+1)*m_iRows+(x+1)], (float)(y + 1.0f)*m_fScale);			

			pkVertex->push_back( V1 );	
			pkVertex->push_back( V2 );	
			pkVertex->push_back( V3 );	
			
			//face
			kFace.iIndex[0] = iIndex++;		
			kFace.iIndex[1] = iIndex++;		
			kFace.iIndex[2] = iIndex++;				
			pkFace->push_back(kFace);
			
			//normal
			kDir1 = V1 - V2;
			kDir2 = V3 - V2;
			kNorm = kDir2.Cross(kDir1);
			kNorm.Normalize();			
			pkNormal->push_back( kNorm );
		}
	}
		
		
	Vector3 kOffset(m_iWidth/2.0,0,m_iHeight/2.0);
	for(int i = 0;i<pkVertex->size();i++)
	{
		(*pkVertex)[i] -= kOffset;
	}
}


float P_Heightmap::CalculateRadius()
{
	float fMax = 0;

	for(int i = 0;i<m_kHeightData.size();i++)
	{
		if(fabs(m_kHeightData[i]) > fMax)
			fMax = fabs(m_kHeightData[i]);
	}
	
	
	return Vector3(m_iWidth/2.0,fMax,m_iHeight/2.0).Length();
}

void P_Heightmap::SetSize(int iWidth,int iHeight)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;		
	m_iRows = static_cast<int>( (m_iWidth/m_fScale)+1 );
	m_iCols = static_cast<int>( (m_iHeight/m_fScale)+1 );
	
	
	m_kBrightness.clear();
	for(int y = 0;y<m_iCols;y++)
		for(int x = 0;x<m_iRows;x++)
		{					
			m_kBrightness.push_back(255);
		}
	
	m_kTextureIDs.clear();
	for(int y = 0;y<m_iCols;y++)
		for(int x = 0;x<m_iRows;x++)
		{					
			m_kTextureIDs.push_back(0);
		}
	
	m_kHeightData.clear();
	for(int y = 0;y<m_iCols;y++)
		for(int x = 0;x<m_iRows;x++)
		{					
 			m_kHeightData.push_back(0);
		}

	m_bHaveRebuilt = false;
	
	
	m_pkEntity->SetRadius(Vector3(m_iWidth/2.0,0,m_iHeight/2.0).Length());	
	//m_pkEntity->SetLocalAABB(GetEntity()->GetRadius());
	m_pkEntity->SetLocalAABB(Vector3(-m_iWidth,m_fExtremeMin,-m_iWidth),Vector3(m_iWidth,m_fExtremeMax,m_iWidth));
}

void P_Heightmap::Smooth()
{
	for(int y = 1;y<m_iCols-1;y++)
	{
		for(int x = 1;x<m_iRows-1;x++)
		{					
			m_kHeightData[y*m_iRows + x] = 	(m_kHeightData[y*m_iRows + x] + 
														m_kHeightData[y*m_iRows + x+1]+
														m_kHeightData[y*m_iRows + x-1]+
														m_kHeightData[(y+1)*m_iRows + x]+
														m_kHeightData[(y-1)*m_iRows + x]) / 5.0;
		}
	}
	
	ResetAllNetUpdateFlags();
	m_bHaveRebuilt = false;
}

void P_Heightmap::FlattenSelection(vector<HMSelectionData>* kSelectionData, float fSample)
{
	for(int i = 0;i<kSelectionData->size();i++)
	{
		
		//save new height
		(*kSelectionData)[i].m_pkHeightMap->m_kHeightData[(*kSelectionData)[i].y*(*kSelectionData)[i].m_pkHeightMap->m_iRows 
			+ (*kSelectionData)[i].x] = fSample - (*kSelectionData)[i].m_pkHeightMap->GetEntity()->GetWorldPosV().y;
				
		//set heightmap as updated
		(*kSelectionData)[i].m_pkHeightMap->ResetAllNetUpdateFlags();
		(*kSelectionData)[i].m_pkHeightMap->m_bHaveRebuilt = false;
	}
}

void P_Heightmap::Modify(vector<HMSelectionData>* kSelectionData,float fMod)
{
	for(int i = 0;i<kSelectionData->size();i++)
	{			
		int iIndex = (*kSelectionData)[i].y * m_iRows + (*kSelectionData)[i].x;		
		m_kHeightData[iIndex] += fMod * (*kSelectionData)[i].m_fValue;
		
		if(m_kHeightData[iIndex] > m_fMaxValue)
			m_kHeightData[iIndex] = m_fMaxValue;
			
		if(m_kHeightData[iIndex] < -m_fMaxValue)
			m_kHeightData[iIndex] = -m_fMaxValue;
	}	
	
	ResetAllNetUpdateFlags();
	m_bHaveRebuilt = false;
}


signed char P_Heightmap::GetMaterialID(const string& strMaterial)
{
	if(strMaterial.empty())
		return (signed char)-1;

	for(unsigned int i = 0;i<m_kMaterials.size();i++)
	{
		if(m_kMaterials[i]->GetRes() == strMaterial)
		{
			return (signed char)i;
		}	
	}
	
	//if material list is full , return the default material
	if(m_kMaterials.size() >= 127)
	{
		cout<<"WARNING: heightmap "<<m_pkEntity->GetEntityID()<<" has exceeded 127 different materials, this is not good"<<endl;
		return 0;
	}

	
	//material not found, adding
	ZFResourceHandle* pkTempMat = new ZFResourceHandle;
	pkTempMat->SetRes(strMaterial);						
	m_kMaterials.push_back(pkTempMat);
	
	cout<<"material "<<strMaterial<<" added to heightmap:"<<m_pkEntity->GetEntityID()<<" as material ID:"<<m_kMaterials.size() - 1<<" , heightmap now has "<<m_kMaterials.size()<<" materials"<<endl;
	
	return (signed char)(m_kMaterials.size() - 1);
}

void P_Heightmap::PurgeUnusedMaterials()
{
	unsigned int iSize = m_kTextureIDs.size();
	int iID = -1;

	for(vector<ZFResourceHandle*>::iterator it= m_kMaterials.begin();it != m_kMaterials.end(); it++)
	{		
		iID++;	
		bool bFound = false;
	
		for(unsigned int j = 0;j<iSize;j++)
		{
			if( m_kTextureIDs[j] == static_cast<signed char>(iID) )
			{
				bFound = true;
				break;
			}
		}
		
		if(!bFound)
		{
			//unused material
			cout<<"material "<<iID<<": "<<(*it)->GetRes()<<" is not in use anymore, removing from heightmap "<<m_pkEntity->GetEntityID()<<" now has "<<m_kMaterials.size()-1<<" materials"<<endl;
		
			//erate material
			m_kMaterials.erase(it);	
		
			//decrese all above materialIDs with 1
			for(unsigned int j = 0;j<iSize;j++)
			{
				if( m_kTextureIDs[j] > iID )
					m_kTextureIDs[j]--;
			}			
					
			//if theres no materials left, just return
			if(m_kMaterials.empty())
				return;
					
			//start from the beginning again
			it = m_kMaterials.begin();
			iID = -1;
		}
	}	
}

void P_Heightmap::SetBrightness(vector<HMSelectionData>* kSelectionData,float fBrightness)
{
	unsigned char cBrightness = static_cast<unsigned char>(255.0*fBrightness);

	for(int i = 0;i<kSelectionData->size();i++)
		m_kBrightness[(*kSelectionData)[i].y * m_iRows + (*kSelectionData)[i].x] = cBrightness;	

	ResetAllNetUpdateFlags();
	m_bHaveRebuilt = false;
}

void P_Heightmap::SetTexture(vector<HMSelectionData>* kSelectionData,const string& strMaterial)
{
	if(!strMaterial.empty())
	{
		if(strMaterial.length() < 4 || strMaterial.substr(strMaterial.length()-3) != "zlm")
		{
			cerr<<"WARNING: invalide material "<<strMaterial<<endl;
			return;
		}
	}
	
	
	signed char cTexture = GetMaterialID(strMaterial);

	for(int i = 0;i<kSelectionData->size();i++)
	{			
		m_kTextureIDs[(*kSelectionData)[i].y * m_iRows + (*kSelectionData)[i].x] = cTexture;	
	}
	
	//purge unused materials
	PurgeUnusedMaterials();
	
	ResetAllNetUpdateFlags();
	m_bHaveRebuilt = false;
}

void P_Heightmap::SmoothSelection(vector<HMSelectionData>* kSelectionData)
{
	//loop all selected vertises
	for(int i = 0;i<kSelectionData->size();i++)
	{
		float x 			= (*kSelectionData)[i].m_kWorld.x; 
		float z 			= (*kSelectionData)[i].m_kWorld.z;
		float fVal 		= (*kSelectionData)[i].m_fValue;	
		float y 			= 0;
		int iSamples 	= 0;
	
		//collect all samples
		for(int j = 0;j<	kSelectionData->size();j++)
		{
			if(	fabs((*kSelectionData)[j].m_kWorld.x - x) <= (*kSelectionData)[j].m_pkHeightMap->m_fScale &&
					fabs((*kSelectionData)[j].m_kWorld.z - z) <= (*kSelectionData)[j].m_pkHeightMap->m_fScale)
			{
				y+= (*kSelectionData)[j].m_kWorld.y;					
				iSamples++;
			}
		}
				
		//avrage samples
		y/= iSamples;
				
		//apply value
		y = y*fVal + (*kSelectionData)[i].m_kWorld.y * (1.0 - fVal);
				
		//save new height
		(*kSelectionData)[i].m_pkHeightMap->m_kHeightData[
			(*kSelectionData)[i].y*(*kSelectionData)[i].m_pkHeightMap->m_iRows + (*kSelectionData)[i].x]
			= y - (*kSelectionData)[i].m_pkHeightMap->GetEntity()->GetWorldPosV().y;
				
		//set heightmap as updated
		(*kSelectionData)[i].m_pkHeightMap->ResetAllNetUpdateFlags();
		(*kSelectionData)[i].m_pkHeightMap->m_bHaveRebuilt = false;
	}
}

void P_Heightmap::Stitch(vector<HMSelectionData>* pkSelectionData)
{
	for(int i = 0;i<pkSelectionData->size();i++)
	{
	
		for(int j = i;j<pkSelectionData->size();j++)
		{
			if(i == j)
				continue;
		
			if((*pkSelectionData)[i].m_kWorld.x == (*pkSelectionData)[j].m_kWorld.x &&
				(*pkSelectionData)[i].m_kWorld.z == (*pkSelectionData)[j].m_kWorld.z)
			{
				float fAvrage = ((*pkSelectionData)[i].m_kWorld.y + (*pkSelectionData)[j].m_kWorld.y) / 2.0;
			
				float fI = fAvrage - (*pkSelectionData)[i].m_pkHeightMap->GetEntity()->GetWorldPosV().y;
				float fJ = fAvrage - (*pkSelectionData)[j].m_pkHeightMap->GetEntity()->GetWorldPosV().y;
// 				float fAvrage = (	(*pkSelectionData)[i].m_pkHeightMap->m_kHeightData[(*pkSelectionData)[i].y * (*pkSelectionData)[i].m_pkHeightMap->m_iRows + (*pkSelectionData)[i].x] +
// 										(*pkSelectionData)[j].m_pkHeightMap->m_kHeightData[(*pkSelectionData)[j].y * (*pkSelectionData)[j].m_pkHeightMap->m_iRows + (*pkSelectionData)[j].x]) / 2.0;
			
				//int iID = (*pkSelectionData)[i].m_pkHeightMap->m_kTextureIDs[(*pkSelectionData)[i].y * (*pkSelectionData)[i].m_pkHeightMap->m_iRows + (*pkSelectionData)[i].x];
				//(*pkSelectionData)[j].m_pkHeightMap->m_kTextureIDs[(*pkSelectionData)[j].y * (*pkSelectionData)[j].m_pkHeightMap->m_iRows + (*pkSelectionData)[j].x] = iID;
			
				(*pkSelectionData)[i].m_pkHeightMap->m_kHeightData[(*pkSelectionData)[i].y * (*pkSelectionData)[i].m_pkHeightMap->m_iRows + (*pkSelectionData)[i].x] = fI;
				(*pkSelectionData)[j].m_pkHeightMap->m_kHeightData[(*pkSelectionData)[j].y * (*pkSelectionData)[j].m_pkHeightMap->m_iRows + (*pkSelectionData)[j].x] = fJ;					
			
				(*pkSelectionData)[i].m_pkHeightMap->ResetAllNetUpdateFlags();
				(*pkSelectionData)[i].m_pkHeightMap->m_bHaveRebuilt = false;
				(*pkSelectionData)[j].m_pkHeightMap->ResetAllNetUpdateFlags();
				(*pkSelectionData)[j].m_pkHeightMap->m_bHaveRebuilt = false;
			
			}
		}
	}
}

bool  P_Heightmap::Inside(float x,float y)
{
	if(x <  m_pkEntity->GetWorldPosV().x - m_iWidth/2.0)
		return false;
	if(x > m_pkEntity->GetWorldPosV().x + m_iWidth/2.0)
		return false;

	if(y <  m_pkEntity->GetWorldPosV().z - m_iHeight/2.0)
		return false;
	if(y > m_pkEntity->GetWorldPosV().z + m_iHeight/2.0)
		return false;
		
	return true;
}

float P_Heightmap::GetHeight(float x,float y)
{		
	x-=m_pkEntity->GetWorldPosV().x;
	y-=m_pkEntity->GetWorldPosV().z;

  	x+=m_iWidth / 2.0;
  	y+=m_iHeight / 2.0;

	x /= m_fScale;
	y /= m_fScale;
	
	int iX = int(x);
	int iY = int(y);

	if(iX < 0 || iX >= m_iRows || iY < 0 || iY >= m_iCols)
		return 0;

	float fXD = x - float(iX);
	float fYD = y - float(iY);
	


	float H1 = m_kHeightData[iY*m_iRows + iX];
	float H2 = m_kHeightData[iY*m_iRows + iX+1];
	float H3 = m_kHeightData[(iY+1)*m_iRows + iX];
	float H4 = m_kHeightData[(iY+1)*m_iRows + iX+1];

	float fIX1 = H1*(1.0-fXD) + H2*(fXD);
	float fIX2 = H3*(1.0-fXD) + H4*(fXD);

	float fHeight = fIX1*(1.0-fYD) + fIX2*(fYD);

	return fHeight+ m_pkEntity->GetWorldPosV().y;
}

void P_Heightmap::GetSelection(const Vector3& kCenter, float fStrength, float fInRadius, float fOutRadius,vector<HMSelectionData>* pkSelectionData)
{
	HMSelectionData kSel;
	kSel.m_fValue = fStrength;

	float fInOutDiff = fOutRadius - fInRadius;

	float fLen;
	float fLen2;
	Vector3 kWorld,kDiff;


	for(int z = 0 ; z < m_iCols; z++) 
	{
		for(int x = 0 ; x < m_iRows ; x++) 
		{	
			kSel.m_pkHeightMap = this;
			
			// Calc World Pos of the vertex.
			int iVertexIndex = z * m_iRows + x;			
			kSel.x = x;
			kSel.y = z;
			kWorld = Vector3(x * m_fScale, m_kHeightData[iVertexIndex] ,z * m_fScale);	//*m_fTileSize
			kWorld += m_pkEntity->GetWorldPosV() - Vector3(m_iWidth/2.0,0,m_iHeight/2.0);
	
			kDiff = kWorld - kCenter;
			kDiff.y = 0;

			fLen = kDiff.Length();

			kSel.m_kWorld = kWorld;
// 			kSel.m_kWorld.y = 0;
			// If inside inner circle set value to max.
			if(fLen < fInRadius) 
			{
				kSel.m_fValue = fStrength;
				pkSelectionData->push_back(kSel);			
			}
			else
			if (fLen >= fInRadius && fLen <= fOutRadius) 
			{
				fLen2 = fLen - fInRadius;

				kSel.m_fValue = fStrength - ((fLen2 / fInOutDiff) * fStrength);
				//cout << kSel.m_fValue << ",";
				
				pkSelectionData->push_back(kSel);			
			}
			
			
		}
	}

// 	cout << "Num Of Vertices Selected: " << kSelection.size()<<endl;

}

void P_Heightmap::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write(m_iWidth);
	pkPackage->Write(m_iHeight);
	pkPackage->Write(m_fScale);
	pkPackage->Write(m_fMaxValue);

	int iSize = m_kHeightData.size();
	pkPackage->Write(iSize);
	
	for(int i = 0;i<iSize;i++)
	{
		pkPackage->Write(m_kHeightData[i]);		
		pkPackage->Write(m_kTextureIDs[i]);
		pkPackage->Write(m_kBrightness[i]);
	}

	//save all materials
	iSize = m_kMaterials.size();
	pkPackage->Write(iSize);
	
	for(int i = 0;i<m_kMaterials.size();i++)
	{
		pkPackage->Write_Str(m_kMaterials[i]->GetRes());
	
	}
}

void P_Heightmap::Load(ZFIoInterface* pkPackage,int iVersion)
{
	switch(iVersion)
	{
		case 1:
		{
			pkPackage->Read(m_iWidth);
			pkPackage->Read(m_iHeight);
			pkPackage->Read(m_fScale);
			pkPackage->Read(m_fMaxValue);
		
			m_pkEntity->SetRadius(Vector3(m_iWidth/2.0,0,m_iHeight/2.0).Length());
			//m_pkEntity->SetLocalAABB(GetEntity()->GetRadius());
			m_pkEntity->SetLocalAABB(Vector3(-m_iWidth,-m_fMaxValue,-m_iWidth),Vector3(m_iWidth,m_fMaxValue,m_iWidth));
		
			m_iRows = static_cast<int>( (m_iWidth/m_fScale)+1 );
			m_iCols = static_cast<int>( (m_iHeight/m_fScale)+1 );
		
			int 				iSize;
			float 			fVal;
			//unsigned char 	cTex;
			m_kHeightData.clear();
			m_kTextureIDs.clear();
			
			pkPackage->Read(iSize);	
			for(int i = 0;i<iSize;i++)
			{
				pkPackage->Read(fVal);
				m_kHeightData.push_back(fVal);
			
				m_kTextureIDs.push_back(0);	
			}
		
			m_bHaveRebuilt = false;
			break;
		}
		
		case 2:
		{
			pkPackage->Read(m_iWidth);
			pkPackage->Read(m_iHeight);
			pkPackage->Read(m_fScale);
			pkPackage->Read(m_fMaxValue);
		
			m_pkEntity->SetRadius(Vector3(m_iWidth/2.0,0,m_iHeight/2.0).Length());
			//m_pkEntity->SetLocalAABB(GetEntity()->GetRadius());
			m_pkEntity->SetLocalAABB(Vector3(-m_iWidth,-m_fMaxValue,-m_iWidth),Vector3(m_iWidth,m_fMaxValue,m_iWidth));
		
			m_iRows = static_cast<int>( (m_iWidth/m_fScale)+1 );
			m_iCols = static_cast<int>( (m_iHeight/m_fScale)+1 );
		
			int 				iSize;
			float 			fVal;
			char 				cTex;
			m_kHeightData.clear();
			m_kTextureIDs.clear();
			
			pkPackage->Read(iSize);	
			for(int i = 0;i<iSize;i++)
			{
				pkPackage->Read(fVal);
				m_kHeightData.push_back(fVal);
			
				pkPackage->Read(cTex);
				m_kTextureIDs.push_back(cTex);	
			}
		
			m_bHaveRebuilt = false;
			break;
		}		
		
		case 3:
		{
			pkPackage->Read(m_iWidth);
			pkPackage->Read(m_iHeight);
			pkPackage->Read(m_fScale);
			pkPackage->Read(m_fMaxValue);
		
			m_pkEntity->SetRadius(Vector3(m_iWidth/2.0,0,m_iHeight/2.0).Length());
			//m_pkEntity->SetLocalAABB(GetEntity()->GetRadius());
			m_pkEntity->SetLocalAABB(Vector3(-m_iWidth,-m_fMaxValue,-m_iWidth),Vector3(m_iWidth,m_fMaxValue,m_iWidth));
		
			m_iRows = static_cast<int>( (m_iWidth/m_fScale)+1 );
			m_iCols = static_cast<int>( (m_iHeight/m_fScale)+1 );
		
			int 				iSize;
			float 			fVal;
			signed char 	cTex;
			m_kHeightData.clear();
			m_kTextureIDs.clear();
			m_kBrightness.clear();
			
			pkPackage->Read(iSize);	
			for(int i = 0;i<iSize;i++)
			{
				pkPackage->Read(fVal);
				m_kHeightData.push_back(fVal);
			
				pkPackage->Read(cTex);
				m_kTextureIDs.push_back(cTex);					
				
				m_kBrightness.push_back(255);					
			}
		
		
			//load all materials
			
			//remove all old materials
			for(int i = 0;i<m_kMaterials.size();i++)
				delete m_kMaterials[i];			
			m_kMaterials.clear();
			
			
			string strMaterialName;
			pkPackage->Read(iSize);									
			for(int i = 0;i<iSize;i++)
			{
				pkPackage->Read_Str(strMaterialName);
					
				ZFResourceHandle* pkTempMat = new ZFResourceHandle;
				pkTempMat->SetRes(strMaterialName);	
					
				m_kMaterials.push_back(pkTempMat);
			}				
			
		
			m_bHaveRebuilt = false;
			break;
		}				
	
		case 4:
		{
			pkPackage->Read(m_iWidth);
			pkPackage->Read(m_iHeight);
			pkPackage->Read(m_fScale);
			pkPackage->Read(m_fMaxValue);
		
			m_pkEntity->SetRadius(Vector3(m_iWidth/2.0,0,m_iHeight/2.0).Length());
			m_pkEntity->SetLocalAABB(Vector3(-m_iWidth,-m_fMaxValue,-m_iWidth),Vector3(m_iWidth,m_fMaxValue,m_iWidth));
		
			m_iRows = static_cast<int>( (m_iWidth/m_fScale)+1 );
			m_iCols = static_cast<int>( (m_iHeight/m_fScale)+1 );
		
			int 				iSize;
			float 			fVal;
			signed char 	cTex;
			unsigned char 	cBrightness;
			m_kHeightData.clear();
			m_kTextureIDs.clear();
			m_kBrightness.clear();

			
			pkPackage->Read(iSize);	
			for(int i = 0;i<iSize;i++)
			{
				pkPackage->Read(fVal);
				m_kHeightData.push_back(fVal);
			
				pkPackage->Read(cTex);
				m_kTextureIDs.push_back(cTex);
									
				pkPackage->Read(cBrightness);
				m_kBrightness.push_back(cBrightness);																		
			}
		
		
			//load all materials
			
			//remove all old materials
			for(int i = 0;i<m_kMaterials.size();i++)
				delete m_kMaterials[i];			
			m_kMaterials.clear();
			
			
			string strMaterialName;
			pkPackage->Read(iSize);									
			for(int i = 0;i<iSize;i++)
			{
				pkPackage->Read_Str(strMaterialName);
					
				ZFResourceHandle* pkTempMat = new ZFResourceHandle;
				pkTempMat->SetRes(strMaterialName);	
					
				m_kMaterials.push_back(pkTempMat);
			}				
			
		
			m_bHaveRebuilt = false;
			break;
		}					
	}
}

void P_Heightmap::PackTo( NetPacket* pkNetPacket,int iConnectionID)
{
	int iStart = pkNetPacket->m_iPos;

	pkNetPacket->Write(m_iWidth);
	pkNetPacket->Write(m_iHeight);
	pkNetPacket->Write(m_fScale);

// 	cout<<"size:"<<pkNetPacket->m_iLength<<endl;


	int iSize = m_kHeightData.size();
	pkNetPacket->Write(iSize);	
	for(int i = 0;i<iSize;i++)
	{
		pkNetPacket->Write(m_kHeightData[i]);
		pkNetPacket->Write(m_kTextureIDs[i]);
		pkNetPacket->Write(m_kBrightness[i]);		
	}
	
// 	cout<<"size:"<<pkNetPacket->m_iLength<<endl;
	
	iSize = m_kMaterials.size();
	pkNetPacket->Write(iSize);
	for(int i = 0;i<iSize;i++)
	{		
		pkNetPacket->Write_Str(m_kMaterials[i]->GetRes());	
	}
	
// 	cout<<"size:"<<pkNetPacket->m_iLength<<endl;
}

void P_Heightmap::PackFrom( NetPacket* pkNetPacket,int iConnectionID)
{
	pkNetPacket->Read(m_iWidth);
	pkNetPacket->Read(m_iHeight);
	pkNetPacket->Read(m_fScale);

	m_pkEntity->SetRadius(Vector3(m_iWidth/2.0,0,m_iHeight/2.0).Length());
	m_pkEntity->SetLocalAABB(Vector3(-m_iWidth,-m_fMaxValue,-m_iWidth),Vector3(m_iWidth,m_fMaxValue,m_iWidth));

	m_iRows = static_cast<int>( (m_iWidth/m_fScale)+1 );
	m_iCols = static_cast<int>( (m_iHeight/m_fScale)+1 );

	int 				iSize;
	float 			fVal;
	signed char 	cTex;
	unsigned char	cBrightness;
	m_kHeightData.clear();
	m_kTextureIDs.clear();
	m_kBrightness.clear();
	
	pkNetPacket->Read(iSize);	
	for(int i = 0;i<iSize;i++)
	{
		pkNetPacket->Read(fVal);
		m_kHeightData.push_back(fVal);
		
		pkNetPacket->Read(cTex);
		m_kTextureIDs.push_back(cTex);
		
		pkNetPacket->Read(cBrightness);
		m_kBrightness.push_back(cBrightness);	
	}


	//clear old materials	
	for(int i =0;i<m_kMaterials.size();i++)
		delete m_kMaterials[i];	
	m_kMaterials.clear();
		
	
	string strMat;
	pkNetPacket->Read(iSize);
	for(int i = 0;i<iSize;i++)
	{	
		pkNetPacket->Read_Str(strMat);	
		
		ZFResourceHandle* pkNewMat = new ZFResourceHandle;	
		pkNewMat->SetRes(strMat);
	
		m_kMaterials.push_back(pkNewMat);		
	}


	m_bHaveRebuilt = false;
}



Property* Create_HeightmapProperty()
{
	return new P_Heightmap;
}

void Register_HeightmapProperty(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Heightmap", Create_HeightmapProperty);				

	// Register Property Script Interface
// 	g_pkScript->ExposeFunction("SetNextAnim",			SI_PMad::SetNextAnim);

}



















/*
void P_Heightmap::DrawHeightmap()
{
	if(!m_bHaveRebuilt)
		RebuildArrays();

	m_pkZShaderSystem->MatrixPush();
	
	m_pkZShaderSystem->BindMaterial((ZMaterial*)(m_pkMaterial->GetResourcePtr()) );
	m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetWorldPosV() - Vector3(m_iWidth/2.0,0,m_iHeight/2.0)  );

	if(m_pkVBO)
	{
		m_pkZShaderSystem->DrawVertexBuffer(m_pkVBO);	
	}
	else
	{
		m_pkZShaderSystem->ResetPointers();
		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&m_kVertexData[0]);
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,&m_kTextureData[0]);
		m_pkZShaderSystem->SetPointer(NORMAL_POINTER,&m_kNormalData[0]);
		m_pkZShaderSystem->SetNrOfVertexs(m_kVertexData.size());
		
		m_pkZShaderSystem->DrawArray(TRIANGLESTRIP_MODE);
	}
	
	m_pkZShaderSystem->MatrixPop();
}

void P_Heightmap::RebuildArrays()
{
	m_bHaveRebuilt = true;

	m_kVertexData.clear();
	m_kNormalData.clear();
	m_kTextureData.clear();

	float fXMod = 0.25 ;// float(m_iRows+1);
	float fYMod = 0.25 ;// float(m_iCols+1);
	float fS = m_fScale;

	bool bRight = true;

	m_kVertexData.push_back(Vector3(0,m_kHeightData[0],0));
	m_kTextureData.push_back(Vector2(0,0));
	m_kNormalData.push_back(GenerateNormal(0,0));

	for(int y = 0;y<m_iCols-1;y++)
	{	
		if(bRight)
		{
			bRight = false;
					
			for(int x = 0;x<m_iRows-1;x++)
			{
				m_kVertexData.push_back(Vector3(x*fS,m_kHeightData[(y+1)*m_iRows + x],(y+1)*fS));
				m_kTextureData.push_back(Vector2( x*fS*fXMod,		(y+1)*fS*fYMod));
				m_kNormalData.push_back(GenerateNormal(x,y+1));
				
				m_kVertexData.push_back(Vector3((x+1)*fS,m_kHeightData[y*m_iRows + (x+1)],y*fS));	
				m_kTextureData.push_back(Vector2((x+1)*fS*fXMod,	y*fS*fYMod));
				m_kNormalData.push_back(GenerateNormal(x+1,y));
			}
		
		
			//add extra polygon in the end
			m_kVertexData.push_back( Vector3((m_iRows-1)*fS,m_kHeightData[(y+1)*m_iRows + m_iRows-1],(y+1)*fS) );
 			m_kVertexData.push_back( Vector3((m_iRows-1)*fS,m_kHeightData[(y+1)*m_iRows + m_iRows-1],(y+1)*fS) );
 			m_kVertexData.push_back( Vector3((m_iRows-1)*fS,m_kHeightData[(y+1)*m_iRows + m_iRows-1],(y+1)*fS) );
 			m_kTextureData.push_back( Vector2((m_iRows-1)*fS * fXMod,(y+1)*fS*fYMod));		
			m_kTextureData.push_back( Vector2((m_iRows-1)*fS * fXMod,(y+1)*fS*fYMod));				 			
 			m_kTextureData.push_back( Vector2((m_iRows-1)*fS * fXMod,(y+1)*fS*fYMod));		
			m_kNormalData.push_back(GenerateNormal(m_iRows-1,y+1));
			m_kNormalData.push_back(GenerateNormal(m_iRows-1,y+1));
			m_kNormalData.push_back(GenerateNormal(m_iRows-1,y+1));
		
		}
		else
		{
			bRight = true;
		
			for(int x = m_iRows-1;x>0;x--)
			{
				m_kVertexData.push_back(Vector3(x*fS,m_kHeightData[(y+1)*m_iRows + x],(y+1)*fS));
				m_kTextureData.push_back(Vector2(x*fS*fXMod,		(y+1)*fS*fYMod));
				m_kNormalData.push_back(GenerateNormal(x,y+1));
				
				m_kVertexData.push_back(Vector3((x-1)*fS,m_kHeightData[y*m_iRows + (x-1)],y*fS));	
				m_kTextureData.push_back(Vector2((x-1)*fS*fXMod,	y*fS*fYMod));												
				m_kNormalData.push_back(GenerateNormal(x-1,y));			
			}
		
			//add extra polygon in the end
			m_kVertexData.push_back(Vector3(0,m_kHeightData[(y+1)*m_iRows],(y+1)*fS));
			m_kVertexData.push_back(Vector3(0,m_kHeightData[(y+1)*m_iRows],(y+1)*fS));
			m_kVertexData.push_back(Vector3(0,m_kHeightData[(y+1)*m_iRows],(y+1)*fS));
			m_kTextureData.push_back(Vector2(0,(y+1)*fS*fYMod));		
			m_kTextureData.push_back(Vector2(0,(y+1)*fS*fYMod));
			m_kTextureData.push_back(Vector2(0,(y+1)*fS*fYMod));		
			m_kNormalData.push_back(GenerateNormal(0,(y+1)*fS) );			
			m_kNormalData.push_back(GenerateNormal(0,(y+1)*fS) );			
			m_kNormalData.push_back(GenerateNormal(0,(y+1)*fS) );			
		
		}		
	}
	
	
	if(m_pkVBO)
		delete m_pkVBO;
		
	if(m_kVertexData.size() > 500)
	{
		m_pkZShaderSystem->ResetPointers();
		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&m_kVertexData[0]);
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,&m_kTextureData[0]);
		m_pkZShaderSystem->SetPointer(NORMAL_POINTER,&m_kNormalData[0]);
		m_pkZShaderSystem->SetNrOfVertexs(m_kVertexData.size());
	
		m_pkZShaderSystem->BindMaterial((ZMaterial*)(m_pkMaterial->GetResourcePtr()) );	
		m_pkVBO = m_pkZShaderSystem->CreateVertexBuffer(TRIANGLESTRIP_MODE);	
	}
}

*/







