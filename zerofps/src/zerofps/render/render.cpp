#include "render.h"

Render::Render()  
 :	ZFObject("Render") {
 
 	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
 	m_pkFrustum = static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
 	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light")); 	
//	m_pkTexMan=pkTexMan;
	
	m_iSlicesize = 32;		//grid size of lod tiles
	m_iDetail = 30;				//height meens greater detail att longer range	
	m_iViewDistance = 300;
	m_iFpsLock = 60;
	m_iAutoLod = 1;
	m_iLodUpdate = 0;	
	m_kOldCamPos = Vector3(0,0,0);
	
	m_iHmTempList=0;
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


void Render::Dot(float x,float y,float z) {
	Line(Vector3(x,y,z),Vector3(x+0.05,y+0.05,z+0.05));

}



void Render::DrawConsole(char* m_aCommand,vector<char*>* m_kText,int iStartLine) {
	SetFont("file:../data/textures/text/console.tga");

	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);	
		
	Quad(Vector3(0,0,-.5),Vector3(0,0,0),Vector3(1.2,.9,1),m_pkTexMan->Load("file:../data/textures/background.tga",0));
	
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	
	
	
	Print(Vector3(-.65,-.5,-.6),Vector3(0,0,0),Vector3(.04,.04,.04),m_aCommand);		
	
	if(iStartLine < 0)
		iStartLine = 0;
	
	for(int i=iStartLine;i<iStartLine+23;i++) {
		if((*m_kText)[i]!=NULL){
			Print(Vector3(-0.65,-0.40+(i-iStartLine)/(float)25,-.6),Vector3(0,0,0),Vector3(0.03,0.03,0.03),(*m_kText)[i]);		
		
		}
	}
}


void Render::DrawBillboard(Matrix4& kModelMatrix,Vector3& kPos,float fSize,int iTexture) {
	fSize/=2;
	
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
	
	glAlphaFunc(GL_GREATER,0.3);
	glEnable(GL_ALPHA_TEST);

//	glTranslatef(kPos.x,kPos.y,kPos.z);	
	x.Set(kModelMatrix[0], kModelMatrix[4], kModelMatrix[8]);
	y.Set(kModelMatrix[1], kModelMatrix[5], kModelMatrix[9]); 

/*	a = kPos + ((-x - y) * iSize);
   b = kPos + ((x - y) * iSize);
   c = kPos + ((x + y) * iSize);
   d = kPos + ((-x + y) * iSize);*/

	a = kPos + ((-x + y) * fSize);
   b = kPos + ((x + y) * fSize);
   c = kPos + ((x - y) * fSize);
   d = kPos + ((-x - y) * fSize);
	
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
		
	
	glDisable(GL_ALPHA_TEST);

	glPopMatrix();
	glPopAttrib();
	glEnable(GL_CULL_FACE);	
	
}


void Render::DrawBoundSphere(float fRadius, Vector3)
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glColor3f(1,1,1);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D );

	float x,y;
	glBegin(GL_LINE_LOOP );
	for(int i=0; i<360; i+=(int)12.25) {
		x = cos(DegToRad(i)) * fRadius;
		y = sin(DegToRad(i)) * fRadius;
		glVertex3f(x,y,0);
	}
	
	glEnd();

	glPopAttrib();
}


