#include "render.h"

Render::Render()  
 :	ZFObject("Render") {
 
 	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
 	m_pkFrustum = static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
//	m_pkTexMan=pkTexMan;
	
	m_iSlicesize=32;		//grid size of lod tiles
	m_iDetail=30;				//height meens greater detail att longer range	
	m_iViewDistance=250;
	m_iFpsLock=60;
	m_iAutoLod=1;
	m_iLodUpdate=0;	
	m_kOldCamPos=Vector3(0,0,0);
}

void Render::Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture){
	glPushMatrix();
		
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);
	glScalef(kScale.x,kScale.y,kScale.z);

//	glBlendFunc(GL_ONE,GL_ZERO);
 		
  	m_pkTexMan->BindTexture(iTexture);  

	glBegin(GL_QUADS);			
	glColor4f(1.0,1.0,1.,1.0);  	  
	glNormal3f(0,0,1);
   glTexCoord2f(0.0,1.0);glVertex3f(-.5,-0.5,0);		 
   glTexCoord2f(1.0,1.0);glVertex3f(.5,-0.5,0);		
 	glTexCoord2f(1.0,0.0);glVertex3f(.5,0.5,0);    
	glTexCoord2f(0.0,0.0);glVertex3f(-0.5,0.5,0);    
	glEnd();			

	glPopMatrix();
}

void Render::PrintChar(char cChar) {
	int texwidth=FONTWIDTH*16;	
	int pos=int(cChar)*FONTWIDTH;		
	float glu=1.0/texwidth;				//opengl texture cordinats is 0-1
	float width=glu*FONTWIDTH;
	
	float y=(float(int(pos/texwidth)*FONTWIDTH)*glu+width);
	float x=float(pos%texwidth)*glu;//+width/2;
	
	
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glAlphaFunc(GL_GREATER,0.1);
	glEnable(GL_ALPHA_TEST);
 	
 	m_pkTexMan->BindTexture(aCurentFont,T_NOMIPMAPPING);  

	glPushMatrix();
	glBegin(GL_QUADS);			
//			glColor4f(1.0,1.0,1.0,1.0);  	  
	glNormal3f(0,0,1);
 	  
	glTexCoord2f(x,y);				glVertex3f(-.5,-0.5,0);		 
	glTexCoord2f(x+width,y);		glVertex3f(.5,-0.5,0);		
	glTexCoord2f(x+width,y-width);glVertex3f(.5,0.5,0);    
	glTexCoord2f(x,y-width);		glVertex3f(-0.5,0.5,0);    
	glEnd();				
	glPopMatrix();
	
	
	glDisable(GL_ALPHA_TEST);

	glPopAttrib();
}

void Render::Print(Vector3 kPos,Vector3 kHead,Vector3 kScale,char* aText) {
	char paText[TEXT_MAX_LENGHT];
	
	strcpy(paText,aText);
	
	glPushMatrix();
		
		glTranslatef(kPos.x,kPos.y,kPos.z);	
		glRotatef(kHead.x, 1, 0, 0);
		glRotatef(kHead.y, 0, 1, 0);	
		glRotatef(kHead.z, 0, 0, 1);
		glScalef(kScale.x,kScale.y,kScale.z);
		
		int i=0;
		while(paText[i]!='\0') {
			PrintChar(paText[i]);
			glTranslatef(1,0,0);
		
			i++;
		}

	glPopMatrix();
}


void Render::SetFont(char* aFont) {
	strcpy(aCurentFont,aFont);

}

void Render::Line(Vector3 kPos1,Vector3 kPos2)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
		glVertex3f(kPos1.x,kPos1.y,kPos1.z);
		glVertex3f(kPos2.x,kPos2.y,kPos2.z);		
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void Render::SetColor(Vector3 kColor)
{
	glColor3f(kColor.x,kColor.y,kColor.z);
}


/*
void Render::DrawHMQT(HeightMap *kmap) {
	glPushMatrix();
	glTranslatef(-50,-10,-50);
	SetColor(Vector3(255,255,255));
	
	DrawQuad(kmap->m_kCenter,kmap->m_iHmSize/2);
//	Dot(kmap->m_iHmSize/2,kmap->m_kCenter->height,kmap->m_iHmSize/2);	
	
	glPopMatrix();
}

void Render::DrawQuad(HM_vert* vert,int width) {
	Dot(0,vert->height,0);


	int i=0;
	for(int q=-1;q<2;q++){
		for(int w=-1;w<2;w++) {
			if(q==0 && w==0) {
			}else {
				if(vert->childs[i]!=NULL){
				glPushMatrix();
					glTranslatef(width*q,0,width*w);
					DrawQuad(vert->childs[i],width/2);
				glPopMatrix();				
				}
			}
		}
	}
	

/*
	for(int i=0;i<8;i++) {
		if(vert->childs[i]!=NULL){
			glPushMatrix();	
				
				DrawQuad
		}	
	}
}
*/

