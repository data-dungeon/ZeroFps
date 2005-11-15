#include "light.h"
#include <algorithm>
#include "../basic/globals.h"
#include "../engine/zerofps.h"
 
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
	
	iListPos				=	-1;
	fIntensity			=	0;
	
}


ZSSLight::ZSSLight()
: ZFSubSystem("ZSSLight") 
{
	//m_iNrOfLights=		8;							//this shuld never be greater than 8
	m_bAmbientOnly =	false;
	m_bEnabled = 		true;
	m_iVersion = 		0;
	m_iCurrentActiveLights = 0;
	
	//RegisterVariable("r_maxlights",		&m_iNrOfLights,CSYS_INT);
	m_kiNrOfLights.Register(this, "r_maxlights", "8", "max number of lights used at the same time");
	m_kfMinIntensity.Register(this, "r_minlightintensity", "0.3", "minimum intensity before disabling light");
	
	m_kSun.kRot = Vector3(2,4,1);
	m_kSun.kSpecular=Vector4(0.8,0.8,0.8,0);
	m_kSun.kDiffuse=Vector4(0.8,0.8,0.8,0);
	m_kSun.kAmbient=Vector4(0.5,0.5,0.5,0);
	m_kSun.iType=DIRECTIONAL_LIGHT;			
	
	Add(&m_kSun);
}


bool ZSSLight::StartUp()	
{ 
	m_pkZeroFps = static_cast<ZSSZeroFps*>(GetSystem().GetObjectPtr("ZSSZeroFps"));		

	SetStartUpValues();
	return true;	
}

bool ZSSLight::ShutDown()
{ 
	Remove(&m_kSun);

	return true;
}

bool ZSSLight::IsValid()
{ 
	return true;
}

void ZSSLight::SetLighting(bool bOn)
{
	if(bOn)
	{
// 		glEnable(GL_LIGHTING);		
		m_bEnabled = true;
	}
	else
	{
// 		glDisable(GL_LIGHTING);
		m_bEnabled = false;
	}
}



void ZSSLight::SetStartUpValues()
{
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0);

	float afAmbient[4] = {0,0,0,0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, afAmbient);

	//enable lighting
	SetLighting(true);

}

void ZSSLight::SetCamera(Vector3 kCamPos) {
	m_kCamPos=kCamPos;
}

void ZSSLight::Add(LightSource *kNewLight)
{
	//cout << "Add Light" << endl;
	m_kLights.push_back(kNewLight);
	
	m_iVersion++;
}

void ZSSLight::Remove(LightSource *kLight) 
{

	//loop trough all lightsources and find wich to view
	for(vector<LightSource*>::iterator it =m_kLights.begin();it!=m_kLights.end();it++) 
	{		
		if(*it == kLight)
		{
			m_kLights.erase(it);
			m_iVersion++;
			return;
		}
		//m_kLights.remove(kLight);		
	}
	
	m_iVersion++;	
}

bool comp(LightSource* x, LightSource* y)
{
	return x->fIntensity > y->fIntensity; 
}


void ZSSLight::Update(LightProfile* pkLightProfile,const Vector3& kRefPos,float fRefRadius )
{
	//disable all lights
	TurnOffAll();

	//fast return if no lighting is enabled
	if(!m_bEnabled)
		return;


	bool bUpdate 				= false;
	float fCurrentTime		= m_pkZeroFps->GetEngineTime();
	float fMinIntensity		= m_kfMinIntensity.GetFloat();

	if(pkLightProfile->m_iLastVersion != m_iVersion)
		bUpdate = true;

	if(fCurrentTime - pkLightProfile->m_fLastTime > 1)
		bUpdate = true;
		
	//want to update current active lights
	if(bUpdate)
	{	
		pkLightProfile->m_iLastVersion = m_iVersion;
		pkLightProfile->m_fLastTime = fCurrentTime;
	
		vector<LightSource*> kSorted;
	
		//loop trough all lightsources and find wich to enable
		int iLights = m_kLights.size();
		for(int i = 0;i< iLights;i++) 
		{
			LightSource* pkL = m_kLights[i];
		
			//always add light with priority >10
			if(pkL->iPriority>=10)
			{
				pkL->fIntensity=999999;
				pkL->iListPos = i;
				kSorted.push_back(pkL);
				continue;
			}
	
			//if its a directional light add it if there is space
			if(pkL->iType==DIRECTIONAL_LIGHT)
			{
				pkL->fIntensity=999999;
				pkL->iListPos = i;
				kSorted.push_back(pkL);
			}
			else 
			{
				//else add the light if it is bright enough
								
				//		opengl LightIntesity equation	min(1, 1 / ((*it)-> + l*d + q*d*d))
				
				
				//distance to center of object
				float fDistance = float(kRefPos.DistanceTo(pkL->kPos));
				//distance to edge of object
				float fClipDistance = float(Max(0.01,kRefPos.DistanceTo(pkL->kPos) - fRefRadius));
					
				
// 				float fIntensity = Min(1.0f , 1 / ( pkL->fConst_Atten + (pkL->fLinear_Atten*fDistance) + (pkL->fQuadratic_Atten*(fDistance*fDistance)) ));
				//intensity calculated to the center of the object				
 				float fIntensity = 1.0 / ( pkL->fConst_Atten + (pkL->fLinear_Atten*fDistance) + (pkL->fQuadratic_Atten*(fDistance*fDistance)) );
 				//intensity calculated to the edge of the object
 				float fClipIntensity = 1.0 / ( pkL->fConst_Atten + (pkL->fLinear_Atten*fDistance) + (pkL->fQuadratic_Atten*(fClipDistance*fClipDistance)) );
				
				//color values
				float r = pkL->kDiffuse.x + pkL->kAmbient.x + pkL->kSpecular.x;
				float g = pkL->kDiffuse.y + pkL->kAmbient.y + pkL->kSpecular.y;
				float b = pkL->kDiffuse.z + pkL->kAmbient.z + pkL->kSpecular.z;				
								
				//use the highest color value as referens								
				float fColor = Max(Max(r,g),b);
				
				//apply color to intensity
 				fClipIntensity *= fColor;
 				fIntensity *= fColor;

				//save intensity for future sorting
 				pkL->fIntensity=fIntensity; 			
 				//save position in light vector for fast access in the future	
				pkL->iListPos = i;								
 				 				 				
 				//check if intensity to the edge of the object is lower then the min treshold value
 				if(fClipIntensity < fMinIntensity)
 					continue;
			
				kSorted.push_back(pkL);
			}
		}
		
		//sort lights
		sort(kSorted.begin(),kSorted.end(),More_Light);		

		//clear current light list
		for(int i = 0;i<8;i++)
			pkLightProfile->m_aiLights[i] = -1;
		
		//how many lights do we use?
		int max = kSorted.size();
		if(max>m_kiNrOfLights.GetInt())
			max=m_kiNrOfLights.GetInt();			
			
		//add new lights in light list
		for(int i=0;i<max;i++)
			pkLightProfile->m_aiLights[i] = kSorted[i]->iListPos;
		
	}
	
	m_iCurrentActiveLights = 0;
	//activate lights in light list
	for(int i = 0;i<8;i++)
	{		
		if(pkLightProfile->m_aiLights[i] != -1)
		{
			m_iCurrentActiveLights++;
			//cout<<"enabling lightsource: "<<pkLightProfile->m_aiLights[i]<<" as gl light "<<i<<endl;
			EnableLight(m_kLights[pkLightProfile->m_aiLights[i]],i);
		}
	}

}
 

