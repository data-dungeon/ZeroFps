#include "levelmanager.h"
#include "../engine_systems/physicsengine/heightmapcs.h"
#include "../basic/zfobjectmanger.h"
#include "../engine_systems/common/heightmap.h"
#include "../engine_systems/common/zoneobject.h"
#include "../engine_systems/propertys/hmrp2.h"
#include "../engine_systems/propertys/waterrenderproperty.h"
#include "../engine_systems/propertys/skyboxrenderproperty.h"
#include "../engine_systems/propertys/physicproperty.h"

//#include "heightmapobject.h"

LevelManager::LevelManager(): ZFSubSystem("LevelManager")
{
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkBasicFS=static_cast<ZFBasicFS*>(g_ZFObjSys.GetObjectPtr("ZFBasicFS"));
	m_pkConsole=static_cast<Console*>(g_ZFObjSys.GetObjectPtr("Console"));
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));
//	m_pkCmd=static_cast<CmdSystem*>(g_ZFObjSys.GetObjectPtr("CmdSystem"));
	m_pkIni=static_cast<ZFIni*>(g_ZFObjSys.GetObjectPtr("ZFIni"));
	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));
	
	m_pkMap=new HeightMap();
	
	m_bVisibleZones		=	true;
	m_iShowDecorations	=	1;
	m_iDecorationStep		=	1;
	m_fZoneRadius			=	150;	
	m_iZpr					=	2;
	m_fZoneDistance		=	64;
	m_kMapBaseDir			=	"../data/maps";
	m_bDrawZones			= false;
	
	g_ZFObjSys.RegisterVariable("l_zoneradius", &m_fZoneRadius,CSYS_FLOAT);
	g_ZFObjSys.RegisterVariable("l_Showdecorations", &m_iShowDecorations,CSYS_INT);
	g_ZFObjSys.RegisterVariable("l_decorationstep", &m_iDecorationStep,CSYS_INT);
	g_ZFObjSys.RegisterVariable("l_showzones", &m_bDrawZones, CSYS_BOOL);

//	m_pkCmd->Add(&m_fZoneRadius,"l_zoneradius",type_float);		
//	m_pkCmd->Add(&m_iShowDecorations,"l_Showdecorations",type_int);		
//	m_pkCmd->Add(&m_iDecorationStep,"l_decorationstep",type_int);			
	

	//default light
	m_bSun=new LightSource;	
		m_bSun->kRot=new Vector3(1,2,1);
		m_bSun->kDiffuse=Vector4(1,1,1,1);
		m_bSun->kAmbient=Vector4(0.05,0.05,0.05,1);
		m_bSun->iType=DIRECTIONAL_LIGHT;			
		m_bSun->iPriority=10;
		m_bSun->fConst_Atten=1;
		m_bSun->fLinear_Atten=0;
		m_bSun->fQuadratic_Atten=0;

	m_bMoon=new LightSource;	
		m_bMoon->kRot=new Vector3(-1,-2,-1);	
		m_bMoon->kDiffuse=Vector4(1,1,1,1);		
		m_bMoon->kAmbient=Vector4(0,0,0,1);
		m_bMoon->iType=DIRECTIONAL_LIGHT;			
		m_bMoon->iPriority=10;
		m_bMoon->fConst_Atten=1;
		m_bMoon->fLinear_Atten=0;
		m_bMoon->fQuadratic_Atten=0;	
	
	SetSunColor(Vector3(1,1,1));
	SetMoonColor(Vector3(0.1,0.1,0.3));

	
	m_pkLight->Add(m_bSun);
	m_pkLight->Add(m_bMoon);
}

void LevelManager::CreateEmptyLevel(int iSize)
{
//	CreateNew(iSize);
//	CreateZones();
}

void LevelManager::Clear() 
{
	CreateNew(128);
}

void LevelManager::ClearObjects()
{
	ClearTrackers();
	m_pkObjectMan->Clear();
	m_kZones.clear();

//	m_pkHeightMapObject=new HeightMapObject(m_pkMap);		
	m_pkHeightMapObject=CreateHeightMapObject(m_pkMap);
	
	m_pkHeightMapObject->SetParent(m_pkObjectMan->GetWorldObject());
	m_pkHeightMapObject->GetPos().Set(0,0,0);
	m_pkMap->SetPosition(Vector3(0,0,0));
}

