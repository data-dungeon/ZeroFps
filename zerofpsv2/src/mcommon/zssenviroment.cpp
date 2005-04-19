#include "zssenviroment.h"
#include <iostream>
#include "p_enviroment.h"

//--------Start of envlist
bool EnvList::Load(const string& strFile)
{
	if(m_kIni.Open(strFile.c_str(),false))
	{
		m_strName = strFile;	
	
		//check for list section
		if(m_kIni.SectionExist("list"))
		{
			char nr[4];
			for(int i = 0;i<50;i++)
			{
				IntToChar(nr,i);				
				string strEnviroment = string("enviroment")+nr;
				
				string strNewEnv;
												
				if(m_kIni.KeyExist("list",strEnviroment.c_str()))
				{
					//try to load enviroment
					strNewEnv = m_kIni.GetValue("list",strEnviroment.c_str());
					
					EnvSetting* pkTemp = new EnvSetting;
					pkTemp->LoadEnviroment(strNewEnv.c_str());					
					m_kEnvSettings.push_back(pkTemp);
					
				}
				else
				{
					cout<<"key:"<<strEnviroment<<" not found"<<endl;
					break;
				}
			}
			
			//check if any enviroments was loaded
			if(m_kEnvSettings.size() == 0)
			{
				cout<<"WARNING: enviromentlist "<<strFile<<" contains no enviroments"<<endl;
				m_kIni.Close();
				return false;
			
			}
			
			cout<<"loaded enviroment list, size:"<<m_kEnvSettings.size()<<endl;			
		}
		
		//no list section, check for enviroment section
		if(m_kIni.SectionExist("enviroment"))
		{
			EnvSetting* pkTemp = new EnvSetting;
			pkTemp->LoadEnviroment(strFile.c_str());					
			m_kEnvSettings.push_back(pkTemp);
		
						
			cout<<"loaded enviroment"<<endl;
			return true;				
		}		
	
		m_kIni.Close();
		return true;
	}
	else
		return false;
}



//--------start of envsetting

EnvSetting::EnvSetting()
{
	Clear();

}

EnvSetting::~EnvSetting()
{

}

