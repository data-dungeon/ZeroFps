#ifndef _BODYPROPERTY_H_
#define _BODYPROPERTY_H_

using namespace std;

#include "../physics_engine/physics_engine.pkg" 
#include "property.h"

class ENGINE_API BodyProperty :public Property
{
	private:
		Body					m_kBody;
		Physics_Engine*	m_pkPhysics_Engine;
	
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


