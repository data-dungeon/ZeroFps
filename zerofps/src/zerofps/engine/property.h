#ifndef _PROPERTY_H_
#define _PROPERTY_H_

//#include "object.h"

#include "objects.pkg"
#include <string.h>

class Property 
{
	protected:
		Object *m_pkObject;
		
	public:
		Property();
		
		char m_acName[10];
			
		void SetObject(Object* pkObject);
		virtual void Update()=0;


};
#endif

/*
protected:
		Object *m_pkObject;
	
	public:
		char m_acName[10];
				
		void SetObject(Object* pkObject);
		virtual void Update()=0;

*/










