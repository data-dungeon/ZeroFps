#include "render.h"

void Render::DrawSkyBox(Vector3 CamPos,Vector3 kHead,int iHor,int iTop) {
	float fYpos;
	
	if(kHead.x==0 && kHead.y==0 && kHead.z==0)
		fYpos=90;
	else
		fYpos=0;
	
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


void Render::DrawWater(Vector3 kCamPos,Vector3 kPosition,Vector3 kHead,int iSize,int iStep,int iTexture) {

	float freq=500.0;
	float amp=0.5;
	
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	
	glTranslatef(kPosition.x,kPosition.y,kPosition.z);
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);
	glTranslatef(-iSize/2,0,-iSize/2);
	
	glDisable(GL_CULL_FACE);
//	glDisable(GL_FOG);		
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
		glColor4f(1,1,1,0.5);	
	
		for(int z=0;z<iSize;z+=iStep) {
			float y= float(sin((SDL_GetTicks()/1000.0)+(z/iStep)*freq)*amp);		
		
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,z/(iStep)+tx);		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,.5,z/(iStep)-tx);				
			glVertex3f(x,y,z);
	
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,1,z/(iStep)+tx);		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.5,z/(iStep)-tx);						
			glVertex3f(x+iStep,y,z);
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
		
//	m_pkTexMan->BindTexture("file:../data/textures/water2.bmp",0);
	m_pkTexMan->BindTexture(iTexture);

	float tx=SDL_GetTicks()/60000.0;
	glBegin(GL_QUADS);
		glColor4fv(&kColor[0]);
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
*/			
		
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
	*/
	 
	glColor4f(1,1,1,1);
	glDisable(GL_COLOR_MATERIAL);
//	glPolygonMode(GL_FRONT,GL_FILL);
	glPopMatrix();
}

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
//				step=5;*/

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
	*/
	glPopMatrix();			
}

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
	
	glFogfv(GL_FOG_COLOR,(float*)&kFogColor[0]);
//	glFogf(GL_FOG_DENSITY,FogDensity);
	glFogf(GL_FOG_START,FogStart);
	glFogf(GL_FOG_END,FogStop);	
}



//this funktion calculates the texture cordinat for a subtexture in a 1024x1024 texture
void Render::GiveTexCor(float &iX,float &iY,int iNr) {	
	iX=(iNr%4);	
	iY=(iNr-(iNr%4))/4;

	iX*=0.25;
	iY*=0.25;
	iY=iY;
	
//	cout<<"X: "<<iX<< "  Y: "<<iY<<endl;
}

void Render::DrawCross(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture1) //,int iTexture2) 
{
	glPushMatrix();
	
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);
	glScalef(kScale.x,kScale.y,kScale.z);
	
//	glDisable(GL_COLOR_MATERIAL);		
//	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);	

//	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 1);
	glDisable(GL_CULL_FACE);	
	
	
	glAlphaFunc(GL_GREATER,0.3);
   glEnable(GL_ALPHA_TEST);
	
	
//	Quad(Vector3(0,0,0),Vector3(0,0,0),Vector3(1,1,1),iTexture1);
//	Quad(Vector3(0,0,0),Vector3(0,90,0),Vector3(1,1,1),iTexture1);	
/*	
	m_pkTexMan->BindTexture(iTexture1);  	
	glBegin(GL_TRIANGLES);
	
		glNormal3f(0,1,0);
		glTexCoord2f(.5,-1);glVertex3f(0,1,0); 
		glTexCoord2f(-1,1);glVertex3f(-1,-0.5,0); 
		glTexCoord2f(2,1);glVertex3f(1,-0.5,0); 	
	
		//glNormal3f(0,1,0);
		glTexCoord2f(.5,-1);glVertex3f(0,1,0); 
		glTexCoord2f(-1,1);glVertex3f(0,-0.5,-1); 
		glTexCoord2f(2,1);glVertex3f(0,-0.5,1); 
	
	glEnd();
*/	
	
	
	m_pkTexMan->BindTexture(iTexture1);  	
	glBegin(GL_QUADS);
	
		glNormal3f(0,1,0);
		glTexCoord2f(0,0);glVertex3f(-0.5,0.5,0); 
		glTexCoord2f(0,1);glVertex3f(-0.5,-0.5,0); 
		glTexCoord2f(1,1);glVertex3f(0.5,-0.5,0); 
		glTexCoord2f(1,0);glVertex3f(0.5,0.5,0); 

		//glNormal3f(0,1,0);
		glTexCoord2f(1,0);glVertex3f(0,0.5,-0.5); 
		glTexCoord2f(0,0);glVertex3f(0,0.5,0.5); 
		glTexCoord2f(0,1);glVertex3f(0,-0.5,0.5); 
		glTexCoord2f(1,1);glVertex3f(0,-0.5,-0.5); 

	glEnd();
	
