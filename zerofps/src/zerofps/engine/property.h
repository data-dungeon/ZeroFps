#ifndef _OPROPERTY_H_
#define _OPROPERTY_H_

#include "../basic/basic.pkg"
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
//#include "object.h"
#include "engine_x.h"
#include "network.h"
#include <cfloat>

//#include "engine.pkg"

using namespace std;

struct Collision;
class Object;
class PropertyFactory;

enum PROPERTY_TYPES{
	PROPERTY_TYPE_ALL,
	PROPERTY_TYPE_NORMAL,
	PROPERTY_TYPE_RENDER,
	PROPERTY_TYPE_PHYSIC,
};

enum PROPERTY_SIDE{
	PROPERTY_SIDE_ALL,
	PROPERTY_SIDE_CLIENT,
	PROPERTY_SIDE_SERVER
};

class ENGINE_API PropertyValues
{
public:
	PropertyValues();
	string kValueName;
	void* pkValue;
	int iValueType;
	float fUpperBound;
	float fLowerBound;
};

class ENGINE_API Property 
{
	private:
		ZeroFps* m_pkZeroFps;

	protected:
		Object *m_pkObject;
		PropertyFactory*	m_pkPropertyFactory;
		/*ENGINE_API struct PropertyValues
		{
			PropertyValues();
			string kValueName;
			void* pkValue;
			int iValueType;
			float fUpperBound;
			float fLowerBound;
		};*/
	
		enum ValueTypes
		{
			VALUETYPE_INT,
			VALUETYPE_FLOAT,
			VALUETYPE_BOOL,
			VALUETYPE_STRING,
			VALUETYPE_VECTOR3,
			VALUETYPE_VECTOR4,

		};

		virtual vector<PropertyValues> GetPropertyValues();
		virtual bool HandleSetValue( string kValueName ,string kValue );
		virtual bool HandleGetValue( string kValueName );

	public:
		
		

		bool SetValue(string kValueName ,string kValue);
		string GetValue(string kValueName);
		vector<string> GetValueNames();
		

		int m_iSortPlace;		//place in update queue
		bool m_bSortDistance;
		
		int m_iType;			//property type
		int m_iSide;			//server or client property
		
		bool	bNetwork;		
		char m_acName[50];		
		
		
		
		Property();
		virtual ~Property(){};
		virtual void Touch(Collision* pkCol){};
		virtual void Init(){};
		virtual void Update(){};			
		virtual void PackTo(NetPacket* pkNetPacket){} ;
		virtual void PackFrom(NetPacket* pkNetPacket){} ;
		virtual void Save(ZFMemPackage* pkPackage);
		virtual void Load(ZFMemPackage* pkPackage);
		inline void SetObject(Object* pkObject){m_pkObject=pkObject;};
		inline Object *GetObject() {return m_pkObject;};

		bool operator<(Property& kOther);

};


	

#endif