void Render::Dot(float x,float y,float z) {
	Line(Vector3(x,y,z),Vector3(x+0.05,y+0.05,z+0.05));

}




void Render::DrawConsole(char* m_aCommand,vector<char*>* m_kText) {
	SetFont("file:../data/textures/text/console.tga");

	Quad(Vector3(0,0,-1.1),Vector3(0,0,0),Vector3(2.64,1.98,2.2),m_pkTexMan->Load("file:../data/textures/background.bmp",0));
	
	Print(Vector3(-1.1,-0.82,-1),Vector3(0,0,0),Vector3(.06,.06,.06),m_aCommand);		
	
	for(int i=0;i<21;i++) {
		if((*m_kText)[i]!=NULL){
			//Print(Vector3(-1.1,-0.70+i/(float)13,-1),Vector3(0,0,0),Vector3(.06,.06,.06),(*m_kText)[i]);		
			Print(Vector3(-1.1,-0.70+i/(float)13,-1),Vector3(0,0,0),Vector3(0.04,0.04,0.04),(*m_kText)[i]);		
		
		}
	}
}

/*
void Render::GetFrustum() {
   Matrix4   proj;
   Matrix4   modl;
   Matrix4   clip;
   
	// Get the current PROJECTION matrix from OpenGL 
   glGetFloatv( GL_PROJECTION_MATRIX, (float*)&proj[0] );

   // Get the current MODELVIEW matrix from OpenGL 
   glGetFloatv( GL_MODELVIEW_MATRIX, (float*)&modl[0] );

//		proj.Print();
//		exit(0);
	
	 clip=modl*proj;
//	clip=proj*modl;
	
/*	
        // Concatenate (Multiply) The Two Matricies
        clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
        clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
        clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
        clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

        clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
        clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
        clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
        clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

        clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
        clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
        clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
        clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

        clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
        clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
        clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
        clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

/
	

	
	// Extract the numbers for the RIGHT plane 
   m_akFrustum[0][0] = clip[ 3] - clip[ 0];
   m_akFrustum[0][1] = clip[ 7] - clip[ 4];
   m_akFrustum[0][2] = clip[11] - clip[ 8];
   m_akFrustum[0][3] = clip[15] - clip[12];
   
   // Extract the numbers for the LEFT plane 
   m_akFrustum[1][0] = clip[ 3] + clip[ 0];
   m_akFrustum[1][1] = clip[ 7] + clip[ 4];
   m_akFrustum[1][2] = clip[11] + clip[ 8];
   m_akFrustum[1][3] = clip[15] + clip[12];  
   
   // Extract the BOTTOM plane 
   m_akFrustum[2][0] = clip[ 3] + clip[ 1];
   m_akFrustum[2][1] = clip[ 7] + clip[ 5];
   m_akFrustum[2][2] = clip[11] + clip[ 9];
   m_akFrustum[2][3] = clip[15] + clip[13];
   
   
   // Extract the TOP plane 
   m_akFrustum[3][0] = clip[ 3] - clip[ 1];
   m_akFrustum[3][1] = clip[ 7] - clip[ 5];
   m_akFrustum[3][2] = clip[11] - clip[ 9];
   m_akFrustum[3][3] = clip[15] - clip[13];
   
   // Extract the FAR plane 
   m_akFrustum[4][0] = clip[ 3] - clip[ 2];
   m_akFrustum[4][1] = clip[ 7] - clip[ 6];
   m_akFrustum[4][2] = clip[11] - clip[10];
   m_akFrustum[4][3] = clip[15] - clip[14];
   
   
   // Extract the NEAR plane 
   m_akFrustum[5][0] = clip[ 3] + clip[ 2];
   m_akFrustum[5][1] = clip[ 7] + clip[ 6];
   m_akFrustum[5][2] = clip[11] + clip[10];
   m_akFrustum[5][3] = clip[15] + clip[14];   
   
   for(int i=0;i<6;i++){
// 	   cout<<"Plain "<<i<< " X:"<<m_akFrustum[i].x<<" Y:"<<m_akFrustum[i].y<<" Z:"<<m_akFrustum[i].z<<" Lenght:"<<m_akFrustum[i].w<<endl;
   	if(m_akFrustum[i].PlainLength()>0)
	   	m_akFrustum[i].PlainNormalize();
	 }
   
}


bool Render::PointInFrustum( Vector3 kPoint)
{
   int p;

   for( p = 0; p < 6; p++ ){
      if( m_akFrustum[p][0] * kPoint.x + m_akFrustum[p][1] * kPoint.y + m_akFrustum[p][2] * kPoint.z + m_akFrustum[p][3] <= 0 ){
         return false;         
		}
//		cout<<"klar"<<endl;
	}
	
   return true;
}

bool Render::SphereInFrustum(Vector4 kPoint)
{
//	kPoint.x+=CamPos.x;
//	kPoint.z+=CamPos.z;

   int p;
   float d;

   for( p = 0; p < 6; p++ )
   {
      d = m_akFrustum[p][0] * kPoint.x + m_akFrustum[p][1] * kPoint.y + m_akFrustum[p][2] * kPoint.z + m_akFrustum[p][3];
      if( d <= -kPoint.w ){
//      	cout<<"FUCK"<<endl;
         return false ;//+ kPoint.w;
      }
   }
///   cout<<d<<endl;
   return true ;//+ kPoint.w;
}


bool Render::CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez )
{
	int p;

	for( p = 0; p < 6; p++ )
	{
		if( m_akFrustum[p][0] * (x - sizex) + m_akFrustum[p][1] * (y - sizey) + m_akFrustum[p][2] * (z - sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x + sizex) + m_akFrustum[p][1] * (y - sizey) + m_akFrustum[p][2] * (z - sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x - sizex) + m_akFrustum[p][1] * (y + sizey) + m_akFrustum[p][2] * (z - sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x + sizex) + m_akFrustum[p][1] * (y + sizey) + m_akFrustum[p][2] * (z - sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x - sizex) + m_akFrustum[p][1] * (y - sizey) + m_akFrustum[p][2] * (z + sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x + sizex) + m_akFrustum[p][1] * (y - sizey) + m_akFrustum[p][2] * (z + sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x - sizex) + m_akFrustum[p][1] * (y + sizey) + m_akFrustum[p][2] * (z + sizez) + m_akFrustum[p][3] > 0 )
			continue;
		if( m_akFrustum[p][0] * (x + sizex) + m_akFrustum[p][1] * (y + sizey) + m_akFrustum[p][2] * (z + sizez) + m_akFrustum[p][3] > 0 )
			continue;
		return false;
	}
	return true;
}
*/

