#ifndef _ENGINE_PROXYPROPERTY_H_
#define _ENGINE_PROXYPROPERTY_H_

#include "../../engine/property.h"
#include <iostream>
#include <limits.h>
#include "../../basic/zfobjectmanger.h"
#include "../../engine/zerofps.h"
#include "../../engine/levelmanager.h"
#include <string.h>
//#include "GL/glut.h"

using namespace std;

enum PROXYPROPERTY_AKTIONS{
	UPDATESTATUS,
	WAKEUP
};


class ENGINE_API ProxyProperty: public Property {
	private:
		ZeroFps* m_pkFps;
		LevelManager* m_pkLevelMan;
		
		list<Object*>* m_pkTrackers;
		
		float m_fRadius;	
		int m_iAktion;
			
	public:
		ProxyProperty();
		void CloneOf(Property* pkProperty) { }
		void Update();
		
		float GetDistance();
		
		void SetRadius(float fRadius) {m_fRadius= fRadius;};
		float GetRadius() {return m_fRadius;};
	
		void SetAktion(int iAktion) {m_iAktion=iAktion;};
		int GetAktion() {return m_iAktion;};
};

Property* Create_ProxyProperty();

#endif




