#include "bunnyobject.h"

BunnyObject::BunnyObject()
{
	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
//	m_pkMap = static_cast<HeightMap*>(g_ZFObjSys.GetObjectPtr("HeightMap"));

	AddProperty(new GravityProperty());
	AddProperty(new FloatProperty());	

//	AddProperty("ModelProperty");
	AddProperty(new BunnyAIProperty());

	Vector3 kColor;
	kColor.Set(rand()%1000/1000.0,rand()%1000/1000.0,rand()%1000/1000.0);
	

/*	ModelProperty* mod = dynamic_cast<ModelProperty*>(GetProperty("ModelProperty"));
	if(mod) {
		mod->m_fRadius=0.15;
		mod->m_iSlices=6;
		mod->m_iStacks=6;
		mod->m_kColor = kColor;
		}*/

	AddProperty("MadProperty");
	MadProperty* madp = dynamic_cast<MadProperty*>(GetProperty("MadProperty"));
	madp->SetBase(m_pkFps->GetMADPtr("../data/mad/golem.mad"));
	madp->SetScale(0.01);
	madp->bFlipFace = true;
	madp->PlayAnimation(4,0);

	// Välj den kanin som ÄR.
	AddProperty("LightProperty");
	LightProperty* pkJumpLight = dynamic_cast<LightProperty*>(GetProperty("LightProperty"));
//	pkJumpLight->SetConst_Atten(0.1);
	pkJumpLight->SetDiffuse(Vector4(kColor.x/5,kColor.y/5,kColor.z/5,0));
//	pkJumpLight->SetQuadratic_Atten(20);
//	pkJumpLight->SetLinear_Atten(4);

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
		
		m_kRot.y+=180;
		pkOther->GetRot().y+=180;		
		
		GetPos().x+=cos((GetRot().y-90)/degtorad)*0.3;			
		GetPos().z+=sin((GetRot().y-90)/degtorad)*0.3;			
		
	} else if(bContinue){
		pkOther->HandleCollision(this,kPos,false);
	}

}






