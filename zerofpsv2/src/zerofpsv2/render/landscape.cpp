#include "render.h" 
   
#include "../engine_systems/common/heightmap.h" 
#include "../engine_systems/common/heightmap2.h"
#include "../engine_systems/common/i_heightmap2.h"
    
void Render::DrawSkyBox_SixSided(Vector3 CamPos,Vector3 kHead,int* aiSideTextures)
{
	glPushMatrix();
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_LIGHTING_BIT|GL_FOG_BIT);

	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);	//dont want lighting on the skybox		
	glDepthMask(GL_FALSE);	//want the skybox to be faaaaaar away =)

	glTranslatef(CamPos.x,CamPos.y,CamPos.z);
 
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);

	glScalef(100,100,100);

//	glDisable(GL_TEXTURE_2D);

	glDisable(GL_CULL_FACE);

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
		glTexCoord2f(0, 1);		glVertex3fv( &CubeVertex[6].x );		
		glTexCoord2f(0, 0);		glVertex3fv( &CubeVertex[5].x);  
		glTexCoord2f(1, 0);		glVertex3fv( &CubeVertex[1].x);		
		glTexCoord2f(1, 1);		glVertex3fv( &CubeVertex[2].x);		
	glEnd();


	/*	

	m_pkTexMan->BindTexture(aiSideTextures[SKYBOXSIDE_LEFT]);	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);		glVertex3f( -1,	1,		-1);		
		glTexCoord2f(0, 1);		glVertex3f( -1,	-1,		-1);
		glTexCoord2f(1, 1);		glVertex3f( 1 ,	-1,	-1);		
		glTexCoord2f(1, 0);		glVertex3f( 1 ,	1,	-1);		
	glEnd();

	m_pkTexMan->BindTexture(aiSideTextures[SKYBOXSIDE_UP]);	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);		glVertex3f( -1,	1,		-1);		
		glTexCoord2f(0, 1);		glVertex3f( -1,	-1,		-1);
		glTexCoord2f(1, 1);		glVertex3f( 1 ,	-1,	-1);		
		glTexCoord2f(1, 0);		glVertex3f( 1 ,	1,	-1);		
	glEnd();

	m_pkTexMan->BindTexture(aiSideTextures[SKYBOXSIDE_DOWN]);	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);		glVertex3f( -1,	1,		-1);		
		glTexCoord2f(0, 1);		glVertex3f( -1,	-1,		-1);
		glTexCoord2f(1, 1);		glVertex3f( 1 ,	-1,	-1);		
		glTexCoord2f(1, 0);		glVertex3f( 1 ,	1,	-1);		
	glEnd();*/

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);		
	
	glPopAttrib();
	glPopMatrix();

}


void Render::DrawSkyBox(Vector3 CamPos,Vector3 kHead,int iHor,int iTop) {
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

}


void Render::DrawWater(Vector3 kCamPos,Vector3 kPosition,Vector3 kHead,int iSize,int iStep,int iTexture, float fBlendValue) {

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
}

void Render::DrawSimpleWater(Vector3 kPosition,Vector4 kColor,int iSize,int iTexture) {
	//recomended color  .3,.3,.4,.99;
	
	glPushMatrix();
	
	glDisable(GL_FOG);	
	glDisable(GL_CULL_FACE);	
	glDisable(GL_LIGHTING);
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


	if(m_FogEnable)//Disable the fog while drawing the sky box
		glEnable(GL_FOG);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);		
	glPopMatrix();	
}