//	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0);	

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glPopMatrix();
}

/*
void Render::DrawGrassPatch(Vector3 kCamPos,Vector3 kPos,Vector3 kScale,int fW,int iNr,HeightMap* kMap,int iTexture,int iFps)
{

	if(m_iAutoLod>0){
		if(SDL_GetTicks()>(m_iGrassLodUpdate+500)){
			m_iGrassLodUpdate=SDL_GetTicks();
			
			if(iFps<(m_iFpsLock-5) && m_iGrassLod>5){
				m_iGrassLod--;	
			} else if(iFps>(m_iFpsLock+5) && m_iGrassLod<50){
				m_iGrassLod++;		
			}
		}
	}
	
//CamPos=Vector3(150,8,100);


//	glPushAttrib();

	if(!m_pkFrustum->CubeInFrustum(kPos.x,kMap->m_kPosition.y+42,kPos.z,fW/2,42,fW/2))
		return;
		
	float fDistance=(kCamPos-Vector3(kPos.x,kCamPos.y,kPos.z)).Length();
//	cout<<"AVSTÅND:"<<fDistance<<endl;
	if(fDistance>fW)
		return;				
	
	if(fDistance>fW/2){
		glColor4f(1,1,1,0.01);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);	
	} else {
		glColor4f(1,1,1,1);
		glDisable(GL_BLEND);
	}




	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	

	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 1);
	glDisable(GL_CULL_FACE);	
	
	GLfloat mat_specular[]={0,0,0,0};	
	GLfloat mat_diffuse[]={.6,.6,.6,.6};	
	GLfloat mat_ambient[]={.8,.8,.8,.8};
	GLfloat mat_shininess[]={0};
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat_diffuse);	
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_ambient);		
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	
	
	glAlphaFunc(GL_GREATER,0.3);
	glEnable(GL_ALPHA_TEST);
	
	m_pkTexMan->BindTexture(iTexture); 	
	srand(1);

	for(int i=0;i<iNr;i++){
		float x= float((rand()%(fW*1000))/1000.0 + (kPos.x-fW/2));
		float z= float((rand()%(fW*1000))/1000.0 + (kPos.z-fW/2));
		float y=kMap->Height(x,z)+kScale.y/2;
	
		if(kMap->GetVert(int(x-kMap->m_kPosition.x),int(z-kMap->m_kPosition.z))->texture!=1)
			continue;
	
		glPushMatrix();	
		
		glTranslatef(x,y,z);					
		glRotatef(rand()%320, 0, 1, 0);					
		glScalef(kScale.x,kScale.y,kScale.z);
		
		glBegin(GL_QUADS);	
		glNormal3f(0,0,1);
		glTexCoord2f(0,0);glVertex3f(-0.5,0.5,0);
		glTexCoord2f(0,1);glVertex3f(-0.5,-0.5,0);
		glTexCoord2f(1,1);glVertex3f(0.5,-0.5,0);
		glTexCoord2f(1,0);glVertex3f(0.5,0.5,0); 
		
		glNormal3f(1,0,0);
		glTexCoord2f(0,0);glVertex3f(0,0.5,0.5);
		glTexCoord2f(0,1);glVertex3f(0,-0.5,0.5);
		glTexCoord2f(1,1);glVertex3f(0,-0.5,-0.5);
		glTexCoord2f(1,0);glVertex3f(0,0.5,-0.5);
		glEnd();
				
		glPopMatrix();
	}
	

	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0);	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);	
//	glPopMatrix();
	glDisable(GL_ALPHA_TEST);	
	glPopMatrix();	
//	glPopAttrib();
};
*/



