#pragma warning( disable : 4786 )

#include "lightproperty.h"
#include "object.h"

LightProperty::LightProperty()
{
	strcpy(m_acName,"LightProperty");

	spot=new LightSource();
	spotpos=new Vector3(.4,.4,.2);
	spotrot=new Vector3(0,0,0);	
	spot->kPos=spotpos;
	spot->kRot=spotrot;
	spot->kDiffuse=Vector4(0,0,0,1.0);
	spot->kAmbient=Vector4(0.0,0.0,0.0,1.0);
	spot->iType=POINT_LIGHT;			
	spot->iPriority=0;
	spot->fCutoff=20;	
	spot->fExp=5;
	spot->fConst_Atten=0;
	spot->fQuadratic_Atten=0.1;

	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));
	m_pkLight->Add(spot);

}

LightProperty::~LightProperty()
{
	m_pkLight->Remove(spot);
}

void LightProperty::Update() 
{
	spot->kPos->Set(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
}

void LightProperty::SetColor(float r, float g, float b)
{
	spot->kDiffuse=Vector4(r,g,b,0);

}


void LightProperty::SetLightPos(Vector3 kNewPos)
{
	spot->kPos->Set(kNewPos.x,kNewPos.y,kNewPos.z);
}


Property* Create_LightProperty()
{
	return new LightProperty;
}
