#ifndef _OPROPERTY_H_
#define _OPROPERTY_H_

#include <string.h>
#include <iostream>
#include "engine_x.h"

using namespace std;

class Object;

class ENGINE_API Property 
{
	protected:
		Object *m_pkObject;
		
	public:
		char m_acName[50];
			
			
		void SetObject(Object* pkObject);
		virtual void Update()=0;

};
#endif

