#include "render.h"
#include "../ogl/zfpsgl.h"
#include "../basic/basicconsole.h"
 
Render::Render()  
:	ZFSubSystem("Render") , m_eLandscapePolygonMode(FILL) 
{

	// Set Our own local variables.
	m_iSlicesize				= 32;						//grid size of lod tiles
	m_iDetail					= 30;						//height meens greater detail att longer range	
	m_iViewDistance				= 300;
	m_iFpsLock					= 60;
	m_iAutoLod					= 1;
	m_iLodUpdate				= 0;	
	m_kOldCamPos				= Vector3(0,0,0);
	m_iMaxLandscapeLayers	= 4;
	m_iDrawLandscape			= 1;
	m_iScreenShootNum			= 0;
	m_iHmTempList				= 0;
	m_kConsoleColor.Set(1,1,1);
	m_bCapture					= false;

	// The default graphics mode.
	m_iWidth						= 640;
	m_iHeight					= 480;
	m_iDepth						= 16;
	m_iFullScreen				= 0;
	
	// Register Our Own variables.
	RegisterVariable("r_maxlayers",		&m_iMaxLandscapeLayers,CSYS_INT);
	RegisterVariable("r_drawland",		&m_iDrawLandscape,CSYS_INT);
	RegisterVariable("r_landlod",			&m_iDetail,CSYS_INT);
	RegisterVariable("r_viewdistance",	&m_iViewDistance,CSYS_INT);
	RegisterVariable("r_autolod",			&m_iAutoLod,CSYS_INT);
	RegisterVariable("r_fpslock",			&m_iFpsLock,CSYS_INT);

	// Register Commands
	RegisterVariable("r_width",			&m_iWidth,CSYS_INT);
	RegisterVariable("r_height",			&m_iHeight,CSYS_INT);
	RegisterVariable("r_depth",			&m_iDepth,CSYS_INT);
	RegisterVariable("r_fullscreen",		&m_iFullScreen,CSYS_INT);

	// Register Our Own commands.
	Register_Cmd("glinfo",FID_GLINFO);	
	Register_Cmd("ccolor",FID_CONSOLECOLOR);	
	
	
}

bool Render::StartUp()
{
	// Get SubSystem Ptrs
	m_pkTexMan	= static_cast<TextureManager*>(GetSystem().GetObjectPtr("TextureManager"));
 	m_pkLight	= static_cast<Light*>(GetSystem().GetObjectPtr("Light")); 	
 	m_pkZShader = static_cast<ZShader*>(GetSystem().GetObjectPtr("ZShader")); 	 	
 	m_pkConsole = static_cast<BasicConsole*>(GetSystem().GetObjectPtr("Console")); 	 	

	InitDisplay(m_iWidth,m_iHeight,m_iDepth);
//	SetDisplay();
	
	return true;
}

void Render::InitDisplay(int iWidth,int iHeight,int iDepth) 
{
	// Anything sent from app overrides default and ini files.
	if(iWidth || iHeight || iDepth) {
		m_iWidth	= iWidth;
		m_iHeight= iHeight;
		m_iDepth	= iDepth;
		}


	SetDisplay();

	// Set w and h for appliction
	//m_pkApp->m_iWidth = m_iWidth;
	//m_pkApp->m_iHeight = m_iHeight;

#ifdef _WIN32
	RenderDLL_InitExtGL();
	extgl_Initialize();
#endif

	//setup some opengl stuff =)
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);	
	glEnable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);

	glShadeModel(GL_SMOOTH);
	glClearColor(0, 0, 0, 0);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glMatrixMode(GL_MODELVIEW);
  
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	
	//m_pkDefaultCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),90,1.333,0.25,250);
	//m_pkConsoleCamera=new Camera(Vector3(0,0,0),Vector3(0,0,0),84,1.333,0.3,250);	
  
	glMatrixMode(GL_MODELVIEW);

	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
}

void Render::ToggleFullScreen(void)
{
	SDL_WM_ToggleFullScreen(m_pkScreen);
}

