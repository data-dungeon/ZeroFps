#define TEXT_MAX_LENGHT 256
#define FONTWIDTH 32
#define TEX_SCALE 2

#ifndef _RENDER_RENDER_H_
#define _RENDER_RENDER_H_


#include <cstring>
#include <iostream>
#include <vector>
#include <cmath>
#include "frustum.h"
#include "render_x.h"
#include "texturemanager.h"
#include "light.h"
#include "../engine/fh.h"
#include "../engine/zfresource.h"
#include "zshader.h"

class HeightMap;
class Heightmap2;

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

class RENDER_API Render : public ZFObject {
	private:
		Frustum*				m_pkFrustum;
		TextureManager*	m_pkTexMan;
		Light*				m_pkLight;
		ZShader*				m_pkZShader;
		
		char			aCurentFont[256];
		bool			m_FogEnable;		
		int			m_iSlicesize;					//	height meens greater detail att longer range		
		int			m_iLodUpdate;		
		Vector3		m_kOldCamPos;	
		
		int			m_iMaxLandscapeLayers;
		int			m_iDrawLandscape;

		GLuint		m_iHmTempList;

		int			m_iScreenShootNum;
		
		ZFResourceHandle	m_kConsoleText;
		
		ZMaterial	m_kHeightmapMaterial;
		

		void RunCommand(int cmdid, const CmdArgument* kCommand) { }
   	
	public:
		int	m_iDetail;				//	grid size of lod tiles for the terran
		int	m_iViewDistance;		//	how far until we cut the landscape	
		int	m_iFpsLock;
		int	m_iAutoLod;

		PolygonMode m_eLandscapePolygonMode;
	
		bool StartUp();
		bool ShutDown()	{ return true;	}
		bool IsValid()		{ return true;	}
		Render();

		TextureManager* GetTexMangager()		{	return m_pkTexMan; }		///< Get ptr to texture manger.
		
		
		void Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture);
		void Sphere(Vector3 kPos,float fRadius,int iRes,Vector3 kColor,bool bSolid);		
		
		void PrintChar(char cChar);
		void Print(Vector3 kPos,Vector3 kHead,Vector3 kScale,char* aText);

		void PrintChar2(char cChar);
		void Print2(Vector3 kPos, char* aText);

		void SetFont(char* aFont);
		void Line(Vector3 kPos1,Vector3 kPos2);
		void Dot(float x,float y,float z);
		void SetColor(Vector3 kColor);
		void DrawConsole(char* m_aCommand,vector<char*>* m_kText, int iStartLine); 

		void DrawHMLodSplat(HeightMap* kmap,Vector3 CamPos,int iFps);
		void DrawAllHM(HeightMap* kmap,Vector3 CamPos,bool bBorders);
		void DrawPatch(HeightMap* kmap,Vector3 CamPos,int xp,int zp,int iSize,bool bBorders);
		
		void DrawPatch_Vim1(HeightMap* kmap,Vector3 CamPos,int xp,int zp,int iSize);
		void DrawHMVertex(HeightMap* kMap);
		void DrawBlocks(HeightMap* kmap);

		void DrawHM2(Heightmap2* pkMap,Vector3 kCamPos);

		void DrawSkyBox(Vector3 CamPos,Vector3 kHead,int iHor,int iTop);
		void DrawSkyBox_SixSided(Vector3 CamPos,Vector3 kHead,int* aiSideTextures);
		void DrawWater(Vector3 kCamPos,Vector3 kPosition,Vector3 kHead,int iSize,int iStep,int iTexture, float fBlendValue);
		void DrawSimpleWater(Vector3 kPosition,Vector4 kColor,int iSize,int iTexture);
		void SetFog(Vector4 kFogColor,float FogStart,float FogStop,bool FogEnable);		
		void GiveTexCor(float &iX,float &iY,int iNr);
		void DrawCross(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture1);//,int iTexture2);
		void DrawBillboard(Matrix4& kModelMatrix,Vector3& kPos,float fSize,int iTexture);

		void DrawBoundSphere(float fRadius, Vector3);
		void DrawBoundingBox(Vector3 kPos,Vector3 kRot,Vector3 kScale);
		void DrawBox(Vector3 kPos,Vector3 kRot,Vector3 kScale,int iTexture);
		void DrawColorBox(Vector3 kPos,Vector3 kRot,Vector3 kScale,Vector3 kColor);
		void DrawAABB( float x, float y, float z, float sizex,float sizey,float sizez, Vector3 kColor );
		void DrawAABB( Vector3 kMin,Vector3 kMax, Vector3 kColor );

		void GetMinMax(HeightMap* kMap, float& fMin, float& fMax, int xp,int zp,int iSize);

		void DumpGLState(void);

		void Draw_AxisIcon(float scale = 1.0);													///< Draw axis lines.
		void Draw_MarkerCross(Vector3 kPos, Vector3 Color, float fScale = 1.0);		///< Draw a cross made up of lines.

		void CaptureScreenShoot( int m_iWidth, int m_iHeight );							///< Take a screenshoot and save it as a TGA.		
};

RENDER_API void RenderDLL_InitExtGL(void);
RENDER_API char* GetOpenGLErrorName(GLenum id );

#endif