void LevelManager::CreateNew(int iSize) 
{	
	ClearObjects();
	
	m_pkMap->Create(iSize);
	m_pkMap->GenerateNormals(); 
//	m_pkMap->GenerateTextures();
	m_pkMap->SetPosition(Vector3(0,0,0));		
	
}

void LevelManager::CreateZones()
{
	m_kZones.clear();
	
	
	for(float x=(m_pkMap->GetPos().x-m_pkMap->GetSize()/2);x< (float)(m_pkMap->GetPos().x+m_pkMap->GetSize()/2);x+=m_fZoneDistance){
		for(float z=(m_pkMap->GetPos().z-m_pkMap->GetSize()/2);z< (float)(m_pkMap->GetPos().z+m_pkMap->GetSize()/2);z+=m_fZoneDistance){	
//			if(m_pkMap->Height(x,z)>-1){			
				ZoneObject *object = new ZoneObject();
				object->GetPos()=Vector3(x,m_pkMap->Height(x,z),z);
				object->SetParent(m_pkObjectMan->GetWorldObject());				
				if(!m_bVisibleZones)
					object->DeleteProperty("MadProperty");
				
				object->GetUpdateStatus()=UPDATE_DYNAMIC;
				
				m_kZones.push_back(object);
//			}
		}
	}	

	

/*
	for(float x=(m_pkMap->GetPos().x-m_pkMap->GetSize()/2);x< (float)(m_pkMap->GetPos().x+m_pkMap->GetSize()/2);x+=m_fZoneRadius/m_iZpr){
		for(float z=(m_pkMap->GetPos().z-m_pkMap->GetSize()/2);z< (float)(m_pkMap->GetPos().z+m_pkMap->GetSize()/2);z+=m_fZoneRadius/m_iZpr){	
			if(m_pkMap->Height(x,z)>-1){
				ZoneObject *object = new ZoneObject();
				object->GetPos()=Vector3(x,m_pkMap->Height(x,z),z);
				object->SetRadius(m_fZoneRadius);
				object->SetParent(m_pkObjectMan->GetWorldObject());				
				if(!m_bVisibleZones)
					object->DeleteProperty("MadProperty");
					
				object->GetUpdateStatus()=UPDATE_DYNAMIC;
					
//				m_kZones.push_back(object);
			}
		}
	}	
*/	
}

bool LevelManager::LoadLevelHmapOnly(const char* acFile)
{
	for(int i=0;acFile[i]!='\0';i++)
	{
		if(acFile[i]=='\\' || acFile[i]=='/')
		{
			m_pkConsole->Print("Bad filename");
			return false;
		}
	}
		
	string kHmfile;
	string kZolfile;
	string kpreinifile;
	string ksuinifile;	
	string kBase;
	
	kBase=m_kMapBaseDir;
	kBase+="/";
	kBase+=acFile;
	kBase+="/";	

	kHmfile=kBase+"heightmap";

	if(m_pkHeightMapObject) {
		m_pkHeightMapObject	=	CreateHeightMapObject(m_pkMap);
		m_pkHeightMapObject->SetParent(m_pkObjectMan->GetWorldObject());
		m_pkHeightMapObject->GetPos().Set(0,0,0);
		m_pkMap->SetPosition(Vector3(0,0,0));
		}
	
	//load heightmap
	if(!m_pkMap->Load(kHmfile.c_str())){
		m_pkConsole->Printf("Error loading heightmap");
		return false;
	};	
	
	//create zoneobjects
//	CreateZones();		
	m_kCurrentMapDir = kBase;
	return true;

}

