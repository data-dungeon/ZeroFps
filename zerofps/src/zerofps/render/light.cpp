#include "light.h"

Light::Light() {
	m_iNrOfLights=8;
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
	m_kActiveLights.clear();
	TurnOffAll();

	for(list<LightSource*>::iterator it=m_kLights.begin();it!=m_kLights.end();it++) {
		Vector3 kPos=(*(*it)->kPos)+(*it)->kConstPos;
		float kDistance=(*m_kCamPos-kPos).Length();

//		if(kDistance<100){
			if(m_kActiveLights.size()<m_iNrOfLights)
				m_kActiveLights.push_back(*it);		
//		}
		
	}
	
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












