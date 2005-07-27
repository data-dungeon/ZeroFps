#ifndef _P_HEIGHTMAP_H_
#define _P_HEIGHTMAP_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../mad/mad_core.h"

using namespace std;


class ENGINE_SYSTEMS_API P_Heightmap : public Property 
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		Light*				m_pkLight;
		Render*				m_pkRender;	
		
		ZFResourceHandle* m_pkMaterial;	
		LightProfile		m_kLightProfile;				//used for lighting		
		
		
		float				m_fScale;
		vector<float>	m_kHeightData;
		int				m_iWidth;
		int				m_iHeight;
		int				m_iRows;
		int				m_iCols;
	
		bool				m_bHaveRebuilt;
	
		vector<Vector3>	m_kVertexData;
		vector<Vector2>	m_kTextureData;
		vector<Vector3>	m_kNormalData;	
		ZVertexBuffer*		m_pkVBO;
	
		void RebuildArrays();
		Vector3 GenerateNormal(int x,int y);
		void DrawHeightmap();
	
		void Smoth();
	public:
		P_Heightmap();
		~P_Heightmap();

		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);		
		void PackTo( NetPacket* pkNetPacket,int iConnectionID);
		void PackFrom( NetPacket* pkNetPacket,int iConnectionID);


	
		void SetSize(int iWidth,int iHeight);
		void GetCollData(vector<Mad_Face>* pkFace,vector<Vector3>* pkVertex , vector<Vector3>* pkNormal);
		float CalculateRadius();
		float GetHeight(float x,float y);
};

Property* Create_HeightmapProperty();
void ENGINE_SYSTEMS_API Register_HeightmapProperty(ZeroFps* pkZeroFps);

#endif