bool LevelManager::LoadLevel(const char* acFile)
{
	for(int i=0;acFile[i]!='\0';i++)
	{
		if(acFile[i]=='\\' || acFile[i]=='/')
		{
			m_pkConsole->Print("Bad filename");
			return false;
		}
	}
		
	string kHmfile;
	string kZolfile;
	string kpreinifile;
	string ksuinifile;	
	string kBase;
	
	kBase=m_kMapBaseDir;
	kBase+="/";
	kBase+=acFile;
	kBase+="/";	

	kHmfile=kBase+"heightmap";
	kZolfile=kBase+"objects.zol";
	kpreinifile=kBase+"preconfig.ini";
	ksuinifile=kBase+"suconfig.ini";

	//clear world
	Clear();
	
	//execute preconfig.ini
	if(!m_pkIni->ExecuteCommands(kpreinifile.c_str()))
		m_pkConsole->Printf("No preconfig.ini found");
	
	//load heightmap
	if(!m_pkMap->Load(kHmfile.c_str())){
		m_pkConsole->Printf("Error loading heightmap");
		return false;
	};	
/*	
	//load textures
	m_pkMap->ClearSet();
	m_pkMap->AddSet("../data/textures/nodetail1.bmp","../data/textures/detail1.bmp","FEL");
	m_pkMap->AddSet("../data/textures/nodetail2.bmp","../data/textures/detail2.bmp",(kBase+"mask1.tga").c_str());
	m_pkMap->AddSet("../data/textures/nodetail3.bmp","../data/textures/detail3.bmp",(kBase+"mask2.tga").c_str());		
	m_pkMap->AddSet("../data/textures/nodetail4.bmp","../data/textures/detail4.bmp",(kBase+"mask3.tga").c_str());	
*/	
	
	//create zoneobjects
	CreateZones();		
	
	//load objects
	if(!m_pkObjectMan->LoadAllObjects(kZolfile .c_str())){
		m_pkConsole->Printf("Error loading objects");
		return false;
	}

	//extract world info from the worldinfo object and setup
	if(!ExtractWorldInfoObject())
		return false;
	
	//setup world from the worldinfoproperty
	SetupWorld();	

	//execute suconfig.ini
	if(!m_pkIni->ExecuteCommands(ksuinifile.c_str()))
		m_pkConsole->Printf("No suconfig.ini found");

	m_kCurrentMapDir = kBase;

	HMRP2* hp= static_cast<HMRP2*>(m_pkHeightMapObject->GetProperty("HMRP2"));
	hp->m_strMapName = acFile;

	return true;
}

bool LevelManager::SaveLevel(const char* acFile)
{

	for(int i=0;acFile[i]!='\0';i++)
	{
		if(acFile[i]=='\\' || acFile[i]=='/')
		{
			m_pkConsole->Print("Bad filename");
			return false;
		}
	}
	
	string dir;
	dir=m_kMapBaseDir+"/";
	dir+=acFile;
	
	if(m_pkBasicFS->DirExist(dir.c_str()))
		m_pkConsole->Printf("Overwriting old %s",acFile);
	else
		m_pkBasicFS->CreateDir(dir.c_str());
		
	string kHmfile;
	string kZolfile;

	kHmfile=m_kMapBaseDir;
	kHmfile+="/";
	kHmfile+=acFile;
	kHmfile+="/";	
	kHmfile+="heightmap";

	kZolfile=m_kMapBaseDir;
	kZolfile+="/";
	kZolfile+=acFile;
	kZolfile+="/";	
	kZolfile+="objects.zol";

	if(!m_pkMap->Save(kHmfile.c_str())){
		m_pkConsole->Printf("Error saving heightmap");
		return false;
	};
	
	//add the world info object before saving
	AddWorldInfoObject();
	
	if(!m_pkObjectMan->SaveAllObjects(kZolfile .c_str())){
		m_pkConsole->Printf("Error saving objects");
		return false;
	}	
	
	//remove it after saving	
	RemoveWorldInfoObject();


	return true;	
}

Object* LevelManager::AddWorldInfoObject()
{
	Object *temp = m_pkObjectMan->CreateObject();

	temp->GetName()="WorldInfoObject";
	temp->AddProperty(&m_kWIP);
	temp->SetParent(m_pkObjectMan->GetWorldObject());
	
	return temp;
}

