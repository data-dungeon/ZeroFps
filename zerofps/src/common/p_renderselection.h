/* Richard Svensson

	Client side property for rendering selected units marker

*/


#ifndef _P_RENDERSELECTION_H_
#define _P_RENDERSELECTION_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

using namespace std;


class COMMON_API P_RenderSelection: public Property {
	private:
		Render*	m_pkRender;
	
	public:
		bool					m_bEnabled;
	
		P_RenderSelection();
		void CloneOf(Property* pkProperty) { }
		void Update();
		
};

COMMON_API Property* Create_P_RenderSelection();


#endif



