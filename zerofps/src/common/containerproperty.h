/* Richard Svensson

	This property describes a basic ingame container.	

*/


#ifndef _CONTAINERPROPERTY_H_
#define _CONTAINERPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "container.h"

using namespace std;

class COMMON_API ContainerProperty: public Property {
	private:
		Container	m_kContainer;
		
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
	
	public:
		ContainerProperty();

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
};

COMMON_API Property* Create_ContainerProperty();


#endif