bool LevelManager::RemoveWorldInfoObject()
{
	vector<Object*> kObjects;	
	Object* pkInfoObject=NULL;
	
	m_pkObjectMan->GetAllObjects(&kObjects);
	
	for(vector<Object*>::iterator it=kObjects.begin();it!=kObjects.end();it++) {
		if((*it)->GetName()=="WorldInfoObject")
		{
			pkInfoObject=(*it);			
			break;
		}
	}	
	
	//clear list dont want to vaste mem =)
	kObjects.clear();
	
	if(pkInfoObject == NULL)
		return false;
		
	//detach worldinfoproperty from the object (so it wont be deleted)
	pkInfoObject->RemoveProperty(&m_kWIP);
//		cout<<"no worldinfopropery when deleting worldinfoobject =/"<<endl;
	
	//delete the worldinfoobject
	delete pkInfoObject;
	
	return true;
}

bool LevelManager::ExtractWorldInfoObject()
{	
	vector<Object*> kObjects;	
	ZFMemPackage pkg;	
	Object* pkInfoObject=NULL;
	WorldInfoProperty* pkWi=NULL;	
	
	//get objects
	m_pkObjectMan->GetAllObjects(&kObjects);
	
	
	for(vector<Object*>::iterator it=kObjects.begin();it!=kObjects.end();it++) {
		if((*it)->GetName()=="WorldInfoObject")
		{
			pkInfoObject=(*it);			
			break;
		}
	}
	
	//clear list dont want to vaste mem =)
	kObjects.clear();
	
	
	//did't find any info object =(
	if(pkInfoObject==NULL){
		m_pkConsole->Printf("ERROR: No info object found");
		return false;
	}
	
	pkWi=static_cast<WorldInfoProperty*>(pkInfoObject->GetProperty("WorldInfoProperty"));
	
	//no WorldInfoProperty found =(
	if(pkWi==NULL){
		m_pkConsole->Printf("ERROR: Info object but no info property");		
		return false;
	}
		
	//save to a zfmempackage
	pkWi->Save(&pkg);
			
	//load the info 
	pkg.SetPos(0);	
	m_kWIP.Load(&pkg);
	
	//delete the object
	delete pkInfoObject;	
	
	return true;
}

void LevelManager::SetupWorld()
{
	Fog(m_kWIP.m_kFogColor,m_kWIP.m_fFogStart,m_kWIP.m_fFogStop);
	
	Water(m_kWIP.m_bWater);
	
	SkyBox(m_kWIP.m_kSkyBoxHor.c_str(),m_kWIP.m_kSkyBoxTop.c_str(),m_kWIP.m_kSkyBoxRotate);
	
	SetSunColor(m_kWIP.m_kSunColor);
	SetMoonColor(m_kWIP.m_kMoonColor);	
	SetAmbientColor(m_kWIP.m_kAmbientColor);		
}

void LevelManager::Fog(Vector3 kColor,float fStart,float fStop)
{
	m_kWIP.m_kFogColor=kColor;
	m_kWIP.m_fFogStart=fStart;
	m_kWIP.m_fFogStop=fStop;
	
	Vector4 kTempColor;
	kTempColor=kColor;
	kTempColor.w=1;
	
	m_pkRender->SetFog(kTempColor,fStart,fStop,true);
}

void LevelManager::Water(bool bWater)
{
	m_kWIP.m_bWater=bWater;
	
	if(bWater)
	{
		//check if there already is a worldwaterobject else create one
		if(m_pkObjectMan->GetObject("WorldWaterObject") == NULL)
		{	
			//water
			Object* water = m_pkObjectMan->CreateObject();
			
			water->GetSave()=false;			
			water->AddProperty("WaterRenderProperty");
			WaterRenderProperty* wrp= static_cast<WaterRenderProperty*>(water->GetProperty("WaterRenderProperty"));
			wrp->SetProperty(m_pkMap->GetSize()+300,100,"../data/textures/water2.bmp");
			
			water->GetName()="WorldWaterObject";
			water->GetPos().Set(0,0,0);
			water->SetParent(m_pkObjectMan->GetWorldObject());		
		}
		else {
			// Force Parent to be WorldObject
			m_pkObjectMan->GetObject("WorldWaterObject")->SetParent(m_pkObjectMan->GetWorldObject());		
		}
	} else
	{
		delete m_pkObjectMan->GetObject("WorldWaterObject");
	}
}

