#ifndef _ENGINE_VEGITATIONPROPERTY_H_
#define _ENGINE_VEGITATIONPROPERTY_H_

#include "../engine_systems_x.h"
#include "../../engine/property.h"
#include <iostream>
#include <string>
#include "../../basic/zfsystem.h"
#include "../../engine/zerofps.h"
#include "../../basic/zfresource.h"

using namespace std;

class P_HMRP2;

/// Property to create bushes / trees.
class ENGINE_SYSTEMS_API P_Vegitation : public Property {
	private:
		struct vegitation
		{
			Vector3 kPos;
			Vector3 kRot;
			float fWindStart;
		};

/*		struct LodLevel
		{
			vector<Vector3>	m_kVertexArray;
			vector<Vector3>	m_kNormalArray;
			vector<Vector2>	m_kTextureArray;		
		};
		vector<LodLevel>	m_kLodLevels;*/
		
		TextureManager*	m_pkTexMan;
		Light*				m_pkLight;
		Render*				m_pkRender;	
		ZeroFps*				m_pkFps;
		ZShaderSystem*		m_pkZShaderSystem;
		
		vector<vegitation>	m_akPositions;

		
		ZFResourceHandle* m_pkMaterial;	
		string				m_strMaterialFile; 		
			
		Vector3				m_kScale;
		float					m_fRadius;		
		int					m_iAmount;
		int					m_iSize;		
		float					m_fWind;
		
		LightProfile		m_kLightProfile;				//used for lighting		
		
		bool					m_CheckedForHM;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );		
	
// 		void BuildArrays();
// 		void DrawArray(int iLodLevel);
		
	public:
		P_Vegitation();
		~P_Vegitation();
		
		void Update();
		void Init();

		void SetMaterial(const char* acNewTex);
		void SetScale(Vector3 kScale);
		void UpdateSet();
		
		void AddPos(const Vector3& kPos);
		void Clear();
		
		void CalculateRadius();
		
		void Random(P_HMRP2* pkHmrp2 = NULL);
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

ENGINE_API Property* Create_VegitationProperty();

#endif

