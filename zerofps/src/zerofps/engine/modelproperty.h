#ifndef _MODELPROPERTY_H_
#define _MODELPROPERTY_H_

#include "property.h"
#include <iostream>

#include "GL/glut.h"

using namespace std;

class ENGINE_API ModelProperty: public Property {

	public:
		ModelProperty();

		void Update();


};



#endif



