#ifndef _OPROPERTY_H_
#define _OPROPERTY_H_

#include "../basic/basic.pkg"
#include <string>
#include <iostream>
#include "engine_x.h"
#include "network.h"

//#include "engine.pkg"

using namespace std;

class Object;
class PropertyFactory;

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

/*
class ENGINE_API Property_Sort
{
	private:
	
	public:
		int m_iPlace;
		
		bool m_bDistance_Sort;
		
		bool operator<(Property_Sort kOther&){
			if(!m_bDistance_Sort)
			return m_iPlace < kOther.m_iPlace;
		}
	
};
*/


class ENGINE_API Property 
{
	private:
		ZeroFps* m_pkZeroFps;

	protected:
		Object *m_pkObject;
		PropertyFactory*	m_pkPropertyFactory;

	public:
		int m_iSortPlace;		//place in update queue
		bool m_bSortDistance;
		
		int m_iType;			//property type
		int m_iSide;			//server or client property
		
		bool	bNetwork;		
		char m_acName[50];		
		
		
		
		Property();
		virtual ~Property(){};
		virtual void Update()=0;			
		virtual void PackTo(NetPacket* pkNetPacket) ;
		virtual void PackFrom(NetPacket* pkNetPacket) ;
		inline void SetObject(Object* pkObject){m_pkObject=pkObject;};
		inline Object *GetObject() {return m_pkObject;};

		bool operator<(Property& kOther);

};


	

#endif

