#ifndef _ENGINE_VEGITATIONPROPERTY_H_
#define _ENGINE_VEGITATIONPROPERTY_H_

#include "../engine_systems_x.h"
#include "../../engine/property.h"
#include <iostream>
#include <string>
#include "../../basic/zfobjectmanger.h"
#include "../../engine/zerofps.h"

using namespace std;


class ENGINE_SYSTEMS_API VegitationProperty: public Property {
	private:
		struct vegitation
		{
			Vector3 kPos;
			Vector3 kRot;
			float fWindStart;
		};

		vector<vegitation>	m_akPositions;

		Frustum*				m_pkFrustum;
		TextureManager*	m_pkTexMan;
		Render*				m_pkRender;	
		ZeroFps*				m_pkFps;
			
		string				m_kTexture; 		
		int					m_iTexture;	
		Vector3				m_kScale;
		float					m_fRadius;
		
		float					m_fWind;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );		
	
	public:
		VegitationProperty();
		void CloneOf(Property* pkProperty) { }
		void Update();
		void Init();

		void SetTexture(const char* acNewTex);
		void SetScale(Vector3 kScale);
		void UpdateSet();
		
		void AddPos(Vector3 kPos);
		void Clear();
		
		void CalculateRadius();
		
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
		
};

ENGINE_API Property* Create_VegitationProperty();

#endif

