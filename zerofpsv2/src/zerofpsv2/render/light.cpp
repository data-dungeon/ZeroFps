#include "light.h"
#include <algorithm>
#include "../basic/globals.h"
 
#include "render.h"
 
LightSource::LightSource() 
{
	//position and rotation
	kPos					=	Vector3(0,0,0);
	kRot					=	Vector3(0,0,0);
	
	//light color
	kDiffuse				=	Vector4(1,1,1,1);
	kAmbient				=	Vector4(0,0,0,1);
	kSpecular			=	Vector4(0,0,0,1);
	
	//spotlight
	fCutoff				=	20;
	fExp					=	20;
	
	//distance attenuation 
	fConst_Atten		=	0;
	fLinear_Atten		=	0;
	fQuadratic_Atten	=	0.01;

	iType					=	POINT_LIGHT;
	iPriority			=	0;
}


Light::Light() 
: ZFSubSystem("Light") 
{
	m_iNrOfLights=8;							//this shuld never be greater than 8

	RegisterVariable("r_maxlights",		&m_iNrOfLights,CSYS_INT);

}

void Light::SetLighting(bool bOn)
{
	if(bOn)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
}

bool Light::StartUp()	
{ 
	SetStartUpValues();
	return true;	
}

bool Light::ShutDown()
{ 
	return true;
}

bool Light::IsValid()
{ 
	return true;
}

void Light::SetStartUpValues()
{
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0);

	float afAmbient[4] = {0,0,0,0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, afAmbient);

	SetLighting(true);
}

void Light::SetCamera(Vector3 kCamPos) {
	m_kCamPos=kCamPos;
}

void Light::Add(LightSource *kNewLight)
{
	cout << "Add Light" << endl;
	m_kLights.push_back(kNewLight);
}

void Light::Remove(LightSource *kLight) 
{
	cout << "Remove Light" << endl;
	m_kLights.remove(kLight);
}

bool comp(LightSource* x, LightSource* y)
{
	return x->fIntensity > y->fIntensity; 
}

 
void Light::Update(Vector3 kRefPos)
{

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
			 
			Vector3 kPos = (*it)->kPos;		
			float fDistance = kRefPos.DistanceTo(kPos);		
			//float fIntensity = min(1 , 1 / ( (*it)->fConst_Atten + ((*it)->fLinear_Atten*fDistance) + ((*it)->fQuadratic_Atten*(fDistance*fDistance)) ));
			float fIntensity = 1 / ( (*it)->fConst_Atten + ((*it)->fLinear_Atten*fDistance) + ((*it)->fQuadratic_Atten*(fDistance*fDistance)) );
			
			(*it)->fIntensity=fIntensity; 
			//(*it)->fIntensity=-fDistance; 
			m_kSorted.push_back(*it);					
		}  
	} 
	
		
	sort(m_kSorted.begin(),m_kSorted.end(),More_Light);

	int max = m_kSorted.size();
	if(max>m_iNrOfLights)
		max=m_iNrOfLights;

	for(int i=0;i<max;i++)
	{
		EnableLight(m_kSorted[i],i);							
		//cout<<"inte: "<<m_kSorted[i]->fIntensity<<endl;
		
		/* //draw some debuging graph
		static Render* pkRender = static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	
		pkRender->Sphere(m_kSorted[i]->kPos,0.1,10,Vector3(0,0,1),true);
		pkRender->Line(m_kSorted[i]->kPos,kRefPos);
		*/
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

void Light::EnableLight(LightSource* pkLight,int iGlLight)
{
		GLenum light;				
		//wich light to change
  		switch(iGlLight) {
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
			default:
				return;
  		}
		glEnable(light);		
			
		glLightfv(light,GL_DIFFUSE, (float*)&pkLight->kDiffuse);		// &m_kActiveLights[i]->kDiffuse[0]
		glLightfv(light,GL_SPECULAR,(float*)&pkLight->kSpecular);	//&m_kActiveLights[i]->kSpecular[0]
		glLightfv(light,GL_AMBIENT, (float*)&pkLight->kAmbient);		//&m_kActiveLights[i]->kAmbient[0]
		
		Vector4 temp;
		float spotdir[]={0,0,-1};  		
  		switch (pkLight->iType) {
  			case DIRECTIONAL_LIGHT:
				temp=pkLight->kRot;  			
  				temp.w = 0;			
				glLightfv(light,GL_POSITION,(float*)&temp);								//		&temp[0]		
  				
  				//seset spot
				//glLightfv(light,GL_SPOT_DIRECTION,spotdir);		  				  		  				
  				//glLightf(light,GL_SPOT_EXPONENT,0);
  				//glLightf(light,GL_SPOT_CUTOFF,180);				  				
  				
  				break;
  			case POINT_LIGHT:
  				temp=pkLight->kPos;
  				temp.w=1;  						
				glLightfv(light,GL_POSITION,(float*)&temp);	  //&temp[0]
		  		
  				
		  		glLightf(light,GL_CONSTANT_ATTENUATION,pkLight->fConst_Atten);
		  		glLightf(light,GL_LINEAR_ATTENUATION,pkLight->fLinear_Atten);
		  		glLightf(light,GL_QUADRATIC_ATTENUATION,pkLight->fQuadratic_Atten);
  				
  				//seset spot		  		
				glLightfv(light,GL_SPOT_DIRECTION,spotdir);		  				  		  				
  				glLightf(light,GL_SPOT_EXPONENT,0);
  				glLightf(light,GL_SPOT_CUTOFF,180);				
				  				
  				break;
  			case SPOT_LIGHT:
  				temp=pkLight->kPos;
  				temp.w=1;  						
				glLightfv(light,GL_POSITION,(float*)&temp);	  // temp[0]
				
				temp=pkLight->kRot;
				glLightfv(light,GL_SPOT_DIRECTION,(float*)&temp);		//&temp[0]
		  		
		  		glLightf(light,GL_CONSTANT_ATTENUATION,pkLight->fConst_Atten);
		  		glLightf(light,GL_LINEAR_ATTENUATION,pkLight->fLinear_Atten);
		  		glLightf(light,GL_QUADRATIC_ATTENUATION,pkLight->fQuadratic_Atten);
  				
  				glLightf(light,GL_SPOT_EXPONENT,pkLight->fExp);
  				glLightf(light,GL_SPOT_CUTOFF,pkLight->fCutoff);  				
  				
  				break;  		
  		}  		

}