void Render::SetDisplay(int iWidth,int iHeight,int iDepth)
{
	m_iWidth=iWidth;
	m_iHeight=iHeight;
	m_iDepth=iDepth;

	SetDisplay();
}


void Render::SetDisplay()
{
	m_pkTexMan->ClearAll();

	//turn of opengl 
	SDL_QuitSubSystem(SDL_OPENGL);

	
	//reinit opengl with the new configuration
	SDL_InitSubSystem(SDL_OPENGL);

//	m_pkScreen= SDL_SetVideoMode(800,600,16,SDL_OPENGL);
	if(m_iFullScreen > 0)
		m_pkScreen= SDL_SetVideoMode(m_iWidth,m_iHeight,m_iDepth,SDL_OPENGL|SDL_FULLSCREEN);	
	else
		m_pkScreen= SDL_SetVideoMode(m_iWidth,m_iHeight,m_iDepth,SDL_OPENGL);


	glViewport(0, 0,m_iWidth,m_iHeight);
	
}

void Render::Swap(void) {
	SDL_GL_SwapBuffers();  //guess

	if(m_bCapture) {
		m_bCapture = false;
		CaptureScreenShoot(m_iWidth, m_iHeight);
		}

	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	

}




void Render::Sphere(Vector3 kPos,float fRadius,int iRes,Vector3 kColor,bool bSolid)
{
	glPushAttrib(GL_ENABLE_BIT|GL_POLYGON_BIT|GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	
	if(bSolid)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);		
	}
	else {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		}

	glColor3f(kColor.x,kColor.y,kColor.z);
	
	glPushMatrix();
		glTranslatef(kPos.x,kPos.y,kPos.z);
		glutSolidSphere(fRadius, iRes, iRes);
	glPopMatrix();
	

	glPopAttrib();
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

void Render::PrintChar(unsigned char cChar) 
{

	int texwidth=FONTWIDTH*16;	
	int pos=int(cChar)*FONTWIDTH;		
	float glu = float(1.0/texwidth);				//opengl texture cordinats is 0-1
	float width=glu*FONTWIDTH;
	
	float y=(float(int(pos/texwidth)*FONTWIDTH)*glu+width);
	float x=float(pos%texwidth)*glu;//+width/2;
	
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glAlphaFunc(GL_GREATER,0.1);
	glEnable(GL_ALPHA_TEST);
 	
 	//m_pkTexMan->BindTexture(aCurentFont,T_NOMIPMAPPING);  
//	RES ResTexture* pkTexture = static_cast<ResTexture*>(m_kConsoleText.GetResourcePtr());
// RES	m_pkTexMan->BindTexture( pkTexture->m_iTextureID );
	m_pkTexMan->BindTexture("data/textures/text/devstr.bmp" ,0 );

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

void Render::PrintChar2(char cChar)
{
	int texwidth	=	FONTWIDTH*16;	
	int pos			=	int(cChar)*FONTWIDTH;		
	float glu		=	float(1.0/texwidth);				//opengl texture cordinats is 0-1
	float width		=	glu*FONTWIDTH;
	
	float y	=	(float(int(pos/texwidth)*FONTWIDTH)*glu+width);
	float x	=	float(pos%texwidth)*glu;//+width/2;

// 	m_pkTexMan->BindTexture(aCurentFont,T_NOMIPMAPPING);  
// RES	ResTexture* pkTexture = static_cast<ResTexture*>(m_kConsoleText.GetResourcePtr());
// RES	m_pkTexMan->BindTexture( pkTexture->m_iTextureID );
	m_pkTexMan->BindTexture("data/textures/text/devstr.bmp" ,0 );


	int iFontSize = 8;
/*
	if(glIsEnabled(GL_LIGHTING))
		cout << "Horrrrrrrrrrraa " << endl;
	glDisable(GL_LIGHT0);	
	glDisable(GL_LIGHT1);	
	glDisable(GL_LIGHT2);	
	glDisable(GL_LIGHT3);	
	glDisable(GL_LIGHT4);	
	glDisable(GL_LIGHT5);	
	glDisable(GL_LIGHT6);	
	glDisable(GL_LIGHT7);	*/	

	glBegin(GL_QUADS);		
		glNormal3f(0,0,1);
 	  
		glTexCoord2f(x,y);				glVertex3i(0,0,0);		 
		glTexCoord2f(x+width,y);		glVertex3i(iFontSize,0,0);		
		glTexCoord2f(x+width,y-width);	glVertex3i(iFontSize,iFontSize,0);    
		glTexCoord2f(x,y-width);		glVertex3i(0,iFontSize,0);    
	glEnd();				
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

void Render::Print2(Vector3 kPos,char* aText) {
	char paText[TEXT_MAX_LENGHT];
	
	strcpy(paText,aText);
	
	glPushMatrix();
		glTranslatef(kPos.x,kPos.y,kPos.z);	
		
		int i=0;
		while(paText[i]!='\0') {
			PrintChar2(paText[i]);
			glTranslatef(8,0,0);
		
			i++;
		}

	glPopMatrix();
}


void Render::SetFont(char* aFont) {
//	strcpy(aCurentFont,aFont);
// RES	m_kConsoleText.SetRes(aFont);
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


void Render::Dot(float x,float y,float z) 
{
	Vector3 kStart(x,y,z);
	Vector3 kEnd = kStart + Vector3(0.05,0.05,0.05);
	Line(kStart, kEnd);
//	Line(Vector3(x,y,z),Vector3(x+0.05,y+0.05,z+0.05));
}

void Render::Mode2D_Start()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 1024, 0, 768);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	SetFont("data/textures/text/devstr.bmp");

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(		GL_BLEND					);
	glDisable(		GL_LIGHTING				);
	glDisable(		GL_ALPHA_TEST			);
	glDepthMask(	GL_FALSE					);	
	glDisable(		GL_CULL_FACE			);
	glDisable(		GL_COLOR_MATERIAL 	);
	glDisable(		GL_FOG					);
}

void Render::Mode2D_End()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();
}



