#ifndef _OPROPERTY_H_
#define _OPROPERTY_H_

#include "../basic/basic.pkg"
#include <string>
#include <iostream>
#include "engine_x.h"
#include "network.h"

using namespace std;

class Object;

enum PROPERTY_TYPES{
	PROPERTY_TYPE_NORMAL=0,
	PROPERTY_TYPE_STATIC=10,
	PROPERTY_TYPE_DYNAMIC=20
};

class ENGINE_API Property 
{
	protected:
		Object *m_pkObject;
		int m_iType;			//property type, can be used when running update to update only selected types


	public:
		Property();
		
		bool	bNetwork;
		
		char m_acName[50];
			
		virtual ~Property(){};
		virtual void Update()=0;			
		virtual void PackTo(NetPacket* pkNetPacket) ;
		virtual void PackFrom(NetPacket* pkNetPacket) ;
		
		inline int GetType(){ return m_iType ;};
		inline void SetObject(Object* pkObject){m_pkObject=pkObject;};
		inline Object *GetObject() {return m_pkObject;};
};


	

#endif

