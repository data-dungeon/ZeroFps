#include "render.h"
#include "../ogl/zfpsgl.h"
#include "../basic/basicconsole.h"
#include "../engine_systems/common/psystem.h"
#include "../engine/psystemmanager.h"
 
FILE* pkGlDumpLog;
 
Render::Render()  
:	ZFSubSystem("Render") , m_eLandscapePolygonMode(FILL) 
{

	// Set Our own local variables.
	m_iSlicesize				= 32;						//grid size of lod tiles
	m_iDetail					= 30;						//height meens greater detail att longer range	
	m_iViewDistance			= 300;
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
	m_bShowInputToken			= false;

	// The default graphics mode.
	m_iWidth						= 800;
	m_iHeight					= 600;
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
	Register_Cmd("r_glinfo",	FID_GLINFO);	
	Register_Cmd("r_gldump",	FID_GLDUMP);	
	Register_Cmd("ccolor",		FID_CONSOLECOLOR);
	Register_Cmd("shot",			FID_SHOT);


	SetFont("data/textures/text/devstr.bmp");

	Setup_EditColors();
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

	//reinitialize opengl
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	//setup sdl_gl_attributes, this has to be done before creating the sdl opengl window
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

	SetDisplay();


#ifdef _WIN32
	RenderDLL_InitExtGL();
	//extgl_Initialize();
	GLeeInit();
#endif

	//setup some opengl stuff =)
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);

	glShadeModel(GL_SMOOTH);
	SetClearColor(Vector4(0,0,0,0));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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


	m_iSDLVideoModeFlags = 0;

	if(m_iFullScreen > 0)
		m_iSDLVideoModeFlags = SDL_OPENGL|SDL_FULLSCREEN;
	else
		m_iSDLVideoModeFlags = SDL_OPENGL;

	if( (m_pkScreen= SDL_SetVideoMode(m_iWidth,m_iHeight,m_iDepth, m_iSDLVideoModeFlags)) == NULL)
	{
		cout<<"ERROR: Creating sdl video surface"<<endl;
		return;
	}

	glViewport(0, 0,m_iWidth,m_iHeight);

	m_pkLight->SetStartUpValues();
}

void Render::Swap(void) {
	SDL_GL_SwapBuffers();  //guess

	if(m_bCapture) {
		m_bCapture = false;
		CaptureScreenShoot(m_iWidth, m_iHeight);
		}

	glLoadIdentity();
	
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
//	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_ACCUM_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
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
	else 
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}
	
	
	static float  X = 0.525731112119133606; 
	static float  Z = 0.850650808352039932;
	
	static float vdata[12][3] = {    
	   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   	{0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
	   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
	};
	static int tindices[20][3] = { 
	   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   	{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
	   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   	{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
	
	
	glColor3f(kColor.x,kColor.y,kColor.z);
	
	glPushMatrix();
	
		glTranslatef(kPos.x,kPos.y,kPos.z);
		float scale = fRadius;
		glScalef(scale,scale,scale);
		
		for (int i = 0; i < 20; i++) {    
		   glBegin(GL_TRIANGLES);    

	   	SubDivide(&vdata[tindices[i][2]][0],       
	   		      &vdata[tindices[i][1]][0],       
						&vdata[tindices[i][0]][0],iRes); 

		   glEnd(); 
		}
		
	glPopMatrix();
	glPopAttrib();
}

void Render::Normalize(float v[3]) {    
   float d = float( sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]) ); 
   
   if (d == 0.0) {  

      return;
   }
   v[0] /= d; 
   v[1] /= d; 
   v[2] /= d; 
}

void Render::SubDivide(float *v1, float *v2, float *v3, long depth)
{
   float v12[3], v23[3], v31[3];
   int i;
   if (depth == 0) 
   {
     // drawtriangle(v1, v2, v3);
     
  	   glVertex3fv(v1); 
    	glVertex3fv(v2); 
	   glVertex3fv(v3);      
    
   	return;
   }
   
   for (i = 0; i < 3; i++) 
   {
   	v12[i] = v1[i]+v2[i];
      v23[i] = v2[i]+v3[i];
      v31[i] = v3[i]+v1[i];
   }
   
   Normalize(v12);
   Normalize(v23);
   Normalize(v31);
   
   SubDivide(v1, v12, v31, depth-1);
   SubDivide(v2, v23, v12, depth-1);
   SubDivide(v3, v31, v23, depth-1);
   SubDivide(v12, v23, v31, depth-1);
}

void Render::Polygon4(const Vector3& kP1,const Vector3& kP2,const Vector3& kP3,const Vector3& kP4,const int& iTexture)
{
	glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT);
	
	
	//glAlphaFunc(GL_GREATER,0.3);
	//glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glDisable(GL_LIGHTING);
		
	m_pkTexMan->BindTexture(iTexture);  
	
	glBegin(GL_QUADS);
				
	
	glNormal3f(0,0,1);
   glTexCoord2f(0.0,1.0);glVertex3fv(&kP1.x);		 
   glTexCoord2f(1.0,1.0);glVertex3fv(&kP2.x);		 
 	glTexCoord2f(1.0,0.0);glVertex3fv(&kP3.x);		 
	glTexCoord2f(0.0,0.0);glVertex3fv(&kP4.x);		 
	
	glEnd();			

		
	glPopAttrib();
	glPopMatrix();
}

void Render::Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture, Vector3 kColor){
	glPushMatrix();
		
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);
	glScalef(kScale.x,kScale.y,kScale.z);

	glEnable(GL_COLOR_MATERIAL);

//	glBlendFunc(GL_ONE,GL_ZERO);
 		
  	m_pkTexMan->BindTexture(iTexture);  

	glColor4f(kColor.x, kColor.y, kColor.z ,1.0);  	  

	glBegin(GL_QUADS);			
	glNormal3f(0,0,1);
   glTexCoord2f(0.0,1.0);glVertex3f(-.5,-0.5,0);		 
   glTexCoord2f(1.0,1.0);glVertex3f(.5,-0.5,0);		
 	glTexCoord2f(1.0,0.0);glVertex3f(.5,0.5,0);    
	glTexCoord2f(0.0,0.0);glVertex3f(-0.5,0.5,0);    
	glEnd();			

	glDisable(GL_COLOR_MATERIAL);

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
	m_pkTexMan->BindTexture(aCurentFont ,T_NOMIPMAPPING );

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
	m_pkTexMan->BindTexture(aCurentFont ,T_NOMIPMAPPING );


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


void Render::Print(Vector3 kPos,Vector3 kScale,char* aText) {
	char paText[TEXT_MAX_LENGHT];
	// VIM: Check for maxlen
	strcpy(paText,aText);
	
	glPushMatrix();
		
		glTranslatef(kPos.x,kPos.y,kPos.z);	

		//make billboard		
		Matrix4 kModelMatrix;
		glGetFloatv(GL_MODELVIEW_MATRIX, &kModelMatrix[0]);		
		
		kModelMatrix.data[0] = 1;
		kModelMatrix.data[4] = 0;		
		kModelMatrix.data[8] = 0;		
		
		kModelMatrix.data[1] = 0;		
		kModelMatrix.data[5] = 1;		
		kModelMatrix.data[9] = 0;				
		
		kModelMatrix.data[2] = 0;		
		kModelMatrix.data[6] = 0;		
		kModelMatrix.data[10] = 1;				
				
		glLoadMatrixf(&kModelMatrix[0]);		
		//blub
		
		//scale
		glScalef(kScale.x,kScale.y,kScale.z);
		
		//center text
		int offset = strlen(paText)/2;		
		glTranslatef(float(-offset),0,0);
		
		
		int i=0;
		while(paText[i]!='\0') {
			PrintChar(paText[i]);
			glTranslatef(1,0,0);
		
			i++;
		}

	glPopMatrix();
}


