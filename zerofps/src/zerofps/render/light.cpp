#include "light.h"

Light::Light(Vector3 *kCamPos) {
	m_kCamPos=kCamPos;
		
	m_iNrOfLights=8;
	m_akCurrentLights=new LightSource[m_iNrOfLights];
	glEnable(GL_LIGHTING);
}

void Light::Add(LightSource *kNewLight){
	m_kLights.push_back(kNewLight);
}

void Light::Remove(LightSource *kLight) {
	m_kLights.remove(kLight);

/*	for(list<LightSource*>::iterator it=m_kLights.begin();it!=m_kLights.end();it++) {
		if(*it==kLight)	{
			m_kLights.erase(it);
			cout<<"deleted object"<<endl;
//			cout<<"deltete"<<endl;
			return;
		}


	}*/
}

void Light::Update() {
	for(list<LightSource*>::iterator it=m_kLights.begin();it!=m_kLights.end();it++) {
	  glLightfv(GL_LIGHT0,GL_DIFFUSE,&(*it)->kDiffuse[0]);
	  glLightfv(GL_LIGHT0,GL_SPECULAR,&(*it)->kSpecular[0]);  
  	glLightfv(GL_LIGHT0,GL_AMBIENT,&(*it)->kAmbient[0]);
  	Vector4 temp;
  	temp=(*(*it)->kPos)+(*it)->kConstPos;
  	
  	if((*it)->iType==DIRECTIONAL_LIGHT)
  		temp[3]=0;
  	else
  		temp[3]=1;
	  
	  cout<<"POS "<<temp[0]<<" "<<temp[1]<<" "<<temp[2]<<endl;
	  glLightfv(GL_LIGHT0,GL_POSITION,&temp[0]);	  
		
		
		
	}

}