void Render::DrawConsole(char* m_aCommand,vector<char*>* m_kText,int iStartLine) 
{
	Mode2D_Start();

	glColor3f(m_kConsoleColor.x,m_kConsoleColor.y,m_kConsoleColor.z);

	Print2(Vector3(8,8,0),m_aCommand);		
	
	if(iStartLine < 0)
		iStartLine = 0;
	
	int iEndLine = iStartLine + 93;
	if(iEndLine >= (*m_kText).size())
		iEndLine = (*m_kText).size();

	for(int i=iStartLine;	i<iEndLine;	i++) 
	{
		if((*m_kText)[i] != NULL)
		{
			Print2(Vector3( 8.0, float(16.0 + 8.0 * float(i) ),0),(*m_kText)[i]);		
		}
	}
	
	Mode2D_End();
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
		x = float( cos(DegToRad( float(i) )) * fRadius );
		y = float( sin(DegToRad( float(i) )) * fRadius );
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

void Render::DrawPyramid(Vector3 kPos, Vector3 kScale, Vector3 kColor, bool bSolid)
{
	glPushMatrix();
		
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glScalef(kScale.x,kScale.y,kScale.z);
	
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_FOG_BIT | GL_LIGHTING_BIT | 
		GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);	

	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);

	
	if(bSolid)
		glBegin(GL_TRIANGLES);
	else
		glBegin(GL_LINES);

	if(bSolid)
	{	
		float kTopColor[3] = {1,0,1};

		float l[3] = {-0.5,-0.5,-0.5};
		float t[3] = {-0.5,-0.5, 0.5};
		float r[3] = { 0.5,-0.5, 0.5};
		float b[3] = { 0.5,-0.5,-0.5};
		float c[3] = { 0.0, 0.5, 0.0};

		// Base
		glVertex3fv(l); glVertex3fv(r); glVertex3fv(t);	
		glVertex3fv(l); glVertex3fv(b); glVertex3fv(r);

		// Left
		glColor4f(kColor.x,kColor.y,kColor.z,1);
		glVertex3fv(l); glVertex3fv(t); 
		glColor3fv(kTopColor); glVertex3fv(c); 

		// Top
		glColor4f(kColor.x,kColor.y,kColor.z,1);
		glVertex3fv(t); glVertex3fv(r); 
		glColor3fv(kTopColor); glVertex3fv(c); 

		// Right
		glColor4f(kColor.x,kColor.y,kColor.z,1);
		glVertex3fv(r); glVertex3fv(b); 
		glColor3fv(kTopColor); glVertex3fv(c); 

		// Bottom
		glColor4f(kColor.x,kColor.y,kColor.z,1);
		glVertex3fv(b); glVertex3fv(l); 
		glColor3fv(kTopColor); glVertex3fv(c);
	}
	else
	{
		glColor4f(kColor.x,kColor.y,kColor.z,1);

		// Base
		glVertex3f(-0.5,-0.5,-0.5); glVertex3f(-0.5,-0.5, 0.5);
		glVertex3f(-0.5,-0.5, 0.5);	glVertex3f( 0.5,-0.5, 0.5);
		glVertex3f( 0.5,-0.5, 0.5);	glVertex3f( 0.5,-0.5,-0.5);
		glVertex3f( 0.5,-0.5,-0.5);	glVertex3f(-0.5,-0.5,-0.5);

		// Left
		glVertex3f(-0.5,-0.5,-0.5); glVertex3f(-0.5,-0.5, 0.5);
		glVertex3f(-0.5,-0.5, 0.5);	glVertex3f( 0.0, 0.5, 0.0);
		glVertex3f( 0.0, 0.5, 0.0);	glVertex3f(-0.5,-0.5,-0.5);

		// Top
		glVertex3f(-0.5,-0.5, 0.5); glVertex3f( 0.5,-0.5, 0.5);
		glVertex3f( 0.5,-0.5, 0.5);	glVertex3f( 0.0, 0.5, 0.0);
		glVertex3f( 0.0, 0.5, 0.0);	glVertex3f(-0.5,-0.5, 0.5);

		// Right
		glVertex3f( 0.5,-0.5, 0.5); glVertex3f( 0.5,-0.5,-0.5);
		glVertex3f( 0.5,-0.5,-0.5);	glVertex3f( 0.0, 0.5, 0.0);
		glVertex3f( 0.0, 0.5, 0.0);	glVertex3f( 0.5,-0.5, 0.5);

		// Bottom
		glVertex3f( 0.5,-0.5,-0.5); glVertex3f(-0.5,-0.5,-0.5);
		glVertex3f(-0.5,-0.5,-0.5);	glVertex3f( 0.0, 0.5, 0.0);
		glVertex3f( 0.0, 0.5, 0.0);	glVertex3f( 0.5,-0.5,-0.5);

	}

	glEnd();

	glPopMatrix();

	glPopAttrib();
}

