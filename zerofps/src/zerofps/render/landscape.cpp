#include "render.h"

void Render::DrawSkyBox(Vector3 CamPos) {
	glPushMatrix();
	glDepthMask(GL_FALSE);
	glTranslatef(CamPos.x,CamPos.y,CamPos.z);
	
	int iSize=801;
	
	glDisable(GL_LIGHTING);
	Quad(Vector3(0,0,-iSize/2),Vector3(0,0,0),Vector3(iSize,iSize,iSize),m_pkTexMan->Load("file:../data/textures/front.bmp"));
	Quad(Vector3(0,iSize/2,0),Vector3(90,0,0),Vector3(iSize,iSize,iSize),m_pkTexMan->Load("file:../data/textures/top.bmp"));
	Quad(Vector3(0,0,iSize/2),Vector3(180,0,180),Vector3(iSize,iSize,iSize),m_pkTexMan->Load("file:../data/textures/back.bmp"));
	
	Quad(Vector3(iSize/2,0,0),Vector3(0,-90,0),Vector3(iSize,iSize,iSize),m_pkTexMan->Load("file:../data/textures/left.bmp"));
	Quad(Vector3(-iSize/2,0,0),Vector3(0,90,0),Vector3(iSize,iSize,iSize),m_pkTexMan->Load("file:../data/textures/right.bmp"));
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);	
	glPopMatrix();
}


void Render::DrawWater(Vector3 kCamPos,Vector3 kPosition,Vector3 kHead,int iSize,int iStep) {
	float freq=500.0;
	float amp=1.0;
	
	glPushMatrix();
	
	glDisable(GL_CULL_FACE);
	
	glTranslatef(kPosition.x,kPosition.y,kPosition.z);
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);
	glTranslatef(-iSize/2,0,-iSize/2);
	
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
		
	glDepthMask(GL_FALSE);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);	
	glBindTexture(GL_TEXTURE_2D,m_pkTexMan->Load("file:../data/textures/water2.bmp"));
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);	
	glBindTexture(GL_TEXTURE_2D,m_pkTexMan->Load("file:../data/textures/water2.bmp"));

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_ADD);
//	m_pkTexMan->BindTexture("file:../data/textures/water2.bmp");
	
	
	glBegin(GL_TRIANGLE_STRIP);		
	for(int z=0;z<iSize;z+=iStep){
		glNormal3f(0,1,0);
		glColor4f(.7,.7,.7,0.4);
		
		float y=sin((SDL_GetTicks()/1000.0)+(z/iStep)*freq)*amp;
		float tx=SDL_GetTicks()/80000.0;
		
//		glTexCoord2f(0,z/iStep+SDL_GetTicks()/60000.0);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,z/(iStep*10.0)+tx);		
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,z/(iStep*10.0)-tx);				
		glVertex3f(0,y,z);
	
//		glTexCoord2f(iSize/iStep,z/iStep+SDL_GetTicks()/60000.0);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,iSize/(iStep*10.0),z/(iStep*10.0)+tx);		
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,iSize/(iStep*10.0),z/(iStep*10.0)-tx);						
		glVertex3f(iSize,y,z);
	}
	glEnd();	
	
	
	/*
	for(int z=0;z<iSize;z+=iStep){
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0,1,0);		
		glColor4f(.7,.7,.7,.9);
		for(int x=0;x<iSize;x+=iStep) {
			float y=sin((SDL_GetTicks()/1000.0)+(x/iStep)*freq)*amp;
			
			glNormal3f(1.0,0,0);
			glTexCoord2f(x/iStep+SDL_GetTicks()/60000.0,1);
			glVertex3f(x,y,z);
			
			glTexCoord2f(x/iStep+SDL_GetTicks()/60000.0,0);			
			glVertex3f(x,y,z+iStep);


		}
		glEnd();
	}*/
	

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);	
//	glActiveTextureARB(GL_TEXTURE0_ARB);
//	glDisable(GL_TEXTURE_2D);	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);	
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glPopMatrix();
}

void Render::DrawSimpleWater(Vector3 kPosition,int iSize) {
	glPushMatrix();
	
	glDisable(GL_CULL_FACE);	
//	glTranslatef(kPosition.x,kPosition.y,kPosition.z);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);	
//	glDisable(GL_COLOR_MATERIAL);	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);
		
	m_pkTexMan->BindTexture("file:../data/textures/water2.bmp");

	float tx=SDL_GetTicks()/60000.0;
	glBegin(GL_QUADS);
		glColor4f(.3,.3,.4,.99);
		glNormal3f(0,1,0);
		glTexCoord2f(tx,0);
		glVertex3f(kPosition.x,kPosition.y,kPosition.z);
		glTexCoord2f(tx,1);		
		glVertex3f(kPosition.x,kPosition.y,kPosition.z+iSize);
		glTexCoord2f(tx+2,0);		
		glVertex3f(kPosition.x+iSize,kPosition.y,kPosition.z+iSize);
		glTexCoord2f(tx+2,1);		
		glVertex3f(kPosition.x+iSize,kPosition.y,kPosition.z);
	
	
//		glColor4f(.5,.5,.5,0.5);
		glTexCoord2f(-tx,0);
		glVertex3f(kPosition.x,kPosition.y,kPosition.z);
		glTexCoord2f(-tx,1);		
		glVertex3f(kPosition.x,kPosition.y,kPosition.z+iSize);
		glTexCoord2f(-tx+2,0);		
		glVertex3f(kPosition.x+iSize,kPosition.y,kPosition.z+iSize);
		glTexCoord2f(-tx+2,1);		
		glVertex3f(kPosition.x+iSize,kPosition.y,kPosition.z);
	
	
	glEnd();

	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);		
	glPopMatrix();	
}


