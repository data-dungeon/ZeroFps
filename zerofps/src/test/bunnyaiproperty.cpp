#include "bunnyaiproperty.h"


BunnyAIProperty::BunnyAIProperty()
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkMap = static_cast<HeightMap*>(g_ZFObjSys.GetObjectPtr("HeightMap"));	

	m_iState=0;

	strcpy(m_acName,"BunnyAIProperty");	

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
}


void BunnyAIProperty::Update() 
{
	float speed=2;
	BunnyObject *bunny = dynamic_cast<BunnyObject*>(m_pkObject);
	
	
	if(bunny->onGround)
		m_pkObject->GetRot().y+= (rand()%40000/1000.0) -20.0;


	//check for water
	Vector3 temppos=m_pkObject->GetPos();
	temppos.x+=cos((m_pkObject->GetRot().y-90)/degtorad)*.5;			
	temppos.z+=sin((m_pkObject->GetRot().y-90)/degtorad)*.5;

	//if water make a U-turn =)
	if(m_pkMap->Height(temppos.x,temppos.z)<0)
		m_pkObject->GetRot().y+=180;		

	//move
	m_pkObject->GetPos().x+=cos((m_pkObject->GetRot().y-90)/degtorad)*m_pkFps->GetFrameTime()*speed;			
	m_pkObject->GetPos().z+=sin((m_pkObject->GetRot().y-90)/degtorad)*m_pkFps->GetFrameTime()*speed;			

	
	if(bunny->onGround){
		m_pkObject->GetVel().y+=2;
		m_pkObject->GetPos().y+=0.05;		
	}
	
	bunny->onGround=false;	
}










