#include "modelproperty.h"
#include "object.h"


ModelProperty::ModelProperty() {
	bNetwork = true;
	strcpy(m_acName,"ModelProperty");

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;	
	m_fRadius = 1.0;
	m_iSlices = 5;
	m_iStacks = 5;
	m_kColor.Set(1,1,1);
	
	m_pkFrustum=static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
}


void ModelProperty::Update() {
	Vector4 sphere=m_pkObject->GetPos();
	sphere.w=m_fRadius;
	
	if(!m_pkFrustum->SphereInFrustum(sphere))
		return;
	
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
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
	pkPackage->Write( m_iType );		
	pkPackage->Write( m_fRadius );		
	pkPackage->Write( m_iSlices );		
	pkPackage->Write( m_iStacks );		
	pkPackage->Write( m_kColor );	
}

void ModelProperty::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read( m_iType );		
	pkPackage->Read( m_fRadius );		
	pkPackage->Read( m_iSlices );		
	pkPackage->Read( m_iStacks );		
	pkPackage->Read( m_kColor );	
}


Property* Create_ModelProperty()
{
	return new ModelProperty;
}




