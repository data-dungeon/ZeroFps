#ifndef _ENGINE_MADPROPERTY_H_
#define _ENGINE_MADPROPERTY_H_

#include "../mad/mad_modell.h"
#include "../../engine/zerofps.h"
#include "../../engine/property.h"
#include <string>
#include "../engine_systems_x.h"
using namespace std;

#define NO_ANIMATION_ID	-1

/// Property to add a Modell (MAD) to a object.
class ENGINE_SYSTEMS_API MadProperty : public Property, public Mad_Modell {
	private:
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
	
		float	m_fLod;

	public:
		MadProperty();
		MadProperty(string strResName);
		void CloneOf(Property* pkProperty) { }
		
		ZeroFps* m_pkZeroFps;

		bool	m_bIsVisible;

		void SetBase(const char* acName);
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);


		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

};

Property* Create_MadProperty();


#endif



