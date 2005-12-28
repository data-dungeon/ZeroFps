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
#include "../basic/zguifont.h"

class RenderState;
class RenderPackage;
class HeightMap;
class Heightmap2;
class BasicConsole;
class HMSelectVertex;

using namespace std;



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
class RENDER_API ZSSRender : public ZFSubSystem {
	private:
		enum FuncId_e
		{
			FID_GLINFO,				// Display basic info about OpenGL to console.		
			FID_GLDUMP,				// Dump current glstate to file.
			FID_CONSOLECOLOR,		
			FID_SHOT,			
			FID_ECSET,				// Set a editor color.
			FID_ECCLEAR,			// Clear all editor colors.
			FID_ECSETSTD,			// Set Std colors.
		};

		TextureManager*	m_pkTexMan;
		ZSSLight*			m_pkLight;
		BasicConsole*		m_pkConsole;
		ZShaderSystem*		m_pkZShaderSystem;
		

		
		//screenshot stuff
		int			m_iScreenShootNum;
		bool  		m_bCapture;									//shuld a screenshot be made this frame?

		//screen info
		SDL_Surface* 	m_pkScreen;								//main sdl screen surface

		ConVar			m_kiWidth;
		ConVar			m_kiHeight;
		ConVar			m_kiDepth;
		ConVar			m_kiFullScreen;

		//bool			m_iFullScreen;
		int				m_iSDLVideoModeFlags;
		
		//console color
		Vector3			m_kConsoleColor;

		//colors...or something
		vector<EditColor>	m_kEditColor;
		
						
		void 	RunCommand(int cmdid, const ConCommandLine* kCommand);
		void	GlInfo();										// Print info about opengl driver to console.
   	void SubDivide(float *v1, float *v2, float *v3, long depth);
   	void Normalize(float v[3]);
		void CaptureScreenShoot( int m_iWidth, int m_iHeight );							///< Take a screenshoot and save it as a TGA.		

	public:
		ZSSRender();	
		bool StartUp();
		bool ShutDown()	{ return true;	}
		bool IsValid()		{ return true;	}


		//display functions		
		void InitDisplay(int iWidth,int iHeight,int iDepth);		
		void SetDisplay(int iWidth,int iHeight,int iDepth,bool bFullScreen);
		void SetDisplay();
		void Swap(void);			
		void ToggleFullScreen(void);
		void ScreenShot() 	{	m_bCapture = true;}
		
		int GetWidth()			{	return m_kiWidth.GetInt();			}
		int GetHeight()		{	return m_kiHeight.GetInt();		}		
		int GetDepth()			{	return m_kiDepth.GetInt();			}		
		bool GetFullscreen()	{	return m_kiFullScreen.GetBool();	}		

		void DumpGLState(char* szFileName);
		
		
		//strange stuff
		void Mode2D_Start();
		void Mode2D_End();


		//printing and console
		void PrintBillboard(const Matrix4& kCamRotMatrix,const Vector3& kPos,float fScale,const string& strText,ZMaterial* pkMaterial,ZGuiFont* pkFont,bool bCentered);		
		void Print(const Vector3& kPos,float fScale,const string& strText,ZMaterial* pkMaterial,ZGuiFont* pkFont);
		void PrintChar(char cChar,float fPos,float fScale = 1.0);
		void Print(Vector3 kPos, const char* aText,float fScale = 1.0);
		void DrawConsole(char* m_aCommand,vector<char*>* m_kText, int iStartLine, int iMarkerPos, int iMarker); 

		//draw line
		void Line(const Vector3& kPos1,const Vector3& kPos2,const Vector3& kColor);
		void Line(const Vector3& kPos1,const Vector3& kPos2);


		
		// primitives and such
		void DrawBillboard(const Matrix4& kModelMatrix,const Vector3& kPos,float fSize,int iTexture, bool DepthWrite, bool bAlpha, bool bBlend);
		void DrawBillboardQuad(const Matrix4& kCamRotMatrix,const Vector3& kPos,float fSize,ZMaterial* pkMaterial);

		void Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture, Vector3 kColor = Vector3(1,1,1));
		void Polygon4(const Vector3& kP1,const Vector3& kP2,const Vector3& kP3,const Vector3& kP4,const ZMaterial* pkMaterial);
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
		void DrawPlane(Plane& kPlane);
		void DrawOcculusionAABB( const Vector3& kMin,const Vector3& kMax);

		void Draw_AxisIcon(float scale = 1.0);													///< Draw axis lines.
		void Draw_MarkerCross(Vector3 kPos, Vector3 Color, float fScale = 1.0);		///< Draw a cross made up of lines.


		//RENDERPACKAGE STUFF
		void AddAABB(vector<Vector3>&	kVertexs,vector<Vector4>&	kColors, const Vector3& kMin,const Vector3& kMax, const Vector4& kColor);
		void AddAABB(RenderPackage& pkRenderPackage, const Vector3& kMin,const Vector3& kMax, const Vector4& kColor);

		void AddTextBillboard(RenderPackage& kRenderPackage,const RenderState& kRenderState,const Vector3& kPos,float fScale,const string& strText,ZMaterial* pkMaterial,ZGuiFont* pkFont,bool bCentered);
		void AddText(RenderPackage& kRenderPackage,const Vector3& kPos,float fScale,const string& strText,ZMaterial* pkMaterial,ZGuiFont* pkFont,bool bCentered);


		void     EditColor_Set(string strName, float f, float g, float b);
		Vector3	GetEditColor(string strName);		
		
};

RENDER_API char* GetOpenGLErrorName(GLenum id );

#endif



