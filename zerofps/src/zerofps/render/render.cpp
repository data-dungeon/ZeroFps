#include "render.h"
#include "../basic/basic.pkg"

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

	m_iMaxLandscapeLayers	= 4;
	m_iDrawLandscape			= 1;

	g_ZFObjSys.RegisterVariable("r_maxlayers", &m_iMaxLandscapeLayers,CSYS_INT);
	g_ZFObjSys.RegisterVariable("r_drawland", &m_iDrawLandscape,CSYS_INT);


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



void Render::DrawConsole(char* m_aCommand,vector<char*>* m_kText,int iStartLine) 
{
	SetFont("file:../data/textures/text/console.tga");

	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);	
		
	Quad(Vector3(0,0,-.5),Vector3(0,0,0),Vector3(1.2,.9,1),
		m_pkTexMan->Load("file:../data/textures/background.tga",0));
	
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	
	Print(Vector3(-.65,-.5,-.6),Vector3(0,0,0),Vector3(.04,.04,.04),m_aCommand);		
	
	if(iStartLine < 0)
		iStartLine = 0;
	
	for(int i=iStartLine;i<iStartLine+23;i++) 
	{
		if((*m_kText)[i]!=NULL)
		{
			Print(Vector3(-0.65,-0.40+(i-iStartLine)/(float)25,-.6),
				Vector3(0,0,0),Vector3(0.03,0.03,0.03),(*m_kText)[i]);
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
	x.Set(kModelMatrix.data[0], kModelMatrix.data[4], kModelMatrix.data[8]);
	y.Set(kModelMatrix.data[1], kModelMatrix.data[5], kModelMatrix.data[9]); 

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
      glVertex3fv((float*) &a);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3fv((float*) &b);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3fv((float*) &c);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3fv((float*) &d);
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

void Render::DrawColorBox(Vector3 kPos,Vector3 kRot,Vector3 kScale,Vector3 kColor)
{
	glPushMatrix();
		
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glRotatef(kRot.x, 1, 0, 0);
	glRotatef(kRot.y, 0, 1, 0);	
	glRotatef(kRot.z, 0, 0, 1);
	glScalef(kScale.x,kScale.y,kScale.z);
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING );
	glColor4f(kColor.x,kColor.y,kColor.z,1);		

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
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
 	
	glPopMatrix();
	
}

void Render::DrawAABB( float x, float y, float z, float sizex,float sizey,float sizez, float fMin,float fMax, Vector3 kColor )
{
	Vector3 kCubeNeg(x - sizex, fMin, z - sizez); 
	Vector3 kCubePos(x + sizex, fMax, z + sizez); 

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING );

	glColor3f(1,1,1);
	glBegin(GL_LINES);
		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubeNeg.z);			glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubePos.z);
		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubePos.z);			glVertex3f(kCubePos.x,kCubeNeg.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubePos.z);			glVertex3f(kCubePos.x,kCubeNeg.y,kCubeNeg.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubeNeg.z);			glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubeNeg.z);

		glVertex3f(kCubeNeg.x,kCubePos.y,kCubeNeg.z);			glVertex3f(kCubeNeg.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubeNeg.x,kCubePos.y,kCubePos.z);			glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);			glVertex3f(kCubePos.x,kCubePos.y,kCubeNeg.z);
		glVertex3f(kCubePos.x,kCubePos.y,kCubeNeg.z);			glVertex3f(kCubeNeg.x,kCubePos.y,kCubeNeg.z);

		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubeNeg.z);			glVertex3f(kCubeNeg.x,kCubePos.y,kCubeNeg.z);
		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubePos.z);			glVertex3f(kCubeNeg.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubePos.z);			glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubeNeg.z);			glVertex3f(kCubePos.x,kCubePos.y,kCubeNeg.z);

		/*		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubePos.z);			glVertex3f(kCubeNeg.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubeNeg.x,kCubePos.y,kCubePos.z);			glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);			glVertex3f(kCubePos.x,kCubeNeg.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubePos.z);			glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubePos.z);

		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubeNeg.z);			glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubeNeg.x,kCubePos.y,kCubeNeg.z);			glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubePos.y,kCubeNeg.z);			glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubeNeg.z);			glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);*/

	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

}

