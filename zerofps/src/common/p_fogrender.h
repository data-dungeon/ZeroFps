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


#define FOG_TEXTURE_SIZE 256

class COMMON_API P_FogRender: public Property {
	private:
		Render*	m_pkRender;
		TextureManager* m_pkTexMan;
			
		string 	m_sFogTexture;			
		bool		m_bHaveChanged;
		float 	m_fScale;			
	public:

		
		
		P_FogRender();
		void CloneOf(Property* pkProperty) { }
		void Update();
		
		void Explore(float x,float y,float r);
		void UnExplore(float x,float y, float r);
		void UnExploreAll();
		void ExploreAll();
		
		void SetScale(float fScale) {m_fScale = fScale;};

};

COMMON_API Property* Create_P_FogRender();


#endif






