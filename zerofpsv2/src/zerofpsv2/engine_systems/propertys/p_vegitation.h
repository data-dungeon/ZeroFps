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

/// Property to create bushes / trees.
class ENGINE_SYSTEMS_API P_Vegitation : public Property {
	private:
		struct vegitation
		{
			Vector3 kPos;
			Vector3 kRot;
			float fWindStart;
		};

		TextureManager*	m_pkTexMan;
		Render*				m_pkRender;	
		ZeroFps*				m_pkFps;

		vector<vegitation>	m_akPositions;

		ZFResourceHandle* m_pkTexture;	
		string				m_kTexture; 		
			
		Vector3				m_kScale;
		float					m_fRadius;		
		int					m_iAmount;
		int					m_iSize;		
		float					m_fWind;
		
		bool					m_CheckedForHM;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );		
	
	public:
		P_Vegitation();
		~P_Vegitation();
		
		void Update();
		void Init();

		void SetTexture(const char* acNewTex);
		void SetScale(Vector3 kScale);
		void UpdateSet();
		
		void AddPos(Vector3 kPos);
		void Clear();
		
		void CalculateRadius();
		
		void Random(P_HMRP2* pkHmrp2 = NULL);
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};

ENGINE_API Property* Create_VegitationProperty();

#endif