void Render::DrawCone(Vector3 kPos, float fRadie, float fHeight, 
					  Vector3 kColor, bool bSolid, int iSegments)
{
	glPushMatrix();
		
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glScalef(fRadie,fHeight,fRadie);
	
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_FOG_BIT | GL_LIGHTING_BIT | 
		GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);	

	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);

	glColor4f(kColor.x,kColor.y,kColor.z,1);

	if(bSolid)
		glBegin(GL_TRIANGLE_FAN);
	else
		glBegin(GL_LINES);

	int i;
	float grad = 0, grad_oka = zf_pi / iSegments, x, z;

	for( i=0; i<iSegments+1; i++)
	{
		x = sinf(grad) * fRadie;
		z = cosf(grad) * fRadie;
		grad += grad_oka;

		if(bSolid)
		{
			if( i == 0)
				glVertex3f( kPos.x, kPos.y+fHeight, kPos.z);
			
			glVertex3f( kPos.x+x, kPos.y, kPos.z+z);
		}
		else
		{
			glVertex3f( kPos.x+x, kPos.y, kPos.z+z); 
			glVertex3f( kPos.x, kPos.y+fHeight, kPos.z);
		}
	}

	if(bSolid)
	{
		glEnd();
		glBegin(GL_TRIANGLES);
		grad = 0;
	}

	for( i=0; i<iSegments+1; i++)
	{
		x = sinf(grad) * fRadie;
		z = cosf(grad) * fRadie;

		if(bSolid)
		{
			glVertex3f( kPos.x, kPos.y, kPos.z);

			float x2 = sinf(grad+grad_oka) * fRadie;
			float z2 = cosf(grad+grad_oka) * fRadie;

			glVertex3f( kPos.x+x2, kPos.y, kPos.z+z2);
			glVertex3f( kPos.x+x, kPos.y, kPos.z+z);
		}
		else
		{
			glVertex3f( kPos.x+x, kPos.y, kPos.z+z); 
		}

		grad += grad_oka;

		if(!bSolid)
		{
			x = sinf(grad) * fRadie;
			z = cosf(grad) * fRadie;	
			glVertex3f( kPos.x+x, kPos.y, kPos.z+z); 
		}
	}

	glEnd();

	glPopMatrix();

	glPopAttrib();
}