void Render::DrawBillboard(Matrix4 kModelMatrix,Vector3 kPos,int iSize,int iTexture){
	Vector3 x;
	Vector3 y;
	
	Vector3 a;
	Vector3 b;	
	Vector3 c;	
	Vector3 d;	
	
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_CULL_FACE);	
	glDisable(GL_LIGHTING);

	glColor4f(1,1,1,1);
	m_pkTexMan->BindTexture(iTexture); 	
//	glAlphaFunc(GL_GREATER,0.3);
//	glEnable(GL_ALPHA_TEST);

	glTranslatef(kPos.x,kPos.y,kPos.z);	
	
	x.Set(kModelMatrix[0], kModelMatrix[4], kModelMatrix[8]);
	y.Set(kModelMatrix[1], kModelMatrix[5], kModelMatrix[9]); 

/*	a = kPos + ((-x - y) * iSize);
   b = kPos + ((x - y) * iSize);
   c = kPos + ((x + y) * iSize);
   d = kPos + ((-x + y) * iSize);*/

	a = kPos + ((-x + y) * iSize);
   b = kPos + ((x + y) * iSize);
   c = kPos + ((x - y) * iSize);
   d = kPos + ((-x - y) * iSize);
	
	glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3fv(&a[0]);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3fv(&b[0]);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3fv(&c[0]);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3fv(&d[0]);
   glEnd();
	
	
	/*
	glBegin(GL_QUADS);
		glNormal3f(0,0,1);
		glTexCoord2f(0,0);glVertex3f(-0.5,0.5,0); 
		glTexCoord2f(0,1);glVertex3f(-0.5,-0.5,0); 
		glTexCoord2f(1,1);glVertex3f(0.5,-0.5,0); 
		glTexCoord2f(1,0);glVertex3f(0.5,0.5,0); 			
	glEnd();*/
		
	
//	glDisable(GL_ALPHA_TEST);

	glPopMatrix();
	glPopAttrib();
	glEnable(GL_CULL_FACE);	
	
}




RENDER_API void RenderDLL_InitExtGL(void)
{
#ifdef _WIN32
	int res = extgl_Initialize();
	cout << "extgl_Initialize: "<< res << endl;
#endif
}