/* COMMENT OUT BY DVOID
void Render::DrawHMlod(HeightMap* kmap,Vector3 CamPos,int iFps){

//	glEnable(GL_CULL_FACE);
	
	if(m_iAutoLod>0){
		if(SDL_GetTicks()>(m_iLodUpdate+500)){
			m_iLodUpdate=SDL_GetTicks();
			
			//dont update lod if the camera has't moved
//			if(CamPos!=m_kOldCamPos){
//				m_kOldCamPos=CamPos;
				if(iFps<(m_iFpsLock-5) && m_iDetail>20){
					m_iDetail--;	
				} else if(iFps>(m_iFpsLock+5) && m_iDetail<100){
					m_iDetail++;		
				}
//			}
		}
	}
	
	glPushMatrix();
	
	//translate to map position
	glTranslatef(kmap->m_kPosition.x,kmap->m_kPosition.y,kmap->m_kPosition.z);

	m_pkTexMan->BindTexture(kmap->m_acTileSet,0);
	
	
	GLfloat mat_specular[]={0,0,0,0};
	GLfloat mat_diffuse[]={1,1,1,1};	
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);	
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	

//	glColorMaterial(GL_FRONT,GL_DIFFUSE);
//	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);	
//	float black[4]={0.1,0.1,0.1,0.1};
	float black[4]={0.0,0.0,0.0,0.0};
	glMaterialfv(GL_FRONT,GL_AMBIENT,black);
	glMaterialfv(GL_FRONT,GL_SPECULAR,black);
	glEnable(GL_COLOR_MATERIAL);		
	
	Vector3 p1,p2;
	
	//calculate number slices depending on the size of the lod tiles size
	int slices=(kmap->m_iHmSize)/m_iSlicesize;
	int step=1;
	
	for(int sz=0;sz<slices;sz++) {
		for(int sx=0;sx<slices;sx++) {
			Vector3 SliceCenter(kmap->m_kPosition.x+sx*m_iSlicesize+m_iSlicesize/2,
									  kmap->m_kPosition.y+34,
									  kmap->m_kPosition.z+sz*m_iSlicesize+m_iSlicesize/2);
		
			if(!m_pkFrustum->CubeInFrustum(SliceCenter.x,SliceCenter.y,SliceCenter.z,m_iSlicesize/2,54,m_iSlicesize/2))
				continue;
		
			//recalculate lights for this land slice
			glPushMatrix();			
				glTranslatef(-kmap->m_kPosition.x,-kmap->m_kPosition.y,-kmap->m_kPosition.z);
				m_pkLight->Update(SliceCenter);
			glPopMatrix();
/*			if(!m_pkFrustum->CubeInFrustum(kmap->m_kPosition.x+sx*m_iSlicesize+m_iSlicesize/2,
													kmap->m_kPosition.y+34,
													kmap->m_kPosition.z+sz*m_iSlicesize+m_iSlicesize/2,m_iSlicesize/2,54,m_iSlicesize/2))
				continue;
			
		
			//set lop steps depending on the distance to the center of the lod tile
			step=int((CamPos-Vector3(kmap->m_kPosition.x+sx*m_iSlicesize+m_iSlicesize/2,
     										 CamPos.y,
										    kmap->m_kPosition.z+sz*m_iSlicesize+m_iSlicesize/2)).Length());																
//	cout<<"STEP"<<step<<endl;
			if(step>m_iViewDistance)
				continue;
				
			step/=m_iDetail;
			
			if(step<1)//step cant be lower than 1
				step=1;				
			if(step>6)//if the step get to high it will look realy bad
				step=6;			
			
			bool flip=false;	//texture fliper
			float t=0;
			float s=0;			 //texture cordinats
			float nt,ns;
			
	
			//start going trouh all vertexes in the slice
			for(int z = sz*m_iSlicesize; z <= sz*m_iSlicesize+m_iSlicesize; z+=step){
				
//				if( z+step > sz*m_iSlicesize+m_iSlicesize )
//					z=(sz*m_iSlicesize+m_iSlicesize-step);
				
				
				glBegin(GL_TRIANGLE_STRIP);						
				for(int x = sx*m_iSlicesize; x <= sx*m_iSlicesize+m_iSlicesize+step; x+=step){
					
//					if( x+step > sx*m_iSlicesize+m_iSlicesize )
//						x=(sx*m_iSlicesize+m_iSlicesize-step);
//						continue;
//
					
					
					//fulhack ultradeluxe to make sure that we dont go outside the grid
					if(x>=kmap->m_iHmSize)
						break;						
					if(z>=kmap->m_iHmSize-1)
						break;
					
					//flip texture
					if(flip==false)
						flip=true;
					else
						flip=false;
					
					if(flip){//change texturecordinats only on flip
				 						 		
				 		//Get New texture cordinats
						GiveTexCor(nt,ns,kmap->verts[z*kmap->m_iHmSize+x].texture);//caculate texture cordinats						
					
						//if the cordinats is the same as last time
						if(nt==t && ns==s){
							//dont do anything =)
						}else  {
							//else put a new polygon with old texture cordinat

//							cout<<"changing texture "<<times++<<endl;
							
							//draw the the new polygons with old cordinat
							p1=Vector3(x,kmap->verts[z*kmap->m_iHmSize+x].height,z);									
							glNormal3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x].normal);						
							glColor3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x].color);
							glTexCoord2f(t+0.01,s+0.01);						
					 		glVertex3fv((float*)&p1);
				 			
							p2=Vector3(x,kmap->verts[(z+step)*kmap->m_iHmSize+x].height,z+step);			 		
						 	glNormal3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x].normal);				 		
							glColor3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x].color);
					 		glTexCoord2f(t+0.01,s+0.24);				
							glVertex3fv((float*)&p2); 
							
							t=nt;s=ns;//set the new cordinats
						}
					}
					
					//vertex down
					p1=Vector3(x,kmap->verts[z*kmap->m_iHmSize+x].height,z);				
					glNormal3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x].normal);
					glColor3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x].color);
					
					if(flip)						
				 		glTexCoord2f(t+0.01,s+0.01);
 					else
 						glTexCoord2f(t+0.24,s+0.01);
 						
			 		glVertex3fv((float*)&p1);//set vertex
 		
 					//vertex up
					p2=Vector3(x,kmap->verts[(z+step)*kmap->m_iHmSize+x].height,z+step);			 		
			 	  glNormal3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x].normal);
					glColor3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x].color);
					
					if(flip)	
				 		glTexCoord2f(t+0.01,s+0.24);
 					else
 						glTexCoord2f(t+0.24,s+0.24);
									   
					glVertex3fv((float*)&p2); //set vertex
			 		
				}					
				glEnd();
			}
		}
	}
	
		//this draw the normals of the heightmap
	/*
	glPolygonMode(GL_FRONT,GL_LINE);
	SetColor(Vector3(255,0,0));
	for(int z=0;z<kmap->m_iHmSize;z++)
		for(int x=0;x<kmap->m_iHmSize;x++) {
			p1=Vector3(x,kmap->verts[z*kmap->m_iHmSize+x].height,z);
 			Line(p1,p1+kmap->verts[z*kmap->m_iHmSize+x].normal);
	
		}
	
	 
	glColor4f(1,1,1,1);
	glDisable(GL_COLOR_MATERIAL);
//	glPolygonMode(GL_FRONT,GL_FILL);
	glPopMatrix();
}
*/

