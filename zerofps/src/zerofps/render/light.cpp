#include "light.h"
#include <algorithm>



LightSource::LightSource() {
//cout<<"new light"<<endl;
	//position and rotation
	kPos=NULL;
	kRot=NULL;
	kConstPos=Vector3(0,0,0);
	kConstRot=Vector3(0,0,0);
	
		//light color
	kDiffuse=Vector4(1,1,1,1);
	kAmbient=Vector4(0,0,0,1);
	kSpecular=Vector4(0,0,0,1);
	
	//spotlight
	fCutoff=20;
	fExp=20;
	
		//distance attenuation 
	fConst_Atten=0;
	fLinear_Atten=0;
	fQuadratic_Atten=0.01;

	iType=POINT_LIGHT;
	iPriority=0;
}


/*
bool LightSource::operator<(const LightSource& l) const 
{
	return (fIntensity < l.fIntensity)
		return true;
	else
		return false;		
}
*/


Light::Light() 
: ZFObject("Light") {
	m_iNrOfLights=8;				//this shuld never be greater than 8
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,0);
	glEnable(GL_LIGHTING);
}

void Light::SetCamera(Vector3 kCamPos) {
	m_kCamPos=kCamPos;
}

void Light::Add(LightSource *kNewLight){
	m_kLights.push_back(kNewLight);
}

void Light::Remove(LightSource *kLight) {
	m_kLights.remove(kLight);
}

bool comp(LightSource* x, LightSource* y)
{
	return x->fIntensity > y->fIntensity; 
}