bool EnvSetting::LoadEnviroment(const char* czName)
{
	Clear();
	
	if(m_kIni.Open(czName,false))
	{
		m_strName = czName;
	
		//music
		if(m_kIni.KeyExist("enviroment","music"))
			m_strMusic = m_kIni.GetValue("enviroment","music");

		//---------------RAIN SPLASHES
		if(m_kIni.KeyExist("enviroment","rain"))
			m_iRain = m_kIni.GetIntValue("enviroment","rain");
		
			
		//---------------PARTICLES
		//particle effect
		if(m_kIni.KeyExist("enviroment","particles"))
			m_strParticles = m_kIni.GetValue("enviroment","particles");
	
		//--------------Skybox
		if(m_kIni.KeyExist("enviroment","space"))
			m_strSpace = m_kIni.GetValue("enviroment","space");	
		
		if(m_kIni.KeyExist("enviroment","sky-day"))
			m_strSky[2] = m_kIni.GetValue("enviroment","sky-day");	
		if(m_kIni.KeyExist("enviroment","sky-evening"))
			m_strSky[3] = m_kIni.GetValue("enviroment","sky-evening");	
		if(m_kIni.KeyExist("enviroment","sky-night"))
			m_strSky[0] = m_kIni.GetValue("enviroment","sky-night");	
		if(m_kIni.KeyExist("enviroment","sky-morning"))
			m_strSky[1] = m_kIni.GetValue("enviroment","sky-morning");	
						
		if(m_kIni.KeyExist("enviroment","cloudhi"))
			m_strCloudHi = m_kIni.GetValue("enviroment","cloudhi");	
		if(m_kIni.KeyExist("enviroment","cloudlow"))
			m_strCloudLow = m_kIni.GetValue("enviroment","cloudlow");	
	
		//---------------FOG
		//fog start/stop
		if(m_kIni.KeyExist("enviroment","fogstart"))
			m_fFogStart = m_kIni.GetFloatValue("enviroment","fogstart");
		if(m_kIni.KeyExist("enviroment","fogstop"))
			m_fFogStop = m_kIni.GetFloatValue("enviroment","fogstop");
		
		//fog color
		if(m_kIni.KeyExist("enviroment","fogcolor-nightR")) m_kFogColor[0].x = m_kIni.GetFloatValue("enviroment","fogcolor-nightR");
		if(m_kIni.KeyExist("enviroment","fogcolor-nightG")) m_kFogColor[0].y = m_kIni.GetFloatValue("enviroment","fogcolor-nightG");
		if(m_kIni.KeyExist("enviroment","fogcolor-nightB")) m_kFogColor[0].z = m_kIni.GetFloatValue("enviroment","fogcolor-nightB");		
		if(m_kIni.KeyExist("enviroment","fogcolor-morningR")) m_kFogColor[1].x = m_kIni.GetFloatValue("enviroment","fogcolor-morningR");
		if(m_kIni.KeyExist("enviroment","fogcolor-morningG")) m_kFogColor[1].y = m_kIni.GetFloatValue("enviroment","fogcolor-morningG");
		if(m_kIni.KeyExist("enviroment","fogcolor-morningB")) m_kFogColor[1].z = m_kIni.GetFloatValue("enviroment","fogcolor-morningB");		
		if(m_kIni.KeyExist("enviroment","fogcolor-dayR")) m_kFogColor[2].x = m_kIni.GetFloatValue("enviroment","fogcolor-dayR");
		if(m_kIni.KeyExist("enviroment","fogcolor-dayG")) m_kFogColor[2].y = m_kIni.GetFloatValue("enviroment","fogcolor-dayG");
		if(m_kIni.KeyExist("enviroment","fogcolor-dayB")) m_kFogColor[2].z = m_kIni.GetFloatValue("enviroment","fogcolor-dayB");		
		if(m_kIni.KeyExist("enviroment","fogcolor-eveningR")) m_kFogColor[3].x = m_kIni.GetFloatValue("enviroment","fogcolor-eveningR");
		if(m_kIni.KeyExist("enviroment","fogcolor-eveningG")) m_kFogColor[3].y = m_kIni.GetFloatValue("enviroment","fogcolor-eveningG");
		if(m_kIni.KeyExist("enviroment","fogcolor-eveningB")) m_kFogColor[3].z = m_kIni.GetFloatValue("enviroment","fogcolor-eveningB");		

				
		m_kFogColor[0].w = 1;
		m_kFogColor[1].w = 1;
		m_kFogColor[2].w = 1;
		m_kFogColor[3].w = 1;
	
		//---------------SUN
		//diffuse sunlight
		if(m_kIni.KeyExist("enviroment","sundiffuseR"))
			m_kSunDiffuseColor.x = m_kIni.GetFloatValue("enviroment","sundiffuseR");
		if(m_kIni.KeyExist("enviroment","sundiffuseG"))
			m_kSunDiffuseColor.y = m_kIni.GetFloatValue("enviroment","sundiffuseG");
		if(m_kIni.KeyExist("enviroment","sundiffuseB"))
			m_kSunDiffuseColor.z = m_kIni.GetFloatValue("enviroment","sundiffuseB");		
		
		m_kSunDiffuseColor.w = 1;
		
		//ambient sunlight
		if(m_kIni.KeyExist("enviroment","sunambientR"))
			m_kSunAmbientColor.x = m_kIni.GetFloatValue("enviroment","sunambientR");
		if(m_kIni.KeyExist("enviroment","sunambientG"))
			m_kSunAmbientColor.y = m_kIni.GetFloatValue("enviroment","sunambientG");
		if(m_kIni.KeyExist("enviroment","sunambientB"))
			m_kSunAmbientColor.z = m_kIni.GetFloatValue("enviroment","sunambientB");
		
		m_kSunAmbientColor.w = 1;
		
		//sun pos/directon
		if(m_kIni.KeyExist("enviroment","sunposX"))
			m_kSunPos.x = m_kIni.GetFloatValue("enviroment","sunposX");
		if(m_kIni.KeyExist("enviroment","sunposY"))
			m_kSunPos.y = m_kIni.GetFloatValue("enviroment","sunposY");
		if(m_kIni.KeyExist("enviroment","sunposZ"))
			m_kSunPos.z = m_kIni.GetFloatValue("enviroment","sunposZ");
		
		if(m_kIni.KeyExist("enviroment","sunflare"))
			m_bSunFlare = m_kIni.GetBoolValue("enviroment","sunflare");
		
						
		m_kIni.Close();
		return true;
	}
	else
	{
		cout<<"Error loading enviroment:"<<czName<<endl;
		return false;		
	}
	
//	return true;
}

void EnvSetting::Clear()
{
	m_strName 		= "";
	
	m_strMusic		= "";
	m_strParticles = "";
	
	m_strSpace		= "";
	m_strSky[0]		= "";		
	m_strSky[1]		= "";		
	m_strSky[2]		= "";		
	m_strSky[3]		= "";		
	
	m_strCloudLow	= "";				
	m_strCloudHi	= "";			
	
		
	m_kSunDiffuseColor.Set(1.6,1.6,1.6,1.0);					
	//m_kSunDiffuseColor.Set(-1,-1,-1,-1);					
	m_kSunAmbientColor.Set(0.8,0.8,0.8,1.0);		
	//m_kSunAmbientColor.Set(-1,-1,-1,-1);		
	m_kSunPos.Set(0.5,0.5,0.0);		
	m_iRain = 0;
	
	m_fFogStart = 10;
	m_fFogStop = 30;	
	m_kFogColor[0].Set(1,1,1,1);
	m_kFogColor[1].Set(1,1,1,1);
	m_kFogColor[2].Set(1,1,1,1);
	m_kFogColor[3].Set(1,1,1,1);
	
	m_bSunFlare = true;
}


//---------end of envsetting

ZSSEnviroment::ZSSEnviroment() : ZFSubSystem("ZSSEnviroment") 
{
	m_fEnviromentChange = 300;
	m_fEnviromentTimeout = 300;
	
	Register_Cmd("changeenviroment",			FID_CHANGEENVIROMENT);
}

