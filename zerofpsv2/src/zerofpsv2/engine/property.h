#ifndef _ENGINE_OPROPERTY_H_
#define _ENGINE_OPROPERTY_H_

#include <string>
#include <vector>
#include "engine_x.h"
#include <cfloat>
#include "entity.h"
#include "zssrenderengine.h"

using namespace std;

struct Collision;

class ZSSPropertyFactory;
class ZSSEntityManager;
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
		string			m_strValueName;
		void*				m_pkValue;
		ValueTypes		m_eValueType;
		float				m_fUpperBound;
		float				m_fLowerBound;
		unsigned int	m_iNumberOfValues;
		bool				m_bResize;
		bool				m_bVector;
	
		PropertyValues(const string& strName,ValueTypes eValueType,void* pkValue);

};


/**	\brief	A property of a Entity.
	 \ingroup Engine

  A Property is the things that make the Entitys in the world diffrent. 
*/
class ENGINE_API Property
{
	private:
		char				m_acName[50];			// Name of Property. Set when property is created.			
		vector<bool>	m_kNetUpdateFlags;
				
		///beware of the the code /Gubb  //////////////
		string ValueToString(void *pkValue, PropertyValues *pkPropertyValue); 
		bool StringToValue(const string& kValue, void *pkValue, PropertyValues *pkPropertyValue);

	protected:
		ZSSZeroFps*			m_pkZeroFps;			
		Entity*				m_pkEntity;
		ZSSEntityManager* m_pkEntityManager;
	
		//vector containing information on all property variables
		vector<PropertyValues>	m_kPropertyValues;
	
		//settings
		int		m_iSortPlace;			//	place in update queue (for rendering)
		bool		m_bSortDistance;		// do we want to sort propertys by dystance from camera (for rendering)		
		bool		m_bReverseSort;		// do we want to sort propertys by dystance from camera (for rendering)		
		int		m_iVersion;				//used for multiple versionsof load ,shuld be incresed if save data changes		
		int		m_iType;					// property type
		int		m_iSide;					// server or client property		
		bool		m_bNetwork;				// True if property needs to be sent o network.
		bool		m_bSave;					// True if propertys should be saved with entity.
		
		
		Property(const char* czName);		
		
		//netflags		
		void	SetNrOfConnections(int iConNR);
	
		virtual bool HandleSetValue( const string& kValueName ,const string& kValue )			{	return false;	};
		virtual void HaveSetValue( const string& kValueName )											{};
				
	public:

		virtual ~Property();

		// Property Edit Interface
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
		
		virtual vector<PropertyValues>& GetPropertyValues()												{	return m_kPropertyValues;	};
		
		/////////////////////////////////////////////////////////		
		
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
		virtual void GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState)	{};
		virtual void PackTo(NetPacket* pkNetPacket, int iConnectionID) {};
		virtual void PackFrom(NetPacket* pkNetPacket, int iConnectionID) {};
		
		// Save / Load
		virtual void Save(ZFIoInterface* pkFile) {};
		virtual void Load(ZFIoInterface* pkFile,int iVersion) {};		

		// Inlines
		inline Entity *GetEntity()	const							{	return m_pkEntity;							};
		inline bool IsType(const char* czType)	const			{	return (strcmp(czType,m_acName) == 0);	};
		inline const char* GetName()								{	return m_acName;								};
		
		virtual bool operator<(const Property& kOther) const;

		friend class Entity;
		friend class ZSSPropertyFactory;
};


	

#endif

