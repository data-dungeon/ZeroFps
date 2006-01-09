#include "render.h"
#include "../ogl/zfpsgl.h"
#include "../basic/basicconsole.h"
#include "../engine_systems/common/psystem.h"
#include "../engine_systems/common/psystemmanager.h"
#include "../basic/globals.h"
#include <time.h>
#include "../basic/math.h"

FILE* pkGlDumpLog;
  
ZSSRender::ZSSRender()  
:	ZFSubSystem("ZSSRender")
{
	m_pkScreen = NULL;
	m_iScreenShootNum			= 0;

	m_kConsoleColor.Set(1,1,1);
	m_bCapture					= false;

	// Register Our Own variables.
	
	m_kiWidth.Register(this, "r_width", "800");
	m_kiHeight.Register(this, "r_height", "600");
	m_kiDepth.Register(this, "r_depth", "16");
	m_kiFullScreen.Register(this, "r_fullscreen", "0");

	// Register Our Own commands.
	Register_Cmd("r_glinfo",	FID_GLINFO);	
	Register_Cmd("r_gldump",	FID_GLDUMP);	
	Register_Cmd("ccolor",		FID_CONSOLECOLOR);
	Register_Cmd("shot",			FID_SHOT);
	
	Register_Cmd("ec_set",		FID_ECSET);
	Register_Cmd("ec_clear",	FID_ECCLEAR);
	Register_Cmd("ec_std",		FID_ECSETSTD);

}

bool ZSSRender::StartUp()
{
	// Get SubSystem Ptrs
	m_pkTexMan	= static_cast<TextureManager*>(GetSystem().GetObjectPtr("TextureManager"));
 	m_pkLight	= static_cast<ZSSLight*>(GetSystem().GetObjectPtr("ZSSLight"));
 	m_pkConsole = static_cast<BasicConsole*>(GetSystem().GetObjectPtr("ZSSConsole"));
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(GetSystem().GetObjectPtr("ZShaderSystem"));
	
	if(!InitDisplay())
	{
		cerr<<"ERROR: initializing display"<<endl;
		return false;
	}
	
	return true;
}

bool ZSSRender::InitDisplay()
{
	//reinitialize opengl
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	//setup sdl_gl_attributes, this has to be done before creating the sdl opengl window
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE		, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE		, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE		, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE		, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER	, 1 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE	, 8 );

	//create sdl surface and setup opengl
	if(!SetDisplay())
		return false;

	//initialize GLee
	GLeeInit();


	glMatrixMode(GL_MODELVIEW);
	
	return true;
}

void ZSSRender::ToggleFullScreen(void)
{
	m_kiFullScreen.SetBool( !m_kiFullScreen.GetBool());
	SDL_WM_ToggleFullScreen(m_pkScreen);
}

void ZSSRender::SetDisplayMode(int iWidth,int iHeight,int iDepth, bool bFullscreen)
{
	m_kiWidth.SetInt(iWidth);
	m_kiHeight.SetInt(iHeight);
	m_kiDepth.SetInt(iDepth);
	m_kiFullScreen.SetInt(bFullscreen);
}


bool ZSSRender::SetDisplay()
{
	m_pkTexMan->ClearAll();


	m_iSDLVideoModeFlags = 0;

	if(m_kiFullScreen.GetBool())
		m_iSDLVideoModeFlags = SDL_OPENGL|SDL_FULLSCREEN;
	else
		m_iSDLVideoModeFlags = SDL_OPENGL;

	//free current videosurface if any
	if(m_pkScreen)
		SDL_FreeSurface(m_pkScreen);


	cout<<"Trying videomode "<<m_kiWidth.GetInt()<<"x"<<m_kiHeight.GetInt()<<" "<<m_kiDepth.GetInt()<<" "<<m_iSDLVideoModeFlags<<endl;


	if( (m_pkScreen= SDL_SetVideoMode(m_kiWidth.GetInt(),m_kiHeight.GetInt(),m_kiDepth.GetInt(), m_iSDLVideoModeFlags)) == NULL)
	{
		cerr<<"ERROR: Creating sdl video surface"<<endl;
		return false;
	}

	cout<<"Videomode OK"<<endl;

	//setup sdlquit
	atexit(SDL_Quit);

	//default viewport
	glViewport(0, 0,m_kiWidth.GetInt(),m_kiHeight.GetInt());

	//default light settings
	m_pkLight->SetStartUpValues();
	
	return true;
}

void ZSSRender::Swap(void) 
{
	SDL_GL_SwapBuffers();  //guess

	if(m_bCapture) 
	{
		m_bCapture = false;
		CaptureScreenShoot(m_kiWidth.GetInt(), m_kiHeight.GetInt());
	}

	glLoadIdentity();	
}



void ZSSRender::Sphere(Vector3 kPos,float fRadius,int iRes,Vector3 kColor,bool bSolid)
{
	m_pkZShaderSystem->Push("render::sphere");

//	glPushAttrib(GL_ENABLE_BIT|GL_POLYGON_BIT|GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
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
	
//	glPushMatrix();
	
		glTranslatef(kPos.x,kPos.y,kPos.z);
		float scale = fRadius;
		glScalef(scale,scale,scale);
		
		for (int i = 0; i < 20; i++) 
		{    
		   glBegin(GL_TRIANGLES);    

	   	SubDivide(&vdata[tindices[i][2]][0],       
	   		       &vdata[tindices[i][1]][0],       
						 &vdata[tindices[i][0]][0],iRes); 

		   glEnd(); 
		}
		
//	glPopMatrix();
//	glPopAttrib();
	
	
	m_pkZShaderSystem->Pop();	
}