bool ZSSEnviroment::StartUp()
{
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	
	return true;
}

void ZSSEnviroment::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) 
	{
		case FID_CHANGEENVIROMENT:
		{
			//reset last updated timer (forces enviroment changes at next update)
			for(int i = 0;i<m_kEnvLists.size();i++)
			{			
				m_kEnvLists[i]->m_fLastUpdated = -m_fEnviromentChange;
			}
		
			break;
		}
	}	
}

void ZSSEnviroment::Update()
{
	//now update enviroment lists
	UpdateEnvLists();

	//update enviroment for each p_enviroment
	for(int i = 0;i<m_kPEnviroments.size();i++)
	{
		UpdatePEnv(m_kPEnviroments[i]);
	}	
}

void ZSSEnviroment::UpdateEnvLists()
{
	float fCurrentTime = m_pkZeroFps->GetEngineTime();

	//update timeouts
   for ( vector<EnvList*>::iterator kIte = m_kEnvLists.begin(); kIte != m_kEnvLists.end(); kIte++ )
   {
		if(fCurrentTime - (*kIte)->m_fLastUsed > m_fEnviromentTimeout)
		{
			cout<<"ZSSEnviroment Timeout: "<<(*kIte)->m_strName<<endl;
			
			delete (*kIte);
			m_kEnvLists.erase ( kIte );
			
			if(m_kEnvLists.empty())
				break;
			
			kIte = m_kEnvLists.begin();
		}
	}	

	//update enviroment systems	
	for(int i = 0;i<m_kEnvLists.size();i++)
	{
		m_kEnvLists[i]->m_bChanged = false;	
		if(fCurrentTime - m_kEnvLists[i]->m_fLastUpdated > m_fEnviromentChange)
		{
			m_kEnvLists[i]->m_fLastUpdated = fCurrentTime;
			
			UpdateEnvList(m_kEnvLists[i]);
		}
	}	
}

void ZSSEnviroment::UpdateEnvList(EnvList* pkEnvList)
{
	//randomize a new enviroment
	
	int iNew =  Randomi(pkEnvList->m_kEnvSettings.size());				
	
	if(iNew == pkEnvList->m_iCurrent)
		return;
			
	pkEnvList->m_iCurrent = iNew;
	pkEnvList->m_bChanged = true;

	//cout<<"ZSSEnviroment Updated: "<<m_kEnvLists[i]->m_strName<<" ,new setting:"<<m_kEnvLists[i]->m_kEnvSettings[m_kEnvLists[i]->m_iCurrent]->m_strName<<endl;
}

void ZSSEnviroment::UpdatePEnv(P_Enviroment* pkPEnv)
{
	//find enviroment list
	if(EnvList* pkEnvList = GetEnvList(pkPEnv->m_strCurrentZoneEnviroment))
	{
		//update last used timer
		pkEnvList->m_fLastUsed = m_pkZeroFps->GetEngineTime();
	
		//update property settings if enviroment has changed or p_env has changed to another enviroment
		if(pkEnvList->m_bChanged || (pkPEnv->m_strLastSetEnviroment != pkPEnv->m_strCurrentZoneEnviroment)  )
		{
			pkPEnv->m_strLastSetEnviroment = pkPEnv->m_strCurrentZoneEnviroment;
			
			//update p_env enviroment pointer
			pkPEnv->UpdateEnvSetting(pkEnvList->m_kEnvSettings[pkEnvList->m_iCurrent]);
		}
	}
}

EnvList* ZSSEnviroment::GetEnvList(const string& strEnviroment)
{
	if(strEnviroment == "")
		return NULL;
	
	//try to find enviroment list
	for(int i =0;i<m_kEnvLists.size();i++)
	{
		if(m_kEnvLists[i]->m_strName == strEnviroment)
		{
			return m_kEnvLists[i];	
		}
	}
	
	//if list was not found, create it	
	cout<<"ZSSEnviroment Created: "<<strEnviroment<<endl;
	
	EnvList* pkNewEnvList = new EnvList;
	

	if(!pkNewEnvList->Load(strEnviroment))
	{
		cout<<"enviroment:"<<strEnviroment<< " could not be loaded"<<endl;
	
		pkNewEnvList->m_strName  = strEnviroment;		
		EnvSetting* temp = new EnvSetting;
		pkNewEnvList->m_kEnvSettings.push_back(temp);	
	}

		
	//update newloaded enviroment
	UpdateEnvList(pkNewEnvList);
	
	//add list 
	m_kEnvLists.push_back(pkNewEnvList);
	return pkNewEnvList;
}

void ZSSEnviroment::Register(P_Enviroment* pkPEnviroment)
{
	m_kPEnviroments.push_back(pkPEnviroment);
}

void ZSSEnviroment::UnRegister(P_Enviroment* pkPEnviroment)
{
   for ( vector<P_Enviroment*>::iterator kIte = m_kPEnviroments.begin();
         kIte != m_kPEnviroments.end(); kIte++ )
   {
		if((*kIte) == pkPEnviroment)
      {
         m_kPEnviroments.erase ( kIte );
         return;
      }
   }
	return;
}