void ZSSLight::TurnOffAll() 
{
	glDisable(GL_LIGHT0);	
	glDisable(GL_LIGHT1);	
	glDisable(GL_LIGHT2);	 
	glDisable(GL_LIGHT3);	
	glDisable(GL_LIGHT4);	
	glDisable(GL_LIGHT5);	
	glDisable(GL_LIGHT6);	
	glDisable(GL_LIGHT7);		
}

void ZSSLight::DisableLight(int iGlLight)
{
	static GLenum light;		
			
	//wich light to change
	switch(iGlLight) 
	{
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
	
	//enable light
	glDisable(light);	
}

void ZSSLight::EnableLight(LightSource* pkLight,int iGlLight)
{
	static GLenum light;		
			
	//wich light to change
	switch(iGlLight) 
	{
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
	
	//enable light
	glEnable(light);		
	
	//setup light
	if(m_bAmbientOnly)
	{
		static float none[] = {0,0,0,0};	
		glLightfv(light,GL_DIFFUSE, (float*)none);		// &m_kActiveLights[i]->kDiffuse[0]
		glLightfv(light,GL_SPECULAR,(float*)none);	//&m_kActiveLights[i]->kSpecular[0]
	}
	else
	{		
		glLightfv(light,GL_DIFFUSE, (float*)&pkLight->kDiffuse);		// &m_kActiveLights[i]->kDiffuse[0]
		glLightfv(light,GL_SPECULAR,(float*)&pkLight->kSpecular);	//&m_kActiveLights[i]->kSpecular[0]
	}
	
	glLightfv(light,GL_AMBIENT, (float*)&pkLight->kAmbient);		//&m_kActiveLights[i]->kAmbient[0]
	
	
	static Vector4 temp;
	static float spotdir[]={0,0,-1};  		
	
	switch (pkLight->iType) 
	{
		case DIRECTIONAL_LIGHT:
			temp=pkLight->kRot;  			
			temp.w = 0;			
			glLightfv(light,GL_POSITION,(float*)&temp);								//		&temp[0]		
			
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



void ZSSLight::GetClosestLights(vector<LightSource*>* pkLights,int iNrOfLights,Vector3 kPos,bool bNoDirectional)
{
	vector<LightSource*> kSorted;

	for(int i = 0;i < m_kLights.size();i++)
	{
		LightSource* pkL = m_kLights[i];
	
		//skip spotlights
		if(pkL->iType==SPOT_LIGHT)
			continue;
	
		//always add point lights
		if(pkL->iType==DIRECTIONAL_LIGHT)
		{
			if(!bNoDirectional)
			{
				pkL->fIntensity=999999;
				kSorted.push_back(pkL);
			}
		}
		else
		{
			float fDistance = float(kPos.DistanceTo(pkL->kPos));

			pkL->fIntensity = float( 1.0 / ( pkL->fConst_Atten + (pkL->fLinear_Atten*fDistance) + (pkL->fQuadratic_Atten*(fDistance*fDistance)) ) );

			kSorted.push_back(pkL);
		}
	}

	sort(kSorted.begin(),kSorted.end(),More_Light);

	int max = kSorted.size();
	if(max>iNrOfLights)
		max=iNrOfLights;

	for(int i=0;i<max;i++)
		pkLights->push_back(kSorted[i]);
}

LightSource* ZSSLight::GetFirstDirectionalLight()
{
	for(int i = 0;i < m_kLights.size();i++)
	{
		if(m_kLights[i]->iType==DIRECTIONAL_LIGHT)
			return m_kLights[i];	
	}
	
	return NULL;
}




