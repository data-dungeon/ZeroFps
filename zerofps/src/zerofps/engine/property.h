#ifndef _ENGINE_OPROPERTY_H_
#define _ENGINE_OPROPERTY_H_

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
class GameMessage;

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
	string kValueName;
	void* pkValue;
	int iValueType;
	float fUpperBound;
	float fLowerBound;
	int iNumberOfValues;
	bool bResize;
	bool bVector;
	inline PropertyValues() : 
			fLowerBound(FLT_MIN),
			fUpperBound(FLT_MAX),
			iNumberOfValues(1),
			bResize(false),
			bVector(false) {} 
};

/// A property of a object.
class ENGINE_API Property 
{
	private:
		ZeroFps* m_pkZeroFps;
		///beware of the the code /Gubb  //////////////
		string ValueToString(void *pkValue, PropertyValues *pkPropertyValue); 
		bool StringToValue(string kValue, void *pkValue, PropertyValues *pkPropertyValue);
		////////////////////////////////////////////7

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
		
		//evil gubb code! haha/////////////////////////////////////////7
		bool SetValue(string kValueName ,string kValue);
		bool SetValue(string kValueName, unsigned int iIndex ,string kValue);
		bool CheckIfResize(string kValueName);
		int GetNumberOfValues(string kValueName);
		string GetValue(string kValueName);
		string GetValue(string kValueName, unsigned int iIndex);
		vector<string> GetValueNames();
		float GetUpperBound(string kValueName);
		float GetLowerBound(string kValueName);
		bool Resize(string kValueName, unsigned int uiNewSize);
		bool CheckIfVector(string kValueName);
		
		//bool SetValue(string kValueName ,string kValue);
		//string GetValue(string kValueName);
		//vector<string> GetValueNames();
		/////////////////////////////////////////////////////////7

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
		
		virtual void HandleGameMessage(GameMessage& Msg);

		inline void SetObject(Object* pkObject){m_pkObject=pkObject;};
		inline Object *GetObject() {return m_pkObject;};

		bool operator<(Property& kOther);

};


	

#endif

