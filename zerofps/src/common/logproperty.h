/* Richard Svensson

	This property describes a basic log item.	

*/


#ifndef _LOGPROPERTY_H_
#define _LOGPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

using namespace std;


class COMMON_API LogProperty: public Property {
	private:
		void ReadLog();
		void RegisterActions();
		void UnRegisterActions();
		
		vector<PropertyValues> GetPropertyValues();		
		
	public:
		string m_kLogIcon;
		string m_sLog;

		LogProperty();
		~LogProperty();		
		void Init();

		void HandleGameMessage(GameMessage& Msg);

		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
				

};

COMMON_API Property* Create_LogProperty();


#endif



