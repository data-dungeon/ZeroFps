#include "bunnyaiproperty.h"


BunnyAIProperty::BunnyAIProperty(ZeroFps *pkFps,HeightMap *pkMap)
{
	m_pkFps=pkFps;
	m_pkMap=pkMap;

	m_iState=0;

	strcpy(m_acName,"BunnyAIProperty");	
}


void BunnyAIProperty::Update() 
{
	float speed=0.002;
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
		m_pkObject->GetVel().y+=0.002;
		m_pkObject->GetPos().y+=0.05;		
	}
	
	bunny->onGround=false;	
}










