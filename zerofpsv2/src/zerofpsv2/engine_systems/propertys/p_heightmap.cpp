#include "p_heightmap.h"
#include "../../engine/zerofps.h"



P_Heightmap::P_Heightmap()
{
	strcpy(m_acName,"P_Heightmap");
	m_bNetwork = true;
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;


	m_pkZShaderSystem =	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
	m_pkLight=				static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light")); 
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			


	m_pkVBO = NULL;
	m_fScale = 2.0;
	m_fMaxValue = 100;
	
	m_pkMaterial = new ZFResourceHandle;		
	m_pkMaterial->SetRes("heightmap.zlm");	
	
	
	ZFResourceHandle* pkTempMat = new ZFResourceHandle;
	pkTempMat->SetRes("heightmap/grass.zlm");	
	m_kMaterials.push_back(pkTempMat);
		
	pkTempMat = new ZFResourceHandle;
	pkTempMat->SetRes("heightmap/rock.zlm");	
	m_kMaterials.push_back(pkTempMat);
	
	pkTempMat = new ZFResourceHandle;
	pkTempMat->SetRes("heightmap/dirt.zlm");	
	m_kMaterials.push_back(pkTempMat);
	
	
	
	SetSize(4,4);
}

void P_Heightmap::Init()
{
	m_pkEntity->SetRadius(Vector3(m_iWidth/2.0,0,m_iHeight/2.0).Length());
}

P_Heightmap::~P_Heightmap()
{
	delete m_pkMaterial;

}

void P_Heightmap::Update()
{
	if(!( (m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_CASTSHADOW ||
			 m_pkZeroFps->GetCam()->GetCurrentRenderMode() == RENDER_SHADOWED)))
		return;

	if(!m_pkZeroFps->GetCam()->GetFrustum()->CubeInFrustum(	m_pkEntity->GetWorldPosV() - Vector3(m_iWidth/2.0,m_fMaxValue,m_iHeight/2.0),
																				m_pkEntity->GetWorldPosV() + Vector3(m_iWidth/2.0,m_fMaxValue,m_iHeight/2.0)))
		return;

	//update light					
 	m_pkLight->Update(&m_kLightProfile,GetEntity()->GetWorldPosV());					


//   	if(m_pkZeroFps->GetDebugGraph())
// 	{
// 		bool bUse = m_pkZShaderSystem->GetUseDefaultGLSLProgram();
// 		m_pkZShaderSystem->UseDefaultGLSLProgram(false);
// 		
//   		m_pkRender->Sphere(m_pkEntity->GetWorldPosV(),0.5,1,Vector3(1,1,1),true);
// 		
// 		m_pkRender->DrawAABB(m_pkEntity->GetWorldPosV() - Vector3(m_iWidth/2.0,4,m_iHeight/2.0),
// 									m_pkEntity->GetWorldPosV()+Vector3(m_iWidth/2.0,4,m_iHeight/2.0),
// 									Vector3(1,1,1),1);
// 		
// 		m_pkZShaderSystem->UseDefaultGLSLProgram(bUse);
// 	}


// 	DrawHeightmap();
	DrawTexturedHeightmap();
}

void P_Heightmap::DrawTexturedHeightmap()
{
	if(!m_bHaveRebuilt)
		BuildTextureArrays();

	m_pkZShaderSystem->MatrixPush();	
	m_pkZShaderSystem->MatrixTranslate(m_pkEntity->GetWorldPosV() - Vector3(m_iWidth/2.0,0,m_iHeight/2.0)  );


	for(int i =0;i<m_kDataArrays.size();i++)
	{
		if(m_kDataArrays[i]->m_kVertexData.empty())
			continue;
	
		//always draw the first material without blending
		if(i == 0)
			m_pkZShaderSystem->ForceBlending(BLEND_FORCE_OPAQ);
		
		m_pkZShaderSystem->BindMaterial((ZMaterial*)(m_kMaterials[i]->GetResourcePtr()) );	


		m_pkZShaderSystem->ResetPointers();
		m_pkZShaderSystem->SetPointer(COLOR_POINTER,&(m_kDataArrays[i]->m_kColorData[0]));
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,&(m_kDataArrays[i]->m_kTextureData[0]));
		m_pkZShaderSystem->SetPointer(NORMAL_POINTER,&(m_kDataArrays[i]->m_kNormalData[0]));
		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,&(m_kDataArrays[i]->m_kVertexData[0]));
		m_pkZShaderSystem->SetNrOfVertexs(m_kDataArrays[i]->m_kVertexData.size());
		
		m_pkZShaderSystem->DrawArray(TRIANGLES_MODE);		
		
		if(i == 0)
			m_pkZShaderSystem->ForceBlending(BLEND_MATERIAL);
	}


	m_pkZShaderSystem->MatrixPop();	
	
	return;
}

