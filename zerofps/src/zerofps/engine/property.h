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
			
		virtual ~Property(){};
		virtual void Update()=0;			
		
		inline void SetObject(Object* pkObject){m_pkObject=pkObject;};
		inline Object *GetObject() {return m_pkObject;};


};
#endif

