#ifndef _MADPROPERTY_H_
#define _MADPROPERTY_H_

#include "mad_modell.h"
#include "zerofps.h"
#include "property.h"
#include <string>
using namespace std;

#define NO_ANIMATION_ID	-1


class ENGINE_API MadProperty : public Property, public Mad_Modell {
	public:
		MadProperty();
		MadProperty(Mad_Core* pkModell);
 
		Frustum* m_pkFrustum;
		ZeroFps* m_pkZeroFps;

		void SetBase(const char* acName);
		void Update();

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
};

Property* Create_MadProperty();


#endif