void Render::Print(Vector3 kPos,Vector3 kHead,Vector3 kScale,char* aText) {
	char paText[TEXT_MAX_LENGHT];
	// VIM: Check for maxlen
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
	strcpy(aCurentFont,aFont);
// RES	m_kConsoleText.SetRes(aFont);
}

void Render::Line(Vector3 kPos1,Vector3 kPos2)
{
	glDisable(GL_LIGHTING );
	glDisable(GL_TEXTURE_2D);
	
	glBegin(GL_LINES);
		glVertex3f(kPos1.x,kPos1.y,kPos1.z);
		glVertex3f(kPos2.x,kPos2.y,kPos2.z);		
	glEnd();
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

}

void Render::SetColor(Vector3 kColor)
{
	glColor3f(kColor.x,kColor.y,kColor.z);
}

void Render::SetClearColor(Vector4 kColor)
{
	glClearColor(kColor.x, kColor.y,kColor.z, kColor.w);

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
	gluOrtho2D(0, 800, 0, 600);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

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



void Render::DrawConsole(char* m_aCommand,vector<char*>* m_kText,int iStartLine, int iMarkerPos, int iMarker) 
{
	Mode2D_Start();
	SetFont("data/textures/text/devstr.bmp");

	glColor3f(m_kConsoleColor.x,m_kConsoleColor.y,m_kConsoleColor.z);

	Print2(Vector3(8,8,0),m_aCommand);		
	
	char kMarker[3];
	if(iMarker >= 0) {
		kMarker[0] = iMarker;
		kMarker[1] = 0;
		//if(m_bShowInputToken)
		Print2(Vector3( float(8+iMarkerPos*8), float(8), 0), kMarker);		
		//m_bShowInputToken = !m_bShowInputToken;
		}

	if(iStartLine < 0)
		iStartLine = 0;
	
	unsigned int iEndLine = iStartLine + 93;
	if(iEndLine >= (*m_kText).size())
		iEndLine = (*m_kText).size();

	for(unsigned int i=iStartLine;	i<iEndLine;	i++) 
	{
		if((*m_kText)[i] != NULL)
		{
			Print2(Vector3( 8.0, float(16.0 + 8.0 * float(i) ),0),(*m_kText)[i]);		
		}
	}
	
	Mode2D_End();
}

void Render::DrawBillboard(Matrix4& kModelMatrix,Vector3& kPos,float fSize,int iTexture) 
{
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

void Render::DrawCircle(vector<Vector3> kCircel, Vector3 kColor)
{
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glColor3f(kColor.x,kColor.y,kColor.z);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D );

	glBegin(GL_LINE_LOOP );
	for(unsigned int i=0; i<kCircel.size(); i++) {
		glVertex3f(kCircel[i].x,kCircel[i].y, kCircel[i].z);
		
		}

	glEnd();
	glPopAttrib();
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
		
		//bottom
		glNormal3f(0,-1,0);
		glTexCoord2f(0.0,1.0);glVertex3f(-0.5  ,-0.5	,-0.5);		 
		glTexCoord2f(1.0,1.0);glVertex3f(0.5	,-0.5	,-0.5);		
		glTexCoord2f(1.0,0.0);glVertex3f(0.5	,-0.5	 ,0.5);    
		glTexCoord2f(0.0,0.0);glVertex3f(-0.5  ,-0.5	 ,0.5);    		
		
		
	glEnd();			
	
	glPopMatrix();

}

void Render::DrawBox(Vector3 kPos, Vector3 kCenter, Matrix4 kRot, Vector3 kSize,int iTexture)
{
	glPushMatrix();

	glDisable (GL_CULL_FACE);

	Vector3 kEdge[8];

	kSize = kSize / 2.f;

	kEdge[0] = Vector3( kSize.x, -kSize.y, -kSize.z );
	kEdge[1] = Vector3( -kSize.x, -kSize.y, -kSize.z );
	kEdge[2] = Vector3( -kSize.x, -kSize.y, kSize.z );
	kEdge[3] = Vector3( kSize.x, -kSize.y, kSize.z );
	kEdge[4] = Vector3( kSize.x, kSize.y, -kSize.z );
	kEdge[5] = Vector3( -kSize.x, kSize.y, -kSize.z );
	kEdge[6] = Vector3( -kSize.x, kSize.y, kSize.z );
	kEdge[7] = Vector3( kSize.x, kSize.y, kSize.z );

	for ( int i = 0; i < 8; i++ )
	{	
		kEdge[i] = kRot.VectorRotate ( kEdge[i] + kCenter );
		kEdge[i] += kPos;
	}

	m_pkTexMan->BindTexture(iTexture);
	glColor4f(1,1,1,1);		

		
	glBegin(GL_QUADS);			
		
		//front
		glNormal3f(0,0,1);
		glTexCoord2f(0.0,1.0);glVertex3f(kEdge[3].x, kEdge[3].y, kEdge[3].z);		 
		glTexCoord2f(1.0,1.0);glVertex3f(kEdge[2].x, kEdge[2].y, kEdge[2].z);		
		glTexCoord2f(1.0,0.0);glVertex3f(kEdge[1].x, kEdge[1].y, kEdge[1].z);    
		glTexCoord2f(0.0,0.0);glVertex3f(kEdge[0].x, kEdge[0].y, kEdge[0].z);    
	
		//back
		glNormal3f(0,0,-1);
		glTexCoord2f(0.0,0.0);glVertex3f(kEdge[3].x, kEdge[3].y, kEdge[3].z);    		
		glTexCoord2f(1.0,0.0);glVertex3f(kEdge[7].x, kEdge[7].y, kEdge[7].z);    		
		glTexCoord2f(1.0,1.0);glVertex3f(kEdge[6].x, kEdge[6].y, kEdge[6].z);			
		glTexCoord2f(0.0,1.0);glVertex3f(kEdge[2].x, kEdge[2].y, kEdge[2].z);		 
	
		//right
		glNormal3f(1,0,0);
		glTexCoord2f(0.0,0.0);glVertex3f(kEdge[0].x, kEdge[0].y, kEdge[3].z);    		
		glTexCoord2f(1.0,0.0);glVertex3f(kEdge[4].x, kEdge[4].y, kEdge[4].z);    		
		glTexCoord2f(1.0,1.0);glVertex3f(kEdge[7].x, kEdge[7].y, kEdge[7].z);			
		glTexCoord2f(0.0,1.0);glVertex3f(kEdge[3].x, kEdge[3].y, kEdge[3].z);		 

		//left
		glNormal3f(-1,0,0);		
		glTexCoord2f(0.0,0.0);glVertex3f(kEdge[4].x, kEdge[4].y, kEdge[4].z);    		
		glTexCoord2f(1.0,0.0);glVertex3f(kEdge[7].x, kEdge[7].y, kEdge[7].z);    		
		glTexCoord2f(1.0,1.0);glVertex3f(kEdge[6].x, kEdge[6].y, kEdge[6].z);			
		glTexCoord2f(0.0,1.0);glVertex3f(kEdge[5].x, kEdge[5].y, kEdge[5].z);		 
		
		//top
		glNormal3f(0,1,0);
		glTexCoord2f(0.0,0.0);glVertex3f(kEdge[5].x, kEdge[5].y, kEdge[5].z);    		
		glTexCoord2f(1.0,0.0);glVertex3f(kEdge[6].x, kEdge[6].y, kEdge[6].z);    		
		glTexCoord2f(1.0,1.0);glVertex3f(kEdge[2].x, kEdge[2].y, kEdge[2].z);			
		glTexCoord2f(0.0,1.0);glVertex3f(kEdge[1].x, kEdge[1].y, kEdge[1].z);		 
		
		//bottom
		glNormal3f(0,-1,0);
		glTexCoord2f(0.0,0.0);glVertex3f(kEdge[0].x, kEdge[0].y, kEdge[0].z);    		
		glTexCoord2f(1.0,0.0);glVertex3f(kEdge[4].x, kEdge[4].y, kEdge[4].z);    		
		glTexCoord2f(1.0,1.0);glVertex3f(kEdge[5].x, kEdge[5].y, kEdge[5].z);			
		glTexCoord2f(0.0,1.0);glVertex3f(kEdge[1].x, kEdge[1].y, kEdge[1].z);		 
		
		
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

void Render::DrawAABB( Vector3 kMin,Vector3 kMax, Vector3 kColor, float fLineSize )
{
	Vector3 kCubeNeg = kMin; 
	Vector3 kCubePos = kMax; 

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING );

	glLineWidth( fLineSize );

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

	glLineWidth( 1.0 );
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
	glPushAttrib(GL_ENABLE_BIT|GL_POLYGON_BIT|GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
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
	glPopAttrib();
	glPopMatrix();
}

void Render::CaptureScreenShoot( int m_iWidth, int m_iHeight )
{
	Image kScreen;
	kScreen.CreateEmpty(m_iWidth, m_iHeight);

	glReadPixels(0,0, m_iWidth,m_iHeight,GL_RGBA, GL_UNSIGNED_BYTE, kScreen.m_pkPixels); 

	char szImageName[256];
	sprintf(szImageName, "screen_%d.tga", m_iScreenShootNum);
	m_iScreenShootNum++;
	kScreen.Save(szImageName ,false);
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
	bool bFlag = false;
	if(glIsEnabled(iGlEnum) == GL_TRUE)
		bFlag = true;

	fprintf(pkGlDumpLog, "%s : %s \n", szName, BoolStr(bFlag) );
//	cout << szName << " : " << BoolStr(bFlag)	<< endl;
}

void GlDump_GetPointv(int iGlEnum, char* szName)
{
	GLvoid *pkValue;
	glGetPointerv(iGlEnum, (GLvoid**)&(pkValue));

	fprintf(pkGlDumpLog, "%s : %d \n", szName, pkValue );
//	cout << szName << " : " << pkValue << endl;
}

void GlDump_GetIntv(int iGlEnum, char* szName, int iValues)
{
	int i;

	int afValues[16];
	for( i=0; i<16; i++)
		afValues[i] = -12345678;

	glGetIntegerv(iGlEnum, afValues);	

	fprintf(pkGlDumpLog, "%s : ", szName);
	//cout << szName << " : ";
	for( i=0; i<iValues; i++) {
		fprintf(pkGlDumpLog, "%d : ", afValues[i]);
		//cout << afValues[i] << " ";
		}
	fprintf(pkGlDumpLog, "\n");
//	cout << endl;
}

char* GetOpenGLDefine(int iNum)
{
	char* szName;
	szName = "Unknown";

	switch(iNum) {
		case GL_FASTEST:				szName = "GL_FASTEST";					break;
		case GL_NICEST:				szName = "GL_NICEST";					break;
		case GL_DONT_CARE:			szName = "GL_DONT_CARE";				break;

		case GL_NEVER:				szName = "GL_NEVER";						break;
		case GL_LESS:				szName = "GL_LESS";						break;
		case GL_LEQUAL:			szName = "GL_LEQUAL";					break;
		case GL_EQUAL:				szName = "GL_EQUAL";						break;
		case GL_GREATER:			szName = "GL_GREATER";					break;
		case GL_NOTEQUAL:			szName = "GL_NOTEQUAL";					break;
		case GL_GEQUAL:			szName = "GL_GEQUAL";					break;
		case GL_ALWAYS:			szName = "GL_ALWAYS";					break;

		case GL_ZERO:						szName = "GL_ZERO";						break;
		case GL_ONE:						szName = "GL_ONE";						break;
		case GL_SRC_COLOR:				szName = "GL_SRC_COLOR";				break;
		case GL_ONE_MINUS_SRC_COLOR:	szName = "GL_ONE_MINUS_SRC_COLOR";	break;
		case GL_DST_COLOR:				szName = "GL_DST_COLOR";				break;
		case GL_ONE_MINUS_DST_COLOR:	szName = "GL_ONE_MINUS_DST_COLOR";	break;
		case GL_SRC_ALPHA:				szName = "GL_SRC_ALPHA";				break;
		case GL_ONE_MINUS_SRC_ALPHA:	szName = "GL_ONE_MINUS_SRC_ALPHA";	break;
		case GL_DST_ALPHA:				szName = "GL_DST_ALPHA";				break;
		case GL_ONE_MINUS_DST_ALPHA:	szName = "GL_ONE_MINUS_DST_ALPHA";	break;
		case GL_SRC_ALPHA_SATURATE:	szName = "GL_SRC_ALPHA_SATURATE";	break;

		case GL_FRONT:						szName = "GL_FRONT";	break;
		case GL_BACK:						szName = "GL_BACK";	break;
		case GL_FRONT_AND_BACK:			szName = "GL_FRONT_AND_BACK";	break;
		case GL_CW:							szName = "GL_CW";	break;
		case GL_CCW:						szName = "GL_CCW";	break;
		
		case GL_POINT:						szName = "GL_POINT";	break;
		case GL_LINE:						szName = "GL_LINE";	break;
		case GL_FILL:						szName = "GL_FILL";	break;
	}

	return szName;
}

void GlDump_GetFloatvName(int iGlEnum, char* szName, int iValues)
{
	int i;

	float afValues[16];
	for( i=0; i<16; i++)
		afValues[i] = -12345678;

	glGetFloatv(iGlEnum, afValues);	

//	cout << szName << " : ";
	fprintf(pkGlDumpLog, "%s : ", szName);
	for( i=0; i<iValues; i++) {
		fprintf(pkGlDumpLog, "%s : ", GetOpenGLDefine(int(afValues[i])));
		//cout <<  << " ";
		}
	fprintf(pkGlDumpLog, "\n");
//	cout << endl;

}

void GlDump_GetFloatv(int iGlEnum, char* szName, int iValues)
{
	int i;

	float afValues[16];
	for( i=0; i<16; i++)
		afValues[i] = -12345678;

	glGetFloatv(iGlEnum, afValues);	

//	cout << szName << " : ";
	fprintf(pkGlDumpLog, "%s : ", szName);
	for( i=0; i<iValues; i++) {
		fprintf(pkGlDumpLog, "%f : ", afValues[i]);
		//cout <<  << " ";
		}
	fprintf(pkGlDumpLog, "\n");
//	cout << endl;

}

void GlDump_GetMatrixv(int iGlEnum, char* szName, int iValues)
{
	int i;

	Matrix4 akValues[64];
	for( i=0; i<64; i++)
		akValues[i].Identity();

	glGetFloatv(iGlEnum, (float*)&akValues[0].data[0]);	

	fprintf(pkGlDumpLog, "%s : ", szName);
	for( i=0; i<iValues; i++) {
		fprintf(pkGlDumpLog, "\n");

		for(int y=0;y<4;y++){
			for(int x=0;x<4;x++){
				fprintf(pkGlDumpLog, "%d\t", akValues[i].data[y*4+x]);
			}
			fprintf(pkGlDumpLog, "\n");
		}
		}
	fprintf(pkGlDumpLog, "\n");
}



void Render::DumpGLState(char* szFileName)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);//disable for the first texture
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE  );	// GL_MODULATE	 GL_REPLACE

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE );	

	pkGlDumpLog = fopen(szFileName,"wt");
	
	// Get and Dump OpenGL State to log.
	//cout << "-------------------------------------------------------------------" << endl;
	//cout << "Render::DumpGLSTate" << endl;

	// NNNNNNNNNNNEEEEEEEEEEEEEEEEEEEEEEWWWWWWWWWWWWWWWWWWWW

	// 6.5 Current Values and Associated Data
	fprintf(pkGlDumpLog, "\n\nCurrent Values and Associated Data: \n");
	GlDump_GetFloatv(GL_CURRENT_COLOR,					"GL_CURRENT_COLOR",				4);				// Current color.
	GlDump_GetFloatv(GL_CURRENT_SECONDARY_COLOR,		"GL_CURRENT_SECONDARY_COLOR",	4);				// Current Secondary color.
	GlDump_GetFloatv(GL_CURRENT_INDEX,					"GL_CURRENT_INDEX",				1);				// Current color index.
	GlDump_GetFloatv(GL_CURRENT_TEXTURE_COORDS,		"GL_CURRENT_TEXTURE_COORDS",	4);				// Current texture coordinates.
	GlDump_GetFloatv(GL_CURRENT_NORMAL,					"GL_CURRENT_NORMAL",				3);				// Current normal.
	// CURRENT FOG COO
	GlDump_GetFloatv(GL_CURRENT_RASTER_POSITION,		"GL_CURRENT_RASTER_POSITION", 4);				// Current raster position
	GlDump_GetFloatv(GL_CURRENT_RASTER_DISTANCE,		"GL_CURRENT_RASTER_DISTANCE", 1);				// Current raster distance
	GlDump_GetFloatv(GL_CURRENT_RASTER_COLOR,			"GL_CURRENT_RASTER_COLOR",		4);				// Color associated with raster position
	GlDump_GetFloatv(GL_CURRENT_RASTER_INDEX,			"GL_CURRENT_RASTER_INDEX",		1);				// Color index associated with raster position
	GlDump_GetFloatv(GL_CURRENT_RASTER_TEXTURE_COORDS, "GL_CURRENT_RASTER_TEXTURE_COORDS", 4);	// Texture coordinates associated with raster position
	GlDump_IsEnabled(GL_CURRENT_RASTER_POSITION_VALID , "GL_CURRENT_RASTER_POSITION_VALID");		// Raster position valid bit
	GlDump_IsEnabled(GL_EDGE_FLAG , "GL_EDGE_FLAG");															// Edge flag



	fprintf(pkGlDumpLog, "\n\nVertex Array Data: \n");
	//		CLIENT ACTIVE TEXTURE
	GlDump_IsEnabled(GL_VERTEX_ARRAY ,			"GL_VERTEX_ARRAY");								// Vertex array enable
	GlDump_GetIntv(GL_VERTEX_ARRAY_SIZE,		"GL_VERTEX_ARRAY_SIZE",		1);				// Coordinates per vertex
	GlDump_GetIntv(GL_VERTEX_ARRAY_TYPE,		"GL_VERTEX_ARRAY_TYPE",		4);				// Type of vertex coordinates
	GlDump_GetIntv(GL_VERTEX_ARRAY_STRIDE,		"GL_VERTEX_ARRAY_STRIDE",	1);				// Stride between vertices
	GlDump_GetPointv(GL_VERTEX_ARRAY_POINTER, "GL_VERTEX_ARRAY_POINTER");					// Pointer to the vertex array

	GlDump_IsEnabled(GL_NORMAL_ARRAY ,			"GL_NORMAL_ARRAY");								// Vertex array enable
	GlDump_GetIntv(GL_NORMAL_ARRAY_TYPE,		"GL_NORMAL_ARRAY_TYPE",		4);				// Type of vertex coordinates
	GlDump_GetIntv(GL_NORMAL_ARRAY_STRIDE,		"GL_NORMAL_ARRAY_STRIDE",	1);				// Stride between vertices
	GlDump_GetPointv(GL_NORMAL_ARRAY_POINTER, "GL_NORMAL_ARRAY_POINTER");					// Pointer to the vertex array

	// FOG COO ARRAY

	GlDump_IsEnabled(GL_COLOR_ARRAY ,			"GL_COLOR_ARRAY");								// Vertex array enable
	GlDump_GetIntv(GL_COLOR_ARRAY_SIZE,			"GL_COLOR_ARRAY_SIZE",		1);				// Coordinates per vertex
	GlDump_GetIntv(GL_COLOR_ARRAY_TYPE,			"GL_COLOR_ARRAY_TYPE",		4);				// Type of vertex coordinates
	GlDump_GetIntv(GL_COLOR_ARRAY_STRIDE,		"GL_COLOR_ARRAY_STRIDE",	1);				// Stride between vertices
	GlDump_GetPointv(GL_COLOR_ARRAY_POINTER,	"GL_COLOR_ARRAY_POINTER");						// Pointer to the vertex array

	// SECONDARY COLOR

	GlDump_IsEnabled(GL_INDEX_ARRAY ,			"GL_INDEX_ARRAY");								// Vertex array enable
	GlDump_GetIntv(GL_INDEX_ARRAY_TYPE,			"GL_INDEX_ARRAY_TYPE",		4);				// Type of vertex coordinates
	GlDump_GetIntv(GL_INDEX_ARRAY_STRIDE,		"GL_INDEX_ARRAY_STRIDE",	1);				// Stride between vertices
	GlDump_GetPointv(GL_INDEX_ARRAY_POINTER,	"GL_INDEX_ARRAY_POINTER");						// Pointer to the vertex array

	GlDump_IsEnabled(GL_TEXTURE_COORD_ARRAY ,				"GL_TEXTURE_COORD_ARRAY");					// Vertex array enable
	GlDump_GetIntv(GL_TEXTURE_COORD_ARRAY_SIZE,			"GL_TEXTURE_COORD_ARRAY_SIZE", 1);		// Coordinates per vertex
	GlDump_GetIntv(GL_TEXTURE_COORD_ARRAY_TYPE,			"GL_TEXTURE_COORD_ARRAY_TYPE", 4);		// Type of vertex coordinates
	GlDump_GetIntv(GL_TEXTURE_COORD_ARRAY_STRIDE,		"GL_TEXTURE_COORD_ARRAY_STRIDE", 1);	// Stride between vertices
	GlDump_GetPointv(GL_TEXTURE_COORD_ARRAY_POINTER,	"GL_TEXTURE_COORD_ARRAY_POINTER");		// Pointer to the vertex array

	GlDump_IsEnabled(GL_EDGE_FLAG_ARRAY ,			"GL_EDGE_FLAG_ARRAY");								// Vertex array enable
	GlDump_GetIntv(GL_EDGE_FLAG_ARRAY_STRIDE,		"GL_EDGE_FLAG_ARRAY_STRIDE",	1);				// Stride between vertices
	GlDump_GetPointv(GL_EDGE_FLAG_ARRAY_POINTER,	"GL_EDGE_FLAG_ARRAY_POINTER");					// Pointer to the vertex array

	// 6.8 Transformation state
	fprintf(pkGlDumpLog, "\n\nTransformation state: \n");
	GlDump_GetMatrixv(GL_MODELVIEW_MATRIX,			"GL_MODELVIEW_MATRIX",			1 );
	GlDump_GetMatrixv(GL_PROJECTION_MATRIX,		"GL_PROJECTION_MATRIX",			1 );
	GlDump_GetMatrixv(GL_TEXTURE_MATRIX,			"GL_TEXTURE_MATRIX",				1 );
	GlDump_GetFloatv(GL_VIEWPORT,						"GL_VIEWPORT",						4);
	GlDump_GetFloatv(GL_DEPTH_RANGE,					"GL_DEPTH_RANGE",					2);
	GlDump_GetFloatv(GL_MODELVIEW_STACK_DEPTH,	"GL_MODELVIEW_STACK_DEPTH",	1);
	GlDump_GetFloatv(GL_PROJECTION_STACK_DEPTH,	"GL_PROJECTION_STACK_DEPTH",	1);
	GlDump_GetFloatv(GL_TEXTURE_STACK_DEPTH,		"GL_TEXTURE_STACK_DEPTH",		1);
	GlDump_GetFloatv(GL_MATRIX_MODE,					"GL_MATRIX_MODE",					1);
	GlDump_IsEnabled(GL_NORMALIZE ,					"GL_NORMALIZE");
	GlDump_IsEnabled(GL_RESCALE_NORMAL ,			"GL_RESCALE_NORMAL");
	// CLIP PLANEi			GetClipPlane
	// CLIP PLANEi			IsEnabled

	// 6.9 Coloring
	fprintf(pkGlDumpLog, "\n\nColoring: \n");
	GlDump_GetFloatv(GL_FOG_COLOR,			"GL_FOG_COLOR", 4);
	GlDump_GetFloatv(GL_FOG_INDEX,			"GL_FOG_INDEX", 1);
	GlDump_GetFloatv(GL_FOG_DENSITY,			"GL_FOG_DENSITY", 1);
	GlDump_GetFloatv(GL_FOG_START,			"GL_FOG_START", 1);
	GlDump_GetFloatv(GL_FOG_END,				"GL_FOG_END", 1);
	GlDump_GetFloatv(GL_FOG_MODE,				"GL_FOG_MODE", 1);
	GlDump_IsEnabled(GL_FOG ,					"GL_FOG");
	// FOG COORDINATE SOURCE
	// COLOR SUM
	GlDump_GetIntv(GL_SHADE_MODEL,			"GL_SHADE_MODEL", 1);			

	// Lighting 6.10
	fprintf(pkGlDumpLog, "\n\nLighting: \n");
	GlDump_IsEnabled(GL_LIGHTING ,						"GL_LIGHTING");
	GlDump_IsEnabled(GL_COLOR_MATERIAL ,				"GL_COLOR_MATERIAL");
	GlDump_GetFloatv(GL_COLOR_MATERIAL_PARAMETER,	"GL_COLOR_MATERIAL_PARAMETER", 1);
	GlDump_GetFloatv(GL_COLOR_MATERIAL_FACE,			"GL_COLOR_MATERIAL_FACE", 1);
	//	AMBIENT
	//	DIFFUSE
	//	SPECULAR
	//	EMISSION
	//	SHININESS
	GlDump_GetFloatv(GL_LIGHT_MODEL_AMBIENT,			"GL_LIGHT_MODEL_AMBIENT", 4);
	GlDump_IsEnabled(GL_LIGHT_MODEL_LOCAL_VIEWER ,	"GL_LIGHT_MODEL_LOCAL_VIEWER");
	GlDump_IsEnabled(GL_LIGHT_MODEL_TWO_SIDE ,		"GL_LIGHT_MODEL_TWO_SIDE");
	//LIGHT_MODEL_COLOR_CONTROL
	//	COLOR INDEXES

	// Dump State of all 8 Std OpenGl Lights.
	int i;
	float afParameters[8];
	int	iMaxLights;
	glGetIntegerv(GL_MAX_LIGHTS, &iMaxLights);	
	
	for(i=0; i<iMaxLights; i++) {
		fprintf(pkGlDumpLog, "Light %d \n", i);
		GlDump_IsEnabled(GL_LIGHT0 + i , " Enabled");
		
		glGetLightfv(GL_LIGHT0+i, GL_AMBIENT, afParameters);
			fprintf(pkGlDumpLog, " GL_AMBIENT: <%f,%f,%f,%f> \n", afParameters[0],afParameters[1],afParameters[2],afParameters[3]);
		glGetLightfv(GL_LIGHT0+i, GL_DIFFUSE, afParameters);
			fprintf(pkGlDumpLog, " GL_DIFFUSE: <%f,%f,%f,%f> \n", afParameters[0],afParameters[1],afParameters[2],afParameters[3]);
		glGetLightfv(GL_LIGHT0+i, GL_SPECULAR, afParameters);
			fprintf(pkGlDumpLog, " GL_SPECULAR: <%f,%f,%f,%f> \n", afParameters[0],afParameters[1],afParameters[2],afParameters[3]);
		glGetLightfv(GL_LIGHT0+i, GL_POSITION, afParameters);
			fprintf(pkGlDumpLog, " GL_POSITION: <%f,%f,%f,%f> \n", afParameters[0],afParameters[1],afParameters[2],afParameters[3]);
		glGetLightfv(GL_LIGHT0+i, GL_CONSTANT_ATTENUATION, afParameters);
			fprintf(pkGlDumpLog, " GL_CONSTANT_ATTENUATION: <%f> \n", afParameters[0]);
		glGetLightfv(GL_LIGHT0+i, GL_LINEAR_ATTENUATION , afParameters);
			fprintf(pkGlDumpLog, " GL_LINEAR_ATTENUATION : <%f> \n", afParameters[0]);
		glGetLightfv(GL_LIGHT0+i, GL_QUADRATIC_ATTENUATION , afParameters);
			fprintf(pkGlDumpLog, " GL_QUADRATIC_ATTENUATION : <%f> \n", afParameters[0]);
		glGetLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, afParameters);
			fprintf(pkGlDumpLog, " GL_SPOT_DIRECTION: <%f,%f,%f> \n", afParameters[0],afParameters[1],afParameters[2]);
		glGetLightfv(GL_LIGHT0+i, GL_SPOT_EXPONENT , afParameters);
			fprintf(pkGlDumpLog, " GL_SPOT_EXPONENT : <%f> \n", afParameters[0]);
		glGetLightfv(GL_LIGHT0+i, GL_SPOT_CUTOFF , afParameters);
			fprintf(pkGlDumpLog, " GL_SPOT_CUTOFF : <%f> \n", afParameters[0]);

		}

	// Rasterization 6.12
	fprintf(pkGlDumpLog, "\n\nRasterization: \n");
	GlDump_GetFloatv(GL_POINT_SIZE,			"GL_POINT_SIZE",			1);
	GlDump_IsEnabled(GL_POINT_SMOOTH ,		"GL_POINT_SMOOTH");
	GlDump_GetFloatv(GL_POINT_SIZE_MIN,		"GL_POINT_SIZE_MIN",		1);
	GlDump_GetFloatv(GL_POINT_SIZE_MAX,		"GL_POINT_SIZE_MAX",		1);
	// POINT FADE THRESHOLD SIZE
	// POINT DISTANCE ATTENUATION
	GlDump_GetFloatv(GL_LINE_WIDTH,			"GL_LINE_WIDTH",			1);
	GlDump_IsEnabled(GL_LINE_SMOOTH ,		"GL_LINE_SMOOTH");
	// LINE STIPPLE PATTERN
	// LINE STIPPLE REPEAT
	GlDump_IsEnabled(GL_LINE_STIPPLE ,		"GL_LINE_STIPPLE");
	GlDump_IsEnabled(GL_CULL_FACE ,			"GL_CULL_FACE");
	GlDump_GetFloatvName(GL_CULL_FACE_MODE,		"GL_CULL_FACE_MODE",		1);
	GlDump_GetFloatvName(GL_FRONT_FACE,		"GL_FRONT_FACE",					1);
	GlDump_IsEnabled(GL_POLYGON_SMOOTH ,	"GL_POLYGON_SMOOTH");
	GlDump_GetFloatvName(GL_POLYGON_MODE,		"GL_POLYGON_MODE",			2);
	// POLYGON OFFSET FACTOR
	// POLYGON OFFSET UNITS
	// POLYGON OFFSET POINT
	// POLYGON OFFSET LINE
	// POLYGON OFFSET FILL
	GlDump_IsEnabled(GL_POLYGON_STIPPLE , "GL_POLYGON_STIPPLE");


	// 6.13 Multisampling 
	GlDump_IsEnabled(GL_MULTISAMPLE ,						"GL_MULTISAMPLE");
	GlDump_IsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE ,		"GL_SAMPLE_ALPHA_TO_COVERAGE");
	GlDump_IsEnabled(GL_SAMPLE_ALPHA_TO_ONE,				"GL_SAMPLE_ALPHA_TO_ONE");
	GlDump_IsEnabled(GL_SAMPLE_COVERAGE ,					"GL_SAMPLE_COVERAGE");
	GlDump_GetFloatv(GL_SAMPLE_COVERAGE_VALUE,			"GL_SAMPLE_COVERAGE_VALUE",			1);
	GlDump_IsEnabled(GL_SAMPLE_COVERAGE_INVERT ,			"GL_SAMPLE_COVERAGE_INVERT");

	// 6.14 Textures (state per texture unit and binding point)
	int iEnvMode;
	char* szGlConstName;
	char szNameUnknown[32];
	strcpy(szNameUnknown, "<UNKNOWN>");
	int iTexObject;
	string strTextureName;
	int iMaxTexUnits;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &iMaxTexUnits);	

	fprintf(pkGlDumpLog, "\n\nTexture: \n");
	for(i=0; i<iMaxTexUnits; i++) {
		fprintf(pkGlDumpLog, "TextureUnit %d \n", i);
		glActiveTextureARB(GL_TEXTURE0_ARB + i);

		GlDump_IsEnabled(GL_TEXTURE_1D ,				" GL_TEXTURE_1D");
		GlDump_IsEnabled(GL_TEXTURE_2D ,				" GL_TEXTURE_2D");
		GlDump_IsEnabled(GL_TEXTURE_3D ,				" GL_TEXTURE_3D");
		GlDump_IsEnabled(GL_TEXTURE_CUBE_MAP ,		" GL_TEXTURE_CUBE_MAP");
		GlDump_GetIntv(GL_TEXTURE_BINDING_1D,		" GL_TEXTURE_BINDING_1D", 1);			

		glGetIntegerv(GL_TEXTURE_BINDING_2D, &iTexObject);	
		strTextureName = m_pkTexMan->GetTextureNameFromOpenGlIndex(iTexObject);
		fprintf(pkGlDumpLog, " GL_TEXTURE_BINDING_2D: %d - %s \n", iTexObject, strTextureName.c_str());

		GlDump_GetIntv(GL_TEXTURE_BINDING_3D,		" GL_TEXTURE_BINDING_3D", 1);			

		glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &iEnvMode);
		szGlConstName = szNameUnknown;
		switch(iEnvMode) {
			case  GL_MODULATE:	szGlConstName = "GL_MODULATE";	break;	
			case  GL_DECAL:		szGlConstName = "GL_DECAL";		break;	
			case  GL_BLEND:		szGlConstName = "GL_BLEND";		break;	
			case  GL_REPLACE:		szGlConstName = "GL_REPLACE";		break;	
			}

		fprintf(pkGlDumpLog, " GL_TEXTURE_ENV.GL_TEXTURE_ENV_MODE: %s \n", szGlConstName);
		}

	// Table 6.15. Textures (state per texture object)
	// Table 6.16. Textures (state per texture image)
	
	// Table 6.17. Texture Environment and Generation
	GlDump_GetFloatv(GL_ACTIVE_TEXTURE,		"GL_ACTIVE_TEXTURE",		1);
	//GlDump_GetFloatv(GL_TEXTURE_ENV_MODE,		"GL_TEXTURE_ENV_MODE", 1);
	//GlDump_GetFloatv(GL_TEXTURE_ENV_COLOR,		"GL_TEXTURE_ENV_COLOR", 4);
	// TEXTURE LOD BIAS
	GlDump_IsEnabled(GL_TEXTURE_GEN_Q , "GL_TEXTURE_GEN_Q");
	GlDump_IsEnabled(GL_TEXTURE_GEN_R , "GL_TEXTURE_GEN_R");
	GlDump_IsEnabled(GL_TEXTURE_GEN_S , "GL_TEXTURE_GEN_S");
	GlDump_IsEnabled(GL_TEXTURE_GEN_T , "GL_TEXTURE_GEN_T");
	// EYE PLANE

	// 6.18 Pixel Operations 
	fprintf(pkGlDumpLog, "\n\nPixel Operations: \n");
	GlDump_IsEnabled(GL_SCISSOR_TEST ,		"GL_SCISSOR_TEST");
	//SCISSOR_BOX
	GlDump_IsEnabled(GL_ALPHA_TEST ,				"GL_ALPHA_TEST");
	GlDump_GetFloatvName(GL_ALPHA_TEST_FUNC,	"GL_ALPHA_TEST_FUNC",	1);			
	GlDump_GetFloatv(GL_ALPHA_TEST_REF,			"GL_ALPHA_TEST_REF",		1);
	GlDump_IsEnabled(GL_STENCIL_TEST ,			"GL_STENCIL_TEST");
	//STENCIL_FUNC
	//STENCIL_VALUE MASK
	//STENCIL_REF
	//STENCIL_FAIL
	//STENCIL_PASS_DEPTH_FAIL
	//STENCIL_PASS_DEPTH_PASS
	GlDump_IsEnabled(GL_DEPTH_TEST ,			"GL_DEPTH_TEST");
	GlDump_GetFloatvName(GL_DEPTH_FUNC,		"GL_DEPTH_FUNC", 1);
	GlDump_IsEnabled(GL_BLEND ,				"GL_BLEND");
	GlDump_GetFloatvName(GL_BLEND_SRC,			"GL_BLEND_SRC", 1);
	GlDump_GetFloatvName(GL_BLEND_SRC_ALPHA,	"GL_BLEND_SRC_ALPHA", 1);
	GlDump_GetFloatvName(GL_BLEND_DST,			"GL_BLEND_DST", 1);
	GlDump_GetFloatvName(GL_BLEND_DST_ALPHA,	"GL_BLEND_DST_ALPHA", 1);
	GlDump_GetFloatv(GL_BLEND_EQUATION,		"GL_BLEND_EQUATION", 1);
	// Blend Color
	GlDump_IsEnabled(GL_DITHER ,				"GL_DITHER");
	GlDump_IsEnabled(GL_INDEX_LOGIC_OP ,	"GL_INDEX_LOGIC_OP");
	GlDump_IsEnabled(GL_COLOR_LOGIC_OP ,	"GL_COLOR_LOGIC_OP");
	// LOGIC OP MODE

	// Framebuffer Control 6.19
	fprintf(pkGlDumpLog, "\n\nFramebuffer Control: \n");
	GlDump_GetFloatv(GL_DRAW_BUFFER,				"GL_DRAW_BUFFER", 1);
	GlDump_GetIntv(GL_INDEX_WRITEMASK,			"GL_INDEX_WRITEMASK", 1);
	GlDump_GetFloatv(GL_COLOR_WRITEMASK,		"GL_COLOR_WRITEMASK", 4);
	GlDump_IsEnabled(GL_DEPTH_WRITEMASK ,		"GL_DEPTH_WRITEMASK");
	GlDump_GetIntv(GL_STENCIL_WRITEMASK,		"GL_STENCIL_WRITEMASK", 1);
	GlDump_GetFloatv(GL_COLOR_CLEAR_VALUE,		"GL_COLOR_CLEAR_VALUE", 4);
	GlDump_GetFloatv(GL_INDEX_CLEAR_VALUE,		"GL_INDEX_CLEAR_VALUE", 1);
	GlDump_GetFloatv(GL_DEPTH_CLEAR_VALUE,		"GL_DEPTH_CLEAR_VALUE", 1);
	GlDump_GetIntv(GL_STENCIL_CLEAR_VALUE,		"GL_STENCIL_CLEAR_VALUE", 1);
	GlDump_GetFloatv(GL_ACCUM_CLEAR_VALUE,		"GL_ACCUM_CLEAR_VALUE", 1);

	// Pixels 6.20
	GlDump_GetFloatv(GL_RED_SCALE,				"GL_RED_SCALE",		1);
	GlDump_GetFloatv(GL_GREEN_SCALE,				"GL_GREEN_SCALE",		1);
	GlDump_GetFloatv(GL_BLUE_SCALE,				"GL_BLUE_SCALE",		1);
	GlDump_GetFloatv(GL_ALPHA_SCALE,				"GL_ALPHA_SCALE",		1);
	GlDump_GetFloatv(GL_DEPTH_SCALE,				"GL_DEPTH_SCALE",		1);
	GlDump_GetFloatv(GL_RED_BIAS,					"GL_RED_BIAS",			1);
	GlDump_GetFloatv(GL_GREEN_BIAS,				"GL_GREEN_BIAS",		1);
	GlDump_GetFloatv(GL_BLUE_BIAS,				"GL_BLUE_BIAS",		1);
	GlDump_GetFloatv(GL_ALPHA_BIAS,				"GL_ALPHA_BIAS",		1);
	GlDump_GetFloatv(GL_DEPTH_BIAS,				"GL_DEPTH_BIAS",		1);
	
	// Table 6.25. Evaluators
	GlDump_IsEnabled(GL_AUTO_NORMAL , "GL_AUTO_NORMAL");
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

	// Hints 6.26 
	fprintf(pkGlDumpLog, "\n\nHints: \n");
	GlDump_GetFloatvName(GL_PERSPECTIVE_CORRECTION_HINT,	"GL_PERSPECTIVE_CORRECTION_HINT", 1);
	GlDump_GetFloatvName(GL_POINT_SMOOTH_HINT,				"GL_POINT_SMOOTH_HINT", 1);
	GlDump_GetFloatvName(GL_POINT_SMOOTH_HINT,				"GL_POINT_SMOOTH_HINT", 1);
	GlDump_GetFloatvName(GL_LINE_SMOOTH_HINT,					"GL_LINE_SMOOTH_HINT", 1);
	GlDump_GetFloatvName(GL_POLYGON_SMOOTH_HINT,				"GL_POLYGON_SMOOTH_HINT", 1);
	GlDump_GetFloatvName(GL_FOG_HINT,							"GL_FOG_HINT", 1);
	//GENERATE MIPMAP HINT
	GlDump_GetFloatvName(GL_TEXTURE_COMPRESSION_HINT,		"GL_TEXTURE_COMPRESSION_HINT", 1);

	//	6.27 Implementation Dependent Values 
	fprintf(pkGlDumpLog, "\n\nImplementation Dependent Values: \n");
	GlDump_GetIntv(GL_MAX_LIGHTS,							"GL_MAX_LIGHTS", 1);			
	GlDump_GetIntv(GL_MAX_CLIP_PLANES,					"GL_MAX_CLIP_PLANES", 1);			
	GlDump_GetIntv(GL_MAX_MODELVIEW_STACK_DEPTH,		"GL_MAX_MODELVIEW_STACK_DEPTH", 1);			
	GlDump_GetIntv(GL_MAX_PROJECTION_STACK_DEPTH,	"GL_MAX_PROJECTION_STACK_DEPTH", 1);			
	GlDump_GetIntv(GL_MAX_TEXTURE_STACK_DEPTH,		"GL_MAX_TEXTURE_STACK_DEPTH", 1);			
	GlDump_GetIntv(GL_SUBPIXEL_BITS,						"GL_SUBPIXEL_BITS", 1);			
	GlDump_GetIntv(GL_MAX_3D_TEXTURE_SIZE,				"GL_MAX_3D_TEXTURE_SIZE", 1);			
	GlDump_GetIntv(GL_MAX_TEXTURE_SIZE,					"GL_MAX_TEXTURE_SIZE", 1);	
	//MAX TEXTURE LOD BIAS
	GlDump_GetIntv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,		"GL_MAX_CUBE_MAP_TEXTURE_SIZE", 1);			
	GlDump_GetIntv(GL_MAX_PIXEL_MAP_TABLE,				"GL_MAX_PIXEL_MAP_TABLE", 1);			
	GlDump_GetIntv(GL_MAX_NAME_STACK_DEPTH,			"GL_MAX_NAME_STACK_DEPTH", 1);			
	GlDump_GetIntv(GL_MAX_LIST_NESTING,					"GL_MAX_LIST_NESTING", 1);			
	GlDump_GetIntv(GL_MAX_EVAL_ORDER,					"GL_MAX_EVAL_ORDER", 1);			
	// MAX VIEWPORT DIMS

	//	6.28 Implementation Dependent Values 
	GlDump_GetIntv(GL_MAX_ATTRIB_STACK_DEPTH,	"GL_MAX_ATTRIB_STACK_DEPTH", 1);			
	GlDump_GetIntv(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH,	"GL_MAX_CLIENT_ATTRIB_STACK_DEPTH", 1);			
	GlDump_GetFloatv(GL_AUX_BUFFERS,			"GL_AUX_BUFFERS", 1);
	GlDump_IsEnabled(GL_RGBA_MODE , "GL_RGBA_MODE");
	GlDump_IsEnabled(GL_INDEX_MODE , "GL_INDEX_MODE");
	GlDump_IsEnabled(GL_DOUBLEBUFFER , "GL_DOUBLEBUFFER");
	GlDump_IsEnabled(GL_STEREO , "GL_STEREO");
	//ALIASED POINT SIZE RANGE
	//SMOOTH POINT SIZE RANGE
	//SMOOTH POINT SIZE GRANULARITY
	//ALIASED LINE WIDTH RANGE
	//SMOOTH_LINE_WIDTH_RANGE
	//SMOOTH LINE WIDTH GRANULARITY

	//	6.29 Implementation Dependent Values 

	GlDump_GetIntv(GL_MAX_ELEMENTS_INDICES,	"GL_MAX_ELEMENTS_INDICES", 1);			
	GlDump_GetIntv(GL_MAX_ELEMENTS_VERTICES,	"GL_MAX_ELEMENTS_VERTICES", 1);			
	GlDump_GetIntv(GL_MAX_TEXTURE_UNITS,	"GL_MAX_TEXTURE_UNITS", 1);			
	GlDump_GetIntv(GL_SAMPLE_BUFFERS,		"GL_SAMPLE_BUFFERS", 1);			
	GlDump_GetIntv(GL_SAMPLES,					"GL_SAMPLES", 1);			
	//COMPRESSED TEXTURE FORMATS
	GlDump_GetIntv(GL_NUM_COMPRESSED_TEXTURE_FORMATS,		"GL_NUM_COMPRESSED_TEXTURE_FORMATS", 1);			

	//	6.30 Implementation Dependent Pixel Depths 
	GlDump_GetFloatv(GL_RED_BITS,					"GL_RED_BITS",		1);
	GlDump_GetFloatv(GL_GREEN_BITS,				"GL_GREEN_BITS",	1);
	GlDump_GetFloatv(GL_BLUE_BITS,				"GL_BLUE_BITS",	1);
	GlDump_GetFloatv(GL_ALPHA_BITS,				"GL_ALPHA_BITS",	1);
	GlDump_GetFloatv(GL_INDEX_BITS,				"GL_INDEX_BITS",	1);
	GlDump_GetFloatv(GL_DEPTH_BITS,				"GL_DEPTH_BITS",	1);
	GlDump_GetFloatv(GL_ACCUM_RED_BITS,			"GL_ACCUM_RED_BITS", 1);
	GlDump_GetFloatv(GL_ACCUM_GREEN_BITS,		"GL_ACCUM_GREEN_BITS", 1);
	GlDump_GetFloatv(GL_ACCUM_BLUE_BITS,		"GL_ACCUM_BLUE_BITS", 1);
	GlDump_GetFloatv(GL_ACCUM_ALPHA_BITS,		"GL_ACCUM_ALPHA_BITS", 1);
	
	//	6.31 Miscellaneous
	GlDump_GetFloatv(GL_LIST_BASE,				"GL_LIST_BASE", 1);
	GlDump_GetFloatv(GL_LIST_INDEX,				"GL_LIST_INDEX", 1);
	GlDump_GetFloatv(GL_LIST_MODE,				"GL_LIST_MODE", 1);
	GlDump_GetFloatv(GL_ATTRIB_STACK_DEPTH,	"GL_ATTRIB_STACK_DEPTH", 1);
	GlDump_GetFloatv(GL_CLIENT_ATTRIB_STACK_DEPTH,	"GL_CLIENT_ATTRIB_STACK_DEPTH", 1);

	fclose(pkGlDumpLog);

	glPopAttrib();
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
		case FID_GLINFO:			GlInfo();							break;
		case FID_CONSOLECOLOR:	m_kConsoleColor.Set(1,0,0);	break;
		case FID_SHOT:				m_bCapture = true;				break;
		case FID_GLDUMP:			DumpGLState("gldump.txt");		break;

		}
}