void Render::DrawHMLodSplat(HeightMap* kMap,Vector3 CamPos,int iFps)
{
	if(m_iAutoLod>0){
		if(SDL_GetTicks()>(m_iLodUpdate+500)){
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
		
	glTranslatef(kMap->m_kPosition.x-kMap->GetSize()/2,kMap->m_kPosition.y,kMap->m_kPosition.z-kMap->GetSize()/2);
	glColor4f(1,1,1,1);

//	if(m_iHmTempList==0)
//		m_iHmTempList = glGenLists(1);
	
	//setup TUs
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);//disable for the first texture
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	
	
	
	//Draw default texture
	m_pkTexMan->BindTexture(kMap->m_kSets[0].m_acTexture,0);
	
//	glNewList(m_iHmTempList,GL_COMPILE);
	DrawAllHM(kMap,CamPos);
//	glEndList();			
//	glCallList(m_iHmTempList);		
		
		
	//set blending
	glDepthFunc(GL_EQUAL);
	glEnable(GL_BLEND);		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);				
		
	
	//enable TU0 again
	glActiveTextureARB(GL_TEXTURE0_ARB);	
	glEnable(GL_TEXTURE_2D);	
		
	
	for(int i=1;i<kMap->m_kSets.size();i++)
	{	
		glActiveTextureARB(GL_TEXTURE0_ARB);	
		m_pkTexMan->BindTexture(kMap->m_kSets[i].m_acMask,0);	
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		m_pkTexMan->BindTexture(kMap->m_kSets[i].m_acTexture,0);		
		
		DrawAllHM(kMap,CamPos);	
//		glCallList(m_iHmTempList);

	}


	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);

	glDisable(GL_BLEND);		

	glPopAttrib();
	glPopMatrix();
}

void Render::DrawAllHM(HeightMap* kMap,Vector3 CamPos)
{
	int iPatchSize=32;

	for(int z=0;z<kMap->m_iHmSize;z+=iPatchSize)
	{
		for(int x=0;x<kMap->m_iHmSize;x+=iPatchSize)
		{
			DrawPatch(kMap,CamPos,x,z,iPatchSize);		
		}
	}
}

void Render::DrawPatch(HeightMap* kMap,Vector3 CamPos,int xp,int zp,int iSize)
{
	int iStep;
	float fDistance;
	
	// See if we can Cull away this patch.
	Vector3 PatchCenter(kMap->m_kPosition.x-(kMap->GetSize()/2) + (xp + iSize/2)*HEIGHTMAP_SCALE,
							  kMap->m_kPosition.y + 34*HEIGHTMAP_SCALE,
							  kMap->m_kPosition.z-(kMap->GetSize()/2) + (zp + iSize/2)*HEIGHTMAP_SCALE);
		
	fDistance=(CamPos-PatchCenter).Length();
		
	if(fDistance > m_iViewDistance)
		return;
	if(!m_pkFrustum->CubeInFrustum(PatchCenter.x,PatchCenter.y,PatchCenter.z,(iSize/2)*HEIGHTMAP_SCALE,54*HEIGHTMAP_SCALE,(iSize/2)*HEIGHTMAP_SCALE))
		return;
		
		
	iStep=PowerOf2(int(fDistance / m_iDetail));
		
//	iStep=32;	
//	cout<<iStep<<endl;
		
	glPushMatrix();			
		glTranslatef(-kMap->m_kPosition.x+kMap->GetSize()/2,-kMap->m_kPosition.y,-kMap->m_kPosition.z+kMap->GetSize()/2);
		m_pkLight->Update(PatchCenter);
	glPopMatrix();

//	glPolygonMode(GL_FRONT,GL_LINE);
	
	// Draw the Terrain Patch.
	float fXDivTexScale;
	float fZDivTexScale;
	float fXDivHmSize;
	float fZDivHmSize;
	int	  iVertexIndex;
	float fScaleX, fScaleZ;

	HM_vert* pkHmVertex = kMap->verts;
	int z,x;
	
	for(z = zp ; z < zp+iSize; z+=iStep){
		if(z >= kMap->m_iHmSize-iStep)
			break;			
			
		fZDivTexScale	= (float) z / TEX_SCALE;
		fZDivHmSize		= (float) z / kMap->m_iHmSize;

		glBegin(GL_TRIANGLE_STRIP);
		for(int x = xp ; x <= xp+iSize ; x+=iStep){	
			fXDivTexScale	= (float) x / TEX_SCALE;
			fXDivHmSize		= (float) x / kMap->m_iHmSize;

			fScaleX = x * HEIGHTMAP_SCALE;
			fScaleZ = z * HEIGHTMAP_SCALE;

			float fZStepDivHmSize = ((float)z+iStep) / kMap->m_iHmSize;
			float fZStepDivTexScale = ((float)z+iStep) / TEX_SCALE;

			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, fXDivHmSize , fZDivHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, fXDivTexScale , fZDivTexScale);
			iVertexIndex = z*kMap->m_iHmSize+x;
			glNormal3fv((float*)&pkHmVertex[ iVertexIndex ].normal);			
			glVertex3f(fScaleX ,pkHmVertex[iVertexIndex].height*HEIGHTMAP_SCALE, fScaleZ);					
			
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, fXDivHmSize, fZStepDivHmSize);		 		 			 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, fXDivTexScale ,fZStepDivTexScale);		
			
			iVertexIndex = (z+iStep)*kMap->m_iHmSize+x;
			glNormal3fv((float*)&pkHmVertex[iVertexIndex].normal);			
			glVertex3f( fScaleX ,pkHmVertex[iVertexIndex].height*HEIGHTMAP_SCALE,(z+iStep)*HEIGHTMAP_SCALE);			
		}		
		glEnd();		
	}

	//damn ulgly lod fix
	 
	z=zp;	
	
	glBegin(GL_TRIANGLE_STRIP);
	for( x=xp;x <=xp+iSize ; x+=iStep)	{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/TEX_SCALE,(float)z/TEX_SCALE);						
		glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
	
