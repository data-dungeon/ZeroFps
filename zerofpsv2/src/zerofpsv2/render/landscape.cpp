#include "render.h"
   
#include "../engine_systems/common/heightmap.h" 
//#include "../engine_systems/common/i_heightmap2.h"
    
void Render::DrawSkyBox_SixSided(Vector3 CamPos,Vector3 kHead,int* aiSideTextures)
{
	m_pkZShaderSystem->Push("Render::DrawSkyBox_SixSided");

	glPushMatrix();
//	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_LIGHTING_BIT|GL_FOG_BIT);

	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);	//dont want lighting on the skybox		
	glDepthMask(GL_FALSE);	//want the skybox to be faaaaaar away =)
	glPolygonMode(GL_FRONT, GL_FILL);

	glTranslatef(CamPos.x,CamPos.y,CamPos.z);
 
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);

	glScalef(100,100,100);

//	glDisable(GL_TEXTURE_2D);

//	glDisable(GL_CULL_FACE);

	Vector3 CubeVertex[8];
	CubeVertex[0].Set(-1,  1, -1);	//Front	// Top Left	
	CubeVertex[1].Set(-1, -1, -1);	// Low left
	CubeVertex[2].Set(1,  -1, -1);	// Low right
	CubeVertex[3].Set(1,   1, -1);	// Top right
	CubeVertex[4].Set(-1,  1,  1);	// Back
	CubeVertex[5].Set(-1, -1,  1);
	CubeVertex[6].Set(1,  -1,  1);
	CubeVertex[7].Set(1,   1,  1);

	glColor4f(1,1,1,1);

	m_pkTexMan->BindTexture(aiSideTextures[SKYBOXSIDE_FORWARD]);	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);		glVertex3fv( &CubeVertex[0].x );		
		glTexCoord2f(0, 1);		glVertex3fv( &CubeVertex[1].x);
		glTexCoord2f(1, 1);		glVertex3fv( &CubeVertex[2].x);		
		glTexCoord2f(1, 0);		glVertex3fv( &CubeVertex[3].x);		
	glEnd();

	m_pkTexMan->BindTexture(aiSideTextures[SKYBOXSIDE_LEFT]);	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);		glVertex3fv( &CubeVertex[3].x );		
		glTexCoord2f(0, 1);		glVertex3fv( &CubeVertex[2].x);
		glTexCoord2f(1, 1);		glVertex3fv( &CubeVertex[6].x);		
		glTexCoord2f(1, 0);		glVertex3fv( &CubeVertex[7].x);		
	glEnd();
	
	m_pkTexMan->BindTexture(aiSideTextures[SKYBOXSIDE_BACK]);	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);		glVertex3fv( &CubeVertex[7].x );		
		glTexCoord2f(0, 1);		glVertex3fv( &CubeVertex[6].x);
		glTexCoord2f(1, 1);		glVertex3fv( &CubeVertex[5].x);		
		glTexCoord2f(1, 0);		glVertex3fv( &CubeVertex[4].x);		
	glEnd();

	m_pkTexMan->BindTexture(aiSideTextures[SKYBOXSIDE_RIGHT]);	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);		glVertex3fv( &CubeVertex[4].x );		
		glTexCoord2f(0, 1);		glVertex3fv( &CubeVertex[5].x);
		glTexCoord2f(1, 1);		glVertex3fv( &CubeVertex[1].x);		
		glTexCoord2f(1, 0);		glVertex3fv( &CubeVertex[0].x);		
	glEnd();

	m_pkTexMan->BindTexture(aiSideTextures[SKYBOXSIDE_UP]);	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);		glVertex3fv( &CubeVertex[7].x );		
		glTexCoord2f(0, 1);		glVertex3fv( &CubeVertex[4].x);
		glTexCoord2f(1, 1);		glVertex3fv( &CubeVertex[0].x);		
		glTexCoord2f(1, 0);		glVertex3fv( &CubeVertex[3].x);		
	glEnd();

	m_pkTexMan->BindTexture(aiSideTextures[SKYBOXSIDE_DOWN]);	
	glBegin(GL_QUADS);
		glTexCoord2f(1, 1);		glVertex3fv( &CubeVertex[2].x );		
		glTexCoord2f(1, 0);		glVertex3fv( &CubeVertex[1].x);  
		glTexCoord2f(0, 0);		glVertex3fv( &CubeVertex[5].x);		
		glTexCoord2f(0, 1);		glVertex3fv( &CubeVertex[6].x);		
	glEnd();


	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);		
	
	glPopAttrib();
	glPopMatrix();

	
	m_pkZShaderSystem->Pop();
}


