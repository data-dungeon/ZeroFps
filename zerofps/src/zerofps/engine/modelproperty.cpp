#include "modelproperty.h"
#include "object.h"


ModelProperty::ModelProperty() {
	bNetwork = true;
	strcpy(m_acName,"ModelProperty");

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;	
	m_fRadius = 1.0;
	m_iSlices = 10;
	m_iStacks = 10;
	m_kColor.Set(1,1,1);
	
	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
}


void ModelProperty::Update() {
	Vector4 sphere=m_pkObject->GetPos();
	sphere.w=m_fRadius;
	
	if(!m_pkFrustum->SphereInFrustum(sphere))
		return;
	
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
//	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	
	glColor3f(m_kColor.x,m_kColor.y,m_kColor.z);
	glPushMatrix();
		glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
		glutSolidSphere(m_fRadius, m_iSlices, m_iStacks);
	glPopMatrix();
	glPopAttrib();
}

void ModelProperty::PackTo( NetPacket* pkNetPacket ) {
	pkNetPacket->Write( m_iType );		
	pkNetPacket->Write( m_fRadius );		
	pkNetPacket->Write( m_iSlices );		
	pkNetPacket->Write( m_iStacks );		
	pkNetPacket->Write( m_kColor );		
}

void ModelProperty::PackFrom( NetPacket* pkNetPacket ) {
	pkNetPacket->Read( m_iType );		
	pkNetPacket->Read( m_fRadius );		
	pkNetPacket->Read( m_iSlices );		
	pkNetPacket->Read( m_iStacks );		
	pkNetPacket->Read( m_kColor );		
	
}

void ModelProperty::Save(ZFMemPackage* pkPackage)
{	
	pkPackage->Write((void*)&m_iType,4 );		
	pkPackage->Write((void*)&m_fRadius,4 );		
	pkPackage->Write((void*)&m_iSlices,4 );		
	pkPackage->Write((void*)&m_iStacks,4 );		
	pkPackage->Write((void*)&m_kColor,12 );	
}

void ModelProperty::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read((void*)&m_iType	,4 );		
	pkPackage->Read((void*)&m_fRadius,4 );		
	pkPackage->Read((void*)&m_iSlices,4 );		
	pkPackage->Read((void*)&m_iStacks,4 );		
	pkPackage->Read((void*)&m_kColor	,12 );	
}


vector<PropertyValues> ModelProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	kReturn[0].kValueName="Radius";
	kReturn[0].iValueType=VALUETYPE_FLOAT;
	kReturn[0].pkValue=(void*)&m_fRadius;

	return kReturn;
};



Property* Create_ModelProperty()
{
	return new ModelProperty;
}
