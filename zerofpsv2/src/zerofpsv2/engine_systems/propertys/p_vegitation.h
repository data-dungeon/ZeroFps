#ifndef _ENGINE_VEGITATIONPROPERTY_H_
#define _ENGINE_VEGITATIONPROPERTY_H_

#include "../engine_systems_x.h"
#include "../../engine/property.h"
#include <iostream>
#include <string>
#include "../../basic/zfsystem.h"
#include "../../engine/zerofps.h"
#include "../../basic/zfresource.h"

#include "p_heightmap.h"

using namespace std;

class Camera;


/// Property to create bushes / trees.
class ENGINE_SYSTEMS_API P_Vegitation : public Property {
	private:
		struct vegitation
		{
			Vector3 kPos;
			Vector3 kRot;
			float fWindStart;
		};

		map< Camera*,ZOcculusionTest>	m_kOCTests;
		bool					m_bHaveOCTested;
		float					m_fLastOcculusionTime;
		bool					m_bOculled;

		vector<Vector3>	m_kVertexArray;
		vector<Vector3>	m_kNormalArray;
		vector<Vector2>	m_kTextureArray;
		vector<Vector4>	m_kColorArray;
		
		TextureManager*	m_pkTexMan;
		ZSSLight*			m_pkLight;
		ZSSRender*			m_pkRender;	
		ZSSZeroFps*			m_pkFps;
		ZShaderSystem*		m_pkZShaderSystem;
		
		vector<vegitation>	m_akPositions;

		
		ZFResourceHandle* m_pkMaterial;	
		string				m_strMaterialFile; 		
			
		Vector3				m_kAABBMin;
		Vector3				m_kAABBMax;
		Vector3				m_kScale;
		float					m_fRadius;		
		int					m_iAmount;
		int					m_iSize;		
		float					m_fWind;
		bool					m_bCastShadow;
		
		LightProfile		m_kLightProfile;				//used for lighting		
		RenderPackage		m_kRenderPackage;
		RenderPackage		m_kDebugRenderPackage;	
		
		bool					m_CheckedForHM;
		
		bool HandleSetValue( const string& kValueName ,const string& kValue );		
	
		void BuildArrays();
		void DrawArray();
		
		void CalculateRadius();
		void CreateAABB();
	public:
		P_Vegitation();
		~P_Vegitation();
		
		void GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState);
		void Update();
		void Init();

		void SetMaterial(const char* acNewTex);
		void SetScale(Vector3 kScale);
		void UpdateSet();
		
		void AddPos(const Vector3& kPos);
		void Clear();
				
		void Random(vector<P_Heightmap*>* pkHMaps= NULL);
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

ENGINE_API Property* Create_VegitationProperty();

#endif