void Render::DrawSkyBox(Vector3 CamPos,Vector3 kHead,int iHor,int iTop) 
{
	m_pkZShaderSystem->Push("Render::DrawSkyBox");
	
	
	float fYpos;
	

	if(kHead.x==0 && kHead.y==0 && kHead.z==0)
		fYpos=90;
	else
		fYpos=0;
	
	//fYpos=0;

	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT|GL_FOG_BIT);
	
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);//dont want lighting on the skybox		
	glDepthMask(GL_FALSE);//want the skybox to be faaaaaar away =)
	glPolygonMode(GL_FRONT, GL_FILL);
	
	glTranslatef(CamPos.x,CamPos.y+fYpos,CamPos.z);
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);
	glScalef(200,200,200);
	
	glColor4f(1,1,1,1);
	 
	m_pkTexMan->BindTexture(iHor);	
	glBegin(GL_QUAD_STRIP);		
		//FRONT
		glNormal3f(0,0,-1);
		glTexCoord2f(0	,0);glVertex3f(-.5,.5  ,-.5);
		glTexCoord2f(0	,1);glVertex3f(-.5,-.5	,-.5);
		
		glTexCoord2f(.25,0);glVertex3f(.5 ,.5	,-.5);		
		glTexCoord2f(.25,1);glVertex3f(.5 ,-.5  ,-.5);		


		//Right
		glNormal3f(-1,0,0);
		glTexCoord2f(.50,0);glVertex3f(.5 ,.5	,.5);		
		glTexCoord2f(.50,1);glVertex3f(.5 ,-.5  ,.5);		

		//Back
		glNormal3f(0,0,1);
		glTexCoord2f(.75,0);glVertex3f(-.5 ,.5	,.5);		
		glTexCoord2f(.75,1);glVertex3f(-.5 ,-.5  ,.5);		
		
		//Left
		glNormal3f(1,0,0);		
		glTexCoord2f(1,0);glVertex3f(-.5 ,.5	,-.5);		
		glTexCoord2f(1,1);glVertex3f(-.5 ,-.5  ,-.5);		
	glEnd();

	m_pkTexMan->BindTexture(iTop);	
	glBegin(GL_QUADS);
		//top
		glNormal3f(0,-1,0);
		glTexCoord2f(0	,0);		 glVertex3f(-.5,.5	,-.5);
		glTexCoord2f(1	,0);		 glVertex3f(.5,.5	,-.5);		
		glTexCoord2f(1	,.49);	glVertex3f(.5 ,.5	,.5);		
		glTexCoord2f(0	,.49);	glVertex3f(-.5 ,.5	,.5);		
		
		//bottom
		glNormal3f(0,1,0);
		glTexCoord2f(0	,.51);	glVertex3f(-.5,-.5	,-.5);
		glTexCoord2f(0	,1);		 glVertex3f(-.5,-.5	,.5);		
		glTexCoord2f(1	,1);		 glVertex3f(.5 ,-.5	,.5);		
		glTexCoord2f(1	,.51);	glVertex3f(.5 ,-.5	,-.5);		
	glEnd();
	
	glPopAttrib();
	glDepthMask(GL_TRUE);	
	glPopMatrix();

	
	m_pkZShaderSystem->Pop();
	
}


