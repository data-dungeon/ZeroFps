#include "levelmanager.h"

LevelManager::LevelManager(): ZFObject("LevelManager")
{
	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan=static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkCollisionMan=static_cast<CollisionManager*>(g_ZFObjSys.GetObjectPtr("CollisionManager"));	
	m_pkBasicFS=static_cast<ZFBasicFS*>(g_ZFObjSys.GetObjectPtr("ZFBasicFS"));
	m_pkConsole=static_cast<Console*>(g_ZFObjSys.GetObjectPtr("Console"));
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));

	m_pkMap=new HeightMap();

	m_bVisibleZones=true;
	m_fZoneRadius=250;
	
	m_kMapBaseDir="../data/maps";
}

void LevelManager::CreateEmptyLevel(int iSize)
{
	CreateNew(iSize);
	CreateZones(m_fZoneRadius,m_bVisibleZones);
}

void LevelManager::Clear() 
{
	CreateNew(100);
}

void LevelManager::CreateNew(int iSize) 
{
	m_pkObjectMan->Clear();
	
	m_pkHeightMapObject=new HeightMapObject(m_pkMap);		
	m_pkHeightMapObject->SetParent(m_pkObjectMan->GetWorldObject());
	
	m_pkCollisionMan->Add(m_pkHeightMapObject);

	m_pkMap->Create(iSize);
	m_pkMap->GenerateNormals(); 
	m_pkMap->GenerateTextures();

	m_pkHeightMapObject->GetPos().Set(0,-4,0);				
	m_pkMap->SetPosition(Vector3(0,-4,0));
			
}

void LevelManager::CreateZones(float fRadius,bool bVisible)
{
	for(float x=0;x< (float)m_pkMap->m_iHmSize;x+=fRadius/3){
		for(float z=0;z< (float)m_pkMap->m_iHmSize;z+=fRadius/3){
			if(m_pkMap->Height(x,z)>-1){
				ZoneObject *object = new ZoneObject();
				object->GetPos()=Vector3(x,m_pkMap->Height(x,z),z);
				object->SetRadius(fRadius);
				object->SetParent(m_pkObjectMan->GetWorldObject());			
			}
		}
	}	
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

	kHmfile=m_kMapBaseDir;
	kHmfile+="/";
	kHmfile+=acFile;
	kHmfile+="/";	
	kHmfile+="heightmap.hm";

	kZolfile=m_kMapBaseDir;
	kZolfile+="/";
	kZolfile+=acFile;
	kZolfile+="/";	
	kZolfile+="objects.zol";

	//clear world
	Clear();
	
	//load heightmap
	if(!m_pkMap->Load(kHmfile.c_str())){
		m_pkConsole->Printf("Error loading heightmap");
		return false;
	};	
	
	//create zoneobjects
	CreateZones(m_fZoneRadius,m_bVisibleZones);		
	
	//load objects
	if(!m_pkObjectMan->LoadAllObjects(kZolfile .c_str())){
		m_pkConsole->Printf("Error loading objects");
		return false;
	}		

	//extract world info from the worldinfo object and setup
	if(!ExtractWorldInfoObject())
		return false;
	
	SetupWorld();	

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
	kHmfile+="heightmap.hm";

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
	Object *temp = new Object;
	temp->GetName()="WorldInfoObject";
	temp->AddProperty(&m_kWIP);
	temp->SetParent(m_pkObjectMan->GetWorldObject());
	
	return temp;
}

bool LevelManager::RemoveWorldInfoObject()
{
	list<Object*> kObjects;	
	Object* pkInfoObject=NULL;
	
	m_pkObjectMan->GetAllObjects(&kObjects);
	
	for(list<Object*>::iterator it=kObjects.begin();it!=kObjects.end();it++) {
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
	if(!pkInfoObject->RemoveProperty(&m_kWIP))
		cout<<"no worldinfopropery when deleting worldinfoobject =/"<<endl;
	
	//delete the worldinfoobject
	delete pkInfoObject;
	
	return true;
}

bool LevelManager::ExtractWorldInfoObject()
{	
	list<Object*> kObjects;	
	ZFMemPackage pkg;	
	Object* pkInfoObject=NULL;
	int iSize;
	WorldInfoProperty* pkWi=NULL;	
	
	//get objects
	m_pkObjectMan->GetAllObjects(&kObjects);
	
	//find out what package size to expect
	m_kWIP.Save(&pkg);
	iSize=pkg.GetSize();	
	pkg.Clear();
	
	for(list<Object*>::iterator it=kObjects.begin();it!=kObjects.end();it++) {
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
			
	//size do not match
	if(pkg.GetSize() != iSize)
	{
		m_pkConsole->Printf("Info size missmatch (map from a different version?)");
		return false;
	}
	

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
			WaterObject *water=new WaterObject(m_pkMap->m_iHmSize,50,"file:../data/textures/water2.bmp");
			water->GetName()="WorldWaterObject";
			water->GetPos().Set(m_pkMap->m_iHmSize/2,0,m_pkMap->m_iHmSize/2);
			water->SetParent(m_pkObjectMan->GetWorldObject());		
		}
	} else
	{
		delete m_pkObjectMan->GetObject("WorldWaterObject");
/*	
		list<Object*> kObjects;		
		m_pkObjectMan->GetAllObjects(&kObjects);
	
		for(list<Object*>::iterator it=kObjects.begin();it!=kObjects.end();it++) {
			if((*it)->GetName()=="WorldWaterObject")
			{
				delete (*it);
				break;
			}
		}	
	
		kObjects.clear();
*/
	}
}