void Light::Update() {
	m_kActiveLights.clear();
	m_kSorted.clear();	
	TurnOffAll();

	list<LightSource*>::iterator it;

	//loop trough all lightsources and find wich to view
	for(it=m_kLights.begin();it!=m_kLights.end();it++) {
		
		//always add light with priority >10
		if((*it)->iPriority>=10){
			(*it)->fIntensity=999999;
			m_kSorted.push_back(*it);			
			continue;
		}
		
		//if its a directional light add it if there is space
		if((*it)->iType==DIRECTIONAL_LIGHT){
			(*it)->fIntensity=999999;
			m_kSorted.push_back(*it);		
		//else add the light if it is bright enough
		} else {
			//		opengl LightIntesity equation	min(1, 1 / ((*it)-> + l*d + q*d*d))
			
			Vector3 kPos = (*(*it)->kPos)+(*it)->kConstPos;		
			float fDistance = (m_kCamPos-kPos).Length();		
			float fIntensity = min(1 , 1 / ( (*it)->fConst_Atten + ((*it)->fLinear_Atten*fDistance) + ((*it)->fQuadratic_Atten*(fDistance*fDistance)) ));
		
			if(fIntensity>0.01){
				(*it)->fIntensity=fIntensity;
				m_kSorted.push_back(*it);					
			}
		}
	}
	
	
	//Put the first 8 in m_kActiveLights
	int i=0;
// BM-NOTE: Vim: Vrווווווווווווווווlll hata ljus.
//#if defined(LINUX) 
	m_kSorted.sort(More_Light);
//#endif

	for( it=m_kSorted.begin();it!=m_kSorted.end();it++) {
		if(i >= m_iNrOfLights)
			break;
				
		m_kActiveLights.push_back(*it);				
		i++;				
	}
	
/*	
	cout<<"LIGHTS SIZE:"<<m_kActiveLights.size()<<endl;
	
	for(int i=0;i<m_kActiveLights.size();i++){
		cout<<m_kActiveLights[i]->fIntensity<<endl;
	
	}
*/
	
	int max=m_kActiveLights.size();
	if(max>8)
		max=8;
		
	//loop trough selected lightsources and enable them
	for( i=0;i<m_kActiveLights.size();i++) {
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
		glEnable(light);		
			
		glLightfv(light,GL_DIFFUSE, &m_kActiveLights[i]->kDiffuse[0]);
		glLightfv(light,GL_SPECULAR,&m_kActiveLights[i]->kSpecular[0]);  
		glLightfv(light,GL_AMBIENT, &m_kActiveLights[i]->kAmbient[0]);		
		
		Vector4 temp;
		float spotdir[]={0,0,-1};  		
  		switch (m_kActiveLights[i]->iType) {
  			case DIRECTIONAL_LIGHT:
				temp=(*m_kActiveLights[i]->kRot)+m_kActiveLights[i]->kConstRot;  			
  				temp[3]=0;			
				glLightfv(light,GL_POSITION,&temp[0]);	    				
  				
  				//seset spot
				glLightfv(light,GL_SPOT_DIRECTION,spotdir);		  				  		  				
  				glLightf(light,GL_SPOT_EXPONENT,0);
  				glLightf(light,GL_SPOT_CUTOFF,180);				  				
  				
  				break;
  			case POINT_LIGHT:
  				temp=(*m_kActiveLights[i]->kPos)+m_kActiveLights[i]->kConstPos;
  				temp[3]=1;  						
				glLightfv(light,GL_POSITION,&temp[0]);	  
		  		
  				
		  		glLightf(light,GL_CONSTANT_ATTENUATION,m_kActiveLights[i]->fConst_Atten);
		  		glLightf(light,GL_LINEAR_ATTENUATION,m_kActiveLights[i]->fLinear_Atten);
		  		glLightf(light,GL_QUADRATIC_ATTENUATION,m_kActiveLights[i]->fQuadratic_Atten);
  				
  				//seset spot		  		
				glLightfv(light,GL_SPOT_DIRECTION,spotdir);		  				  		  				
  				glLightf(light,GL_SPOT_EXPONENT,0);
  				glLightf(light,GL_SPOT_CUTOFF,180);				
				  				
  				break;
  			case SPOT_LIGHT:
  				temp=(*m_kActiveLights[i]->kPos)+m_kActiveLights[i]->kConstPos;
  				temp[3]=1;  						
				glLightfv(light,GL_POSITION,&temp[0]);	  
				
				temp=(*m_kActiveLights[i]->kRot)+m_kActiveLights[i]->kConstRot;
				glLightfv(light,GL_SPOT_DIRECTION,&temp[0]);
		  		
		  		glLightf(light,GL_CONSTANT_ATTENUATION,m_kActiveLights[i]->fConst_Atten);
		  		glLightf(light,GL_LINEAR_ATTENUATION,m_kActiveLights[i]->fLinear_Atten);
		  		glLightf(light,GL_QUADRATIC_ATTENUATION,m_kActiveLights[i]->fQuadratic_Atten);
  				
  				glLightf(light,GL_SPOT_EXPONENT,m_kActiveLights[i]->fExp);
  				glLightf(light,GL_SPOT_CUTOFF,m_kActiveLights[i]->fCutoff);  				
  				
  				break;  		
  		}
  		
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

/*
bool Light::Comp(LightSource* l1,LightSource* l2){
	if( (*l1) < (*l2) )
		return true;
	else
		return false;


}
*/

/*
void Light::Sort() 
{
	for(list<LightSource*>::iterator it=m_kSorted.begin();it!=m_kSorted.end();it++) {


	}
}

*/











		
/*	
	
	int i=0;		
	for(vector<LightSource*>::iterator itv=m_kActiveLights.begin();itv!=m_kActiveLights.end();itv++) {
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
		cout<<"LIGHT: "<<light<<endl;

		
		glLightfv(light,GL_DIFFUSE, &(*itv)->kDiffuse[0]);
		glLightfv(light,GL_SPECULAR,&(*itv)->kSpecular[0]);  
		glLightfv(light,GL_AMBIENT, &(*itv)->kAmbient[0]);		
		
		
		Vector4 temp;
  		switch ((*itv)->iType) {
  			case DIRECTIONAL_LIGHT:
				temp=(*(*itv)->kRot)+(*itv)->kConstRot;  			
  				temp[3]=0;			
				glLightfv(light,GL_POSITION,&temp[0]);	    				
  				break;
  			case POINT_LIGHT:
  				temp=(*(*itv)->kPos)+(*itv)->kConstPos;
  				temp[3]=1;  						
				glLightfv(light,GL_POSITION,&temp[0]);	  
		  		
		  		glLightf(light,GL_CONSTANT_ATTENUATION,(*itv)->fConst_Atten);
		  		glLightf(light,GL_LINEAR_ATTENUATION,(*itv)->fLinear_Atten);
		  		glLightf(light,GL_QUADRATIC_ATTENUATION,(*itv)->fQuadratic_Atten);
  				
  				break;
  			case SPOT_LIGHT:
  				temp=(*(*itv)->kPos)+(*itv)->kConstPos;
  				temp[3]=1;  						
				glLightfv(light,GL_POSITION,&temp[0]);	  
				
				temp=(*(*itv)->kRot)+(*itv)->kConstRot;
				glLightfv(light,GL_SPOT_DIRECTION,&temp[0]);
		  		
		  		glLightf(light,GL_CONSTANT_ATTENUATION,(*itv)->fConst_Atten);
		  		glLightf(light,GL_LINEAR_ATTENUATION,(*itv)->fLinear_Atten);
		  		glLightf(light,GL_QUADRATIC_ATTENUATION,(*itv)->fQuadratic_Atten);
  				
  				glLightf(light,GL_SPOT_EXPONENT,(*itv)->fExp);
  				glLightf(light,GL_SPOT_CUTOFF,(*itv)->fCutoff);  				
  				
  				break;  		
  		}
  		
		glEnable(light);
   	i++;	 						

	}
*/