void ZSSRender::Normalize(float v[3]) {    
   float d = float( sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]) ); 
   
   if (d == 0.0) {  

      return;
   }
   v[0] /= d; 
   v[1] /= d; 
   v[2] /= d; 
}

void ZSSRender::SubDivide(float *v1, float *v2, float *v3, long depth)
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

void ZSSRender::Polygon4(const Vector3& kP1,const Vector3& kP2,const Vector3& kP3,const Vector3& kP4,const ZMaterial* pkMaterial)
{	
	static float afUvs[] = {	0,1,
							 			1,1,
										1,0,
										0,0};
	
	static Vector3 akVerts[4];
	
	akVerts[0] = kP1;
	akVerts[1] = kP2;
	akVerts[2] = kP3;
	akVerts[3] = kP4;
	
	m_pkZShaderSystem->BindMaterial(pkMaterial);														 
	
	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,akVerts);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);
	m_pkZShaderSystem->SetNrOfVertexs(4);
	m_pkZShaderSystem->DrawArray(QUADS_MODE);

}

void ZSSRender::Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture, Vector3 kColor)
{
	m_pkZShaderSystem->Push("render::sphere");	

//	glPushAttrib(GL_ENABLE_BIT|GL_LIGHTING_BIT);
//	glPushMatrix();
		
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


//	glPopMatrix();
//	glPopAttrib();
	
	m_pkZShaderSystem->Pop();
}

void ZSSRender::DrawBillboardQuad(const Matrix4& kCamRotMatrix,const Vector3& kPos,float fSize,ZMaterial* pkMaterial)
{
	static Matrix4 temp;
	temp=kCamRotMatrix;
	temp.Transponse();

	m_pkZShaderSystem->BindMaterial(pkMaterial);
	
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(kPos);
	m_pkZShaderSystem->MatrixScale(fSize);
	m_pkZShaderSystem->MatrixMult(temp);
	
	static float afVerts[] = {	-0.5,	0.5,	
							 			-0.5,	-0.5,	
							 			0.5,	-0.5,	
										0.5,	0.5};
	
	static float afUvs[] = {	0,1,
							 			1,1,
										1,0,
										0,0};
														 
	
	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX2D_POINTER,afVerts);
	m_pkZShaderSystem->SetPointer(TEXTURE_POINTER0,afUvs);
	m_pkZShaderSystem->SetNrOfVertexs(4);
	m_pkZShaderSystem->DrawArray(QUADS_MODE);

		
	m_pkZShaderSystem->MatrixPop();
}

void ZSSRender::AddTextBillboard(RenderPackage& kRenderPackage,const RenderState& kRenderState,const Vector3& kPos,float fScale,const string& strText,ZMaterial* pkMaterial,ZGuiFont* pkFont,bool bCentered)
{
	float fXScale = 1.0/pkFont->m_iTextureWidth;
	float fYScale = 1.0/pkFont->m_iTextureHeight;

	float fUVX,fUVY,fUVW,fUVH; 
	float fW,fH;
	float fXPos=0;
	
	if(bCentered)
	{
 		fW = (float(pkFont->m_iSpaceWidth) /  float(pkFont->m_iRowHeight)) * fScale;
 		fXPos = -( float(strText.length())/1.5 ) * fW;
	}	
	
	static Matrix4 ble;
	ble = kRenderState.m_kCameraRotation;
	ble.Transponse();
	
	Vector3 kXOffset = ble.VectorTransform(Vector3(1,0,0));
	Vector3 kYOffset = ble.VectorTransform(Vector3(0,1,0));
	
 	Vector3 kMove;
 	Vector3 kWidth;
	Vector3 kHeight;
 			
 			
	int iCharacters = strText.length();
	for(int i = 0;i<iCharacters;i++)
	{
		int iChar = int(strText[i]);				
		if(iChar == 32)
		{
			fW = (float(pkFont->m_iSpaceWidth) /  float(pkFont->m_iRowHeight))*fScale;
			fXPos += fW;
			continue;
		}
		
		fUVX =  float(pkFont->m_aChars[iChar].iPosX) * fXScale;
		fUVY = -(float(pkFont->m_aChars[iChar].iPosY) * fYScale);
		fUVW =  float(pkFont->m_aChars[iChar].iSizeX) * fXScale;
		fUVH =  float(pkFont->m_aChars[iChar].iSizeY) * fYScale;
	
		fW	= (float(pkFont->m_aChars[iChar].iSizeX) / float(pkFont->m_iRowHeight))*fScale;
		fH	= (float(pkFont->m_aChars[iChar].iSizeY) / float(pkFont->m_iRowHeight))*fScale;
 		
 		kMove = kPos + kXOffset * fXPos;
 		kWidth = kXOffset * fW;
 		kHeight = kYOffset * fH;
 		
 		kRenderPackage.m_kMeshData.m_kVertises.push_back(kMove ); 		
 		kRenderPackage.m_kMeshData.m_kVertises.push_back(kMove +kWidth); 		
 		kRenderPackage.m_kMeshData.m_kVertises.push_back(kMove +kHeight+kWidth ); 		
 		kRenderPackage.m_kMeshData.m_kVertises.push_back(kMove +kHeight); 		
		
		kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(fUVX,fUVY-fUVH));
		kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(fUVX+fUVW,fUVY-fUVH));
		kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(fUVX+fUVW,fUVY));
		kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(fUVX,fUVY));
		
		fXPos += fW;		
	}

}