//		glVertex3f(x*HEIGHTMAP_SCALE,-100*HEIGHTMAP_SCALE,z*HEIGHTMAP_SCALE);								
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


//	glPolygonMode(GL_FRONT,GL_FILL);
}

/*
//-------------------------Geforce 4 secttion


void Render::G4DrawHMLodSplat(HeightMap* kMap,Vector3 CamPos,int iFps)
{
	if(m_iAutoLod>0){
		if(SDL_GetTicks()>(m_iLodUpdate+500)){
			m_iLodUpdate=SDL_GetTicks();
			
			if(iFps<(m_iFpsLock-5) && m_iDetail>8){
				m_iDetail--;	
			} else if(iFps>(m_iFpsLock+5) && m_iDetail<100){
				m_iDetail++;		
			}
		}
	}
	
	glPushMatrix();
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	
	glTranslatef(kMap->m_kPosition.x,kMap->m_kPosition.y,kMap->m_kPosition.z);
	glColor4f(1,1,1,1);


	//setup TUs
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);//disable for the first texture
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);	

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	
	
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	
	
	//Draw default texture
	glActiveTextureARB(GL_TEXTURE1_ARB);	
	m_pkTexMan->BindTexture(kMap->m_kSets[0].m_acTexture,0);
	G4DrawAllHM(kMap,CamPos);	
	
	
	//set blending
	glDepthFunc(GL_EQUAL);
	glEnable(GL_BLEND);		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);				

	
	//enable TU0 again
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glEnable(GL_TEXTURE_2D);
	
	
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	m_pkTexMan->BindTexture(kMap->m_kSets[1].m_acMask,0);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	m_pkTexMan->BindTexture(kMap->m_kSets[1].m_acTexture,0);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	m_pkTexMan->BindTexture(kMap->m_kSets[2].m_acMask,0);
	glActiveTextureARB(GL_TEXTURE3_ARB);
	m_pkTexMan->BindTexture(kMap->m_kSets[2].m_acTexture,0);
	
	
	
	G4DrawAllHM(kMap,CamPos);	





/*		
	
	for(int i=1;i<kMap->m_kSets.size();i++)
	{		
		glActiveTextureARB(GL_TEXTURE0_ARB);	
		m_pkTexMan->BindTexture(kMap->m_kSets[i].m_acMask,0);	
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		m_pkTexMan->BindTexture(kMap->m_kSets[i].m_acTexture,0);		
		
		DrawAllHM(kMap,CamPos);	
	}
*

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);		

	glPopAttrib();
	glPopMatrix();
}

void Render::G4DrawAllHM(HeightMap* kMap,Vector3 CamPos)
{
	int iPatchSize=64;

	for(int z=0;z<kMap->m_iHmSize;z+=iPatchSize)
	{
		for(int x=0;x<kMap->m_iHmSize;x+=iPatchSize)
		{
			G4DrawPatch(kMap,CamPos,x,z,iPatchSize);		
		}
	}
}

void Render::G4DrawPatch(HeightMap* kMap,Vector3 CamPos,int xp,int zp,int iSize)
{
	int iStep;
	float fDistance;
	
	Vector3 PatchCenter(kMap->m_kPosition.x + xp + iSize/2,
							  kMap->m_kPosition.y+34,
							  kMap->m_kPosition.z + zp + iSize/2);
		
	fDistance=(CamPos-PatchCenter).Length();
		
	if(fDistance > m_iViewDistance)
		return;
		
	//cull
	if(!m_pkFrustum->CubeInFrustum(PatchCenter.x,PatchCenter.y,PatchCenter.z,iSize/2,54,iSize/2))
		return;
		
		
	iStep=PowerOf2(int(fDistance / m_iDetail));
		
	glPushMatrix();			
		glTranslatef(-kMap->m_kPosition.x,-kMap->m_kPosition.y,-kMap->m_kPosition.z);
		m_pkLight->Update(PatchCenter);
	glPopMatrix();
	
	//draw
	for(int z = zp ; z < zp+iSize ; z+=iStep){
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = xp ; x <= xp+iSize ; x+=iStep){	
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/8,(float)z/8);						
			
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,(float)x/8,(float)z/8);						
			
			glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
			glVertex3f(x,kMap->verts[z*kMap->m_iHmSize+x].height,z);					
			
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,((float)z+iStep)/kMap->m_iHmSize);		 		 			 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/8,((float)z+iStep)/8);						
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB,(float)x/kMap->m_iHmSize,((float)z+iStep)/kMap->m_iHmSize);		 		 			 			 		
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB,(float)x/8,((float)z+iStep)/8);						
			
			
			glNormal3fv((float*)&kMap->verts[(z+iStep)*kMap->m_iHmSize+x].normal);			
			glVertex3f(x,kMap->verts[(z+iStep)*kMap->m_iHmSize+x].height,z+iStep);			
		}		
		glEnd();
	}
	

	//damn ulgly lod fix
	
	int z=zp;	
	glBegin(GL_TRIANGLE_STRIP);
	for(int x=xp;x <=xp+iSize ; x+=iStep)
	{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/8,(float)z/8);						
			glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
		
			glVertex3f(x,-100,z);								
			glVertex3f(x,kMap->verts[z*kMap->m_iHmSize+x].height,z);
	}
	glEnd();	
	
	z=zp+iSize;
	glBegin(GL_TRIANGLE_STRIP);	
	for(int x=xp;x <=xp+iSize ; x+=iStep)
	{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/8,(float)z/8);						
			glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
		
			glVertex3f(x,kMap->verts[z*kMap->m_iHmSize+x].height,z);			
			glVertex3f(x,-100,z);								
	}
	glEnd();	

	int x=xp;	
	glBegin(GL_TRIANGLE_STRIP);
	for(int z=zp;z <=zp+iSize ; z+=iStep)
	{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/8,(float)z/8);						
			glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
		
			glVertex3f(x,kMap->verts[z*kMap->m_iHmSize+x].height,z);
			glVertex3f(x,-100,z);									
	}
	glEnd();	
	
	x=xp+iSize;	
	glBegin(GL_TRIANGLE_STRIP);
	for(int z=zp;z <=zp+iSize ; z+=iStep)
	{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,(float)x/kMap->m_iHmSize,(float)z/kMap->m_iHmSize);		 		 			 		
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,(float)x/8,(float)z/8);						
			glNormal3fv((float*)&kMap->verts[z*kMap->m_iHmSize+x].normal);			
		
			glVertex3f(x,-100,z);			
			glVertex3f(x,kMap->verts[z*kMap->m_iHmSize+x].height,z);								
	}
	glEnd();	
}
*/