void Render::DrawWater(Vector3 kCamPos,Vector3 kPosition,Vector3 kHead,int iSize,int iStep,int iTexture, float fBlendValue) 
{
	m_pkZShaderSystem->Push("Render::DrawWater");


	float freq=500.0;
	float amp=0.5;
	
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	
	glTranslatef(kPosition.x,kPosition.y,kPosition.z);
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);
	glTranslatef( float(-iSize/2), 0, float(-iSize/2) );
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);	
	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);		
	
	glDepthMask(GL_FALSE);
	
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	m_pkTexMan->BindTexture(iTexture);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);	
	m_pkTexMan->BindTexture(iTexture);	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	
	float tx= float(SDL_GetTicks()/80000.0);	
	
	for(int x=0;x<iSize;x+=iStep){	
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0,1,0);
//		glColor4f(.5,.5,.7,0.5);	
		glColor4f(1,1,1, fBlendValue);	
	
		for(int z=0;z<iSize;z+=iStep) {
			float y= float(sin((SDL_GetTicks()/1000.0)+(z/iStep)*freq)*amp);		
		
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,z/(iStep)+tx);		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,.5,z/(iStep)-tx);				
			glVertex3f(float(x),float(y),float(z));
	
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,z/(iStep)+tx);		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.5,z/(iStep)-tx);						
			glVertex3f(float(x+iStep),float(y),float(z));
		}
		glEnd();
	}
	
		
	
	/*
	
	glBegin(GL_TRIANGLE_STRIP);		
	glNormal3f(0,1,0);
	glColor4f(.5,.5,.7,0.4);
	
	
	for(int z=0;z<iSize;z+=iStep){	
		float y=sin((SDL_GetTicks()/1000.0)+(z/iStep)*freq)*amp;
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,z/(iStep*10.0)+tx);		
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,z/(iStep*10.0)-tx);				
		glVertex3f(0,y,z);
	
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,iSize/(iStep*10.0),z/(iStep*10.0)+tx);		
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,iSize/(iStep*10.0),z/(iStep*10.0)-tx);						
		glVertex3f(iSize,y,z);
	}
	glEnd();	
	*/
	

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	glPopAttrib();
	glPopMatrix();

	glColor4f(1,1,1, 1);	
	
	m_pkZShaderSystem->Pop();
}

void Render::DrawSimpleWater(Vector3 kPosition,Vector4 kColor,int iSize,int iTexture) {
	//recomended color  .3,.3,.4,.99;
	
	glPushMatrix();

	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT);
		
	glDisable(GL_FOG);	
	glDisable(GL_LIGHTING);
	
	glDisable(GL_CULL_FACE);	
	glDepthMask(GL_FALSE);	
//	glDisable(GL_COLOR_MATERIAL);	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);
		

	m_pkTexMan->BindTexture(iTexture);

	float tx = float(SDL_GetTicks()/60000.0);
	glBegin(GL_QUADS);
		glColor4fv((float*)&kColor);
//		glColor4f(.3,.3,.4,.99);
		glNormal3f(0,1,0);
		glTexCoord2f(tx,0);
		glVertex3f(kPosition.x,kPosition.y,kPosition.z);
		glTexCoord2f(tx,1);		
		glVertex3f(kPosition.x,kPosition.y,kPosition.z+iSize);
		glTexCoord2f(tx+2,0);		
		glVertex3f(kPosition.x+iSize,kPosition.y,kPosition.z+iSize);
		glTexCoord2f(tx+2,1);		
		glVertex3f(kPosition.x+iSize,kPosition.y,kPosition.z);
	
		glTexCoord2f(-tx,0);
		glVertex3f(kPosition.x,kPosition.y,kPosition.z);
		glTexCoord2f(-tx,1);		
		glVertex3f(kPosition.x,kPosition.y,kPosition.z+iSize);
		glTexCoord2f(-tx+2,0);		
		glVertex3f(kPosition.x+iSize,kPosition.y,kPosition.z+iSize);
		glTexCoord2f(-tx+2,1);		
		glVertex3f(kPosition.x+iSize,kPosition.y,kPosition.z);
	glEnd();


	//if(m_FogEnable)//Disable the fog while drawing the sky box
	//	glEnable(GL_FOG);
	//glEnable(GL_LIGHTING);
	
	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);		
	glPopAttrib();
	glPopMatrix();	
}



//this funktion calculates the texture cordinat for a subtexture in a 1024x1024 texture
void Render::GiveTexCor(float &iX,float &iY,int iNr) {	
	iX = float(iNr%4);	
	iY = float((iNr-(iNr%4))/4);

	iX*=0.25;
	iY*=0.25;
	iY=iY;
	
//	cout<<"X: "<<iX<< "  Y: "<<iY<<endl;
}



