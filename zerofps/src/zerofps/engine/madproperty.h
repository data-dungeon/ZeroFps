#ifndef _ENGINE_MADPROPERTY_H_
#define _ENGINE_MADPROPERTY_H_

#include "mad_modell.h"
#include "zerofps.h"
#include "property.h"
#include <string>
using namespace std;

#define NO_ANIMATION_ID	-1

class ENGINE_API MadProperty : public Property, public Mad_Modell {
	private:
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
	
		float	m_fLod;

	public:
		MadProperty();
		MadProperty(string strResName);
		
		Frustum* m_pkFrustum;
		ZeroFps* m_pkZeroFps;

		bool	m_bIsVisible;

		void SetBase(const char* acName);
		void Update();

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);


		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

};

Property* Create_MadProperty();


#endif



