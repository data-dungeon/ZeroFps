#include "primitives3d.h"
#include "../../engine/object.h"
#include "../../basic/zfobjectmanger.h"
 
P_Primitives3D::P_Primitives3D(PrimType eType) : m_ePrimType(eType)
{
	bNetwork = true;
	strcpy(m_acName,"P_Primitives3D");

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;	
	m_fRadius = 0.5;
	m_iSlices = 10;
	m_iStacks = 10;
	m_kColor.Set(1,1,1);
	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	
	m_pkFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	
}


void P_Primitives3D::Update() {
//	Vector4 sphere=m_pkObject->GetPos();
//	sphere.w=m_fRadius;
	
//	if(!m_pkFps->GetCam()->m_kFrustum.SphereInFrustum(m_pkObject->GetPos(),m_fRadius))
//		return;


	m_pkRender->Sphere(m_pkObject->GetIPos(),m_fRadius,m_iSlices,m_kColor,true);

/*
	glPushAttrib(GL_FOG_BIT|GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT );
//	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);	
	glDisable(GL_TEXTURE_2D);
	
	glColor3f(m_kColor.x,m_kColor.y,m_kColor.z);
	glPushMatrix();
		glTranslatef(m_pkObject->GetIPos().x,m_pkObject->GetIPos().y,m_pkObject->GetIPos().z);
		glutSolidSphere(m_fRadius, m_iSlices, m_iStacks);
	glPopMatrix();
	glPopAttrib();*/
}

void P_Primitives3D::PackTo( NetPacket* pkNetPacket ) {
	pkNetPacket->Write( m_iType );		
	pkNetPacket->Write( m_fRadius );		
	pkNetPacket->Write( m_iSlices );		
	pkNetPacket->Write( m_iStacks );		
	pkNetPacket->Write( m_kColor );		
}

void P_Primitives3D::PackFrom( NetPacket* pkNetPacket ) {
	pkNetPacket->Read( m_iType );		
	pkNetPacket->Read( m_fRadius );		
	pkNetPacket->Read( m_iSlices );		
	pkNetPacket->Read( m_iStacks );		
	pkNetPacket->Read( m_kColor );		
	
}

void P_Primitives3D::Save(ZFMemPackage* pkPackage)
{	
	pkPackage->Write((void*)&m_iType,4 );		
	pkPackage->Write((void*)&m_fRadius,4 );		
	pkPackage->Write((void*)&m_iSlices,4 );		
	pkPackage->Write((void*)&m_iStacks,4 );		
	pkPackage->Write((void*)&m_kColor,12 );	
}

void P_Primitives3D::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read((void*)&m_iType	,4 );		
	pkPackage->Read((void*)&m_fRadius,4 );		
	pkPackage->Read((void*)&m_iSlices,4 );		
	pkPackage->Read((void*)&m_iStacks,4 );		
	pkPackage->Read((void*)&m_kColor	,12 );	
}


vector<PropertyValues> P_Primitives3D::GetPropertyValues()
{
	vector<PropertyValues> kReturn(1);

	kReturn[0].kValueName="Radius";
	kReturn[0].iValueType=VALUETYPE_FLOAT;
	kReturn[0].pkValue=(void*)&m_fRadius;

	return kReturn;
};



Property* Create_Prim3DProperty()
{
	return new P_Primitives3D;
}
