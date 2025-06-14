#ifndef _MOVEPSPROP_H_
#define _MOVEPSPROP_H_

#include "particleproperty.h"
#include "../../../basic/math.h"

class MovePSProp : public ParticleProperty
{
private:
	Vector3 m_kForce, m_kRelPos;
public:
	void Update(const RenderState* pkRenderState);
	MovePSProp ( PSystem *pkParent );

	void SetForce ( Vector3 kForce )						{ m_kForce = kForce; }

};

#endif





