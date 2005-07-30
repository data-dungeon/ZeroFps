#ifndef _P_HEIGHTMAP_H_
#define _P_HEIGHTMAP_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../mad/mad_core.h"

using namespace std;


class P_Heightmap;

class ENGINE_SYSTEMS_API HMSelectionData
{
	public:
		P_Heightmap*	m_pkHeightMap;	// Hmap this vertex is part of.
		int				x;
		int				y;
		float				m_fValue;
		Vector3			m_kWorld;			// World coo of this hmap vertex.
};


class ENGINE_SYSTEMS_API P_Heightmap : public Property 
{
	private:
		class HeightmapArrays
		{
			public:
				vector<Vector3>	m_kVertexData;
				vector<Vector2>	m_kTextureData;
				vector<Vector3>	m_kNormalData;	
				vector<Vector4>	m_kColorData;	
				ZVertexBuffer*		m_pkVBO;		
				
				HeightmapArrays()
				{
					m_pkVBO = NULL;
				};
				
				~HeightmapArrays()
				{
					if(m_pkVBO)
						delete m_pkVBO;
				}
		};
		
		
		ZShaderSystem*		m_pkZShaderSystem;
		Light*				m_pkLight;
		Render*				m_pkRender;	
		
		
		vector<ZFResourceHandle*>		m_kMaterials;
		
		ZFResourceHandle* m_pkMaterial;	
		LightProfile		m_kLightProfile;				//used for lighting				
	
		
		float				m_fScale;
		vector<float>	m_kHeightData;
		vector<char>	m_kTextureIDs;
		
		int				m_iWidth;
		int				m_iHeight;
		float				m_fMaxValue;
		int				m_iRows;
		int				m_iCols;
	
		bool				m_bHaveRebuilt;
	
		vector<Vector3>	m_kVertexData;
		vector<Vector2>	m_kTextureData;
		vector<Vector3>	m_kNormalData;	
		ZVertexBuffer*		m_pkVBO;
	
		vector<HeightmapArrays*>	m_kDataArrays;
	
		void RebuildArrays();
		Vector3 GenerateNormal(int x,int y);
		void DrawHeightmap();
	
		void BuildTextureArrays();
		void AddPolygon(HeightmapArrays* pkNewArrays,int x,int y,int i,bool bTop);
		void DrawTexturedHeightmap();
	public:
		P_Heightmap();
		~P_Heightmap();

		void Update();
		void Init();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);		
		void PackTo( NetPacket* pkNetPacket,int iConnectionID);
		void PackFrom( NetPacket* pkNetPacket,int iConnectionID);


		void Smooth(vector<HMSelectionData>* kSelectionData = NULL);
		void Modify(vector<HMSelectionData>* kSelectionData,float fMod);
		void SetTexture(vector<HMSelectionData>* kSelectionData,char iTexture);
	
		void SetSize(int iWidth,int iHeight);
		void SetMaxValue(float fMax)							{	m_fMaxValue = fMax;	};
		
		void GetCollData(vector<Mad_Face>* pkFace,vector<Vector3>* pkVertex , vector<Vector3>* pkNormal);
		float CalculateRadius();
		float GetHeight(float x,float y);
		bool  Inside(float x,float y);
		void GetSelection(const Vector3& kCenter, float fInRadius, float fOutRadius,vector<HMSelectionData>* pkSelectionData);


		static void Stitch(vector<HMSelectionData>* pkSelectionData);

};

Property* Create_HeightmapProperty();
void ENGINE_SYSTEMS_API Register_HeightmapProperty(ZeroFps* pkZeroFps);

#endif





