#ifndef _MOVEPSPROP_H_
#define _MOVEPSPROP_H_

#include "particleproperty.h"
#include "../../../basic/vector3.h"

class MovePSProp : public ParticleProperty
{
private:
	Vector3 m_kForce;
public:
	void Update();
	MovePSProp ( PSystem *pkParent );

	void SetForce ( Vector3 kForce )						{ m_kForce = kForce; }

};

#endif





