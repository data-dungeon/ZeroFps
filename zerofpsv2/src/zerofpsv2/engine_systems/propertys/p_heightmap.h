#ifndef _P_HEIGHTMAP_H_
#define _P_HEIGHTMAP_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../mad/mad_core.h"
#include "../../render/zocculusionquery.h"
#include "../../render/zvertexbuffer.h"

using namespace std;

class Camera;
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

class ENGINE_SYSTEMS_API Vegitation
{
	public:
		ZFResourceHandle*			m_pkMaterial;
		
		vector<LightProfile*>	m_kLightProfiles;
		vector<RenderPackage*>	m_kRenderPackages;
		
		Vegitation()
		{
			m_pkMaterial = NULL;
		};
		
		~Vegitation()
		{
			delete m_pkMaterial;
			for(int i = 0;i<m_kRenderPackages.size();i++)
				delete m_kRenderPackages[i];
		
			for(int i = 0;i<m_kLightProfiles.size();i++)
				delete m_kLightProfiles[i];		
		};
		
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
				vector<Vector3>	m_kTangentData;	
				vector<Vector3>	m_kBiTangentData;	
				vector<Vector4>	m_kColorData;
				
		};
		
		
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSLight*			m_pkLight;
		ZSSRender*			m_pkRender;	
		
		
		LightProfile		m_kLightProfile;				//used for lighting				
		
		map< Camera*,ZOcculusionTest>	m_kOCTests;
		bool					m_bHaveOCTested;
		float					m_fLastOcculusionTime;
		bool					m_bOculled;
			
		int					m_iLod;
			
		float				m_fScale;		
		int				m_iSize;
		float				m_fMaxValue;
		int				m_iRows;
		bool				m_bHaveRebuilt;
		
		float				m_fExtremeMax;
		float				m_fExtremeMin;
		
	
		ZMaterial		m_kBottom;
		vector<RenderPackage>			m_kRenderPackages;
		vector<ZFResourceHandle*>		m_kMaterials;		
		vector<float>						m_kHeightData;
		vector<signed char>				m_kTextureIDs;
		vector<unsigned char>			m_kBrightness;

		vector<Vegitation*>				m_kVegitation;
			
		vector<vector<HeightmapArrays*> >	m_kLodLevels;
		
		void FindExtremeValues(float& fMax,float& fMin);
		Vector3 GenerateNormal(int x,int y);	
		void BuildTextureArrays();
		void AddPolygon(HeightmapArrays* pkNewArrays,int x,int y,int i,bool bTop,int iStep = 1);
		void AddVertex(HeightmapArrays* pkNewArrays,int x,int y,int iID);
		void DrawTexturedHeightmap();
		
		signed char GetMaterialID(const string& strMaterial);
		void PurgeUnusedMaterials();
		
		void BuildVegitation();
		void AddVegitable(RenderPackage& kRenderPackage,Vector3& kPos,Vector3& kNormal,float fHeight,float fBrightness);
		
	public:
		P_Heightmap();
		~P_Heightmap();

		void Update();
		void GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState);
		void Init();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);		
		void PackTo( NetPacket* pkNetPacket,int iConnectionID);
		void PackFrom( NetPacket* pkNetPacket,int iConnectionID);


		void Smooth();
		void Modify(vector<HMSelectionData>* kSelectionData,float fMod);
		void SetTexture(vector<HMSelectionData>* kSelectionData,const string& strMaterial);
		void SetBrightness(vector<HMSelectionData>* kSelectionData,float fBrightness);

		void SetSize(int iSize);
		void SetMaxValue(float fMax);
		
		bool TestLine(const Vector3& kStart,const Vector3& kStop, Vector3& kResult);
		void GetCollData(vector<Mad_Face>* pkFace,vector<Vector3>* pkVertex , vector<Vector3>* pkNormal);
		float CalculateRadius();
		float GetHeight(float x,float y);	//world
		bool  Inside(float x,float y);		//world
		void GetSelection(const Vector3& kCenter, float fStrength, float fInRadius, float fOutRadius,vector<HMSelectionData>* pkSelectionData);


		static void Stitch(vector<HMSelectionData>* pkSelectionData);
		static void SmoothSelection(vector<HMSelectionData>* pkSelectionData);
		static void FlattenSelection(vector<HMSelectionData>* kSelectionData, float fSample);
};

Property* Create_HeightmapProperty();
void ENGINE_SYSTEMS_API Register_HeightmapProperty(ZSSZeroFps* pkZeroFps);

#endif





