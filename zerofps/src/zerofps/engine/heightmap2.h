#ifndef _HEIGHTMAP2_H_
#define _HEIGHTMAP2_H_


#include "../basic/basic.pkg"
#include "../render/render.pkg"


#include "engine_x.h"


class ENGINE_API HM2_vert
{
	public:
		float		fHeight;
		Vector3	kNormal;
};

class ENGINE_API HM2_face
{
	public:
		unsigned int iIndex[3];
};

class ENGINE_API HM2_level
{
	public:
		vector<Vector3>	kVertex;
		vector<Vector3>	kNormal;
		vector<HM2_face>	kIndex;
};

class ENGINE_API HM2_patch
{
	public:
		vector<HM2_level>	kLevels;
};


class ENGINE_API Heightmap2
{
	private:
		
		class LodVertex
		{
			public:			
				
		
		};
		
		
		TextureManager*	m_pkTexMan;
	
		vector<HM2_vert>	m_kBasicData;
		vector<HM2_patch> m_kRenderData;
		
		int					m_iWidth;
		int					m_iHeight;
		
		int					m_iLodLevels;
		int					m_iPatchWidth;		
		int					m_iPatchHeight;		
		
		
		bool LoadBasicDataFromImage(const char* acFile);
		void SmoothBasicData(int iLoops);	
		
		void GenerateNormals();
		void GenerateNormals(int iStartx,int iStartz,int iWidth,int iHeight);
		
		void GenerateRenderData();
		void GeneratePatch(HM2_patch* pkPatch,int iStartX,int iStartY,int iStopX,int iStopY);		
		void GenerateLodLevel(HM2_patch* pkPatch,float iDiff);
		
	public:
		Heightmap2();

		bool CreateHMFromImage(const char* acFile);		
		HM2_vert* GetVert(int x,int y){return &m_kBasicData[(y*m_iWidth)+x];};
	
	
		//friends
		friend class Render;
};


#endif