void ZSSRender::AddText(RenderPackage& kRenderPackage,const Vector3& kPos,float fScale,const string& strText,ZMaterial* pkMaterial,ZGuiFont* pkFont,bool bCentered)
{
	float fXScale = 1.0/pkFont->m_iTextureWidth;
	float fYScale = 1.0/pkFont->m_iTextureHeight;

	int iChar;
	float fUVX,fUVY,fUVW,fUVH; 
	float fW,fH;
	float fXPos=0;
	
	if(bCentered)
	{
 		fW = float(pkFont->m_iSpaceWidth) /  float(pkFont->m_iRowHeight);
 		fXPos = -( float(strText.length())/1.5 ) * fW;
	}	
	
	int iCharacters = strText.length();
	for(int i = 0;i<iCharacters;i++)
	{
		iChar = int(strText[i]);				
		if(iChar == 32)
		{
			fW = float(pkFont->m_iSpaceWidth) /  float(pkFont->m_iRowHeight);
			fXPos += fW;
			continue;
		}
		
		fUVX =  float(pkFont->m_aChars[iChar].iPosX) * fXScale;
		fUVY = -(float(pkFont->m_aChars[iChar].iPosY) * fYScale);
		fUVW =  float(pkFont->m_aChars[iChar].iSizeX) * fXScale;
		fUVH =  float(pkFont->m_aChars[iChar].iSizeY) * fYScale;
	
		fW	= float(pkFont->m_aChars[iChar].iSizeX) / float(pkFont->m_iRowHeight);
		fH	= float(pkFont->m_aChars[iChar].iSizeY) / float(pkFont->m_iRowHeight);
 		
		kRenderPackage.m_kMeshData.m_kVertises.push_back(kPos+Vector3(fXPos			,0 ,0));
		kRenderPackage.m_kMeshData.m_kVertises.push_back(kPos+Vector3(fXPos + fW	,0 ,0));
		kRenderPackage.m_kMeshData.m_kVertises.push_back(kPos+Vector3(fXPos + fW	,fH,0));
		kRenderPackage.m_kMeshData.m_kVertises.push_back(kPos+Vector3(fXPos  		,fH,0));
		
		kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(fUVX,fUVY-fUVH));
		kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(fUVX+fUVW,fUVY-fUVH));
		kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(fUVX+fUVW,fUVY));
		kRenderPackage.m_kMeshData.m_kTexture[0].push_back(Vector2(fUVX,fUVY));
		
		fXPos += fW;		
	}
	
}

void ZSSRender::PrintBillboard(const Matrix4& kCamRotMatrix,const Vector3& kPos,float fScale,const string& strText,ZMaterial* pkMaterial,ZGuiFont* pkFont,bool bCentered)
{
	static Matrix4 temp;
	
	temp = kCamRotMatrix;
	temp.Transponse();

	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(kPos);
	m_pkZShaderSystem->MatrixMult(temp);

	if(bCentered)
	{
		float fLen = pkFont->GetLength(strText.c_str(),strText.length()) / pkFont->m_iRowHeight;
		m_pkZShaderSystem->MatrixTranslate(Vector3(-(fLen /2.0  )*fScale,0,0));
	}
		
	Print(Vector3(0,0,0),fScale,strText,pkMaterial,pkFont);
		
	m_pkZShaderSystem->MatrixPop();	


}

void ZSSRender::Print(const Vector3& kPos,float fScale,const string& strText,ZMaterial* pkMaterial,ZGuiFont* pkFont)
{
	m_pkZShaderSystem->BindMaterial(pkMaterial);
	
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(kPos);
	m_pkZShaderSystem->MatrixScale(fScale);
	m_pkZShaderSystem->ClearGeometry();

	float fXScale = 1.0/pkFont->m_iTextureWidth;
	float fYScale = 1.0/pkFont->m_iTextureHeight;
	
	int iChar;
	float fUVX,fUVY,fUVW,fUVH; 
	float fW,fH;
	float fXPos=0;
	
	for(int i = 0;i<strText.length();i++)
	{
		iChar = int(strText[i]);				
		if(iChar == 32)
		{
			fW = float(pkFont->m_iSpaceWidth) /  float(pkFont->m_iRowHeight);
			fXPos += fW;
			continue;
		}
		
		fUVX =  float(pkFont->m_aChars[iChar].iPosX) * fXScale;
		fUVY = -(float(pkFont->m_aChars[iChar].iPosY) * fYScale);
		fUVW =  float(pkFont->m_aChars[iChar].iSizeX) * fXScale;
		fUVH =  float(pkFont->m_aChars[iChar].iSizeY) * fYScale;
	
		fW	= float(pkFont->m_aChars[iChar].iSizeX) / float(pkFont->m_iRowHeight);
		fH	= float(pkFont->m_aChars[iChar].iSizeY) / float(pkFont->m_iRowHeight);
 		
		
		m_pkZShaderSystem->AddQuadV(	Vector2(fXPos    ,0),		Vector2(fXPos + fW,0),
												Vector2(fXPos + fW,fH),	   Vector2(fXPos    ,fH));

		m_pkZShaderSystem->AddQuadUV(	Vector2(fUVX,fUVY-fUVH),	Vector2(fUVX+fUVW,fUVY-fUVH),
												Vector2(fUVX+fUVW,fUVY),	Vector2(fUVX,fUVY));
		
		fXPos += fW;
	}
	
	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);	
	m_pkZShaderSystem->MatrixPop();		
	
}


