#pragma warning( disable : 4786 )

#include "lightproperty.h"
#include "object.h"

LightProperty::LightProperty()
{
	bNetwork = true;
	strcpy(m_acName,"LightProperty");
	
	spot=new LightSource();
	pkPos=new Vector3(0,0,0);
	pkRot=new Vector3(0,0,0);
	spot->kPos=pkPos;
	spot->kRot=pkPos;
	
/*	
	spot->kDiffuse=Vector4(0,0,0,1.0);
	spot->kAmbient=Vector4(0.0,0.0,0.0,1.0);
	spot->iType=POINT_LIGHT;			
	spot->iPriority=0;
	spot->fCutoff=20;	
	spot->fExp=5;
	spot->fConst_Atten=1;
	spot->fLinear_Atten=0;
	spot->fQuadratic_Atten=0;
*/

	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light"));
	m_pkLight->Add(spot);

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;

}

LightProperty::~LightProperty()
{
	m_pkLight->Remove(spot);
	delete spot;
	delete pkPos;
	delete pkRot;
}

void LightProperty::Update() 
{
	SetPos(m_pkObject->GetPos());
//	spot->kPos->Set(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
}

void LightProperty::PackTo( NetPacket* pkNetPacket ) {
	pkNetPacket->Write( spot->kDiffuse );		
}

void LightProperty::PackFrom( NetPacket* pkNetPacket ) {
	pkNetPacket->Read( spot->kDiffuse );		

}

Property* Create_LightProperty()
{
	return new LightProperty;
}