/*
void Render::DrawHM(HeightMap *kmap) {
	glPushMatrix();
	Vector3 p1;
	Vector3 p2;
	Vector3 p3;
	Vector3 p4;
	
	glTranslatef(-50,-10,-50);
	m_pkTexMan->BindTexture(kmap->m_acTileSet,0);
	
	GLfloat mat_specular[]={1,1,1,1};
	GLfloat mat_shininess[]={10};
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	SetColor(Vector3(0,0,0));

//	glPolygonMode(GL_FRONT,GL_LINE);

	int step=1;	
	int x;	
	for(int z=0;z<kmap->m_iHmSize-step;z+=step){

		glBegin(GL_TRIANGLE_STRIP);		
		
		x=0;				
		p1=Vector3(x,kmap->verts[z*kmap->m_iHmSize+x].height,z);				
	  glNormal3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x].normal);
 		glTexCoord2f(0.0,0.0);glVertex3fv((float*)&p1);
 		
		p2=Vector3(x,kmap->verts[(z+step)*kmap->m_iHmSize+x].height,z+step);			 		
 	   glNormal3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x].normal);
 		glTexCoord2f(0.0,1);glVertex3fv((float*)&p2);		 				
 		
// 		cout<<"STEP:"<<step<<endl;
 		
		for(x=1;x<kmap->m_iHmSize-step;x+=step) {
	
/*			step=int(sqrt(pow(CamPos.x-x,2)+pow(CamPos.y,2)+pow(CamPos.z-z,2)))/20;
			if(step<1)
				step=1;
//			if(step>5)				
//				step=5;

			p3=Vector3(x+step,kmap->verts[z*kmap->m_iHmSize+x+step].height,z);							
			glNormal3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x+step].normal);
  			glTexCoord2f(x/step,0.0);glVertex3fv((float*)&p3);    			
  			
			p4=Vector3(x+step,kmap->verts[(z+step)*kmap->m_iHmSize+x+step].height,z+step);
			glNormal3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x+step].normal);  			
  			glTexCoord2f(x/step,1);glVertex3fv((float*)&p4);    				
	
		}		
		glEnd();

	}
//	glPolygonMode(GL_FRONT,GL_FILL);			
//	glFrontFace(GL_CCW);
	
	//this draw the normals of the heightmap
	/*
	SetColor(Vector3(255,0,0));
	for(int z=0;z<kmap->m_iHmSize;z++)
		for(int x=0;x<kmap->m_iHmSize;x++) {
			p1=Vector3(x,kmap->verts[z*kmap->m_iHmSize+x].height,z);
 			Line(p1,p1+kmap->verts[z*kmap->m_iHmSize+x].normal);
	
		}
	*
	glPopMatrix();			
}
*/

//void Render::SetFog(Vector4 kFogColor,float FogDensity,float FogStart,float FogStop,bool FogEnabled){