void ZSSRender::PrintChar(char cChar,float fPos,float fScale) 
{
	static float fWidth = 1.0/16.0;
	
	float fX = (int(cChar) % 16) * fWidth;
	float fY = ((int(cChar) / 16)+1) * fWidth;
	
	
	m_pkZShaderSystem->AddQuadV(	Vector2(fPos    ,0),		Vector2(fPos + 1,0),
											Vector2(fPos + 1,1),	   Vector2(fPos    ,1));
													
	m_pkZShaderSystem->AddQuadUV(	Vector2(fX,fY),						Vector2(fX+fWidth,fY),
											Vector2(fX+fWidth,fY-fWidth),		Vector2(fX,fY-fWidth));
					
}
/*
void Render::PrintBillboard(const Matrix4& kCamRotMatrix,Vector3 kPos,const char* aText,float fScale,bool bCentered) 
{
	Matrix4 temp = kCamRotMatrix;
	temp.Transponse();

	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(kPos);
	m_pkZShaderSystem->MatrixMult(temp);


	if(bCentered)
		m_pkZShaderSystem->MatrixTranslate(Vector3(-(strlen(aText)/2.0)*fScale,0,0));

	Print(Vector3(0,0,0),aText,fScale);
		
	m_pkZShaderSystem->MatrixPop();
}*/

void ZSSRender::Print(Vector3 kPos,const char* aText,float fScale) 
{
	if(strlen(aText) == 0)
		return;

	float fSize = 1;
	
	char paText[TEXT_MAX_LENGHT];	
	strcpy(paText,aText);
	
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixTranslate(kPos);
	m_pkZShaderSystem->MatrixScale(fScale);
	m_pkZShaderSystem->ClearGeometry();
		
	int i=0;
	while(aText[i]!='\0') 
	{
		PrintChar(aText[i],float(i*1.0));
		i++;
	}

	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);
	
	m_pkZShaderSystem->MatrixPop();		
}


void ZSSRender::Line(const Vector3& kPos1,const Vector3& kPos2,const Vector3& kColor)
{
	static ZMaterial* pkLine = NULL;
	if(!pkLine)
	{
		pkLine = new ZMaterial;
		pkLine->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkLine->GetPass(0)->m_iCullFace = 			CULL_FACE_BACK;		
		pkLine->GetPass(0)->m_bLighting = 			false;
		pkLine->GetPass(0)->m_bFog = 					false;
		pkLine->GetPass(0)->m_bColorMaterial =		true;
	}
	
	pkLine->GetPass(0)->m_kVertexColor = kColor;		
	m_pkZShaderSystem->BindMaterial(pkLine,true);
	
	m_pkZShaderSystem->ClearGeometry();
	
	m_pkZShaderSystem->AddLineV(kPos1,kPos2);
	
	m_pkZShaderSystem->SetDrawMode(LINES_MODE);
	m_pkZShaderSystem->DrawGeometry();
}

void ZSSRender::Line(const Vector3& kPos1,const Vector3& kPos2)
{
	static ZMaterial* pkLine = NULL;
	if(!pkLine)
	{
		pkLine = new ZMaterial;
		pkLine->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkLine->GetPass(0)->m_iCullFace = 			CULL_FACE_BACK;		
		pkLine->GetPass(0)->m_bLighting = 			false;
		pkLine->GetPass(0)->m_bFog = 					false;
	}

	m_pkZShaderSystem->BindMaterial(pkLine);
	m_pkZShaderSystem->ClearGeometry();
	
	m_pkZShaderSystem->AddLineV(kPos1,kPos2);
	
	m_pkZShaderSystem->SetDrawMode(LINES_MODE);
	m_pkZShaderSystem->DrawGeometry();
	
}
/*
void ZSSRender::SetColor(Vector3 kColor)
{
	//glColor3f(kColor.x,kColor.y,kColor.z);
}
*/


void ZSSRender::Mode2D_Start()
{
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixIdentity();

/*	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();*/
	gluOrtho2D(0, 800, 0, 600);

	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPush();
	m_pkZShaderSystem->MatrixIdentity();
/*	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
*/	
/*
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(		GL_BLEND					);
	glDisable(		GL_LIGHTING				);
	glDisable(		GL_ALPHA_TEST			);
	glDepthMask(	GL_FALSE					);	
	glDisable(		GL_CULL_FACE			);
	glDisable(		GL_COLOR_MATERIAL 	);
	glDisable(		GL_FOG					);
	
*/	
}

