#ifndef _ENGINE_PHYSICPROPERTY_H_
#define _ENGINE_PHYSICPROPERTY_H_

#include "../../engine/property.h"
#include "../physicsengine/collisionshape.h"
#include <vector>
#include "../engine_systems_x.h"

#include "../../basic/zfsystem.h"

using namespace std;

class CollisionShape;

/// Property to make a object use the physicsengine.
class ENGINE_SYSTEMS_API PhysicProperty :public Property
{
	private:
		CollisionShape* m_pkColSphere;
		CollisionShape* m_pkColObject;

		vector<PropertyValues> GetPropertyValues();
	
	public:
		bool m_bGravity;
		bool m_bFloat;
		bool m_bSolid;
		bool m_bGlide;
		bool m_bStride;
		
		float m_bStrideHeight;
		
		float m_fColShape;
		bool m_bDummyValue;
		
		Vector3 m_kOldPos;
		Vector3 m_kNewPos;	//for the physicengine
		Vector3 m_kNewVel;	//for the physicengine
		Vector3 m_kNewAcc;	//for the physicengine
		
		
		PhysicProperty();
		~PhysicProperty();		
		void CloneOf(Property* pkProperty) { }
		
		void Update();
		
		float GetBoundingRadius();
		
		CollisionShape* GetColSphere(){return m_pkColSphere;};
		CollisionShape* GetColShape(){return m_pkColObject;};

		void SetColShape(CollisionShape* pkCs);

		bool HandleSetValue( string kValueName ,string kValue );

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
};

Property* Create_PhysicProperty();


#endif