void Render::SetFog(Vector4 kFogColor,float FogStart,float FogStop,bool FogEnabled){
	if(FogEnabled){
		glEnable(GL_FOG);
		m_FogEnable=true;
	} else {
		glDisable(GL_FOG);
		m_FogEnable=false;
	}	
	
	glFogi(GL_FOG_MODE,GL_LINEAR);
	glFogi(FOG_DISTANCE_MODE_NV,EYE_RADIAL_NV);
	glHint(GL_FOG_HINT,GL_NICEST);	
	
	glFogfv(GL_FOG_COLOR,(float*)&kFogColor);
//	glFogf(GL_FOG_DENSITY,FogDensity);
	glFogf(GL_FOG_START,FogStart);
	glFogf(GL_FOG_END,FogStop);	
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



void Render::DrawCross(Vector3& kPos,Vector3& kHead,Vector3& kScale,int& iTexture1) //,int iTexture2) 
{
	static Vector3 pointdata[8] = { Vector3(-0.5,0.5,0),
												Vector3(-0.5,-0.5,0),
												Vector3(0.5,-0.5,0),
												Vector3(0.5,0.5,0),
										
												Vector3(0,0.5,-0.5),
												Vector3(0,0.5,0.5),
												Vector3(0,-0.5,0.5),
												Vector3(0,-0.5,-0.5)};

/*	static Vector3 normaldata[8] = { Vector3(0,1,0),
												Vector3(0,1,0),
												Vector3(0,1,0),
												Vector3(0,1,0),
												
												Vector3(0,1,0),
												Vector3(0,1,0),
												Vector3(0,1,0),
												Vector3(0,1,0),};
*/

	static Vector2 texdata[8] = { Vector2(0,1),
											Vector2(0,0),
											Vector2(1,0),
											Vector2(1,1),
											
											Vector2(1,1),
											Vector2(0,1),
											Vector2(0,0),
											Vector2(1,0)};

	glPushMatrix();
	
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);
	glScalef(kScale.x,kScale.y,kScale.z);
	
	
	glDisable(GL_CULL_FACE);			
	glAlphaFunc(GL_GREATER,0.3);
   glEnable(GL_ALPHA_TEST);

	m_pkTexMan->BindTexture(iTexture1);  	
	
/*	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,texdata);						

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,pointdata);					
	
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glVertexPointer(3,GL_FLOAT,0,normaldata);						
	
	
	glNormal3f(0,1,0);
	
	glDrawArrays(GL_QUADS,0,8);
*/	
	glBegin(GL_QUADS);
		glNormal3f(0,1,0);
		glTexCoord2f(0,1);glVertex3f(-0.5,1,0); 
		glTexCoord2f(0,0);glVertex3f(-0.5,0,0); 
		glTexCoord2f(1,0);glVertex3f(0.5,0,0); 
		glTexCoord2f(1,1);glVertex3f(0.5,1,0); 
		
		glTexCoord2f(1,1);glVertex3f(0,1,-0.5); 
		glTexCoord2f(0,1);glVertex3f(0,1,0.5); 
		glTexCoord2f(0,0);glVertex3f(0,0,0.5); 
		glTexCoord2f(1,0);glVertex3f(0,0,-0.5); 
	glEnd();
		
	
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glPopMatrix();
}


void Render::DrawHMLodSplat(HeightMap* kMap,Vector3 CamPos,int iFps)
{
	if(kMap->m_iNumOfHMVertex == 0)
		return;

	if(!m_iDrawLandscape)
		return;

  if(m_iAutoLod>0){
		if((int)SDL_GetTicks()>(m_iLodUpdate+500)){
			m_iLodUpdate=SDL_GetTicks();
			
			if(iFps<(m_iFpsLock-5) && m_iDetail>20){
				m_iDetail--;	
			} else if(iFps>(m_iFpsLock+5) && m_iDetail<100){
				m_iDetail++;		
			}
		}
	}
	
	glPushMatrix();
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_COLOR_MATERIAL);

	int iLightMode = m_pkZShader->GetForceLighting();
	if(iLightMode == LIGHT_ALWAYS_OFF)
		glDisable(GL_LIGHTING);
	else 
		glEnable(GL_LIGHTING);

	glTranslatef(kMap->m_kCornerPos.x,kMap->m_kCornerPos.y,kMap->m_kCornerPos.z);
	glColor4f(1,1,1,1);
	if(kMap->m_bInverted) glCullFace(GL_FRONT);

//	DrawHMVertex(kMap);
//	glPopAttrib();
//	glPopMatrix();
//	return;

//	DrawBlocks(kMap);
//	if(m_iHmTempList==0)
//		m_iHmTempList = glGenLists(1);
	

//	m_eLandscapePolygonMode = LINE;

	//set polygon mode
	switch(m_eLandscapePolygonMode)
	{
		case LINE:
			glPolygonMode(GL_FRONT, GL_LINE);
			break;
		case FILL:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
	}
	
	
	//setup TUs
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);//disable for the first texture
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE  );	// GL_MODULATE	 GL_REPLACE

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE );	
	
	
	//Draw default texture	
