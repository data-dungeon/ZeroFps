/* Richard Svensson

	This property describes a basic door item.	

*/


#ifndef _DOORPROPERTY_H_
#define _DOORPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"


using namespace std;


class COMMON_API DoorProperty: public Property {
	private:
		void OpenDoor();
		void CloseDoor();
		
		vector<PropertyValues> GetPropertyValues();		
		
	public:
		bool	m_bOpen;
		bool	m_bLocked;
		string m_sKeyName;

		DoorProperty();

		void HandleGameMessage(GameMessage& Msg);

//		void Save(ZFMemPackage* pkPackage);
//		void Load(ZFMemPackage* pkPackage);
				

};

COMMON_API Property* Create_DoorProperty();


#endif






