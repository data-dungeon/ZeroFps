#ifndef _HEIGHTMAP2_H_
#define _HEIGHTMAP2_H_


#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "engine_x.h"


class ENGINE_API HM2_texcor
{
	public:
		float x;
		float y;
};

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
		vector<HM2_texcor> kTexCor;
};

class ENGINE_API HM2_patch
{
	public:
		vector<HM2_level>	kLevels;	//list of lod levels

		int iX;								//this patch pos in x
		int iY;								//this patch pos in y
		int iWidth;							//this patch width
		int iHeight;						//this patch height
		
		int iReLev;							//recomended lod level
		
		float fMaxHeight;					//maximum vertex height in this patch
		float fMinHeight;					//maximum vertex height in this patch
		float fAvrageHeight;
		
};


class ENGINE_API Heightmap2
{
	private:
		
		TextureManager*	m_pkTexMan;
		Frustum*				m_pkFrustum;	
	
		vector<HM2_vert>	m_kBasicData;
		vector<HM2_patch> m_kRenderData;
		
		
		int					m_iWidth;
		int					m_iHeight;
		
		int					m_iPatchWidth;		
		int					m_iPatchHeight;		
		
		float					m_fScale;
		
		float					m_fDetail;
		
		
		bool LoadBasicDataFromImage(const char* acFile);
		void SmoothBasicData(int iLoops);	
		
		void GenerateNormals();
		void GenerateNormals(int iStartx,int iStartz,int iWidth,int iHeight);
		
		void GenerateRenderData();
		void GeneratePatch(HM2_patch* pkPatch,int iStartX,int iStartY,int iStopX,int iStopY);		
		void GenerateLodLevel(HM2_patch* pkPatch,int iStep);
		
		void GeneratePatchMinMaxHeight(HM2_patch* pkPatch); 
		
	public:
		Heightmap2();

		bool CreateHMFromImage(const char* acFile);		
		void UpdateRecLodLevel(Vector3 kCamPos);
		void UpdateFrustumCulling();
		
		
		HM2_vert* GetVert(int x,int y)
		{
			if(x >= m_iWidth)
				cout<<"error X"<<x<<endl;
			
			if(x < 0)
				cout<<"error X"<<x<<endl;
				
			if(y >= m_iHeight)
				cout<<"error Y "<<y<<endl;
			
			if(y < 0)
				cout<<"error Y"<<y<<endl;
			
			return &m_kBasicData[(y*m_iWidth)+x];
		
		}; 
	
	
	
	
		//friends
		friend class Render;
};


#endif