/*	
	RES:
	ZFResourceHandle m_kConsoleText;
	m_kConsoleText.SetRes(kMap->m_kSets[0].m_acTexture);		
	ResTexture* pkTexture = static_cast<ResTexture*>(m_kConsoleText.GetResourcePtr());
	
	m_pkTexMan->BindTexture(pkTexture->m_iTextureID);*/

	m_pkTexMan->BindTexture(kMap->m_kLayer[0].m_strTexture.c_str(),0);
//	m_pkTexMan->AddMipMapLevel(0,kMap->m_kLayer[0].m_strTexture.c_str());	
	
//	m_pkTexMan->BindTexture(kMap->m_kSets[0].m_acTexture,0);
//	m_pkTexMan->AddMipMapLevel(0,kMap->m_kSets[0].m_acDetailTexture);	
		
	DrawAllHM(kMap,CamPos,true);
		
	//set blending
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
		m_pkTexMan->BindTexture(kMap->m_kLayer[i].m_strMask.c_str(),T_NOMIPMAPPING|T_ALPHA);	
		
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		m_pkTexMan->BindTexture(kMap->m_kLayer[i].m_strTexture.c_str(),0);		
//		m_pkTexMan->AddMipMapLevel(0,kMap->m_kLayer[i].m_strTexture.c_str());					
		
		DrawAllHM(kMap,CamPos,true);	
	}


		
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);

	glDisable(GL_BLEND);	

	if(kMap->m_bInverted) glCullFace(GL_BACK);

	glPopAttrib();
	glPopMatrix();
}

