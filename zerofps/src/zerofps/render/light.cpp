#include "light.h"

Light::Light() {
	m_iNrOfLights=8;
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,0);
	glEnable(GL_LIGHTING);
}

void Light::SetCamera(Vector3 *kCamPos) {
	m_kCamPos=kCamPos;
}

void Light::Add(LightSource *kNewLight){
	m_kLights.push_back(kNewLight);
}

void Light::Remove(LightSource *kLight) {
	m_kLights.remove(kLight);
}

void Light::Update() {
	m_kActiveLights.clear();
	TurnOffAll();

	//loop trough all lightsources and find wich to view
	for(list<LightSource*>::iterator it=m_kLights.begin();it!=m_kLights.end();it++) {
		
		//always add light with priority >10
		if((*it)->iPriority>=10)
			if(m_kActiveLights.size()<m_iNrOfLights)
				m_kActiveLights.push_back(*it);			
				
		//if its a directional light add it if there is space
		if((*it)->iType==DIRECTIONAL_LIGHT){
			if(m_kActiveLights.size()<m_iNrOfLights)
				m_kActiveLights.push_back(*it);		
		//else add the light if it is bright enough
		} else {
			//		opengl LightIntesity equation	min(1, 1 / ((*it)-> + l*d + q*d*d))
			
			Vector3 kPos = (*(*it)->kPos)+(*it)->kConstPos;		
			float fDistance = (*m_kCamPos-kPos).Length();		
			float fIntensity = min(1, 1 / ((*it)->fConst_Atten + (*it)->fLinear_Atten*fDistance + (*it)->fQuadratic_Atten*fDistance*fDistance));
		
			if(fIntensity>0.05)
				if(m_kActiveLights.size()<m_iNrOfLights)
					m_kActiveLights.push_back(*it);					
		}
	}
	
	//loop trough selected lightsources and enable them
	int i=0;		
	for(vector<LightSource*>::iterator it=m_kActiveLights.begin();it!=m_kActiveLights.end();it++) {
 				
		GLenum light;				
		//wich light to change
  		switch(i) {
  			case 0:
				light=GL_LIGHT0;
				break; 		
  			case 1:
				light=GL_LIGHT1;
				break; 		
  			case 2:
				light=GL_LIGHT2;
				break; 		
  			case 3:
				light=GL_LIGHT3;
				break; 		
  			case 4:
				light=GL_LIGHT4;
				break; 		
  			case 5:
				light=GL_LIGHT5;
				break; 		
  			case 6:
				light=GL_LIGHT6;
				break; 		
  			case 7:
				light=GL_LIGHT7;
				break; 						
  		}
//		cout<<"LIGHT: "<<light;
		
		glLightfv(light,GL_DIFFUSE,&(*it)->kDiffuse[0]);
		glLightfv(light,GL_SPECULAR,&(*it)->kSpecular[0]);  
  		glLightfv(light,GL_AMBIENT,&(*it)->kAmbient[0]);		
		
		
		Vector4 temp;
  		switch ((*it)->iType) {
  			case DIRECTIONAL_LIGHT:
				temp=(*(*it)->kRot)+(*it)->kConstRot;  			
  				temp[3]=0;			
				glLightfv(light,GL_POSITION,&temp[0]);	    				
  				break;
  			case POINT_LIGHT:
  				temp=(*(*it)->kPos)+(*it)->kConstPos;
  				temp[3]=1;  						
				glLightfv(light,GL_POSITION,&temp[0]);	  
		  		
		  		glLightf(light,GL_CONSTANT_ATTENUATION,(*it)->fConst_Atten);
		  		glLightf(light,GL_LINEAR_ATTENUATION,(*it)->fLinear_Atten);
		  		glLightf(light,GL_QUADRATIC_ATTENUATION,(*it)->fQuadratic_Atten);
  				
  				break;
  			case SPOT_LIGHT:
  				temp=(*(*it)->kPos)+(*it)->kConstPos;
  				temp[3]=1;  						
				glLightfv(light,GL_POSITION,&temp[0]);	  
				
				temp=(*(*it)->kRot)+(*it)->kConstRot;
				glLightfv(light,GL_SPOT_DIRECTION,&temp[0]);
		  		
		  		glLightf(light,GL_CONSTANT_ATTENUATION,(*it)->fConst_Atten);
		  		glLightf(light,GL_LINEAR_ATTENUATION,(*it)->fLinear_Atten);
		  		glLightf(light,GL_QUADRATIC_ATTENUATION,(*it)->fQuadratic_Atten);
  				
  				glLightf(light,GL_SPOT_EXPONENT,(*it)->fExp);
  				glLightf(light,GL_SPOT_CUTOFF,(*it)->fCutoff);  				
  				
  				break;  		
  		}
  		
		glEnable(light);
		
   	i++;	
	}

}

void Light::TurnOffAll() {
	glDisable(GL_LIGHT0);	
	glDisable(GL_LIGHT1);	
	glDisable(GL_LIGHT2);	
	glDisable(GL_LIGHT3);	
	glDisable(GL_LIGHT4);	
	glDisable(GL_LIGHT5);	
	glDisable(GL_LIGHT6);	
	glDisable(GL_LIGHT7);		
}












