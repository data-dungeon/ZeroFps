#ifndef _ENGINE_WORLDINFOPROPERTY_H_
#define _ENGINE_WORLDINFOPROPERTY_H_

#include <string>
#include "../engine_systems_x.h"
#include "../../engine/property.h"
#include "../../basic/zfsystem.h"
#include "../../engine/zerofps.h"

class ZeroFps;

class ENGINE_SYSTEMS_API P_WorldInfo : public Property {
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
		
		Vector3 m_kSunColor;
		Vector3 m_kMoonColor;
		Vector3 m_kAmbientColor;
		
		P_WorldInfo();
		void Update();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
};

Property* Create_WorldInfoProperty();

#endif






