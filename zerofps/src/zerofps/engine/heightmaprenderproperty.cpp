#include "heightmaprenderproperty.h"

HeightMapRenderProperty::HeightMapRenderProperty(HeightMap* pkHeightMap) 
{
	m_pkHeightMap=pkHeightMap;	
	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));		

/*
	m_iSlicesize=32;		//grid size of lod tiles
	m_iDetail=30;				//height meens greater detail att longer range	
	m_iViewDistance=250;
	m_iFpsLock=60;
	m_iAutoLod=1;
	m_iLodUpdate=0;	
//	m_kOldCamPos=Vector3(0,0,0);
*/
	m_iType=PROPERTY_TYPE_STATIC;
}


void HeightMapRenderProperty::Update() 
{	
	m_pkHeightMap->SetPosition(m_pkObject->GetPos());
	m_pkRender->DrawHMlod(m_pkHeightMap,m_pkZeroFps->GetCam()->GetPos(),m_pkZeroFps->m_iFps);
}

/*
void HeightMapRenderProperty::DrawHMlod(HeightMap* kmap,Vector3 CamPos,int iFps){
	if(m_iAutoLod>0){
		if(SDL_GetTicks()>(m_iLodUpdate+500)){
			m_iLodUpdate=SDL_GetTicks();
			
			//dont update lod if the camera has't moved
//			if(CamPos!=m_kOldCamPos){
//				m_kOldCamPos=CamPos;
				if(iFps<(m_iFpsLock-5) && m_iDetail>20){
					m_iDetail--;	
				} else if(iFps>(m_iFpsLock+5) && m_iDetail<80){
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
	float black[4]={0.1,0.1,0.1,0.1};
	glMaterialfv(GL_FRONT,GL_AMBIENT,black);
//	glMaterialfv(GL_FRONT,GL_SPECULAR,black);
	glEnable(GL_COLOR_MATERIAL);		
	
	Vector3 p1,p2;
	
	//calculate number slices depending on the size of the lod tiles size
	int slices=(kmap->m_iHmSize)/m_iSlicesize;
	int step=1;
	
	for(int sz=0;sz<slices;sz++) {
		for(int sx=0;sx<slices;sx++) {
			if(!m_pkFrustum->CubeInFrustum(kmap->m_kPosition.x+sx*m_iSlicesize+m_iSlicesize/2,
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
	*
	glDisable(GL_COLOR_MATERIAL);	
//	glPolygonMode(GL_FRONT,GL_FILL);
	glPopMatrix();
}


void HeightMapRenderProperty::GiveTexCor(float &iX,float &iY,int iNr) {	
	iX=(iNr%4);	
	iY=(iNr-(iNr%4))/4;

	iX*=0.25;
	iY*=0.25;
	iY=iY;
	
//	cout<<"X: "<<iX<< "  Y: "<<iY<<endl;
}

*/