void Render::DrawCross(const Vector3& kPos,const Vector3& kHead,const Vector3& kScale)//,int& iTexture1) //,int iTexture2) 
{
	static Vector3 normaldata[8] = { Vector3(0,1,0),
												Vector3(0,1,0),
												Vector3(0,1,0),
												Vector3(0,1,0),
												
												Vector3(0,1,0),
												Vector3(0,1,0),
												Vector3(0,1,0),
												Vector3(0,1,0),};

	
	static Vector3 apointdata[8] = { Vector3(-0.5,1,0),
												Vector3(-0.5,0,0),
												Vector3( 0.5,0,0),
												Vector3( 0.5,1,0),
										
												Vector3(0,1,-0.5),
												Vector3(0,1, 0.5),
												Vector3(0,0, 0.5),
												Vector3(0,0,-0.5)};

	static Vector2 atexdata[8] = { Vector2(0,1),
											Vector2(0,0),
											Vector2(1,0),
											Vector2(1,1),
											
											Vector2(1,1),
											Vector2(0,1),
											Vector2(0,0),
											Vector2(1,0)};
	
	
	m_pkZShaderSystem->MatrixPush();		
	
		m_pkZShaderSystem->MatrixTranslate(kPos);		
		m_pkZShaderSystem->MatrixRotate(kHead);		
		m_pkZShaderSystem->MatrixScale(kScale);		
	
		m_pkZShaderSystem->ResetPointers();
		m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,atexdata);
		m_pkZShaderSystem->SetPointer(VERTEX_POINTER,apointdata);
		m_pkZShaderSystem->SetPointer(NORMAL_POINTER,normaldata);
		m_pkZShaderSystem->SetNrOfVertexs(8);
		
		m_pkZShaderSystem->DrawArray(QUADS_MODE);							
	
	
	m_pkZShaderSystem->MatrixPop();
}


void Render::DrawHMLodSplat(HeightMap* kMap,Vector3 CamPos,int iFps)
{
	// Check if we should draw this at all.
	if(!m_iDrawLandscape)					return;
	if(kMap->m_iNumOfHMVertex == 0)		return;

	
  if(m_iAutoLod>0)
  {
		if((int)SDL_GetTicks()>(m_iLodUpdate+500))
		{
			m_iLodUpdate=SDL_GetTicks();
			
			if(iFps<(m_iFpsLock-5) && m_iDetail>20)
			{
				m_iDetail--;	
			} 
			else if(iFps>(m_iFpsLock+5) && m_iDetail<100)
			{
				m_iDetail++;		
			}
		}
	}

	m_pkZShaderSystem->Push("Render::DrawHMLodSplat");	
	
		
// 	glPushMatrix();
// 	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glTranslatef(kMap->m_kCornerPos.x,kMap->m_kCornerPos.y,kMap->m_kCornerPos.z);
	glColor4f(1,1,1,1);
	if(kMap->m_bInverted) glCullFace(GL_FRONT);

	//set polygon mode
	if(m_eLandscapePolygonMode == LINE)
			glPolygonMode(GL_FRONT, GL_LINE);
	else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//setup TUs
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE  );	// GL_MODULATE	 GL_REPLACE
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE );	
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
			
	m_pkTexMan->BindTexture(kMap->m_kLayer[0]->m_strTexture.c_str(),0);
	DrawAllHM(kMap,CamPos,true);

	// Set blending
	glDepthFunc(GL_EQUAL);
	glEnable(GL_BLEND);		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);				
	
 	//enable TU0 again
	glActiveTextureARB(GL_TEXTURE0_ARB);	
	glEnable(GL_TEXTURE_2D);	
		
	for(unsigned int i=1;i<kMap->m_kLayer.size();i++)
	{	
		if(i >= m_iMaxLandscapeLayers)
			break;

		glActiveTextureARB(GL_TEXTURE0_ARB);	
		//m_pkTexMan->BindTexture(kMap->m_kLayer[i].m_strMask.c_str(),T_NOMIPMAPPING|T_ALPHA);	
		string kLayerStr = kMap->m_kLayer[i]->m_strMask + string("ost");
		int iTexID = m_pkTexMan->CreateTextureFromRGB(kLayerStr.c_str(), 
			(color_rgb*)kMap->m_kLayer[i]->m_kAlphaImage.m_pkPixels, 
			kMap->m_kLayer[i]->m_kAlphaImage.m_iWidth, kMap->m_kLayer[i]->m_kAlphaImage.m_iHeight, 
			TextureManager::RGBA,
         false, TextureManager::RGBA);
		m_pkTexMan->BindTexture( iTexID );

		glActiveTextureARB(GL_TEXTURE1_ARB);
		m_pkTexMan->BindTexture(kMap->m_kLayer[i]->m_strTexture.c_str(),0);		
		DrawAllHM(kMap,CamPos,true);	
	}
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_BLEND);	
	if(kMap->m_bInverted) glCullFace(GL_BACK);

	if(m_eLandscapePolygonMode == FILLLINE)
	{
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_POLYGON_OFFSET_LINE);	glPolygonOffset(-1,-5);
		glPolygonMode(GL_FRONT, GL_LINE);
		DrawAllHM(kMap,CamPos,true);
		glDisable(GL_POLYGON_OFFSET_LINE);	glPolygonOffset(0,0);	
	}

