#include "p_fogrender.h"

P_FogRender::P_FogRender()
{
	strcpy(m_acName,"P_FogRender");
	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));		
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_kScale.Set(512,512,1);
	m_iSortPlace	=	20;	
	m_sFogTexture = "../data/textures/fog.tga";
	
}

void P_FogRender::Update()
{
	glAlphaFunc(GL_GREATER,0.1);
	glEnable(GL_ALPHA_TEST);
	
	glDisable(GL_DEPTH_TEST);

	m_pkRender->Quad(m_pkObject->GetPos()+Vector3(0,5,0),Vector3(-90,0,0),m_kScale,m_pkTexMan->Load(m_sFogTexture.c_str(),0));

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);	
}

void P_FogRender::Explore(float x,float y,float r)
{
	m_pkTexMan->BindTexture(m_sFogTexture.c_str(),0);
	
	for(int i=1;i<r;i++)
	{
		for(int j=0;j<360;j+=5)
		{
			float xx=sin(j)*i;
			float yy=cos(j)*i;
			
			xx+=x;
			yy+=y;
			
			m_pkTexMan->PsetRGBA(xx,yy,0,0,0,255);
		}
	}
}

COMMON_API Property* Create_P_FogRender()
{
	return new P_FogRender();

}