void P_Heightmap::BuildTextureArrays()
{
	m_bHaveRebuilt = true;

	//clear data arrays
	for(int i = 0;i<m_kDataArrays.size();i++)
	{
		delete m_kDataArrays[i];
	}
	m_kDataArrays.clear(); 
	
	

	for(int i =0;i<m_kMaterials.size();i++)
	{
		HeightmapArrays* pkNewArrays = new HeightmapArrays;
		m_kDataArrays.push_back(pkNewArrays);									
		
		//draw the first material all over the place
		if(i == 0)
		{
			for(int y = 0;y<m_iCols-1;y++)
			{	
				for(int x = 0;x<m_iRows-1;x++)
				{	
					AddPolygon(pkNewArrays,x,y,i,true);
					AddPolygon(pkNewArrays,x,y,i,false);			
				}
			}				
		}
		else
		{
			for(int y = 0;y<m_iCols-1;y++)
			{	
				for(int x = 0;x<m_iRows-1;x++)
				{					
					//TOP POLYGON
					if(m_kTextureIDs[y*m_iRows + x] == i ||
						m_kTextureIDs[(y+1)*m_iRows + x] == i||
						m_kTextureIDs[y*m_iRows + x+1] == i)
					{
						AddPolygon(pkNewArrays,x,y,i,true);
					}					
					
					//BOTTOM POLYGON											
					if(m_kTextureIDs[y*m_iRows + x+1] == i ||
						m_kTextureIDs[(y+1)*m_iRows + x] == i||
						m_kTextureIDs[(y+1)*m_iRows + x+1] == i)
					{
						AddPolygon(pkNewArrays,x,y,i,false);
					}
				}
			}				
		}		
	}
}

