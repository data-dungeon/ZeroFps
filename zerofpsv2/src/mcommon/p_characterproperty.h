#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"


using namespace std;

class P_CharacterProperty: public Property
{
	private:


	public:

		P_CharacterProperty();
		~P_CharacterProperty();


};

MCOMMON_API Property* Create_P_CharacterProperty();

#endif








