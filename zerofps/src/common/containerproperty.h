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
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
		void HandleGameMessage(GameMessage& Msg);
		bool m_bOpen;
	
	public:
		const bool IsOpen() {return m_bOpen; }
		void OpenContainer();
		Container	m_kContainer;
		
		ContainerProperty();
		~ContainerProperty();

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);

		void RegisterActions();
		void UnRegisterActions();
		void Init();
};

COMMON_API Property* Create_ContainerProperty();


#endif