void P_Heightmap::AddPolygon(HeightmapArrays* pkNewArrays,int x,int y,int i,bool bTop)
{
	static float fTexMod = 0.25;
	static Vector4 kC1,kC2,kC3;

	if(bTop)
	{
		//color
		if(i == 0)
		{
			kC1.Set(1,1,1,1);
			kC2.Set(1,1,1,1);
			kC3.Set(1,1,1,1);
		}
		else
		{
			if(m_kTextureIDs[y*m_iRows + x] == i)
				kC1.Set(1,1,1,1);else kC1.Set(1,1,1,0);
			if(m_kTextureIDs[(y+1)*m_iRows + x] == i)
				kC2.Set(1,1,1,1);else kC2.Set(1,1,1,0);
			if(m_kTextureIDs[y*m_iRows + x+1] == i)
				kC3.Set(1,1,1,1);else kC3.Set(1,1,1,0);					
		}
					
		pkNewArrays->m_kColorData.push_back(kC1);
		pkNewArrays->m_kColorData.push_back(kC2);
		pkNewArrays->m_kColorData.push_back(kC3);
			
			
		//UV's
		pkNewArrays->m_kTextureData.push_back(Vector2(x*fTexMod,y*fTexMod));
		pkNewArrays->m_kTextureData.push_back(Vector2(x*fTexMod,(y+1)*fTexMod));
		pkNewArrays->m_kTextureData.push_back(Vector2((x+1)*fTexMod,y*fTexMod));				
		
		//normals
		pkNewArrays->m_kNormalData.push_back(GenerateNormal(x,y));
		pkNewArrays->m_kNormalData.push_back(GenerateNormal(x,y+1));
		pkNewArrays->m_kNormalData.push_back(GenerateNormal(x+1,y));				
		
		//vertex
		pkNewArrays->m_kVertexData.push_back(Vector3(x*m_fScale,m_kHeightData[y*m_iRows + x],y*m_fScale));
		pkNewArrays->m_kVertexData.push_back(Vector3(x*m_fScale,m_kHeightData[(y+1)*m_iRows + x],(y+1)*m_fScale));
		pkNewArrays->m_kVertexData.push_back(Vector3((x+1)*m_fScale,m_kHeightData[y*m_iRows + x+1],y*m_fScale));
						
	}
	else
	{
		//color
		if(i == 0)
		{
			kC1.Set(1,1,1,1);
			kC2.Set(1,1,1,1);
			kC3.Set(1,1,1,1);
		}
		else
		{			
			if(m_kTextureIDs[y*m_iRows + x+1] == i)
				kC1.Set(1,1,1,1);else kC1.Set(1,1,1,0);
			if(m_kTextureIDs[(y+1)*m_iRows + x] == i)
				kC2.Set(1,1,1,1);else kC2.Set(1,1,1,0);
			if(m_kTextureIDs[(y+1)*m_iRows + x+1] == i)
				kC3.Set(1,1,1,1);else kC3.Set(1,1,1,0);					
		}
			
		pkNewArrays->m_kColorData.push_back(kC1);
		pkNewArrays->m_kColorData.push_back(kC2);
		pkNewArrays->m_kColorData.push_back(kC3);
			
			
		//UV's
		pkNewArrays->m_kTextureData.push_back(Vector2((x+1)*fTexMod,y*fTexMod));
		pkNewArrays->m_kTextureData.push_back(Vector2(x*fTexMod,(y+1)*fTexMod));
		pkNewArrays->m_kTextureData.push_back(Vector2((x+1)*fTexMod,(y+1)*fTexMod));				
		
		//normals
		pkNewArrays->m_kNormalData.push_back(GenerateNormal(x+1,y));
		pkNewArrays->m_kNormalData.push_back(GenerateNormal(x,y+1));
		pkNewArrays->m_kNormalData.push_back(GenerateNormal(x+1,y+1));				
		
		//vertex
		pkNewArrays->m_kVertexData.push_back(Vector3((x+1)*m_fScale,m_kHeightData[y*m_iRows + x+1],y*m_fScale));
		pkNewArrays->m_kVertexData.push_back(Vector3(x*m_fScale,m_kHeightData[(y+1)*m_iRows + x],(y+1)*m_fScale));
		pkNewArrays->m_kVertexData.push_back(Vector3((x+1)*m_fScale,m_kHeightData[(y+1)*m_iRows + x+1],(y+1)*m_fScale));	
	}
}


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
	
	int iTileIndex;

	bool bRight = false;

	for(int y=0; y<m_iCols-1; y++) 
	{
		for(int x=0; x<m_iRows-1; x++) 
		{
			iTileIndex = y * m_iRows + x;

			//top polygon
// 			if(bRight)
// 			{
				V1 = Vector3((float)x*m_fScale, 			m_kHeightData[y*m_iRows+x], 			(float)y*m_fScale);
				V2 = Vector3((float)x*m_fScale, 	m_kHeightData[(y+1)*m_iRows+x], 				(float)(y + 1.0f)*m_fScale);
				V3 = Vector3((float)(x + 1)*m_fScale, 	m_kHeightData[(y)*m_iRows+(x+1)], 	(float)y*m_fScale);
// 			}
// 			else
// 			{
// 				V1 = Vector3((float)x*m_fScale, 			m_kHeightData[y*m_iRows+x], 			(float)y*m_fScale);
// 				V2 = Vector3((float)(x )*m_fScale, 	m_kHeightData[(y+1)*m_iRows+(x)], (float)(y + 1.0f)*m_fScale);
// 				V3 = Vector3((float)(x + 1)*m_fScale, 	m_kHeightData[(y)*m_iRows+(x+1)], 	(float)y*m_fScale);						
// 			}
			
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
// 			if(bRight)
// 			{
				V1 = Vector3((float)(x+1)*m_fScale, 	m_kHeightData[y*m_iRows+x+1], 		(float)y*m_fScale); 
				V2 = Vector3((float)x*m_fScale , 		m_kHeightData[(y+1)*m_iRows+x], 		(float)(y + 1.0f)*m_fScale);
				V3 = Vector3((float)(x + 1)*m_fScale, 	m_kHeightData[(y+1)*m_iRows+(x+1)], (float)(y + 1.0f)*m_fScale);			
// 			}
// 			else
// 			{
// 				V1 = Vector3((float)(x+1)*m_fScale, 	m_kHeightData[y*m_iRows+x+1], 			(float)y*m_fScale); 
// 				V2 = Vector3((float)x*m_fScale , 		m_kHeightData[(y+1)*m_iRows+x], 		(float)(y + 1.0f)*m_fScale);
// 				V3 = Vector3((float)(x + 1)*m_fScale, 	m_kHeightData[(y+1)*m_iRows+(x+1)], (float)(y + 1.0f)*m_fScale);						
// 			}
// 			
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
		
		if(bRight)
			bRight = false;
		else
			bRight = true;
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
	m_iRows = (m_iWidth/m_fScale)+1;
	m_iCols = (m_iHeight/m_fScale)+1;
	
	
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
			
// 			m_kHeightData.push_back(sin((x*m_fScale+y*m_fScale)/8.0) *2.0);
 			m_kHeightData.push_back(0);
// 			if(x == 0 || x == m_iRows-1 || y == 0|| y == m_iCols-1)
// 				m_kHeightData.push_back(0);
// 			else	
// 				m_kHeightData.push_back(Randomf(16)-8);
		}

// 	Smooth();
// 	Smooth();

	m_bHaveRebuilt = false;
}

