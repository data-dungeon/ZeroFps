#include "light.h"

Light::Light(Vector3 *kCamPos) {
	m_kCamPos=kCamPos;
		
	m_iNrOfLights=8;
	m_akCurrentLights=new LightSource[m_iNrOfLights];
}

void Light::Add(LightSource *kNewLight){
	m_kLights.push_back(kNewLight);
}

void Light::Remove(LightSource *kLight) {
	for(list<LightSource*>::iterator it=m_kLights.begin();it!=m_kLights.end();it++) {
		if(*it==kLight)		
			cout<<"deltete"<<endl;



	}
}

void Light::Update() {
	for(list<LightSource*>::iterator it=m_kLights.begin();it!=m_kLights.end();it++) {
		
	
	}

}