void LevelManager::SkyBox(const char* acHor,const char* acTop,Vector3 kRotate)
{
	m_kWIP.m_kSkyBoxHor=acHor;
	m_kWIP.m_kSkyBoxTop=acTop;
	m_kWIP.m_kSkyBoxRotate=kRotate;
	
	Object* pkSkybox;
	
	//check if there already is a SkyBoxObject else create one
	if(m_pkObjectMan->GetObject("SkyBoxObject") == NULL)
	{	
		pkSkybox	=	m_pkObjectMan->CreateObject();
		pkSkybox->GetSave()=false;
		pkSkybox->SetParent(m_pkObjectMan->GetWorldObject());	
		pkSkybox->AddProperty("SkyBoxRenderProperty");
		pkSkybox->GetName()="SkyBoxObject";
	}else {
		pkSkybox=m_pkObjectMan->GetObject("SkyBoxObject");
	}
	
		// Force Parent to be WorldObject
	pkSkybox->SetParent(m_pkObjectMan->GetWorldObject());	

	SkyBoxRenderProperty* pkSBRP = static_cast<SkyBoxRenderProperty*>(pkSkybox->GetProperty("SkyBoxRenderProperty"));
	
	pkSBRP->SetTexture(acHor,acTop);
	pkSBRP->SetRotate(kRotate);			
	
//	pkSkybox->SetTexture(acHor,acTop);
//	pkSkybox->SetRotate(kRotate);			
	
}

list<Object*>* LevelManager::GetTrackerList()
{
	return &m_kTrackedObjects;
}

void LevelManager::AddTracker(Object* kObject)
{
	m_kTrackedObjects.push_back(kObject);
}

void LevelManager::RemoveTracker(Object* kObject)
{
	m_kTrackedObjects.remove(kObject);
}

int LevelManager::GetNrOfTrackedObjects()
{
	return m_kTrackedObjects.size();
}

void LevelManager::ClearTrackers()
{
	m_kTrackedObjects.clear();
}

void LevelManager::SetMoonColor(Vector3 kColor)
{
	m_bMoon->kDiffuse=kColor;
	m_kWIP.m_kMoonColor=kColor;
}

void LevelManager::SetSunColor(Vector3 kColor)
{
	m_bSun->kDiffuse=kColor;
	m_kWIP.m_kSunColor=kColor;	
}

void LevelManager::SetAmbientColor(Vector3 kColor)
{
	m_bMoon->kAmbient=kColor;
	m_kWIP.m_kAmbientColor=kColor;	
}


void LevelManager::UpdateZones()
{
//	float zpr=m_fZoneRadius/m_iZpr;

	int tot=int(m_pkMap->GetSize()/m_fZoneDistance);


//	cout<<"TOT"<<tot<<endl;
	if(m_kZones.size() <= 0)
		return;

	for(unsigned int i=0;i<m_kZones.size();i++)
		m_kZones[i]->GetUpdateStatus() = /*UPDATE_NONE*/UPDATE_DYNAMIC;	//UPDATE_DYNAMIC | UPDATE_PLAYERS;	



	for(list<Object*>::iterator it=m_kTrackedObjects.begin();it!=m_kTrackedObjects.end();it++)
	{
		Vector3 pos=(*it)->GetPos();
	
		int x=int(((pos.x+m_pkMap->GetSize()/2)+m_fZoneDistance/2)/(m_fZoneDistance));
		int z=int(((pos.z+m_pkMap->GetSize()/2)+m_fZoneDistance/2)/(m_fZoneDistance));		

		EnableZone(x,z,pos);
	}

	if(m_kTrackedObjects.size()==0)
	{
		Camera* pkCam = m_pkZeroFps->GetCam();
		if(pkCam) {
			Vector3 pos = pkCam->GetPos();
	
			int x=int(((pos.x+m_pkMap->GetSize()/2)+m_fZoneDistance/2)/(m_fZoneDistance));
			int z=int(((pos.z+m_pkMap->GetSize()/2)+m_fZoneDistance/2)/(m_fZoneDistance));			

			EnableZone(x,z,pos);	
		}
	}	

}

