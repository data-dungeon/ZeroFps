#define TEXT_MAX_LENGHT 256
#define FONTWIDTH 32

#ifndef _RENDER_H_
#define _RENDER_H_

#include "render_x.h"
#include "../basic/basic.pkg"
#include <cstring>
#include <iostream>
#include <vector>
#include <cmath>
#include "texturemanager.h"
#include "../engine/engine.pkg"


using namespace std;

class RENDER_API Render {
	private:
		TextureManager* m_pkTexMan;
		char aCurentFont[256];
		bool m_FogEnable;		

		int m_iSlicesize;	  //height meens greater detail att longer range
		int m_iLodUpdate;
		int m_iGrassLodUpdate;		
		Vector3 m_kOldCamPos;
		
		Vector4 m_akFrustum[6];
		
	public:
		int m_iDetail;			//grid size of lod tiles for the terran
		int m_iGrassLod;
		int m_iViewDistance;	//how far until we cut the landscape	
		int m_iFpsLock;
		int m_iAutoLod;
	
		Render(TextureManager* pkTexMan);
		
		void GetFrustum();
		bool PointInFrustum(Vector3 kPoint);
		bool SphereInFrustum(Vector3 CamPos,Vector4 kPoint);
		bool CubeInFrustum( float x, float y, float z, float sizex,float sizey,float sizez );
		
		void Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture);
		void Pyra(float x,float y,float z);
		void PrintChar(char cChar);
		void Print(Vector3 kPos,Vector3 kHead,Vector3 kScale,char* aText);
		void SetFont(char* aFont);
		void Line(Vector3 kPos1,Vector3 kPos2);
		void Dot(float x,float y,float z);
		void SetColor(Vector3 kColor);
//		void DrawHMQT(HeightMap* kmap);		
//		void DrawQuad(HM_vert* vert,int width);
		void DrawConsole(char* m_aCommand,vector<char*>* m_kText); 
		
		void DrawHM(HeightMap* kmap);
		void DrawHMlod(HeightMap* kmap,Vector3 CamPos,int iFps);		
		void DrawSkyBox(Vector3 CamPos);
		void DrawWater(Vector3 kCamPos,Vector3 kPosition,Vector3 kHead,int iSize,int iStep);
		void DrawSimpleWater(Vector3 kPosition,int iSize);
		void SetFog(Vector4 kFogColor,float FogDensity,float FogStart,float FogStop,bool FogEnable);		
		void GiveTexCor(float &iX,float &iY,int iNr);
		void DrawCross(Vector3 kCamPos,Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture1,int iTexture2);
		void DrawGrassPatch(Vector3 kCamPos,Vector3 kPos,Vector3 kScale,int fW,int iNr,HeightMap* kMap,int iTexture,int iFps);
		
};


#endif