void Render::DrawPSystem( PSystem *pkPSystem )
{
	glPushMatrix();

   glDisable (GL_LIGHTING);

   glDisableClientState (GL_NORMAL_ARRAY);

   glDisable (GL_FOG);

	// PSystem uses color&alpha values
	if ( pkPSystem->GetColors() )
	{
		glEnable(GL_COLOR_MATERIAL);

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer( 4, GL_FLOAT, 0, pkPSystem->GetColors() + pkPSystem->Start() * 12 );
	}
	else
	{
		glColor4f (255,255,255,255);
		glDisableClientState (GL_COLOR_ARRAY);
		glDisable (GL_COLOR_MATERIAL);
	}

	
	// if PS uses texture
	if ( pkPSystem->GetTexCoords() )
	{
		// Turn on the texture coordinate state
		glEnable (GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer( 2, GL_FLOAT, 0, pkPSystem->GetTexCoords() + pkPSystem->Start() * 6 );
	}
	else
	{
		glDisableClientState (GL_TEXTURE_COORD_ARRAY);
		glDisable (GL_TEXTURE_2D);
	}

	// Turn on the vertex array state
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, pkPSystem->GetVertices() + pkPSystem->Start() * 9);

	// draw the stuff
	glDrawElements(GL_TRIANGLES, pkPSystem->Particles() * 3, 
					   GL_UNSIGNED_INT, pkPSystem->GetIndices() );


	// For debugging cullingbox (without rotation)
	/*
	Vector3 kScale = pkPSystem->GetPSystemType()->m_kPSystemBehaviour.m_kMaxSize;

	Vector3 kPos = pkPSystem->GetPosition() + 
						pkPSystem->GetRotation().VectorRotate(pkPSystem->GetPSystemType()->m_kPSystemBehaviour.m_kPosOffset);			

	kScale *= 2.f;

	glEnable (GL_CULL_FACE);
	glDisable (GL_FOG);
	glDisable (GL_LIGHTING);
	glDepthMask(GL_FALSE);
	DrawBox(kPos, pkPSystem->GetPSystemType()->m_kPSystemBehaviour.m_kCullPosOffset, 
			  pkPSystem->GetRotation(), kScale, 1);
	*/

	glPopMatrix();
}