void Render::DrawBoundingBox(Vector3 kPos,Vector3 kRot,Vector3 kScale, Vector3 kColor)
{
	glPushMatrix();
		
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glRotatef(kRot.x, 1, 0, 0);
	glRotatef(kRot.y, 0, 1, 0);	
	glRotatef(kRot.z, 0, 0, 1);
	glScalef(kScale.x,kScale.y,kScale.z);
	
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_FOG_BIT | GL_LIGHTING_BIT | 
		GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);

	glColor4f(kColor.x,kColor.y,kColor.z,1);		
		
	Vector3 a(-0.5f, -0.5f, -0.5f);
	Vector3 b(-0.5f,  0.5f, -0.5f);
	Vector3 c( 0.5f,  0.5f, -0.5f);
	Vector3 d( 0.5f, -0.5f, -0.5f);

	Vector3 e(-0.5f, -0.5f,  0.5f);
	Vector3 f(-0.5f,  0.5f,  0.5f);
	Vector3 g( 0.5f,  0.5f,  0.5f);
	Vector3 h( 0.5f, -0.5f,  0.5f);
	
	Line(a,b); 
	Line(b,c); 
	Line(c,d); 
	Line(d,a); 

	Line(e,f); 
	Line(f,g); 
	Line(g,h); 
	Line(h,e); 

	Line(b,f); 
	Line(c,g); 
	Line(d,h); 
	Line(a,e); 
		
	glPopMatrix();

	glPopAttrib();

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

void Render::DrawAABB( Vector3 kMin,Vector3 kMax, Vector3 kColor )
{
	Vector3 kCubeNeg = kMin; 
	Vector3 kCubePos = kMax; 

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING );

	glColor3f(kColor.x,kColor.y,kColor.z);
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

void Render::DrawSolidAABB( Vector3 kMin,Vector3 kMax, Vector3 kColor )
{
	Vector3 kCubeNeg = kMin; 
	Vector3 kCubePos = kMax; 

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING );

	glColor3f(kColor.x,kColor.y,kColor.z);
	glBegin(GL_QUADS);
		glVertex3f(kCubeNeg.x,kCubePos.y,kCubeNeg.z);
		glVertex3f(kCubePos.x,kCubePos.y,kCubeNeg.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubeNeg.z);
		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubeNeg.z);
		
		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubeNeg.x,kCubePos.y,kCubePos.z);

		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubeNeg.z);
		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubePos.z);
		glVertex3f(kCubeNeg.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubeNeg.x,kCubePos.y,kCubeNeg.z);

		glVertex3f(kCubePos.x,kCubePos.y,kCubeNeg.z);
		glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubeNeg.z);

		glVertex3f(kCubePos.x,kCubeNeg.y,kCubeNeg.z);
		glVertex3f(kCubePos.x,kCubeNeg.y,kCubePos.z);
		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubePos.z);
		glVertex3f(kCubeNeg.x,kCubeNeg.y,kCubeNeg.z);

		glVertex3f(kCubeNeg.x,kCubePos.y,kCubeNeg.z);
		glVertex3f(kCubeNeg.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubePos.y,kCubePos.z);
		glVertex3f(kCubePos.x,kCubePos.y,kCubeNeg.z);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);


}


