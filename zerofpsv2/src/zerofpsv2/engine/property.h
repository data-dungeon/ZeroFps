#ifndef _ENGINE_OPROPERTY_H_
#define _ENGINE_OPROPERTY_H_

#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include "engine_x.h"
#include "network.h"
#include <cfloat>
#include "../basic/zfmempackage.h"
#include <typeinfo> 
#include "../basic/zfvfs.h"
using namespace std;

struct Collision;
class Entity;
class PropertyFactory;
class EntityManager;
class GameMessage;

/*	When property should be updated. 
*/enum PROPERTY_TYPES
{
	/*
	PROPERTY_TYPE_ALL		= 1,		// Run on all.
	PROPERTY_TYPE_NORMAL	= 2,		// Run at normal update.
	PROPERTY_TYPE_RENDER	= 4,		// Run at render update.
	PROPERTY_TYPE_PHYSIC	= 8,		// Run at physics update.
	*/
	PROPERTY_TYPE_NORMAL	= 1,		// Run at normal update.
	PROPERTY_TYPE_RENDER	= 2,		// Run at render update.
	PROPERTY_TYPE_PHYSIC	= 4,		// Run at physics update.
	PROPERTY_TYPE_ALL		= 7,		// Run on all.
};

/* On wich side the propertys should update. */
enum PROPERTY_SIDE
{	
	PROPERTY_SIDE_CLIENT	= 16,		// Run on client
	PROPERTY_SIDE_SERVER	= 32,		// Run on server
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

/////GUBB ULTRACLASS!!/// 
class Property; 
class TPointerBase
{
public:
	
	virtual bool Set(Property*) = 0;
	virtual bool Unset(Property*) = 0;
	virtual const type_info& GetType() = 0;
	
};

template <typename T> class TPointer : public TPointerBase
{
public:
	TPointer(T **pPointer) : m_ppPointer(pPointer) {}
	bool Set(Property *pProp) 
	{
		if(*m_ppPointer)
		{
			return false;
		}
		else
		{
			if((*m_ppPointer) = dynamic_cast<T*>(pProp))
			{
				return true;
			}
			else 
			{
				return false;
			}
		}
	}
	bool Unset(Property *pProp)
	{
		if(!(*m_ppPointer))
		{
			return false;
		}
		if((*m_ppPointer) == pProp)
		{
			*m_ppPointer= 0;
			return true;
		}
		else 
			return true;
	}
	const type_info& GetType()
	{
		return typeid(T*);
	}
	
private:
	T **m_ppPointer;
};

/////END OF GUBB ULTRACLASS!!/// 

/**	\brief	A property of a object.

  A Property is the things that make the objects in the world diffrent. 
*/
class ENGINE_API Property 
{
	private:

		///beware of the the code /Gubb  //////////////
		string ValueToString(void *pkValue, PropertyValues *pkPropertyValue); 
		bool StringToValue(string kValue, void *pkValue, PropertyValues *pkPropertyValue);
		
		vector<TPointerBase*> m_kPointerVector;
		void PropertyFound(Property* pkProperty); 
		void PropertyLost(Property* pkProperty); 
		///////////////////////////////////////////
	protected:
		ZeroFps*				m_pkZeroFps;			// Vim - Change To ObjectManger
		Entity*				m_pkObject;
		EntityManager*    m_pkObjMan;
		//PropertyFactory*		m_pkPropertyFactory;	// Vim - Removes
	

		virtual vector<PropertyValues> GetPropertyValues();
		virtual bool HandleSetValue( string kValueName ,string kValue );
		virtual bool HandleGetValue( string kValueName );
		
		///////////EVIL GUBB WAS HERE
		virtual void PointerFound(const type_info& Type) {};
		virtual void PointerLost(const type_info& Type) {};
		
		template<class T> void GetProperty(T *&pT)
		{
			pT = 0;
			if(m_pkObject)
			{
				vector<Property*>::iterator kIt = m_pkObject->m_akPropertys.begin();
				while(kIt != m_pkObject->m_akPropertys.end())
				{
					if(pT = dynamic_cast<T*>(*kIt))
					{
						this->PointerFound(typeid(T*));
						kIt = m_pkObject->m_akPropertys.end();
					}
					else
						++kIt;
				}
			}
			m_kPointerVector.push_back(new TPointer<T>(&pT));
		}
		//////////////////////////////////////////////

	public:
		int						m_iNetUpdateFlags;					
		int						NeedToSend( ) { return m_iNetUpdateFlags; }

		Property();
		virtual ~Property();

		int		m_iSortPlace;			//	place in update queue
		bool	m_bSortDistance;
		
		int		m_iType;				// property type
		int		m_iSide;				// server or client property
		
		bool	bNetwork;				// True if property needs to be sent o network.
		bool	m_bSave;				// True if propertys should be saved with object.
		char	m_acName[50];			// Name of Property. Set when property is created.	


		// Property Edit Interface
		//evil gubb code! haha/////////////////////////////////////////
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
		virtual void Touch(int iId) {};					//executet when collision occurs		
		virtual void ZoneChange(int iCurrent,int iNew) {};
		virtual void Init() {};											//executet when property is added to an object
		virtual void Update() {};										//executet once every game loop
		virtual void PackTo(NetPacket* pkNetPacket, int iConnectionID)		{	};
		virtual void PackFrom(NetPacket* pkNetPacket, int iConnectionID)	{	};
		
		// Save / Load
		virtual void Save(ZFIoInterface* pkFile){};
		virtual void Load(ZFIoInterface* pkFile){};		
//		virtual void Save(ZFMemPackage* pkPackage) {};				//save property
//		virtual void Load(ZFMemPackage* pkPackage) {};				//load property
		virtual void CloneOf(Property* pkProperty) = 0;

		// Inlines
		inline void SetObject(Entity* pkObject)	{	m_pkObject=pkObject;	};
		inline Entity *GetObject()						{	return m_pkObject;	};

		bool operator<(Property& kOther);

		friend class Entity;
};


	

#endif

