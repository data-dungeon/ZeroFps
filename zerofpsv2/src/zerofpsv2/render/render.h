#ifndef _RENDER_RENDER_H_
#define _RENDER_RENDER_H_

#define TEXT_MAX_LENGHT 256
#define FONTWIDTH 32
#define TEX_SCALE 2


#include <cstring>
#include <iostream>
#include <vector>
#include <cmath>
#include "frustum.h"
#include "render_x.h"
#include "texturemanager.h"
#include "light.h"
#include "zshadersystem.h"

class PSystem;
class HeightMap;
class Heightmap2;
class BasicConsole;
class HMSelectVertex;

using namespace std;

enum SkyBoxSides 
{
	SKYBOXSIDE_FORWARD,
	SKYBOXSIDE_RIGHT,
	SKYBOXSIDE_BACK,
	SKYBOXSIDE_LEFT,
	SKYBOXSIDE_UP,
	SKYBOXSIDE_DOWN,
	SKYBOXSIDE_MAX,
};


enum PolygonMode 
{ 
	FILL, 
	LINE 
};

class EditColor
{
public:
	EditColor(string strName, Vector3 kColor) { m_strName = strName; m_kColor = kColor;	 }
	string	m_strName;
	Vector3	m_kColor;
};


/**	\brief	Render SubSystem
		\ingroup Render
*/
class RENDER_API Render : public ZFSubSystem {
	private:
		enum FuncId_e
		{
			FID_GLINFO,				// Display basic info about OpenGL to console.		
			FID_GLDUMP,				// Dump current glstate to file.
			FID_CONSOLECOLOR,		
			FID_SHOT,				
		};

		TextureManager*	m_pkTexMan;
		Light*				m_pkLight;
		BasicConsole*		m_pkConsole;
		ZShaderSystem*		m_pkZShaderSystem;
		
				
		//heightmap stuff
		int			m_iLodUpdate;				
		int			m_iMaxLandscapeLayers;
		int			m_iDrawLandscape;

		int			m_iDetail;				//	grid size of lod tiles for the terran
		int			m_iViewDistance;		//	how far until we cut the landscape	
		int			m_iFpsLock;
		int			m_iAutoLod;
		
		
		//screenshot stuff
		int			m_iScreenShootNum;
		bool  		m_bCapture;									//shuld a screenshot be made this frame?

		//screen info
		SDL_Surface* 	m_pkScreen;								//main sdl screen surface
		int				m_iWidth,m_iHeight,m_iDepth;
		int				m_iFullScreen;
		int				m_iSDLVideoModeFlags;
		
		//console color
		Vector3		m_kConsoleColor;

		//colors...or something
		vector<EditColor>	m_kEditColor;

		
						
		void 	RunCommand(int cmdid, const CmdArgument* kCommand);
		void	GlInfo();										// Print info about opengl driver to console.
   	void SubDivide(float *v1, float *v2, float *v3, long depth);
   	void Normalize(float v[3]);
		void CaptureScreenShoot( int m_iWidth, int m_iHeight );							///< Take a screenshoot and save it as a TGA.		

	public:
		PolygonMode m_eLandscapePolygonMode;

		Render();	
		bool StartUp();
		bool ShutDown()	{ return true;	}
		bool IsValid()		{ return true;	}


		//display functions		
		void InitDisplay(int iWidth,int iHeight,int iDepth);		
		void SetDisplay(int iWidth,int iHeight,int iDepth);
		void SetDisplay();
		void Swap(void);			
		void ToggleFullScreen(void);
		void ScreenShot() 	{	m_bCapture = true;}
		
		int GetWidth()			{	return m_iWidth;	}
		int GetHeight()		{	return m_iHeight;	}		
		int GetDepth()			{	return m_iDepth;	}		

		void DumpGLState(char* szFileName);
		
		
		//strange stuff
		void Mode2D_Start();
		void Mode2D_End();


		//printing and console
		void PrintChar(char cChar,float fPos,float fScale = 1.0);
		void Print(Vector3 kPos, const char* aText,float fScale = 1.0);
		void DrawConsole(char* m_aCommand,vector<char*>* m_kText, int iStartLine, int iMarkerPos, int iMarker); 

		//draw line
		void Line(const Vector3& kPos1,const Vector3& kPos2,const Vector3& kColor);
		void Line(const Vector3& kPos1,const Vector3& kPos2);