void Render::DrawBox(Vector3 kPos,Vector3 kRot,Vector3 kScale,int iTexture)
{
	glPushMatrix();
		
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glRotatef(kRot.x, 1, 0, 0);
	glRotatef(kRot.y, 0, 1, 0);	
	glRotatef(kRot.z, 0, 0, 1);
	glScalef(kScale.x,kScale.y,kScale.z);
	
	m_pkTexMan->BindTexture(iTexture); 	
	glColor4f(1,1,1,1);		
		
	glBegin(GL_QUADS);			
		
		//front
		glNormal3f(0,0,1);
		glTexCoord2f(0.0,1.0);glVertex3f(-0.5  ,-0.5	,0.5);		 
		glTexCoord2f(1.0,1.0);glVertex3f(0.5	,-0.5	,0.5);		
		glTexCoord2f(1.0,0.0);glVertex3f(0.5	,0.5	 ,0.5);    
		glTexCoord2f(0.0,0.0);glVertex3f(-0.5  ,0.5	 ,0.5);    
	
		//back
		glNormal3f(0,0,-1);
		glTexCoord2f(0.0,0.0);glVertex3f(-0.5  ,0.5	 ,-0.5);    		
		glTexCoord2f(1.0,0.0);glVertex3f(0.5	,0.5	 ,-0.5);    		
		glTexCoord2f(1.0,1.0);glVertex3f(0.5	,-0.5	,-0.5);			
		glTexCoord2f(0.0,1.0);glVertex3f(-0.5  ,-0.5	,-0.5);		 
	
		//right
		glNormal3f(1,0,0);
		glTexCoord2f(0.0,0.0);glVertex3f(0.5  ,0.5	 ,-0.5);    		
		glTexCoord2f(1.0,0.0);glVertex3f(0.5	,0.5	 ,0.5);    		
		glTexCoord2f(1.0,1.0);glVertex3f(0.5	,-0.5	,0.5);			
		glTexCoord2f(0.0,1.0);glVertex3f(0.5  ,-0.5	,-0.5);				

		//left
		glNormal3f(-1,0,0);		
		glTexCoord2f(0.0,1.0);glVertex3f(-0.5  ,-0.5	,-0.5);		 
		glTexCoord2f(1.0,1.0);glVertex3f(-0.5	,-0.5	,0.5);		
		glTexCoord2f(1.0,0.0);glVertex3f(-0.5	,0.5	 ,0.5);    
		glTexCoord2f(0.0,0.0);glVertex3f(-0.5  ,0.5	 ,-0.5);   
		
		//top
		glNormal3f(0,1,0);
		glTexCoord2f(0.0,1.0);glVertex3f(-0.5  ,0.5	,0.5);		 
		glTexCoord2f(1.0,1.0);glVertex3f(0.5	,0.5	,0.5);		
		glTexCoord2f(1.0,0.0);glVertex3f(0.5	,0.5	 ,-0.5);    
		glTexCoord2f(0.0,0.0);glVertex3f(-0.5  ,0.5	 ,-0.5);    		
		
		//botom
		glNormal3f(0,-1,0);
		glTexCoord2f(0.0,1.0);glVertex3f(-0.5  ,-0.5	,-0.5);		 
		glTexCoord2f(1.0,1.0);glVertex3f(0.5	,-0.5	,-0.5);		
		glTexCoord2f(1.0,0.0);glVertex3f(0.5	,-0.5	 ,0.5);    
		glTexCoord2f(0.0,0.0);glVertex3f(-0.5  ,-0.5	 ,0.5);    		
		
		
	glEnd();			
	
	glPopMatrix();

}

/*
RENDER_API char* GetOpenGLErrorName(GLenum id )
{
	switch(id) 
	{
		case GL_NO_ERROR:			return "GL_NO_ERROR";				break;
		case GL_INVALID_ENUM:		return "GL_INVALID_ENUM ";			break;
		case GL_INVALID_VALUE :		return "GL_INVALID_VALUE ";			break;
		case GL_INVALID_OPERATION:	return "GL_INVALID_OPERATION";			break;
		case GL_STACK_OVERFLOW :	return "GL_STACK_OVERFLOW ";			break;
		case GL_STACK_UNDERFLOW :	return "GL_STACK_UNDERFLOW ";			break;
		case GL_OUT_OF_MEMORY :		return "GL_OUT_OF_MEMORY ";			break;
		//case XXX:		return "XXX";			break;
	}

	return "*** --- ***";
}
*/



RENDER_API void RenderDLL_InitExtGL(void)
{
#ifdef _WIN32
	int res = extgl_Initialize();
	cout << "extgl_Initialize: "<< res << endl;
#endif
}



