#ifndef _PARTICLEPROPERTY_H_
#define _PARTICLEPROPERTY_H_

//#include "../../engine_systems/common/psystem.h"

class PSystem;

class ParticleProperty
{
protected:
	PSystem *m_pkParent;

public:
	virtual void Update() = 0;
};



#endif