		// LandScape
		void DrawHMSelected(HeightMap* kmap, vector<HMSelectVertex> kSelected);
		void DrawNormals(HeightMap* kmap,Vector3 CamPos,int iFps);
		void DrawHMLodSplat(HeightMap* kmap,Vector3 CamPos,int iFps);
		void DrawAllHM(HeightMap* kmap,Vector3 CamPos,bool bBorders);
		void DrawPatch(HeightMap* kmap,Vector3 CamPos,int xp,int zp,int iSize,bool bBorders);
		void DrawPatch_Vim1(HeightMap* kmap,Vector3 CamPos,int xp,int zp,int iSize);
		void DrawHMVertex(HeightMap* kMap);
		void DrawBlocks(HeightMap* kmap);
		void GetData(HeightMap* kMap, float x, float z, Vector3& kPos, Vector3& kNormal, Vector3& kTex1, Vector3& kTex2 );

		// SkyBox
		void DrawSkyBox(Vector3 CamPos,Vector3 kHead,int iHor,int iTop);
		void DrawSkyBox_SixSided(Vector3 CamPos,Vector3 kHead,int* aiSideTextures);
		
		// Water
		void DrawWater(Vector3 kCamPos,Vector3 kPosition,Vector3 kHead,int iSize,int iStep,int iTexture, float fBlendValue);
		void DrawSimpleWater(Vector3 kPosition,Vector4 kColor,int iSize,int iTexture);

		void GiveTexCor(float &iX,float &iY,int iNr);
		
		// primitives and such
		void DrawCross(Vector3& kPos,Vector3& kHead,Vector3& kScale,int& iTexture1);//,int iTexture2);
		void DrawBillboard(const Matrix4& kModelMatrix,const Vector3& kPos,float fSize,int iTexture);

		void Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture, Vector3 kColor = Vector3(1,1,1));
		void Polygon4(const Vector3& kP1,const Vector3& kP2,const Vector3& kP3,const Vector3& kP4,const int& iTexture);
		void Sphere(Vector3 kPos,float fRadius,int iRes,Vector3 kColor,bool bSolid);		
		void DrawCircle(vector<Vector3> kCircel, Vector3 kColor);
		
		void DrawCone(Vector3 kPos, float fRadie, float fHeight, Vector3 kColor, bool bSolid, int iNumSegments=20);
		void DrawPyramid(Vector3 kCenterPos, Vector3 kSize, Vector3 kColor, bool bSolid);
		void DrawBoundSphere(float fRadius, Vector3);
		void DrawBoundingBox(Vector3 kPos,Vector3 kRot,Vector3 kScale, Vector3 kColor);
		void DrawBox(Vector3 kPos,Vector3 kRot,Vector3 kScale,int iTexture);
		void DrawBox(Vector3 kPos,Vector3 kCenter, Matrix4 kRot,Vector3 kScale,int iTexture);
		void DrawColorBox(Vector3 kPos,Vector3 kRot,Vector3 kScale,Vector3 kColor);
		void DrawAABB( float x, float y, float z, float sizex,float sizey,float sizez, Vector3 kColor );
		void DrawAABB( const Vector3& kMin,const Vector3& kMax,const Vector3& kColor, float fLineSize = 1.0 );
		void DrawAABB(const Vector3& kMin,const Vector3& kMax);
		void DrawSolidAABB( const Vector3& kMin,const Vector3& kMax, const Vector3& kColor );

		void Draw_AxisIcon(float scale = 1.0);													///< Draw axis lines.
		void Draw_MarkerCross(Vector3 kPos, Vector3 Color, float fScale = 1.0);		///< Draw a cross made up of lines.


		void DrawPSystem( PSystem *pkPSystem );

		void 		Setup_EditColors();
		Vector3	GetEditColor(string strName);

		
		
		//old stuff
		//void SetClearColor(Vector4 kColor);
		//void SetFog(Vector4 kFogColor,float FogStart,float FogStop,bool FogEnable);		

		//void GetMinMax(HeightMap* kMap, float& fMin, float& fMax, int xp,int zp,int iSize);
		//void DrawHM2(Heightmap2* pkMap,Vector3 kCamPos);
		//void Dot(float x,float y,float z);
		//void SetColor(Vector3 kColor);
		/*
		void PrintChar(unsigned char cChar);
		void Print(Vector3 kPos,Vector3 kHead,Vector3 kScale,char* aText);
		void Print(Vector3 kPos,Vector3 kScale,char* aText);		
		void SetFont(char* aFont);
		*/
		
};

RENDER_API void RenderDLL_InitExtGL(void);
RENDER_API char* GetOpenGLErrorName(GLenum id );

#endif



