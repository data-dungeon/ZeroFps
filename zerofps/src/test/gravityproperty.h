#ifndef _GRAVITYPROPERTY_H_
#define _GRAVITYPROPERTY_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/basic/basic.pkg"

class GravityProperty: public Property {
	private:
		ZeroFps *m_pkFps;
		
	public:
		float m_fGravity;		
	
		GravityProperty();
		void Update();



};


Property* Create_GravityProperty();

#endif



