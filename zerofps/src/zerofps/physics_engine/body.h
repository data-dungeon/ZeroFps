#ifndef _BODY_H_
#define _BODY_H_

#include "../engine/mad_core.h"

#include <iostream>

using namespace std;

class Body
{
	private:
		Mad_Core*	m_pkMad;
		float			m_fBoundingRadius;		

	public:
		bool			m_bPolygonCheck;
		bool			m_bGravity;
		
		Body();
		void Reset();
};

#endif


