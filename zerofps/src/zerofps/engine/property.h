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
	PROPERTY_TYPE_ALL,
	PROPERTY_TYPE_NORMAL,
	PROPERTY_TYPE_RENDER
};

enum PROPERTY_SIDE{
	PROPERTY_SIDE_ALL,
	PROPERTY_SIDE_CLIENT,
	PROPERTY_SIDE_SERVER
};


class ENGINE_API Property 
{
	protected:
		Object *m_pkObject;


	public:
		int m_iType;			//property type
		int m_iSide;			//server or client property
		
		bool	bNetwork;		
		char m_acName[50];		
		
		
		
		Property();
		virtual ~Property(){};
		virtual void Update()=0;			
		virtual void PackTo(NetPacket* pkNetPacket) ;
		virtual void PackFrom(NetPacket* pkNetPacket) ;
		
//		inline int GetType(){ return m_iType ;};
		inline void SetObject(Object* pkObject){m_pkObject=pkObject;};
		inline Object *GetObject() {return m_pkObject;};
};


	

#endif

