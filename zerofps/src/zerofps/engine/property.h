#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include "object.h"
#include <string.h>

class Object;

class Property {
	private:
		Object *m_pkObject;
	
	public:
		char m_acName[10];
				
		void SetObject(Object* pkObject);
		virtual void Update()=0;

};

#endif






