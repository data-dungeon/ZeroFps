#ifndef _BODYPROPERTY_H_
#define _BODYPROPERTY_H_

#include "iostream"

using namespace std;

#include "../physics_engine/physics_engine.pkg" 
#include "property.h"
#include "zerofps.h"

class ENGINE_API BodyProperty :public Property
{
	private:
		Physics_Engine*	m_pkPhysics_Engine;
		ZeroFps*				m_pkZeroFps;	
			
		Body					m_kBody;
	
		bool					m_bHaveSetBody;
	
		vector<PropertyValues> GetPropertyValues();
		
	public:
		BodyProperty();		
		~BodyProperty();
		void Init();
		
		void Update();
		
		void CloneOf(Property* pkProperty) { }

		bool Enable();
		bool Disable();
		
		void SetBodyToObject();
		void SetObjectToBody();

		void SetBodyPos(Vector3 kPos);
		Vector3 GetBodyPos();
};

Property* Create_BodyProperty();

#endif