// 	glPopAttrib();
// 	glPopMatrix();

	if(m_iDrawLandNormal)
		DrawNormals(kMap,kMap->m_kPosition,0);

		
	m_pkZShaderSystem->Pop();
}

void Render::DrawAllHM(HeightMap* kMap,Vector3 CamPos,bool bBorders)
{
	int iPatchSize = 1;
	glColor3f(1,1,1);

	for(int z=0;z<kMap->m_iTilesSide;z+=iPatchSize)
	{
		for(int x=0;x<kMap->m_iTilesSide;x+=iPatchSize)
		{
			DrawPatch(kMap,CamPos,x,z,iPatchSize,bBorders);		
		}
	}
}

void Render::DrawHMVertex(HeightMap* kMap)
{
	HM_vert* pkHmVertex = kMap->verts;

	glColor3f(1,1,1);

	glBegin(GL_POINTS);
	for(int z=0;z<kMap->m_iTilesSide;z++)
	{
		for(int x=0;x<kMap->m_iTilesSide;x++)
		{
			int iVertexIndex = z*kMap->m_iTilesSide+x;
			float fScaleX = float(x * kMap->m_fTileSize);
			float fScaleZ = float(z * kMap->m_fTileSize);
			glVertex3f(fScaleX ,pkHmVertex[iVertexIndex].height, fScaleZ);	//*kMap->m_fTileSize
				
		}
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void Render::DrawNormals(HeightMap* kMap,Vector3 CamPos,int iFps)
{
	glDisable(GL_TEXTURE_2D);

	Vector3 kVertex;
	Vector3 kNormal;

	glColor3f(1,1,1);
	glBegin(GL_LINES );
	
	HM_vert* pkHmVertex = kMap->verts;

	for(int z = 0 ; z < kMap->m_iVertexSide; z++){
		for(int x = 0 ; x < kMap->m_iVertexSide ; x++){	
			float	fScaleX = float(x * kMap->m_fTileSize);
			float fScaleZ = float(z * kMap->m_fTileSize);
			int iVertexIndex = z*kMap->m_iVertexSide+x;

			kVertex.Set(fScaleX ,pkHmVertex[iVertexIndex].height, fScaleZ);	//*kMap->m_fTileSize
			kVertex += (kMap->m_kCornerPos);
			glVertex3f(kVertex.x,kVertex.y,kVertex.z);					
			
			kNormal = pkHmVertex[ iVertexIndex ].normal;
			kVertex += kNormal * 1;
			glVertex3f(kVertex.x,kVertex.y,kVertex.z);					
		}
	}


	glEnd();
}

void Render::DrawHMSelected(HeightMap* kmap, vector<HMSelectVertex> kSelected)
{
	for(unsigned int i=0; i<kSelected.size(); i++) {
		kmap->verts[kSelected[i].m_iIndex].height = 3 * kSelected[i].m_fValue;
		}
}



void Render::GetData(HeightMap* kMap, float x, float z, Vector3& kPos, Vector3& kNormal, Vector3& kTex1, Vector3& kTex2 )
{
	int iTestX = kMap->m_iVertexSide;
	int iVertexIndex = int(z*iTestX+x);
	kPos.x = float(x * kMap->m_fTileSize);
	kPos.y = kMap->verts[iVertexIndex].height;  //kMap->m_fTileSize;
	kPos.z = float(z * kMap->m_fTileSize);

	kNormal = kMap->verts[ iVertexIndex ].normal;

	kTex1.Set(x / kMap->m_iVertexSide,z / kMap->m_iVertexSide,0);
	kTex2.Set(x / TEX_SCALE, z/TEX_SCALE,0);
}

void Render::DrawPatch(HeightMap* kMap,Vector3 CamPos,int xp,int zp,int iSize,bool bBorders)
{
	int	iStep;
	float fDistance;
	
	// See if we can Cull away this patch...this is optimized for zeroRTS at the moment..Dvoid  assuming max height of 30
	Vector3 PatchCenter(kMap->m_kCornerPos.x + (xp + iSize/2)*kMap->m_fTileSize,
							  kMap->m_kCornerPos.y + 7.5f,	
							  kMap->m_kCornerPos.z + (zp + iSize/2)*kMap->m_fTileSize);

	fDistance = (CamPos-PatchCenter).Length() ;
	if(fDistance > m_iViewDistance)
		return;
		
/*	if(!m_pkFrustum->CubeInFrustum(PatchCenter.x,PatchCenter.y,PatchCenter.z,
		(iSize/2)*HEIGHTMAP_SCALE,15*HEIGHTMAP_SCALE,(iSize/2)*HEIGHTMAP_SCALE))
		return; */
	
	iStep=PowerOf2(int(fDistance / m_iDetail));
	iStep = 1;

	// Draw the Terrain Patch.
	float fXDivTexScale;
	float fZDivTexScale;
	float fXDivHmSize;
	float fZDivHmSize;
	float fScaleX, fScaleZ;

	int z;
	int iTestX = kMap->m_iVertexSide;

	Vector3 kPos, kNormal, kTex1, kTex2;

	int iTileIndex;

	for(z = zp ; z < zp+iSize; z+=iStep){
		//if(z >= kMap->m_iHmSize-iStep)
		//	break;			
			
		fZDivTexScale	= (float) z / TEX_SCALE;
		fZDivHmSize		= (float) z / iTestX;

		glBegin( GL_TRIANGLES );	
		for(int x = xp ; x < xp+iSize ; x+=iStep){	
			fXDivTexScale	= (float) x / TEX_SCALE;
			fXDivHmSize		= (float) x / iTestX;

			fScaleX = float(x * kMap->m_fTileSize);
			fScaleZ = float(z * kMap->m_fTileSize);

			// Triangle 1
			iTileIndex = z * kMap->m_iTilesSide + x;
			if(kMap->m_pkTileFlags[iTileIndex] & HM_FLAGVISIBLE2) 
			{
				GetData(kMap,float(x),float(z),kPos,kNormal, kTex1,kTex2);
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB, kTex1.x , kTex1.y);		 		 			 		
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB, kTex2.x , kTex2.y);
					glNormal3fv((float*)&kNormal);			
					glVertex3f(kPos.x, kPos.y, kPos.z);					
				
				GetData(kMap,float(x),float(z+1),kPos,kNormal, kTex1,kTex2);
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB, kTex1.x , kTex1.y);		 		 			 		
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB, kTex2.x , kTex2.y);
					glNormal3fv((float*)&kNormal);			
					glVertex3f(kPos.x, kPos.y, kPos.z);					

				GetData(kMap,float(x+1),float(z),kPos,kNormal, kTex1,kTex2);
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB, kTex1.x , kTex1.y);		 		 			 		
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB, kTex2.x , kTex2.y);
					glNormal3fv((float*)&kNormal);			
					glVertex3f(kPos.x, kPos.y, kPos.z);					
			}

			// Triangle 2
			if(kMap->m_pkTileFlags[iTileIndex] & HM_FLAGVISIBLE1) 
			{
				GetData(kMap,float(x+1),float(z),kPos,kNormal, kTex1,kTex2);
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB, kTex1.x , kTex1.y);		 		 			 		
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB, kTex2.x , kTex2.y);
					glNormal3fv((float*)&kNormal);			
					glVertex3f(kPos.x, kPos.y, kPos.z);					

				GetData(kMap,float(x),float(z+1),kPos,kNormal, kTex1,kTex2);
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB, kTex1.x , kTex1.y);		 		 			 		
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB, kTex2.x , kTex2.y);
					glNormal3fv((float*)&kNormal);			
					glVertex3f(kPos.x, kPos.y, kPos.z);					

				GetData(kMap,float(x+1),float(z+1),kPos,kNormal, kTex1,kTex2);
					glMultiTexCoord2fARB(GL_TEXTURE0_ARB, kTex1.x , kTex1.y);		 		 			 		
					glMultiTexCoord2fARB(GL_TEXTURE1_ARB, kTex2.x , kTex2.y);
					glNormal3fv((float*)&kNormal);			
					glVertex3f(kPos.x, kPos.y, kPos.z);					
			}
		}		
		glEnd();		
	}
}