void LevelManager::EnableZone(int xp,int zp,Vector3 &kPos)
{
//	float zpr=m_fZoneRadius/m_iZpr;
	int w=int(m_fZoneRadius/m_fZoneDistance);
	int tot=int(m_pkMap->GetSize()/m_fZoneDistance);	
	
//	cout<<"WIDTH"<<w<<endl;
	
	for(int x = xp-w;x <= xp +w;x++)
	{
		for(int z = zp-w;z <= zp +w;z++)
		{
			if(x<0 || z<0 || x>=tot || z>=tot)
				continue;
			
			if(( kPos - m_kZones[x*tot+z]->GetPos()).Length()<m_fZoneRadius)
				if(m_iShowDecorations>0)
//					m_kZones[x*tot+z]->GetUpdateStatus()=UPDATE_ALL|UPDATE_LIGHT;		
					m_kZones[x*tot+z]->GetUpdateStatus()=UPDATE_STATIC|UPDATE_DYNAMIC|UPDATE_PLAYERS|UPDATE_STATDYN|UPDATE_LIGHT|UPDATE_DECORATION;		
				else
					m_kZones[x*tot+z]->GetUpdateStatus()=UPDATE_STATIC|UPDATE_DYNAMIC|UPDATE_PLAYERS|UPDATE_STATDYN|UPDATE_LIGHT;					
				//m_kZones[x*tot+z]->GetUpdateStatus()=UPDATE_ALL;		
		}
	}
}


Object* LevelManager::GetClosestZone(Vector3 &kPos)
{
	int tot=int(m_pkMap->GetSize()/m_fZoneDistance);	
	
	int x=int(((kPos.x+m_pkMap->GetSize()/2)+m_fZoneDistance/2)/(m_fZoneDistance));
	int z=int(((kPos.z+m_pkMap->GetSize()/2)+m_fZoneDistance/2)/(m_fZoneDistance));		

	
	return m_kZones[x*tot+z];		

}


Object* LevelManager::CreateHeightMapObject(HeightMap* pkMap)
{
	Object* ob= m_pkObjectMan->CreateObject();
	
	ob->GetName()="HeightMapObject";
	ob->GetSave()=false;	
	ob->GetObjectType()=OBJECT_TYPE_STATIC;
	
	ob->AddProperty("PhysicProperty");
	PhysicProperty* PP =static_cast<PhysicProperty*>(ob->GetProperty("PhysicProperty"));
	PP->SetColShape(new HeightMapCS(pkMap));	
	PP->m_bFloat=false;		
	PP->m_bGravity=false;		

	ob->AddProperty("HMRP2");
	(static_cast<HMRP2*>(ob->GetProperty("HMRP2")))->SetHeightMap(pkMap, "None");	

	return ob;
};

void LevelManager::DrawZones()
{
	if(!m_bDrawZones)
		return;

	for(unsigned int i=0;i<m_kZones.size();i++) {
		if(m_kZones[i]->GetUpdateStatus() & UPDATE_STATIC)
			m_pkRender->DrawColorBox(m_kZones[i]->GetPos(),Vector3::ZERO, Vector3(1,1,1),Vector3(0,1,0));
		else 
			m_pkRender->DrawColorBox(m_kZones[i]->GetPos(),Vector3::ZERO, Vector3(1,1,1),Vector3(1,0,0));
		}
}


const string LevelManager::GetLogFileFullName()
{
	string strPath;
	strPath.assign(m_kCurrentMapDir);
	strPath.append("log.txt");
	return strPath;
}

void LevelManager::ChangeLandscapeFillMode(PolygonMode eMode)
{
	HMRP2* hp= static_cast<HMRP2*>(m_pkHeightMapObject->GetProperty("HMRP2"));
	hp->SetPolyMode(eMode);
}


bool LevelManager::StartUp()	{ return true; }
bool LevelManager::ShutDown() { return true; }
bool LevelManager::IsValid()	{ return true; }







