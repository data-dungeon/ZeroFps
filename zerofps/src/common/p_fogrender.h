/* Richard Svensson

	Client side property for rendering fog

*/


#ifndef _P_FOGRENDER_H_
#define _P_FOGRENDER_H_

#include "../zerofps/engine/objects.pkg"
#include "../zerofps/engine/engine.pkg"
#include "../zerofps/basic/basic.pkg"
#include <iostream>
#include "common_x.h"

using namespace std;


class COMMON_API P_FogRender: public Property {
	private:
		Render*	m_pkRender;
		TextureManager* m_pkTexMan;
			
		string 	m_sFogTexture;
			
	public:
		Vector3 	m_kScale;
		
		
		P_FogRender();
		void CloneOf(Property* pkProperty) { }
		void Update();
		
		void Explore(float x,float y,float r);
		void UnExplore(float x,float y, float r);
		void SetScale(Vector3 kScale) {m_kScale = kScale;};

};

COMMON_API Property* Create_P_FogRender();


#endif






