#ifndef _OPROPERTY_H_
#define _OPROPERTY_H_



//#include "objects.pkg"
//#include "object.h"
#include <string.h>
#include <iostream>

using namespace std;

class Object;

class Property 
{
	protected:
		Object *m_pkObject;
		
	public:
		char m_acName[50];
			
			
		void SetObject(Object* pkObject);
		virtual void Update()=0;

};
#endif