void ZSSRender::Mode2D_End()
{
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixPop();
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
	m_pkZShaderSystem->MatrixPop();
	
/*	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
*/
//	glPopAttrib();
}


 
void ZSSRender::DrawConsole(char* m_aCommand,vector<char*>* m_kText,int iStartLine, int iMarkerPos, int iMarker) 
{
	static ZMaterial* pkConsole = NULL;
	if(!pkConsole)
	{
		pkConsole = new ZMaterial;
		pkConsole->GetPass(0)->m_pkTUs[0]->SetRes("text/devstr.bmp");
		pkConsole->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
		pkConsole->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
		pkConsole->GetPass(0)->m_bLighting = 				false;		
		pkConsole->GetPass(0)->m_bColorMaterial = 		true;
		pkConsole->GetPass(0)->m_bFog = 						false;		
		pkConsole->GetPass(0)->m_bDepthTest = 				false;				//needs to disable z-test , else marker wont work
		//uncomment these for a more funny looking console =D
		//pkConsole->m_bCopyData = true;
		//pkConsole->m_bWaves = true;
	}
	
	pkConsole->GetPass(0)->m_kVertexColor = m_kConsoleColor;
	
	m_pkZShaderSystem->BindMaterial(pkConsole);
		

	Mode2D_Start();
	//SetFont("data/textures/text/devstr.bmp");

	//glColor3f(m_kConsoleColor.x,m_kConsoleColor.y,m_kConsoleColor.z);
	//SetClearColor(Vector4(0,0,0,0));
	//glClear(GL_COLOR_BUFFER_BIT);	
	//glPolygonMode(GL_FRONT, GL_FILL);
	
	Print(Vector3(8,8,0),m_aCommand,8.0);		
	
	char kMarker[3];
	if(iMarker >= 0) 
	{
		kMarker[0] = iMarker;
		kMarker[1] = 0;
		
		Print(Vector3( float(8+iMarkerPos*8), float(8), 0), kMarker,8.0);				
	}

	if(iStartLine < 0)
		iStartLine = 0;
	
	unsigned int iEndLine = iStartLine + 93;
	if(iEndLine >= (*m_kText).size())
		iEndLine = (*m_kText).size();


	int iPos = 0;
	for(unsigned int i=iStartLine;	i<iEndLine;	i++) 
	{
		if((*m_kText)[i] != NULL)
		{
			Print(Vector3( 8.0, float(16.0 + 8.0 * float(iPos) ),0),(*m_kText)[i],8.0);		
		}
		iPos++;
	}
				
	
	Mode2D_End();
}

void ZSSRender::DrawBillboard(const Matrix4& kModelMatrix,const Vector3& kPos,float fSize,int iTexture, bool DepthWrite, bool bAlpha, bool bBlend) 
{
	m_pkZShaderSystem->Push("DrawBillboard");

	fSize/=2;
	
	Vector3 x;
	Vector3 y;
	
	Vector3 a;
	Vector3 b;	
	Vector3 c;	
	Vector3 d;	
	
// 	glPushMatrix();
// 	glPushAttrib(GL_LIGHTING_BIT|GL_ENABLE_BIT);
	glDisable(GL_CULL_FACE);	
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	glColor4f(1,1,1,1);
	m_pkTexMan->BindTexture(iTexture); 	
	
	if(bAlpha)
	{
		glAlphaFunc(GL_GREATER,0.3);
		glEnable(GL_ALPHA_TEST);
	}
	if(!DepthWrite)
		glDepthMask(GL_FALSE);
	if(bBlend)
	{
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	

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

// 	glPopMatrix();
// 	glPopAttrib();
	glEnable(GL_CULL_FACE);	
	
	m_pkZShaderSystem->Pop();
}

void ZSSRender::DrawCircle(vector<Vector3> kCircel, Vector3 kColor)
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


void ZSSRender::DrawBoundSphere(float fRadius, Vector3)
{
	cout<<"Render::DrawBoundSphere"<<endl;

	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glColor3f(1,1,1);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D );

	float x,y;
	glBegin(GL_LINE_LOOP );
	for(int i=0; i<360; i+=(int)12.25) {
		x = float( cos(Math::DegToRad( float(i) )) * fRadius );
		y = float( sin(Math::DegToRad( float(i) )) * fRadius );
		glVertex3f(x,y,0);
	}
	
	glEnd();

	glPopAttrib();
}


void ZSSRender::DrawBox(Vector3 kPos,Vector3 kRot,Vector3 kScale,int iTexture)
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

void ZSSRender::DrawBox(Vector3 kPos, Vector3 kCenter, Matrix4 kRot, Vector3 kSize,int iTexture)
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

void ZSSRender::DrawPyramid(Vector3 kPos, Vector3 kScale, Vector3 kColor, bool bSolid)
{
	cout<<"Render::DrawPyramid"<<endl;

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

void ZSSRender::DrawPlane(Plane& kPlane)
{
	glPushMatrix();
		
	glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_FOG_BIT | GL_LIGHTING_BIT | 
		GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);	

	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);

	Vector3 kAxisX(1,0,0);
	Vector3 kAxisY(0,1,0);

	Matrix4 kMat;
	kMat.Identity();
	kMat.LookDir(kPlane.m_kNormal, Vector3::AXIS_Y);
	kMat.SetPos( kPlane.m_kNormal * kPlane.m_fD );
	glMultMatrixf((float*)&kMat);

	Vector3 kPointOnPlane = kPlane.m_kNormal * kPlane.m_fD;

	Vector3 kVertex;
	glColor3f(1,1,1);

	glBegin(GL_QUADS);
		kVertex = (kAxisX * -10) + (kAxisY * 10);
		glVertex3f(kVertex.x,kVertex.y,kVertex.z);

		kVertex = (kAxisX * 10) + (kAxisY * 10);
		glVertex3f(kVertex.x,kVertex.y,kVertex.z);

		kVertex = (kAxisX * 10) + (kAxisY * -10);
		glVertex3f(kVertex.x,kVertex.y,kVertex.z);

		kVertex = (kAxisX * -10) + (kAxisY * -10);
		glVertex3f(kVertex.x,kVertex.y,kVertex.z);

	glEnd();

	glPopMatrix();
	glPopAttrib();
}