/*	Set the colors that are used in the editor to display information.	*/
void Render::Setup_EditColors()
{
	m_kEditColor.push_back( EditColor(string("3dview/background"),		Vector3(0.631, 0.631, 0.631)));	
	m_kEditColor.push_back( EditColor(string("3dview/selbackground"),	Vector3(0.731, 0.731, 0.731)));	
	m_kEditColor.push_back( EditColor(string("grid/line"),				Vector3(0.250, 0.250, 0.250)));	
	m_kEditColor.push_back( EditColor(string("grid/axis"),				Vector3(0,0,0)));	

	// Markers
	m_kEditColor.push_back( EditColor(string("zonemarker"),				Vector3(1,1,1) ));	
	m_kEditColor.push_back( EditColor(string("hmapbrush"),				Vector3(0.8, 0.8, 0.0) ));	

	// Active - Selected Items in the editor.
	m_kEditColor.push_back( EditColor(string("active/zone"),				Vector3(1,1,1) ));	
	m_kEditColor.push_back( EditColor(string("active/entity"),			Vector3(1,1,1) ));	
	m_kEditColor.push_back( EditColor(string("active/firstentity"),	Vector3(0.263,1.000,0.639) ));	

	// Inactive - Items not selected.
	m_kEditColor.push_back( EditColor(string("inactive/zonebuild"),		Vector3(0,		1,			1) ));	
	m_kEditColor.push_back( EditColor(string("inactive/zoneon"),			Vector3(0,		1,			0) ));	
	m_kEditColor.push_back( EditColor(string("inactive/zoneoff"),			Vector3(1,		0,			0) ));	
	m_kEditColor.push_back( EditColor(string("inactive/zoneunloading"),	Vector3(0,		0,			1) ));	
	m_kEditColor.push_back( EditColor(string("inactive/zoneloading"),		Vector3(0,		1,			1) ));	
	m_kEditColor.push_back( EditColor(string("inactive/entity"),			Vector3(1,1,1) ));	

	// Std Colors
	m_kEditColor.push_back( EditColor(string("black"),		Vector3( 0,0,0) ));	
	m_kEditColor.push_back( EditColor(string("gray"),		Vector3( 0.5,0.5,0.5) ));
	m_kEditColor.push_back( EditColor(string("silver"),	Vector3( 0.753,0.753,0.753) ));	
	m_kEditColor.push_back( EditColor(string("white"),		Vector3(1,1,1) ));	
	m_kEditColor.push_back( EditColor(string("maroon"),	Vector3(0.5,0,0) ));	
	m_kEditColor.push_back( EditColor(string("red"),		Vector3(1,0,0) ));	
	m_kEditColor.push_back( EditColor(string("purple"),	Vector3(0.5,0,0.5) ));	
	m_kEditColor.push_back( EditColor(string("fuchsia"),	Vector3(1,0,1) ));	
	m_kEditColor.push_back( EditColor(string("green"),		Vector3(0,0.5,0) ));	
	m_kEditColor.push_back( EditColor(string("lime"),		Vector3(0,1,0) ));	
	m_kEditColor.push_back( EditColor(string("olive"),		Vector3(0.5,0.5,0) ));	
	m_kEditColor.push_back( EditColor(string("yellow"),	Vector3(1,1,0) ));	
	m_kEditColor.push_back( EditColor(string("navy"),		Vector3(1,0,0.5) ));	
	m_kEditColor.push_back( EditColor(string("blue"),		Vector3(0,0,1) ));	
	m_kEditColor.push_back( EditColor(string("teal"),		Vector3(0,0.5,0.5) ));	
	m_kEditColor.push_back( EditColor(string("aqua"),		Vector3(0,1,1) ));	
	
	// AI Colors
	m_kEditColor.push_back( EditColor(string("ai/rawpath"),	Vector3(1,0,0) ));	
	m_kEditColor.push_back( EditColor(string("ai/path"),		Vector3(0,1,0) ));	

}

Vector3 Render::GetEditColor(string strName)
{
	Vector3 kColor(1,0,0);

	for(unsigned int i=0; i<m_kEditColor.size(); i++) 
	{
		if(m_kEditColor[i].m_strName == strName)
			return m_kEditColor[i].m_kColor;
	}


	return kColor;
}


RENDER_API void RenderDLL_InitExtGL(void)
{
#ifdef _WIN32
	//int res; 
	//extgl_Initialize();
//	cout << "extgl_Initialize: "<< res << endl;
#endif
}






