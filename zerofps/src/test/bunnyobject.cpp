#include "bunnyobject.h"

BunnyObject::BunnyObject(HeightMap *pkMap)
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));

	AddProperty(new GravityProperty());
	//AddProperty("GravityProperty");
	AddProperty("ModelProperty");
	AddProperty(new BunnyAIProperty(pkMap));
	
	ModelProperty* mod = dynamic_cast<ModelProperty*>(GetProperty("ModelProperty"));
	if(mod) {
		mod->m_fRadius=0.15;
		mod->m_iSlices=6;
		mod->m_iStacks=6;
		mod->m_kColor.Set(rand()%1000/1000.0,rand()%1000/1000.0,rand()%1000/1000.0);
		}
	
	AddProperty(new CollisionProperty(&m_kPos,new float(.15)));
//	AddProperty(new CollisionProperty(&m_kPos));
	
	onGround=false;
}


void BunnyObject::HandleCollision(Object* pkOther,Vector3 kPos,bool bContinue)
{
	
	if(typeid(*pkOther)==typeid(HeightMapObject)){
//		HeightMapObject *kO=dynamic_cast<HeightMapObject*>(pkOther);
	
		GetPos()=kPos+Vector3(0,.15,0);
		m_kVel.y=0;
		onGround=true;
	} else if(typeid(*pkOther)==typeid(BunnyObject)){
//		BunnyObject *kO=dynamic_cast<BunnyObject*>(pkOther);
		
		m_kRot.y+=180;
		pkOther->GetRot().y+=180;		
		
		GetPos().x+=cos((GetRot().y-90)/degtorad)*0.1;			
		GetPos().z+=sin((GetRot().y-90)/degtorad)*0.1;			

		
	} else if(bContinue){
		pkOther->HandleCollision(this,kPos,false);
	}

}