void Render::DrawHMlod(HeightMap* kmap,Vector3 CamPos){
	glPushMatrix();
	
	//translate to map position
	glTranslatef(kmap->m_kPosition.x,kmap->m_kPosition.y,kmap->m_kPosition.z);

	m_pkTexMan->BindTexture(kmap->m_acTileSet);
//	glPolygonMode(GL_FRONT,GL_LINE);	
	
	GLfloat mat_specular[]={0,0,0,0};
	GLfloat mat_diffuse[]={1,1,1,1};	
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);	
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	
	glEnable(GL_COLOR_MATERIAL);	
	glColorMaterial(GL_FRONT,GL_DIFFUSE);
	float black[4]={0.2,0.2,0.2,0};
	glMaterialfv(GL_FRONT,GL_AMBIENT,black);
//	glMaterialfv(GL_FRONT,GL_SPECULAR,black);
	
	
	Vector3 p1,p2;
	
	//calculate number slices depending on the size of the lod tiles size
	int slices=(kmap->m_iHmSize)/m_iSlicesize;
	int step=1;
	
	for(int sz=0;sz<slices;sz++) {
		for(int sx=0;sx<slices;sx++) {
			//set lop steps depending on the distance to the center of the lod tile
			step=int((CamPos-Vector3(kmap->m_kPosition.x+sx*m_iSlicesize+m_iSlicesize/2,
											//this distance realy sux..
											CamPos.y,//this is nice =)
//											kmap->m_kPosition.y+kmap->verts[(sz*m_iSlicesize+m_iSlicesize/2)*kmap->m_iHmSize+(sx*m_iSlicesize+m_iSlicesize/2)].height,
											kmap->m_kPosition.z+sz*m_iSlicesize+m_iSlicesize/2)
											).length()/m_iDetail);																
//	cout<<"STEP"<<step<<endl;
			if(step>13)
				continue;
			
			if(step<1)//step cant be lower than 1
				step=1;				
			if(step>6)//if the step get to high it will look realy bad
				step=6;			
			
			bool flip=false;	//texture fliper
			float t=0;
			float s=0;			 //texture cordinats
			float nt,ns;
			
			//start going trouh all vertexes in the slice
			for(int z=sz*m_iSlicesize;z<sz*m_iSlicesize+m_iSlicesize;z+=step){
				glBegin(GL_TRIANGLE_STRIP);						
				for(int x=sx*m_iSlicesize;x<sx*m_iSlicesize+m_iSlicesize+step+1;x+=step){
					
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
							glTexCoord2f(t+0.01,s-0.01);						
					 		glVertex3fv((float*)&p1);
				 			
							p2=Vector3(x,kmap->verts[(z+step)*kmap->m_iHmSize+x].height,z+step);			 		
						 	glNormal3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x].normal);				 		
							glColor3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x].color);
					 		glTexCoord2f(t+0.01,s-0.24);				
							glVertex3fv((float*)&p2); 
							
							t=nt;s=ns;//set the new cordinats
						}
					}
					
					//vertex down
					p1=Vector3(x,kmap->verts[z*kmap->m_iHmSize+x].height,z);				
					glNormal3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x].normal);
					glColor3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x].color);
					
					if(flip)						
				 		glTexCoord2f(t+0.01,s-0.01);
 					else
 						glTexCoord2f(t+0.24,s-0.01);
 						
			 		glVertex3fv((float*)&p1);//set vertex
 		
 					//vertex up
					p2=Vector3(x,kmap->verts[(z+step)*kmap->m_iHmSize+x].height,z+step);			 		
			 	  glNormal3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x].normal);
					glColor3fv((float*)&kmap->verts[(z+step)*kmap->m_iHmSize+x].color);
					
					if(flip)	
				 		glTexCoord2f(t+0.01,s-0.24);
 					else
 						glTexCoord2f(t+0.24,s-0.24);
									   
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
	glDisable(GL_COLOR_MATERIAL);	
	glPolygonMode(GL_FRONT,GL_FILL);
	glPopMatrix();
}

void Render::DrawHM(HeightMap *kmap) {
	glPushMatrix();
	Vector3 p1;
	Vector3 p2;
	Vector3 p3;
	Vector3 p4;
	
	glTranslatef(-50,-10,-50);
	m_pkTexMan->BindTexture(kmap->m_acTileSet);
	
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
