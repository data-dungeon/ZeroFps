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
/*	vector<LightSource*>::iterator it;

	while(it!=m_kLights.end()){
		if(it.reference=kLight)
			cout<<"deleted"<<endl;
	
	}*/
}

void Light::Update() {
	


//	for(int i=0;i<m_iLights.size();i++) {	
		
	
///	}

}