void Render::DrawAABB( float x, float y, float z, float sizex,float sizey,float sizez, Vector3 kColor )
{
	Vector3 kCubeNeg(x - sizex, y - sizey, z - sizez); 
	Vector3 kCubePos(x + sizex, y + sizey, z + sizez); 

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING );

	glColor3f(kColor.x,kColor.y,kColor.z);
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

void Render::Draw_AxisIcon(float scale)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING );


	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);			glVertex3f(scale,0,0);
	glEnd();

	glColor3f(0,1,0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);			glVertex3f(0,scale,0);
	glEnd();

	glColor3f(0,0,1);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);			glVertex3f(0,0,scale);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void Render::Draw_MarkerCross(Vector3 kPos, Vector3 Color, float fScale)
{
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING );
	
	float fHScale = float( cos(float(45.0)) * fScale );

	glTranslatef(kPos.x, kPos.y, kPos.z);

	glColor3fv((float*)&Color);
	glBegin(GL_LINES);
		glVertex3f(-fScale,0,0);			glVertex3f(fScale,0,0);
		glVertex3f(0,-fScale,0);			glVertex3f(0,fScale,0);
		glVertex3f(0,0,-fScale);			glVertex3f(0,0,fScale);

		glVertex3f(-fHScale,fHScale,fHScale);		glVertex3f(fHScale,-fHScale,-fHScale);
		glVertex3f(fHScale,-fHScale,fHScale);		glVertex3f(-fHScale,fHScale,-fHScale);

		glVertex3f(-fHScale,-fHScale,-fHScale);		glVertex3f(fHScale,fHScale,fHScale);
		glVertex3f(fHScale,fHScale,-fHScale);		glVertex3f(-fHScale,-fHScale,fHScale);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void Render::CaptureScreenShoot( int m_iWidth, int m_iHeight )
{
	Image kScreen;
	kScreen.create_empty(m_iWidth, m_iHeight);

	glReadPixels(0,0, m_iWidth,m_iHeight,GL_RGBA, GL_UNSIGNED_BYTE, kScreen.pixels); 

	char szImageName[256];
	sprintf(szImageName, "screen_%d.tga", m_iScreenShootNum);
	m_iScreenShootNum++;
	kScreen.save(szImageName ,false);
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
	bool bFlag = glIsEnabled(iGlEnum);
	cout << szName << " : " << BoolStr(bFlag)	<< endl;

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

void Render::GlInfo()
{
	m_pkConsole->Printf("Gl Version: %s", glGetString(GL_VERSION));
	m_pkConsole->Printf("Gl Vendor:  %s", glGetString(GL_VENDOR));
	m_pkConsole->Printf("Gl Render:  %s", glGetString(GL_RENDERER));
	m_pkConsole->Printf("Gl Extensions:");
	
	int i = 0;
	char szExtName[256];
	unsigned char* pcExt1 = const_cast<unsigned char*>(glGetString(GL_EXTENSIONS));
	char* pcExt = (char*)pcExt1;
	while(sscanf(pcExt, "%s", szExtName) != EOF) {
		m_pkConsole->Printf(" Ext[%d]: %s", i,szExtName);
		i++;
		pcExt += strlen(szExtName) + 1;
		}
}

void Render::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_GLINFO:	GlInfo();	break;
		case FID_CONSOLECOLOR:	m_kConsoleColor.Set(1,0,0);	break;
		}
}

RENDER_API void RenderDLL_InitExtGL(void)
{
#ifdef _WIN32
	int res = extgl_Initialize();
	cout << "extgl_Initialize: "<< res << endl;
#endif
}