void ZSSRender::DrawCone(Vector3 kPos, float fRadie, float fHeight, 
					  Vector3 kColor, bool bSolid, int iSegments)
{
	cout<<"Render::DrawCone"<<endl;

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
	float grad = 0, grad_oka = Math::TWO_PI / iSegments, x, z;	// zf_pi

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

void ZSSRender::DrawBoundingBox(Vector3 kPos,Vector3 kRot,Vector3 kScale, Vector3 kColor)
{
	cout<<"Render::DrawBoundingBox"<<endl;

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

void ZSSRender::DrawColorBox(Vector3 kPos,Vector3 kRot,Vector3 kScale,Vector3 kColor)
{
	cout<<"Render::DrawColorBox"<<endl;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

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
//	glEnable(GL_TEXTURE_2D);
//	glEnable(GL_LIGHTING);
	glPopAttrib(); 	

	glPopMatrix();
	
}
void ZSSRender::DrawAABB( const Vector3& kMin,const Vector3& kMax)
{
	m_pkZShaderSystem->ClearGeometry();
	

	//botom
	m_pkZShaderSystem->AddQuadN(Vector3(0,-1,0),Vector3(0,-1,0),Vector3(0,-1,0),Vector3(0,-1,0));
	m_pkZShaderSystem->AddQuadUV(Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(1,0));
	m_pkZShaderSystem->AddQuadV(kMin,Vector3(kMax.x,kMin.y,kMin.z),
										 Vector3(kMax.x,kMin.y,kMax.z),Vector3(kMin.x,kMin.y,kMax.z));
	//top
	m_pkZShaderSystem->AddQuadN(Vector3(0,1,0),Vector3(0,1,0),Vector3(0,1,0),Vector3(0,1,0));
	m_pkZShaderSystem->AddQuadUV(Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(1,0));
	m_pkZShaderSystem->AddQuadV(kMax,Vector3(kMax.x,kMax.y,kMin.z),
										 Vector3(kMin.x,kMax.y,kMin.z),Vector3(kMin.x,kMax.y,kMax.z));

	//front
	m_pkZShaderSystem->AddQuadN(Vector3(0,0,-1),Vector3(0,0,-1),Vector3(0,0,-1),Vector3(0,0,-1));
	m_pkZShaderSystem->AddQuadUV(Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(1,0));
	m_pkZShaderSystem->AddQuadV(kMin,Vector3(kMin.x,kMax.y,kMin.z),
										 Vector3(kMax.x,kMax.y,kMin.z),Vector3(kMax.x,kMin.y,kMin.z));
	
	//back									 										 
	m_pkZShaderSystem->AddQuadN(Vector3(0,0,1),Vector3(0,0,1),Vector3(0,0,1),Vector3(0,0,1));
	m_pkZShaderSystem->AddQuadUV(Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(1,0));
	m_pkZShaderSystem->AddQuadV(kMax,Vector3(kMin.x,kMax.y,kMax.z),
										 Vector3(kMin.x,kMin.y,kMax.z),Vector3(kMax.x,kMin.y,kMax.z));
										 
										 
	//left
	m_pkZShaderSystem->AddQuadN(Vector3(1,0,0),Vector3(1,0,0),Vector3(1,0,0),Vector3(1,0,0));
	m_pkZShaderSystem->AddQuadUV(Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(1,0));
	m_pkZShaderSystem->AddQuadV(kMax,Vector3(kMax.x,kMin.y,kMax.z),
										 Vector3(kMax.x,kMin.y,kMin.z),Vector3(kMax.x,kMax.y,kMin.z));

	//right
	m_pkZShaderSystem->AddQuadN(Vector3(-1,0,0),Vector3(-1,0,0),Vector3(-1,0,0),Vector3(-1,0,0));
	m_pkZShaderSystem->AddQuadUV(Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(1,0));
	m_pkZShaderSystem->AddQuadV(kMin,Vector3(kMin.x,kMin.y,kMax.z),
										Vector3(kMin.x,kMax.y,kMax.z),Vector3(kMin.x,kMax.y,kMin.z));
									 										 
										 
										 										 
	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);
}

void ZSSRender::DrawOcculusionAABB( const Vector3& kMin,const Vector3& kMax)
{
	static ZMaterial* pkMaterial = NULL;
	if(!pkMaterial)
	{
		pkMaterial = new ZMaterial;
		pkMaterial->GetPass(0)->m_iCullFace = CULL_FACE_NONE;	
		pkMaterial->GetPass(0)->m_bLighting = false;
		pkMaterial->GetPass(0)->m_bFog = false;
		pkMaterial->GetPass(0)->m_bDepthMask = false;
	}

	static Vector3 kVertises[24];
	
	
	//botom
	kVertises[0] = kMin;
	kVertises[1].Set(kMax.x,kMin.y,kMin.z);
	kVertises[2].Set(kMax.x,kMin.y,kMax.z);
	kVertises[3].Set(kMin.x,kMin.y,kMax.z);
	
	//top
	kVertises[4] = kMax;
	kVertises[5].Set(kMax.x,kMax.y,kMin.z);
	kVertises[6].Set(kMin.x,kMax.y,kMin.z);
	kVertises[7].Set(kMin.x,kMax.y,kMax.z);

	//front
	kVertises[8] = kMin;	
	kVertises[9].Set(kMin.x,kMax.y,kMin.z);
	kVertises[10].Set(kMax.x,kMax.y,kMin.z);
	kVertises[11].Set(kMax.x,kMin.y,kMin.z);

	//left
	kVertises[12] = kMin;
	kVertises[13].Set(kMin.x,kMin.y,kMax.z);
	kVertises[14].Set(kMin.x,kMax.y,kMax.z);
	kVertises[15].Set(kMin.x,kMax.y,kMin.z);

	//right
	kVertises[16] = kMax;
	kVertises[17].Set(kMax.x,kMin.y,kMax.z);
	kVertises[18].Set(kMax.x,kMin.y,kMin.z);
	kVertises[19].Set(kMax.x,kMax.y,kMin.z);
	
	//back
	kVertises[20] = kMax;
	kVertises[21].Set(kMin.x,kMax.y,kMax.z);
	kVertises[22].Set(kMin.x,kMin.y,kMax.z);
	kVertises[23].Set(kMax.x,kMin.y,kMax.z);	


	//disable all glsl stuff
	m_pkZShaderSystem->SetForceDisableGLSL(true);	

	//set materail
	m_pkZShaderSystem->BindMaterial(pkMaterial);	

	m_pkZShaderSystem->ResetPointers();
	m_pkZShaderSystem->SetPointer(VERTEX_POINTER,kVertises);
	m_pkZShaderSystem->SetNrOfVertexs(24);
	
	
	m_pkZShaderSystem->DrawArray(QUADS_MODE);
	
	//reenable all glsl stuff
	m_pkZShaderSystem->SetForceDisableGLSL(false);

}

void ZSSRender::DrawAABB( const Vector3& kMin,const Vector3& kMax, const Vector3& kColor, float fLineSize )
{
	static ZMaterial* pkLine = NULL;
	if(!pkLine)
	{
		pkLine = new ZMaterial;
		pkLine->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkLine->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
		pkLine->GetPass(0)->m_iCullFace = CULL_FACE_NONE;		
		pkLine->GetPass(0)->m_bLighting = false;
		pkLine->GetPass(0)->m_bColorMaterial = true;
		pkLine->GetPass(0)->m_bFog = false;
		
	}
	pkLine->GetPass(0)->m_kVertexColor = Vector4(kColor.x,kColor.y,kColor.z,1);
	pkLine->GetPass(0)->m_fLineWidth = fLineSize;
	
	
	m_pkZShaderSystem->BindMaterial(pkLine,true);
	
	
	m_pkZShaderSystem->ClearGeometry();
	

	//botom
	m_pkZShaderSystem->AddQuadV(kMin,Vector3(kMax.x,kMin.y,kMin.z),
										 Vector3(kMax.x,kMin.y,kMax.z),Vector3(kMin.x,kMin.y,kMax.z));
	//top
	m_pkZShaderSystem->AddQuadV(kMax,Vector3(kMax.x,kMax.y,kMin.z),
										 Vector3(kMin.x,kMax.y,kMin.z),Vector3(kMin.x,kMax.y,kMax.z));

	//front
	m_pkZShaderSystem->AddQuadV(kMin,Vector3(kMin.x,kMax.y,kMin.z),
										 Vector3(kMax.x,kMax.y,kMin.z),Vector3(kMax.x,kMin.y,kMin.z));
	
	//back									 										 
	m_pkZShaderSystem->AddQuadV(kMax,Vector3(kMin.x,kMax.y,kMax.z),
										 Vector3(kMin.x,kMin.y,kMax.z),Vector3(kMax.x,kMin.y,kMax.z));

										 										 
	m_pkZShaderSystem->DrawGeometry(QUADS_MODE);
}

void ZSSRender::AddAABB(RenderPackage& pkRenderPackage, const Vector3& kMin,const Vector3& kMax, const Vector4& kColor)
{
	MeshData& kMesh = pkRenderPackage.m_kMeshData;
		
	//front
	kMesh.m_kVertises.push_back(kMin);
	kMesh.m_kVertises.push_back(Vector3(kMin.x,kMax.y,kMin.z));
	kMesh.m_kVertises.push_back(Vector3(kMax.x,kMax.y,kMin.z));
	kMesh.m_kVertises.push_back(Vector3(kMax.x,kMin.y,kMin.z));	
	
	//back
	kMesh.m_kVertises.push_back(Vector3(kMin.x,kMin.y,kMax.z));
	kMesh.m_kVertises.push_back(Vector3(kMin.x,kMax.y,kMax.z));
	kMesh.m_kVertises.push_back(kMax);
	kMesh.m_kVertises.push_back(Vector3(kMax.x,kMin.y,kMax.z));	

	//left
	kMesh.m_kVertises.push_back(kMin);
	kMesh.m_kVertises.push_back(Vector3(kMin.x,kMin.y,kMax.z));
	kMesh.m_kVertises.push_back(Vector3(kMin.x,kMax.y,kMax.z));
	kMesh.m_kVertises.push_back(Vector3(kMin.x,kMax.y,kMin.z));

	//right
	kMesh.m_kVertises.push_back(Vector3(kMax.x,kMin.y,kMin.z));
	kMesh.m_kVertises.push_back(Vector3(kMax.x,kMin.y,kMax.z));
	kMesh.m_kVertises.push_back(kMax);
	kMesh.m_kVertises.push_back(Vector3(kMax.x,kMax.y,kMin.z));
		
	for(int i = 0;i<16;i++)
		kMesh.m_kColors.push_back(kColor);		
	
}

void ZSSRender::AddAABB(vector<Vector3>&	kVertexs,vector<Vector4>&	kColors, const Vector3& kMin,const Vector3& kMax, const Vector4& kColor)
{
	//front
	kVertexs.push_back(kMin);
	kVertexs.push_back(Vector3(kMin.x,kMax.y,kMin.z));
	kVertexs.push_back(Vector3(kMax.x,kMax.y,kMin.z));
	kVertexs.push_back(Vector3(kMax.x,kMin.y,kMin.z));	
	
	//back
	kVertexs.push_back(Vector3(kMin.x,kMin.y,kMax.z));
	kVertexs.push_back(Vector3(kMin.x,kMax.y,kMax.z));
	kVertexs.push_back(kMax);
	kVertexs.push_back(Vector3(kMax.x,kMin.y,kMax.z));	

	//left
	kVertexs.push_back(kMin);
	kVertexs.push_back(Vector3(kMin.x,kMin.y,kMax.z));
	kVertexs.push_back(Vector3(kMin.x,kMax.y,kMax.z));
	kVertexs.push_back(Vector3(kMin.x,kMax.y,kMin.z));

	//right
	kVertexs.push_back(Vector3(kMax.x,kMin.y,kMin.z));
	kVertexs.push_back(Vector3(kMax.x,kMin.y,kMax.z));
	kVertexs.push_back(kMax);
	kVertexs.push_back(Vector3(kMax.x,kMax.y,kMin.z));
		
	for(int i = 0;i<16;i++)
		kColors.push_back(kColor);		

}

void ZSSRender::DrawSolidAABB( const Vector3& kMin,const Vector3& kMax, const Vector3& kColor )
{
	cout<<"Render::DrawSolidAABB"<<endl;

	glPushAttrib(GL_LIGHTING_BIT);
	
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
	//glEnable(GL_LIGHTING);

	glPopAttrib();
}


void ZSSRender::DrawAABB( float x, float y, float z, float sizex,float sizey,float sizez, Vector3 kColor )
{
	cout<<"Render::DrawAABB"<<endl;

	glPushAttrib(GL_LIGHTING_BIT);	
	
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
	//glEnable(GL_LIGHTING);
	glPopAttrib();
}

void ZSSRender::Draw_AxisIcon(float scale)
{

	Line(Vector3(0,0,0),Vector3(scale,0,0),Vector3(1,0,0));
	Line(Vector3(0,0,0),Vector3(0,scale,0),Vector3(0,1,0));
	Line(Vector3(0,0,0),Vector3(0,0,scale),Vector3(0,0,1));

}

void ZSSRender::Draw_MarkerCross(Vector3 kPos, Vector3 Color, float fScale)
{
	m_pkZShaderSystem->Push("Draw_AxisIcon");

//	glPushMatrix();
//	glPushAttrib(GL_ENABLE_BIT|GL_POLYGON_BIT|GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
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

	
//	glPopAttrib();
//	glPopMatrix();
	
	m_pkZShaderSystem->Pop();
}

void ZSSRender::CaptureScreenShoot( int m_iWidth, int m_iHeight )
{
	Image kScreen;
	kScreen.CreateEmpty(m_iWidth, m_iHeight);

	glReadPixels(0,0, m_iWidth,m_iHeight,GL_RGBA, GL_UNSIGNED_BYTE, kScreen.m_pkPixels); 

	char szImageName[256];
	char szName[256];
	string strAppName = GetSystem().GetPreLogName();

	//sprintf(szImageName, "screen_%d.tga", m_iScreenShootNum);
	//m_iScreenShootNum++;

	time_t	kTime;
	time(&kTime);
	struct tm *pkLocalTime;
	pkLocalTime = localtime( &kTime );
	strftime(szName, 250, "%y%m%d_%H%M%S", pkLocalTime);
	sprintf(szImageName, "screenshots/%s_%s.tga", strAppName.c_str(), szName);
	
	kScreen.Save(szImageName ,false);
}

			



//char* BASIC_API BoolStr(bool bFlag);												//returns string "true" or "false" i think	

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



void ZSSRender::DumpGLState(char* szFileName)
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

void ZSSRender::GlInfo()
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

void ZSSRender::RunCommand(int cmdid, const ConCommandLine* kCommand)
{
	float r,g,b;

	switch(cmdid) {
		case FID_GLINFO:			GlInfo();							break;
		case FID_CONSOLECOLOR:	m_kConsoleColor.Set(1,0,0);	break;
		case FID_SHOT:				m_bCapture = true;				break;
		case FID_GLDUMP:			DumpGLState("gldump.txt");		break;
		case FID_ECSET:	
			if(kCommand->m_kSplitCommand.size() < 4)
				break;

			r = atof(kCommand->m_kSplitCommand[2].c_str());
			g = atof(kCommand->m_kSplitCommand[3].c_str());
			b = atof(kCommand->m_kSplitCommand[4].c_str());
			EditColor_Set(kCommand->m_kSplitCommand[1], r,g,b);
			//m_kEditColor.push_back( EditColor(kCommand->m_kSplitCommand[2]),		Vector3( r,g,b) ));	
			break;
		case FID_ECCLEAR:			m_kEditColor.clear();		   break;
		case FID_ECSETSTD:		
			// Add Std colors.
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
			break;
		}
}

void ZSSRender::EditColor_Set(string strName, float r, float g, float b)
{
	for(unsigned int i=0; i<m_kEditColor.size(); i++) 
	{
		if(m_kEditColor[i].m_strName == strName)
		{
			m_kEditColor[i].m_kColor.Set(r,g,b);
			return;
		}
	}	

	m_kEditColor.push_back( EditColor(strName,	Vector3(r,g,b) ));	
}

Vector3 ZSSRender::GetEditColor(string strName)
{
	Vector3 kColor(0,0,0);

	for(unsigned int i=0; i<m_kEditColor.size(); i++) 
	{
		if(m_kEditColor[i].m_strName == strName)
			return m_kEditColor[i].m_kColor;
	}

	g_Logf("WARNING: Request for a unknown EditColor '%s'\n", strName.c_str());
	return kColor;
}




