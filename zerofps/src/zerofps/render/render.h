#define TEXT_MAX_LENGHT 256
#define FONTWIDTH 32
#define TEX_SCALE 2

#ifndef _RENDER_RENDER_H_
#define _RENDER_RENDER_H_

#include "../basic/basic.pkg"
#include <cstring>
#include <iostream>
#include <vector>
#include <cmath>
#include "frustum.h"
#include "render_x.h"
//#include "../engine/engine.pkg"
#include "texturemanager.h"
#include "light.h"

using namespace std;

class RENDER_API Render : public ZFObject {
	private:
		Frustum* m_pkFrustum;
		TextureManager* m_pkTexMan;
		Light* m_pkLight;
		
		char aCurentFont[256];
		bool m_FogEnable;		
		int m_iSlicesize;	  //height meens greater detail att longer range		
		int m_iLodUpdate;		
		Vector3 m_kOldCamPos;		

		GLuint m_iHmTempList;

		void RunCommand(int cmdid, const CmdArgument* kCommand) { }

	public:
		int m_iDetail;			//grid size of lod tiles for the terran
		int m_iViewDistance;	//how far until we cut the landscape	
		int m_iFpsLock;
		int m_iAutoLod;
	
		Render();

		TextureManager* GetTexMangager() { return m_pkTexMan; }
		
/*		
		void GetFrustum();
		bool PointInFrustum(Vector3 kPoint);
		bool SphereInFrustum(Vector4 kPoint);
		bool CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez );
*/		
		void Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture);
		
		
		void PrintChar(char cChar);
		void Print(Vector3 kPos,Vector3 kHead,Vector3 kScale,char* aText);
		void SetFont(char* aFont);
		void Line(Vector3 kPos1,Vector3 kPos2);
		void Dot(float x,float y,float z);
		void SetColor(Vector3 kColor);
//		void DrawHMQT(HeightMap* kmap);		
//		void DrawQuad(HM_vert* vert,int width);
		void DrawConsole(char* m_aCommand,vector<char*>* m_kText, int iStartLine); 
		

		void DrawHMLodSplat(HeightMap* kmap,Vector3 CamPos,int iFps);
		void DrawAllHM(HeightMap* kmap,Vector3 CamPos);
		void DrawPatch(HeightMap* kmap,Vector3 CamPos,int xp,int zp,int iSize);
		
/*		
		void G4DrawHMLodSplat(HeightMap* kmap,Vector3 CamPos,int iFps);
		void G4DrawAllHM(HeightMap* kmap,Vector3 CamPos);
		void G4DrawPatch(HeightMap* kmap,Vector3 CamPos,int xp,int zp,int iSize);
*/		
		
		void DrawHM(HeightMap* kmap);
		void DrawHMlod(HeightMap* kmap,Vector3 CamPos,int iFps);		
		void DrawSkyBox(Vector3 CamPos,Vector3 kHead,int iHor,int iTop);
		void DrawWater(Vector3 kCamPos,Vector3 kPosition,Vector3 kHead,int iSize,int iStep,int iTexture);
		void DrawSimpleWater(Vector3 kPosition,Vector4 kColor,int iSize,int iTexture);
		void SetFog(Vector4 kFogColor,float FogStart,float FogStop,bool FogEnable);		
		void GiveTexCor(float &iX,float &iY,int iNr);
		void DrawCross(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture1);//,int iTexture2);
//		void DrawGrassPatch(Vector3 kCamPos,Vector3 kPos,Vector3 kScale,int fW,int iNr,HeightMap* kMap,int iTexture,int iFps);
		void DrawBillboard(Matrix4& kModelMatrix,Vector3& kPos,float fSize,int iTexture);

		void DrawBoundSphere(float fRadius, Vector3);
		void DrawBox(Vector3 kPos,Vector3 kRot,Vector3 kScale,int iTexture);

		void DumpGLState(void);
};

RENDER_API void RenderDLL_InitExtGL(void);
//RENDER_API char* GetOpenGLErrorName(GLenum id );

#endif



