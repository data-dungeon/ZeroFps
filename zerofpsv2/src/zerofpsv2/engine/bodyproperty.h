#ifndef _BODYPROPERTY_H_
#define _BODYPROPERTY_H_
24539478tq9we7rfhoöauiwe46243y5
#include "iostream"

using namespace std;

#include "../physics_engine/physics_engine.pkg" 
#include "property.h"
#include "zerofps.h"

class ENGINE_API P_Body :public Property
{
	private:
		Physics_Engine*	m_pkPhysics_Engine;
		ZeroFps*				m_pkZeroFps;	
			
		Body					m_kBody;
	
		bool					m_bHaveSetBody;
		int					m_iMeshID;
		bool					m_bHaveSetMesh;
	
		vector<PropertyValues> GetPropertyValues();
		
	public:
		P_Body();		
		~P_Body();
		void Init();
		
		void Update();
		
		void CloneOf(Property* pkProperty) { }

		bool Enable();
		bool Disable();
		
		void SetBodyToObject();
		void SetObjectToBody();

		void SetBodyPos(Vector3 kPos);
		Vector3 GetBodyPos();
		
		bool SetUpMesh();
};

Property* Create_BodyProperty();

#endif