void P_Heightmap::Smooth(vector<HMSelectionData>* kSelectionData)
{
	if(kSelectionData)
	{
		for(int i = 0;i<kSelectionData->size();i++)
		{			
			if((*kSelectionData)[i].y == 0 || (*kSelectionData)[i].y == m_iCols-1 ||
				(*kSelectionData)[i].x == 0 || (*kSelectionData)[i].x == m_iRows-1)
				continue;
		
			int iIndex = (*kSelectionData)[i].y * m_iRows + (*kSelectionData)[i].x;		
			m_kHeightData[iIndex] = (	m_kHeightData[iIndex] + 
												m_kHeightData[iIndex+1]+
												m_kHeightData[iIndex-1]+
												m_kHeightData[iIndex+m_iRows]+
												m_kHeightData[iIndex-m_iRows]) / 5.0;
					
		}	
	}
	else
	{
		for(int y = 1;y<m_iCols-1;y++)
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

void P_Heightmap::SetTexture(vector<HMSelectionData>* kSelectionData,char cTexture)
{
	for(int i = 0;i<kSelectionData->size();i++)
	{			
		m_kTextureIDs[(*kSelectionData)[i].y * m_iRows + (*kSelectionData)[i].x] = cTexture;	
	}
	
	ResetAllNetUpdateFlags();
	m_bHaveRebuilt = false;
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
			
				int iID = (*pkSelectionData)[i].m_pkHeightMap->m_kTextureIDs[(*pkSelectionData)[i].y * (*pkSelectionData)[i].m_pkHeightMap->m_iRows + (*pkSelectionData)[i].x];
				(*pkSelectionData)[j].m_pkHeightMap->m_kTextureIDs[(*pkSelectionData)[j].y * (*pkSelectionData)[j].m_pkHeightMap->m_iRows + (*pkSelectionData)[j].x] = iID;
			
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

void P_Heightmap::GetSelection(const Vector3& kCenter, float fInRadius, float fOutRadius,vector<HMSelectionData>* pkSelectionData)
{
	HMSelectionData kSel;
	kSel.m_fValue = 1.0;

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
				kSel.m_fValue = 1.0;
				pkSelectionData->push_back(kSel);			
			}
			else
			if (fLen >= fInRadius && fLen <= fOutRadius) 
			{
				fLen2 = fLen - fInRadius;

				kSel.m_fValue = 1.0 - (fLen2 / fInOutDiff);
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
	}

}

void P_Heightmap::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read(m_iWidth);
	pkPackage->Read(m_iHeight);
	pkPackage->Read(m_fScale);
	pkPackage->Read(m_fMaxValue);

	m_pkEntity->SetRadius(Vector3(m_iWidth/2.0,0,m_iHeight/2.0).Length());

	m_iRows = (m_iWidth/m_fScale)+1;
	m_iCols = (m_iHeight/m_fScale)+1;

	int iSize;
	float fVal;
	char cTex;
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
}

void P_Heightmap::PackTo( NetPacket* pkNetPacket,int iConnectionID)
{
	int iStart = pkNetPacket->m_iPos;

	pkNetPacket->Write(m_iWidth);
	pkNetPacket->Write(m_iHeight);
	pkNetPacket->Write(m_fScale);

	int iSize = m_kHeightData.size();
	pkNetPacket->Write(iSize);
	
	for(int i = 0;i<iSize;i++)
	{
		pkNetPacket->Write(m_kHeightData[i]);
		pkNetPacket->Write(m_kTextureIDs[i]);
	}
}

void P_Heightmap::PackFrom( NetPacket* pkNetPacket,int iConnectionID)
{
	pkNetPacket->Read(m_iWidth);
	pkNetPacket->Read(m_iHeight);
	pkNetPacket->Read(m_fScale);

	m_pkEntity->SetRadius(Vector3(m_iWidth/2.0,0,m_iHeight/2.0).Length());

	m_iRows = (m_iWidth/m_fScale)+1;
	m_iCols = (m_iHeight/m_fScale)+1;

	int iSize;
	float fVal;
	char cTex;
	m_kHeightData.clear();
	m_kTextureIDs.clear();
	
	pkNetPacket->Read(iSize);	
	for(int i = 0;i<iSize;i++)
	{
		pkNetPacket->Read(fVal);
		m_kHeightData.push_back(fVal);
		
		pkNetPacket->Read(cTex);
		m_kTextureIDs.push_back(cTex);
	}

	m_bHaveRebuilt = false;
}



Property* Create_HeightmapProperty()
{
	return new P_Heightmap;
}

void Register_HeightmapProperty(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_Heightmap", Create_HeightmapProperty);				

	// Register Property Script Interface
// 	g_pkScript->ExposeFunction("SetNextAnim",			SI_PMad::SetNextAnim);

}