void Render::DrawAllHM(HeightMap* kMap,Vector3 CamPos,bool bBorders)
{
	int iPatchSize = 4;
	
	//glDisable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	//glDisable(GL_LIGHTING);

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

	//glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING );
	glColor3f(1,1,1);

	glBegin(GL_POINTS);
	for(int z=0;z<kMap->m_iTilesSide;z++)
	{
		for(int x=0;x<kMap->m_iTilesSide;x++)
		{
			int iVertexIndex = z*kMap->m_iTilesSide+x;
			float fScaleX = float(x * HEIGHTMAP_SCALE);
			float fScaleZ = float(z * HEIGHTMAP_SCALE);
			glVertex3f(fScaleX ,pkHmVertex[iVertexIndex].height*HEIGHTMAP_SCALE, fScaleZ);
				
		}
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void Render::DrawNormals(HeightMap* kMap,Vector3 CamPos,int iFps)
{
	glDisable(GL_TEXTURE_2D);
//	glDisable(GL_LIGHTING );

	Vector3 kVertex;
	Vector3 kNormal;

	glColor3f(1,1,1);
	glBegin(GL_LINES );
	
	HM_vert* pkHmVertex = kMap->verts;

	for(int z = 0 ; z < kMap->m_iVertexSide; z++){
		for(int x = 0 ; x < kMap->m_iVertexSide ; x++){	
			float	fScaleX = float(x * HEIGHTMAP_SCALE);
			float fScaleZ = float(z * HEIGHTMAP_SCALE);
			int iVertexIndex = z*kMap->m_iVertexSide+x;

			kVertex.Set(fScaleX ,pkHmVertex[iVertexIndex].height*HEIGHTMAP_SCALE, fScaleZ);
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
	for(int i=0; i<kSelected.size(); i++) {
		kmap->verts[kSelected[i].m_iIndex].height = 3 * kSelected[i].m_fValue;
		}
}

// Returns Min/Max hojd i vald patch
void Render::GetMinMax(HeightMap* kMap, float& fMin, float& fMax, int xp,int zp,int iSize)
{
	fMin = 100000;
	fMax = -100000;
	HM_vert* pkHmVertex = kMap->verts;
	float fHojd;
	
	//	fMin = 
	for(int z = zp ; z < zp+iSize; z++){
		for(int x = xp ; x <= xp+iSize ; x++){	
			int iVertexIndex = (z)*kMap->m_iTilesSide+x;
			fHojd = float(pkHmVertex[iVertexIndex].height*HEIGHTMAP_SCALE);
			
			if(fHojd < fMin)
				fMin = fHojd;
			if(fHojd > fMax)
				fMax = fHojd;
		}
	}
}

void Render::DrawPatch(HeightMap* kMap,Vector3 CamPos,int xp,int zp,int iSize,bool bBorders)
{
	int	iStep;
	float fDistance;
	
	// See if we can Cull away this patch...this is optimized for zeroRTS at the moment..Dvoid  assuming max height of 30
	Vector3 PatchCenter(kMap->m_kCornerPos.x + (xp + iSize/2)*HEIGHTMAP_SCALE,
							  kMap->m_kCornerPos.y + 7.5f*HEIGHTMAP_SCALE,
							  kMap->m_kCornerPos.z + (zp + iSize/2)*HEIGHTMAP_SCALE);
		
	fDistance = (CamPos-PatchCenter).Length() ;
	if(fDistance > m_iViewDistance)
		return;
//	glDisable(GL_TEXTURE_2D);
		
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
	int	iVertexIndex;
	float fScaleX, fScaleZ;

	HM_vert* pkHmVertex = kMap->verts;
	int z;
	//int iTestX = kMap->m_iTilesSide;
	int iTestX = kMap->m_iVertexSide;

//	this->DumpGLState("horatyp.txt");

	for(z = zp ; z < zp+iSize; z+=iStep){
		//if(z >= kMap->m_iHmSize-iStep)
		//	break;			
			
		fZDivTexScale	= (float) z / TEX_SCALE;
		fZDivHmSize		= (float) z / iTestX;

		glBegin(GL_TRIANGLE_STRIP);
		for(int x = xp ; x <= xp+iSize ; x+=iStep){	
			fXDivTexScale	= (float) x / TEX_SCALE;
			fXDivHmSize		= (float) x / iTestX;

			fScaleX = float(x * HEIGHTMAP_SCALE);
			fScaleZ = float(z * HEIGHTMAP_SCALE);

			float fZStepDivHmSize = ((float)z+iStep) / iTestX;
			float fZStepDivTexScale = ((float)z+iStep) / TEX_SCALE;

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, fXDivHmSize , fZDivHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, fXDivTexScale , fZDivTexScale);
			iVertexIndex = z*iTestX +x;
			glNormal3fv((float*)&pkHmVertex[ iVertexIndex ].normal);			
			glVertex3f(fScaleX ,pkHmVertex[iVertexIndex].height*HEIGHTMAP_SCALE, fScaleZ);					
			
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, fXDivHmSize, fZStepDivHmSize);		 		 			 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, fXDivTexScale ,fZStepDivTexScale);		
			
			iVertexIndex = (z+iStep)*iTestX + x;
			glNormal3fv((float*)&pkHmVertex[iVertexIndex].normal);			
			glVertex3f( fScaleX ,float(pkHmVertex[iVertexIndex].height*HEIGHTMAP_SCALE) , float((z+iStep)*HEIGHTMAP_SCALE));			
		}		
		glEnd();		
	}

	//damn ulgly lod fix
/*	if(bBorders)
	{
		z=zp;	
	
		glBegin(GL_TRIANGLE_STRIP);
		for( x=xp;x <=xp+iSize ; x+=iStep)	{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/TEX_SCALE,(float)z/TEX_SCALE);						
			glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
	
//			glVertex3f(x*HEIGHTMAP_SCALE,-100*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);								
			glVertex3f(x*HEIGHTMAP_SCALE,(pkHmVertex[z*kMap->m_iHmSize+x].height - 10 )*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);								
			glVertex3f(x*HEIGHTMAP_SCALE,(pkHmVertex[z*kMap->m_iHmSize+x].height)*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);
		}
		glEnd();	
	
		z=zp+iSize;
	
		if(z==kMap->m_iHmSize-iStep)
			z-=iStep;	
	
		glBegin(GL_TRIANGLE_STRIP);	
		for(x=xp;x <=xp+iSize ; x+=iStep)
		{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/TEX_SCALE,(float)z/TEX_SCALE);						
			glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
	
			glVertex3f(x*HEIGHTMAP_SCALE,(pkHmVertex[z*kMap->m_iHmSize+x].height)*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);
			glVertex3f(x*HEIGHTMAP_SCALE,(pkHmVertex[z*kMap->m_iHmSize+x].height - 10 )*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);											
		}
		glEnd();	

		x=xp;	
		glBegin(GL_TRIANGLE_STRIP);
		for(z=zp;z <=zp+iSize ; z+=iStep)
		{
			if(z==kMap->m_iHmSize-iStep)
				break;
		
		
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/TEX_SCALE,(float)z/TEX_SCALE);						
			glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
		
			glVertex3f(x*HEIGHTMAP_SCALE,kMap->verts[z*kMap->m_iHmSize+x].height*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);
			glVertex3f(x*HEIGHTMAP_SCALE,(pkHmVertex[z*kMap->m_iHmSize+x].height - 10 )*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);										
		}
		glEnd();	
	
		x=xp+iSize;	
		glBegin(GL_TRIANGLE_STRIP);
		for( z=zp;z <=zp+iSize ; z+=iStep)
		{
			if(z==kMap->m_iHmSize-iStep)
				break;
		
		
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/TEX_SCALE,(float)z/TEX_SCALE);						
			glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
		
			glVertex3f(x*HEIGHTMAP_SCALE,(pkHmVertex[z*kMap->m_iHmSize+x].height - 10 )*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);			
			glVertex3f(x*HEIGHTMAP_SCALE,(pkHmVertex[z*kMap->m_iHmSize+x].height)*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);					
		}
		glEnd();	
	};*/


}

int iPatchIndex[4096];

/*
void Render::DrawBlocks(HeightMap* kmap)
{
	for(unsigned int i=0; i<kmap->m_kTerrainBlocks.size(); i++) {
		DrawAABB(kmap->m_kTerrainBlocks[i].kAABB_Min,kmap->m_kTerrainBlocks[i].kAABB_Max, Vector3(0,1,0));

		}
}


void Render::DrawPatch_Vim1(HeightMap* kMap,Vector3 CamPos,int xp,int zp,int iSize)
{
	Vector3* pkLandVertex;
	Vector3* pkLandNormals;
	Vector3* pkLandTextureCoo1;
	Vector3* pkLandTextureCoo2;

//	I_HeightMap2* pkIMap = dynamic_cast<I_HeightMap2*>(kMap);

	// Get Patch Center
	Vector3 PatchCenter(kMap->m_kCornerPos.x + (xp + iSize/2)*HEIGHTMAP_SCALE,
							  kMap->m_kCornerPos.y + 34*HEIGHTMAP_SCALE,
							  kMap->m_kCornerPos.z + (zp + iSize/2)*HEIGHTMAP_SCALE);
/*	Vector3 PatchCenter(kMap->GetPos().x-(kMap->GetScale()/2) + (xp + iSize/2)*HEIGHTMAP_SCALE,
							  kMap->GetPos().y + 34*HEIGHTMAP_SCALE,
							  kMap->GetPos().z-(kMap->GetScale()/2) + (zp + iSize/2)*HEIGHTMAP_SCALE);


	//DrawCross(PatchCenter,Vector3::ZERO, Vector3(1,1,1),-1);
	return;

	// Distance Cull Patch
	float fDistance = (CamPos - PatchCenter).Length();
	if(fDistance > m_iViewDistance)
		return;

	// Cull AABB
/*	if(!m_pkFrustum->CubeInFrustum(PatchCenter.x,PatchCenter.y,PatchCenter.z,
		(iSize/2)*HEIGHTMAP_SCALE,54*HEIGHTMAP_SCALE,(iSize/2)*HEIGHTMAP_SCALE))
		return;
		
	// Min/Max	AABB
	float fMin,fMax;
	GetMinMax(kMap,fMin,fMax,xp,zp,iSize);
		
	// Get StepSize.
	int iStep;
	iStep=PowerOf2(int(fDistance / m_iDetail));
		
	// Set Lightning.
	glPushMatrix();			
		glTranslatef(-kMap->m_kPosition.x+kMap->m_iHmScaleSize/2,-kMap->m_kPosition.y,-kMap->m_kPosition.z+kMap->m_iHmScaleSize/2);
		m_pkLight->Update(PatchCenter);
		//DrawAABB(PatchCenter.x,PatchCenter.y,PatchCenter.z,
		//	(iSize/2)*HEIGHTMAP_SCALE,54*HEIGHTMAP_SCALE,(iSize/2)*HEIGHTMAP_SCALE, fMin,fMax, Vector3(1,1,1));
	glPopMatrix();

	// Alloc Mem.

	int iNumOfPatchVertex = iSize * iSize;
	pkLandVertex		= new Vector3 [iNumOfPatchVertex];		// Create Vertex
	pkLandNormals		= new Vector3 [iNumOfPatchVertex];		// Create Normals
	// Create TexCoo
	pkLandTextureCoo1 = new Vector3 [iNumOfPatchVertex];				
	pkLandTextureCoo2 = new Vector3 [iNumOfPatchVertex];


	
	// Draw the Terrain Patch.
//	float fXDivTexScale;
//	float fZDivTexScale;
//	float fXDivHmSize;
//	float fZDivHmSize;
	int	  iVertexIndex;
	float fScaleX, fScaleZ;

	HM_vert* pkHmVertex = kMap->verts;
	int z,x;

//	fZDivTexScale	= (float) z / TEX_SCALE;
//	fZDivHmSize		= (float) z / kMap->m_iHmSize;

		//	fMin = 
	int iPatchIndex = 0;
	for( z = zp ; z < zp+iSize; z++){
		for( x = xp ; x < xp+iSize ; x++){	
			int iVertexIndex = (z)*kMap->m_iTilesSide+x;
			fScaleX = float(x * HEIGHTMAP_SCALE);
			fScaleZ = float(z * HEIGHTMAP_SCALE);
			
			pkLandVertex[ iPatchIndex ]	= Vector3(fScaleX, float(pkHmVertex[iVertexIndex].height*HEIGHTMAP_SCALE) ,fScaleZ);
			//pkLandNormals[ iPatchIndex ]	= pkHmVertex[ iVertexIndex ].normal;
			iPatchIndex++;
		}
	}

	cout << "iNumOfPatchVertex: "<< iNumOfPatchVertex << endl;
	cout << "iPatchIndex: "<< iPatchIndex << endl;

	// Create Elements List
	glDisable(GL_TEXTURE_2D);
//	glDisable(GL_LIGHTING );

	for( z = 0 ; z < iSize; z++){
		glBegin(GL_TRIANGLE_STRIP);
		for( x = 0 ; x < iSize ; x++){	
			iVertexIndex = z * iSize + x;
			//glNormal3fv((float*)&pkLandNormals[iVertexIndex]);			
			glVertex3fv((float*)&pkLandVertex[iVertexIndex]);					
			iVertexIndex = (z+iStep) * iSize + x;
			//glNormal3fv((float*)&pkLandNormals[iVertexIndex]);			
			glVertex3fv((float*)&pkLandVertex[iVertexIndex]);					
		}
		glEnd();		
	} 
	
	glEnable(GL_TEXTURE_2D);
//	glEnable(GL_LIGHTING );


	// Draw Layers
/*	glColor3f(1,1,1);
	glPointSize(15);
	glBegin(GL_POINTS);
		for(int i=0; i<iNumOfPatchVertex; i++)
			glVertex3f(pkLandVertex[i].x, pkLandVertex[i].y, pkLandVertex[i].z);
	glEnd();
	glPointSize(1);

	// Fill Holes.

	delete [] pkLandVertex;
	delete [] pkLandNormals;
	delete [] pkLandTextureCoo1;
	delete [] pkLandTextureCoo2;
}*/



void Render::DrawHM2(Heightmap2* pkMap,Vector3 kCamPos)
{
	I_HeightMap2* pkIMap = dynamic_cast<I_HeightMap2*>(pkMap);



	if(!pkIMap->Loaded()) 
		return;
					
	pkIMap->UpdateRecLodLevel(kCamPos); 

	glPushMatrix();

	Vector3 kPos = pkMap->GetPos() - 
		Vector3(float(pkMap->m_iWidth*pkMap->m_fScale / 2.0),
		0,
		float(pkMap->m_iHeight*pkMap->m_fScale / 2.0));
	glTranslatef(kPos.x,kPos.y,kPos.z);


	
	int px =  pkMap->m_iWidth / pkMap->m_iPatchWidth;
	int py =  pkMap->m_iHeight / pkMap->m_iPatchHeight;
	
	vector<HM2_patch>* m_kRenderData = &pkMap->m_kRenderData;
		
		

	ZMaterial* test = (ZMaterial*)pkMap->GetMaterial()->GetResourcePtr();
	
	//cout<<"hora loading material"<<endl;
	//ZMaterial* test = (ZMaterial*)m_pkMap2->GetMaterial()->GetResourcePtr();		
	/*ResTexture*  bla = (ResTexture*)test->GetPass(0)->m_kTUs[0].GetResourcePtr();
	if(bla)
		cout<<"tu0: "<<bla->strTextureName<<endl;		
*/
	
	m_pkZShader->Reset();
	m_pkZShader->SetMaterial(test);
	m_pkZShader->SetDrawMode(TRIANGLESTRIP_MODE);
	
	for(int y = 0;y<py;y++)
	{			
		for(int x = 0;x<px;x++)
		{
			HM2_patch* pkPatch = &(*m_kRenderData)[ (y*px) + x];
		
			int iLevel = pkPatch->iReLev;
			if(iLevel < 0)
				continue;
		
			HM2_level* pkLevel = &(pkPatch->kLevels[iLevel]);
		
			m_pkZShader->SetPointer(VERTEX_POINTER,&pkLevel->kVertex[0]);
			m_pkZShader->SetPointer(NORMAL_POINTER,&pkLevel->kNormal[0]);
			m_pkZShader->SetPointer(TEXTURE_POINTER0,&pkLevel->kMaskTexCor[0]);						
			m_pkZShader->SetPointer(TEXTURE_POINTER1,&pkLevel->kTexCor[0]);

			
			m_pkZShader->SetNrOfVertexs(pkLevel->kVertex.size());
			
			m_pkZShader->Draw();
			
		
		}
	}
	
	glPopMatrix();
}
