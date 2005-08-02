#ifndef _ENGINE_OPROPERTY_H_
#define _ENGINE_OPROPERTY_H_

#include <string>
#include <vector>
#include "engine_x.h"
#include <cfloat>
#include "entity.h"

using namespace std;

struct Collision;
//class Entity;
class PropertyFactory;
class EntityManager;
class GameMessage;

/*	When property should be updated. 
*/enum PROPERTY_TYPES
{
	PROPERTY_TYPE_NORMAL				= 1,		// Run at normal update.
	PROPERTY_TYPE_RENDER				= 2,		// Run at render update.
	PROPERTY_TYPE_ALL					= 3,		// Run on all.
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

/**	\brief	A property of a Entity.
	 \ingroup Engine

  A Property is the things that make the Entitys in the world diffrent. 
*/
class ENGINE_API Property
{
	private:
		vector<bool>	m_kNetUpdateFlags;
				
		///beware of the the code /Gubb  //////////////
		string ValueToString(void *pkValue, PropertyValues *pkPropertyValue); 
		bool StringToValue(const string& kValue, void *pkValue, PropertyValues *pkPropertyValue);
		
		vector<TPointerBase*> m_kPointerVector;
		void PropertyFound(Property* pkProperty); 
		void PropertyLost(Property* pkProperty); 
		///////////////////////////////////////////
	protected:
		ZeroFps*				m_pkZeroFps;			
		Entity*				m_pkEntity;
		EntityManager*    m_pkEntityManager;
	
		//netflags
		void	SetNrOfConnections(int iConNR);

		
		
		virtual vector<PropertyValues> GetPropertyValues();
		virtual bool HandleSetValue( const string& kValueName ,const string& kValue );
		virtual bool HandleGetValue( const string& kValueName );
		
		///////////EVIL GUBB WAS HERE
		virtual void PointerFound(const type_info& Type) {};
		virtual void PointerLost(const type_info& Type) {};
		
		template<class T> void GetProperty(T *&pT)
		{
			pT = 0;
			if(m_pkEntity)
			{
				vector<Property*>::iterator kIt = m_pkEntity->m_akPropertys.begin();
				while(kIt != m_pkEntity->m_akPropertys.end())
				{
					if(pT = dynamic_cast<T*>(*kIt))
					{
						this->PointerFound(typeid(T*));
						kIt = m_pkEntity->m_akPropertys.end();
					}
					else
						++kIt;
				}
			}
			m_kPointerVector.push_back(new TPointer<T>(&pT));
		}
		//////////////////////////////////////////////

	public:
		int		m_iSortPlace;			//	place in update queue (for rendering)
		bool		m_bSortDistance;		// do we want to sort propertys by dystance from camera (for rendering)		
		int		m_iVersion;				//used for multiple versionsof load ,shuld be incresed if save data changes		
		int		m_iType;					// property type
		int		m_iSide;					// server or client property		
		bool		m_bNetwork;				// True if property needs to be sent o network.
		bool		m_bSave;					// True if propertys should be saved with entity.
		char		m_acName[50];			// Name of Property. Set when property is created.	


		Property();
		virtual ~Property();

		// Property Edit Interface
		//evil gubb code! haha/////////////////////////////////////////
		bool SetValue(const string& kValueName ,const string& kValue);
		bool SetValue(const string& kValueName, unsigned int iIndex ,const string& kValue);
		bool CheckIfResize(const string& kValueName);
		int GetNumberOfValues(const string& kValueName);
		string GetValue(const string& kValueName);
		string GetValue(const string& kValueName, unsigned int iIndex);
		vector<string> GetValueNames();
		float GetUpperBound(const string& kValueName);
		float GetLowerBound(const string& kValueName);
		bool Resize(const string& kValueName, unsigned int uiNewSize);
		bool CheckIfVector(const string& kValueName);
		
		/////////////////////////////////////////////////////////7
		
		//handle netupdate flags		
		void	SetNetUpdateFlag(int iConID,bool bValue);
		void	SetNetUpdateFlag(bool bValue);
		bool	GetNetUpdateFlag(int iConID);
		void	ResetAllNetUpdateFlags();
		
		// Game Messages
		virtual void OnEvent(GameMessage& Msg);

		// Virtuals
		virtual void Touch(int iId) {};								//executet when collision occurs		
		virtual void ZoneChange(int iCurrent,int iNew) {};
		virtual void Init() {};											//executet when property is added to an entity
		virtual void Update() {};										//executet once every game loop
		virtual void PackTo(NetPacket* pkNetPacket, int iConnectionID) {};
		virtual void PackFrom(NetPacket* pkNetPacket, int iConnectionID) {};
		
		// Save / Load
		virtual void Save(ZFIoInterface* pkFile) {};
		virtual void Load(ZFIoInterface* pkFile,int iVersion) {};		

		// Inlines
		inline Entity *GetEntity()	const							{	return m_pkEntity;							};
		inline bool IsType(const char* czType)	const			{	return (strcmp(czType,m_acName) == 0);	};
		
		
		virtual bool operator<(const Property& kOther) const;

		friend class Entity;
};


	

#endif

