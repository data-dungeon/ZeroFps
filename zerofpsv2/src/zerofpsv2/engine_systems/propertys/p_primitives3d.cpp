#include "p_primitives3d.h"
#include "../../engine/entity.h"
#include "../../basic/zfsystem.h"
 
P_Primitives3D::P_Primitives3D(PrimType eType) : m_ePrimType(eType)
{
	m_pkRender	=	static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));	
	m_pkFps		=	static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));	

	strcpy(m_acName,"P_Primitives3D");
	m_bNetwork = true;

	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;	
	m_fRadius = 0.5;
	m_iSlices = 2;
	m_iStacks = 2;
	m_kColor.Set(1,1,1);

	m_kMin.Set(-1,-1,-1);
	m_kMax.Set(1,1,1);
}


void P_Primitives3D::Update() 
{

//	Vector4 sphere=m_pkObject->GetPos();
//	sphere.w=m_fRadius;
	
//	if(!m_pkFps->GetCam()->m_kFrustum.SphereInFrustum(m_pkObject->GetPos(),m_fRadius))
//		return;

	Vector3 min, max;
	Vector3 pos = m_pkEntity->GetIWorldPosV();

	switch(m_ePrimType)
	{
	case SPHERE:
		m_pkRender->Sphere(pos,m_fRadius,m_iSlices,m_kColor,true);
		break;
	case BBOX:
		m_pkRender->DrawBoundingBox(pos, Vector3(0,0,1), 
			Vector3(m_fRadius, m_fRadius, m_fRadius), m_kColor);
		break;
	case AABBOX:
		min = Vector3(pos.x-m_fRadius/2, pos.y-m_fRadius/2, pos.z-m_fRadius/2);
		max = Vector3(pos.x+m_fRadius/2, pos.y+m_fRadius/2, pos.z+m_fRadius/2);
		m_pkRender->DrawAABB(min, max, m_kColor);
		break;
	case PYRAMID:
		m_pkRender->DrawPyramid(pos,Vector3(m_fRadius,m_fRadius,m_fRadius),m_kColor, true);
		break;
	case CONE:
		m_pkRender->DrawCone(pos, m_fRadius, m_fRadius*2, m_kColor, false);
		break;
	case SOLIDBBOX:
		m_pkRender->DrawSolidAABB(pos + m_kMin, pos + m_kMax, m_kColor);
		break;
	}
	
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

void P_Primitives3D::PackTo( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Write( m_iType );		
	pkNetPacket->Write( m_fRadius );		
	pkNetPacket->Write( m_iSlices );		
	pkNetPacket->Write( m_iStacks );		
	pkNetPacket->Write( m_kColor );		
}

void P_Primitives3D::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read( m_iType );		
	pkNetPacket->Read( m_fRadius );		
	pkNetPacket->Read( m_iSlices );		
	pkNetPacket->Read( m_iStacks );		
	pkNetPacket->Read( m_kColor );		
}

void P_Primitives3D::Save(ZFIoInterface* pkPackage)
{	
	pkPackage->Write((void*)&m_iType,4 ,1);		
	pkPackage->Write((void*)&m_fRadius,4 ,1);		
	pkPackage->Write((void*)&m_iSlices,4,1 );		
	pkPackage->Write((void*)&m_iStacks,4,1);		
	pkPackage->Write((void*)&m_kColor,12,1 );	
}

void P_Primitives3D::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read((void*)&m_iType	,4 ,1);		
	pkPackage->Read((void*)&m_fRadius,4,1 );		
	pkPackage->Read((void*)&m_iSlices,4 ,1);		
	pkPackage->Read((void*)&m_iStacks,4 ,1);		
	pkPackage->Read((void*)&m_kColor	,12 ,1);	
}

vector<PropertyValues> P_Primitives3D::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);

	kReturn[0].kValueName="Radius";
	kReturn[0].iValueType=VALUETYPE_FLOAT;
	kReturn[0].pkValue=(void*)&m_fRadius;

	kReturn[1].kValueName = "color";
	kReturn[1].iValueType = VALUETYPE_VECTOR3;
	kReturn[1].pkValue    = (void*)&m_kColor;


	return kReturn;
};

bool P_Primitives3D::HandleSetValue( string kValueName ,string kValue )
{
	if(strcmp(kValueName.c_str(), "color") == 0) 
		SetNetUpdateFlag(true);

	return false;
}

Property* Create_Prim3DProperty()
{
	return new P_Primitives3D;
}
