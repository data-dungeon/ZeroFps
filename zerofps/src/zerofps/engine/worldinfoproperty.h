#ifndef _WORLDINFOPROPERTY_H_
#define _WORLDINFOPROPERTY_H_

#include <string>

#include "property.h"
//#include "zerofps.h"

class ZeroFps;

class WorldInfoProperty: public Property {
	private:
		ZeroFps *m_pkFps;
	
	public:
		string m_kSkyBoxHor;
		string m_kSkyBoxTop;
		Vector3 m_kSkyBoxRotate;
		
		Vector3 m_kFogColor;
		float m_fFogStart;
		float m_fFogStop;
				
		bool m_bWater;
		
		WorldInfoProperty();
		void Update();
		
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
};

Property* Create_WorldInfoProperty();

#endif