char* BoolStr(bool bFlag)
{
	if(bFlag)
		return "True";
	else
		return "False";
}


void GlDump_IsEnabled(int iGlEnum, char* szName)
{
	cout << szName << " : " << BoolStr(glIsEnabled(iGlEnum))	<< endl;

}
void GlDump_GetFloatv(int iGlEnum, char* szName, int iValues)
{
	float afValues[16];
	glGetFloatv(iGlEnum, afValues);	

	cout << szName << " : ";
	for(int i=0; i<iValues; i++) {
		cout << afValues[i] << " ";
		}
	cout << endl;
}

void Render::DumpGLState(void)
{
	// Get and Dump OpenGL State to log.
	cout << "-------------------------------------------------------------------" << endl;
	cout << "Render::DumpGLSTate" << endl;

	GlDump_IsEnabled(GL_AUTO_NORMAL , "GL_AUTO_NORMAL");
	GlDump_IsEnabled(GL_COLOR_MATERIAL , "GL_COLOR_MATERIAL");
	GlDump_IsEnabled(GL_CULL_FACE , "GL_CULL_FACE");
	GlDump_IsEnabled(GL_DEPTH_TEST , "GL_DEPTH_TEST");
	GlDump_IsEnabled(GL_DITHER , "GL_DITHER");
	GlDump_IsEnabled(GL_LIGHTING , "GL_LIGHTING");
	GlDump_IsEnabled(GL_LINE_SMOOTH , "GL_LINE_SMOOTH");
	GlDump_IsEnabled(GL_LINE_STIPPLE , "GL_LINE_STIPPLE");
	GlDump_IsEnabled(GL_LOGIC_OP , "GL_LOGIC_OP");
	GlDump_IsEnabled(GL_MAP1_COLOR_4 , "GL_MAP1_COLOR_4");
	GlDump_IsEnabled(GL_MAP1_INDEX , "GL_MAP1_INDEX");
	GlDump_IsEnabled(GL_MAP1_NORMAL , "GL_MAP1_NORMAL");
	GlDump_IsEnabled(GL_MAP1_TEXTURE_COORD_1 , "GL_MAP1_TEXTURE_COORD_1");
	GlDump_IsEnabled(GL_MAP1_TEXTURE_COORD_2 , "GL_MAP1_TEXTURE_COORD_2");
	GlDump_IsEnabled(GL_MAP1_TEXTURE_COORD_3 , "GL_MAP1_TEXTURE_COORD_3");
	GlDump_IsEnabled(GL_MAP1_TEXTURE_COORD_4 , "GL_MAP1_TEXTURE_COORD_4");
	GlDump_IsEnabled(GL_MAP1_VERTEX_3 , "GL_MAP1_VERTEX_3");
	GlDump_IsEnabled(GL_MAP1_VERTEX_4 , "GL_MAP1_VERTEX_4");
	GlDump_IsEnabled(GL_MAP2_COLOR_4 , "GL_MAP2_COLOR_4");
	GlDump_IsEnabled(GL_MAP2_INDEX , "GL_MAP2_INDEX");
	GlDump_IsEnabled(GL_MAP2_NORMAL , "GL_MAP2_NORMAL");
	GlDump_IsEnabled(GL_MAP2_NORMAL , "GL_MAP2_NORMAL");
	GlDump_IsEnabled(GL_MAP2_TEXTURE_COORD_1 , "GL_MAP2_TEXTURE_COORD_1");
	GlDump_IsEnabled(GL_MAP2_TEXTURE_COORD_2 , "GL_MAP2_TEXTURE_COORD_2");
	GlDump_IsEnabled(GL_MAP2_TEXTURE_COORD_3 , "GL_MAP2_TEXTURE_COORD_3");
	GlDump_IsEnabled(GL_MAP2_TEXTURE_COORD_4 , "GL_MAP2_TEXTURE_COORD_4");
	GlDump_IsEnabled(GL_MAP2_VERTEX_3 , "GL_MAP2_VERTEX_3");
	GlDump_IsEnabled(GL_MAP2_VERTEX_4 , "GL_MAP2_VERTEX_4");
	GlDump_IsEnabled(GL_NORMALIZE , "GL_NORMALIZE");
	GlDump_IsEnabled(GL_POINT_SMOOTH , "GL_POINT_SMOOTH");
	GlDump_IsEnabled(GL_POLYGON_SMOOTH , "GL_POLYGON_SMOOTH");
	GlDump_IsEnabled(GL_POLYGON_STIPPLE , "GL_POLYGON_STIPPLE");
	GlDump_IsEnabled(GL_SCISSOR_TEST , "GL_SCISSOR_TEST");
	GlDump_IsEnabled(GL_STENCIL_TEST , "GL_STENCIL_TEST");

	cout << "Texture:" << endl;
	GlDump_IsEnabled(GL_TEXTURE_1D , "GL_TEXTURE_1D");
	GlDump_IsEnabled(GL_TEXTURE_2D , "GL_TEXTURE_2D");
	GlDump_IsEnabled(GL_TEXTURE_GEN_Q , "GL_TEXTURE_GEN_Q");
	GlDump_IsEnabled(GL_TEXTURE_GEN_R , "GL_TEXTURE_GEN_R");
	GlDump_IsEnabled(GL_TEXTURE_GEN_S , "GL_TEXTURE_GEN_S");
	GlDump_IsEnabled(GL_TEXTURE_GEN_T , "GL_TEXTURE_GEN_T");

	cout << "Accumulation:" << endl;
	GlDump_GetFloatv(GL_ACCUM_CLEAR_VALUE,		"GL_ACCUM_CLEAR_VALUE", 1);
	GlDump_GetFloatv(GL_ACCUM_RED_BITS,			"GL_ACCUM_RED_BITS", 1);
	GlDump_GetFloatv(GL_ACCUM_GREEN_BITS,		"GL_ACCUM_GREEN_BITS", 1);
	GlDump_GetFloatv(GL_ACCUM_BLUE_BITS,		"GL_ACCUM_BLUE_BITS", 1);
	GlDump_GetFloatv(GL_ACCUM_ALPHA_BITS,		"GL_ACCUM_ALPHA_BITS", 1);

	cout << "Alpha:" << endl;
	GlDump_IsEnabled(GL_ALPHA_TEST ,			"GL_ALPHA_TEST");
	GlDump_GetFloatv(GL_ALPHA_TEST_REF,			"GL_ALPHA_TEST_REF", 1);

	cout << "Blend:" << endl;
	GlDump_IsEnabled(GL_BLEND , "GL_BLEND");
	GlDump_GetFloatv(GL_BLEND_DST,				"GL_BLEND_DST", 1);
	GlDump_GetFloatv(GL_BLEND_SRC,				"GL_BLEND_SRC", 1);

	
	GlDump_GetFloatv(GL_ATTRIB_STACK_DEPTH,		"GL_ATTRIB_STACK_DEPTH", 1);
	GlDump_GetFloatv(GL_AUX_BUFFERS,			"GL_AUX_BUFFERS", 1);
	GlDump_GetFloatv(GL_COLOR_CLEAR_VALUE,		"GL_COLOR_CLEAR_VALUE", 4);
	GlDump_GetFloatv(GL_COLOR_MATERIAL_FACE,	"GL_COLOR_MATERIAL_FACE", 1);
	GlDump_GetFloatv(GL_COLOR_MATERIAL_PARAMETER, "GL_COLOR_MATERIAL_PARAMETER", 1);
	GlDump_GetFloatv(GL_CULL_FACE_MODE,			"GL_CULL_FACE_MODE", 1);
	GlDump_GetFloatv(GL_CURRENT_INDEX,			"GL_CURRENT_INDEX", 1);
	GlDump_GetFloatv(GL_CURRENT_RASTER_DISTANCE, "GL_CURRENT_RASTER_DISTANCE", 1);
	GlDump_GetFloatv(GL_CURRENT_RASTER_INDEX,	"GL_CURRENT_RASTER_INDEX", 1);
	GlDump_GetFloatv(GL_DEPTH_CLEAR_VALUE,		"GL_DEPTH_CLEAR_VALUE", 1);
	GlDump_GetFloatv(GL_DEPTH_FUNC,				"GL_DEPTH_FUNC", 1);
	GlDump_GetFloatv(GL_DEPTH_SCALE,			"GL_DEPTH_SCALE", 1);
	GlDump_GetFloatv(GL_DRAW_BUFFER,			"GL_DRAW_BUFFER", 1);
	GlDump_GetFloatv(GL_FRONT_FACE,				"GL_FRONT_FACE", 1);
	GlDump_GetFloatv(GL_INDEX_BITS,				"GL_INDEX_BITS", 1);
	GlDump_GetFloatv(GL_INDEX_CLEAR_VALUE,		"GL_INDEX_CLEAR_VALUE", 1);
	GlDump_GetFloatv(GL_LINE_WIDTH,				"GL_LINE_WIDTH", 1);
	GlDump_GetFloatv(GL_LIST_BASE,				"GL_LIST_BASE", 1);
	GlDump_GetFloatv(GL_LIST_INDEX,				"GL_LIST_INDEX", 1);
	GlDump_GetFloatv(GL_LIST_MODE,				"GL_LIST_MODE", 1);
	GlDump_GetFloatv(GL_TEXTURE_ENV_MODE,		"GL_TEXTURE_ENV_MODE", 1);
	GlDump_GetFloatv(GL_TEXTURE_ENV_COLOR,		"GL_TEXTURE_ENV_COLOR", 4);
	GlDump_GetFloatv(GL_COLOR_WRITEMASK,		"GL_COLOR_WRITEMASK", 1);
	GlDump_GetFloatv(GL_CURRENT_COLOR,			"GL_CURRENT_COLOR", 1);
	GlDump_GetFloatv(GL_CURRENT_RASTER_COLOR,	"GL_CURRENT_RASTER_COLOR", 1);
	GlDump_GetFloatv(GL_CURRENT_RASTER_POSITION, "GL_CURRENT_RASTER_POSITION", 1);
	GlDump_GetFloatv(GL_CURRENT_RASTER_TEXTURE_COORDS, "GL_CURRENT_RASTER_TEXTURE_COORDS", 1);
	GlDump_GetFloatv(GL_CURRENT_TEXTURE_COORDS, "GL_CURRENT_TEXTURE_COORDS", 1);
	GlDump_GetFloatv(GL_LIGHT_MODEL_AMBIENT,	"GL_LIGHT_MODEL_AMBIENT", 1);
	GlDump_GetFloatv(GL_DEPTH_RANGE,			"GL_DEPTH_RANGE", 2);
	GlDump_GetFloatv(GL_VIEWPORT,				"GL_VIEWPORT", 4);

	GlDump_GetFloatv(GL_ALPHA_BITS,				"GL_ALPHA_BITS", 1);
	GlDump_GetFloatv(GL_RED_BITS,				"GL_RED_BITS", 1);
	GlDump_GetFloatv(GL_GREEN_BITS,				"GL_GREEN_BITS", 1);
	GlDump_GetFloatv(GL_BLUE_BITS,				"GL_BLUE_BITS", 1);
	GlDump_GetFloatv(GL_DEPTH_BITS,				"GL_DEPTH_BITS", 1);

	cout << "PixelTransfer:" << endl;
	GlDump_GetFloatv(GL_ALPHA_BIAS,				"GL_ALPHA_BIAS", 1);
	GlDump_GetFloatv(GL_ALPHA_SCALE,			"GL_ALPHA_SCALE", 1);
	GlDump_GetFloatv(GL_RED_BIAS,				"GL_RED_BIAS", 1);
	GlDump_GetFloatv(GL_RED_SCALE,				"GL_RED_SCALE", 1);
	GlDump_GetFloatv(GL_BLUE_BIAS,				"GL_BLUE_BIAS", 1);
	GlDump_GetFloatv(GL_BLUE_SCALE,				"GL_BLUE_SCALE", 1);
	GlDump_GetFloatv(GL_GREEN_BIAS,				"GL_GREEN_BIAS", 1);
	GlDump_GetFloatv(GL_GREEN_SCALE,			"GL_GREEN_SCALE", 1);
	GlDump_GetFloatv(GL_DEPTH_BIAS,				"GL_DEPTH_BIAS", 1);

	cout << "Matrix:" << endl;
	GlDump_GetFloatv(GL_MATRIX_MODE,			"GL_MATRIX_MODE", 1);
	GlDump_GetFloatv(GL_PROJECTION_STACK_DEPTH, "GL_PROJECTION_STACK_DEPTH", 1);
	GlDump_GetFloatv(GL_MODELVIEW_STACK_DEPTH,	"GL_MODELVIEW_STACK_DEPTH", 1);

	cout << "Fog:" << endl;
	GlDump_IsEnabled(GL_FOG , "GL_FOG");
	GlDump_GetFloatv(GL_FOG_DENSITY,			"GL_FOG_DENSITY", 1);
	GlDump_GetFloatv(GL_FOG_END,				"GL_FOG_END", 1);
	GlDump_GetFloatv(GL_FOG_HINT,				"GL_FOG_HINT", 1);
	GlDump_GetFloatv(GL_FOG_INDEX,				"GL_FOG_INDEX", 1);
	GlDump_GetFloatv(GL_FOG_MODE,				"GL_FOG_MODE", 1);
	GlDump_GetFloatv(GL_FOG_START,				"GL_FOG_START", 1);
	GlDump_GetFloatv(GL_FOG_COLOR,				"GL_FOG_COLOR", 4);

	cout << "Limits:" << endl;
	GlDump_GetFloatv(GL_MAX_TEXTURE_SIZE,		"GL_MAX_TEXTURE_SIZE", 1);

//	glGetFloatv(XXX, afValues);			cout << "XXX: " << afValues[0] << endl;
//	glGetFloatv(XXX, afValues);		cout << "XXX: " << afValues[0] <<"," << afValues[1] << "," << afValues[2] << "," << afValues[3] << endl;

}


RENDER_API char* GetOpenGLErrorName(GLenum id )
{
	switch(id) 
	{
		case GL_NO_ERROR:			return "GL_NO_ERROR";				break;
		case GL_INVALID_ENUM:		return "GL_INVALID_ENUM ";			break;
		case GL_INVALID_VALUE :		return "GL_INVALID_VALUE ";			break;
		case GL_INVALID_OPERATION:	return "GL_INVALID_OPERATION";		break;
		case GL_STACK_OVERFLOW :	return "GL_STACK_OVERFLOW ";		break;
		case GL_STACK_UNDERFLOW :	return "GL_STACK_UNDERFLOW ";		break;
		case GL_OUT_OF_MEMORY :		return "GL_OUT_OF_MEMORY ";			break;
		//case XXX:		return "XXX";			break;
	}

	return "*** --- ***";
}




RENDER_API void RenderDLL_InitExtGL(void)
{
#ifdef _WIN32
	int res = extgl_Initialize();
	cout << "extgl_Initialize: "<< res << endl;
#endif
}



