#ifndef _ENGINE_OPROPERTY_H_
#define _ENGINE_OPROPERTY_H_

#include "../basic/basic.pkg"
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include "engine_x.h"
#include "network.h"
#include <cfloat>

using namespace std;

struct Collision;
class Object;
class PropertyFactory;
class GameMessage;

/*	When property should be updated. 
*/enum PROPERTY_TYPES
{
	PROPERTY_TYPE_ALL		= 1,		// Run on all.
	PROPERTY_TYPE_NORMAL	= 2,		// Run at normal update.
	PROPERTY_TYPE_RENDER = 4,		// Run at render update.
	PROPERTY_TYPE_PHYSIC = 8,		// Run at physics update.
};

/* On wich side the propertys should update. */
enum PROPERTY_SIDE
{	
	PROPERTY_SIDE_CLIENT	= 16,		// Run on client
	PROPERTY_SIDE_SERVER = 32,		// Run on server
	PROPERTY_SIDE_ALL		= 48,		// Run on both client/server
};

enum ValueTypes
{
	VALUETYPE_INT,
	VALUETYPE_FLOAT,
	VALUETYPE_BOOL,
	VALUETYPE_STRING,
	VALUETYPE_VECTOR3,
	VALUETYPE_VECTOR4,
	VALUETYPE_CHARS,
	VALUETYPE_CHARVAL,
};

class ENGINE_API PropertyValues
{
public:
	string	kValueName;
	void*		pkValue;
	int		iValueType;
	float		fUpperBound;
	float		fLowerBound;
	int		iNumberOfValues;
	bool		bResize;
	bool		bVector;

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
		ZeroFps*					m_pkZeroFps;

		///beware of the the code /Gubb  //////////////
		string ValueToString(void *pkValue, PropertyValues *pkPropertyValue); 
		bool StringToValue(string kValue, void *pkValue, PropertyValues *pkPropertyValue);
		////////////////////////////////////////////7

	protected:
		Object*					m_pkObject;
		PropertyFactory*		m_pkPropertyFactory;
	

		virtual vector<PropertyValues> GetPropertyValues();
		virtual bool HandleSetValue( string kValueName ,string kValue );
		virtual bool HandleGetValue( string kValueName );

	public:
		int							m_iNetUpdateFlags;					

		Property();
		virtual ~Property() { };

		int	m_iSortPlace;		//	place in update queue
		bool	m_bSortDistance;
		
		int	m_iType;				// property type
		int	m_iSide;				// server or client property
		
		bool	bNetwork;		
		bool	m_bSave;
		char	m_acName[50];		


		// Property Edit Interface
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
		
		// Game Messages
		virtual void HandleGameMessage(GameMessage& Msg);

		// Virtuals
		virtual void Touch(Collision* pkCol) {};					//executet when collision occurs
		virtual void Init() {};											//executet when property is added to an object
		virtual void Update() {};										//executet once every game loop
		virtual void PackTo(NetPacket* pkNetPacket) {} ;
		virtual void PackFrom(NetPacket* pkNetPacket) {} ;
		
		// Save / Load
		virtual void Save(ZFMemPackage* pkPackage);				//save property
		virtual void Load(ZFMemPackage* pkPackage);				//load property
		virtual void CloneOf(Property* pkProperty) = 0;

		// Inlines
		inline void SetObject(Object* pkObject)	{	m_pkObject=pkObject;	};
		inline Object *GetObject()						{	return m_pkObject;	};

		bool operator<(Property& kOther);

		friend class Object;
};


	

#endif

