#ifndef _FLOATPROPERTY_H_
#define _FLOATPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"

class FloatProperty: public Property {
	private:
		ZeroFps *m_pkFps;
		
	public:
		float m_fGravity;		
	
		FloatProperty();
		void Update();
		void Touch(CollisionData* Data);


};

Property* Create_FloatProperty();

#endif

