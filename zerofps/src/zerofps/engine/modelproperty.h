#ifndef _MODELPROPERTY_H_
#define _MODELPROPERTY_H_

//#include "property.h"
//#include "objects.pkg"
#include "property.h"
#include <iostream>
#include "../engine/engine.pkg"

#include "GL/glut.h"

using namespace std;

class ModelProperty: public Property {

	public:
		ModelProperty();

		void Update();


};



#endif



