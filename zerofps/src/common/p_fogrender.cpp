#include "p_fogrender.h"

P_FogRender::P_FogRender()
{
	strcpy(m_acName,"P_FogRender");
	
	m_pkRender=static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));		
	
	m_iType=PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT;
	
	m_fScale =			512;
	m_iSortPlace =		20;	
	m_bHaveChanged =	false;
	m_sFogTexture = 	"../data/textures/fog.tga";

	memset(m_bExploredSqrs, 0, sizeof(bool)*FOG_TEXTURE_SIZE*FOG_TEXTURE_SIZE);
	
}

void P_FogRender::Update()
{
	if(m_bHaveChanged)
	{
		m_bHaveChanged = false;
		m_pkTexMan->BindTexture(m_sFogTexture.c_str(),0);	
		m_pkTexMan->SwapTexture();
	}

	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);

	glAlphaFunc(GL_GREATER,0.1);
	glEnable(GL_ALPHA_TEST);

	glDisable(GL_DEPTH_TEST);

	m_pkRender->Quad(m_pkObject->GetPos()+Vector3(0,5,0),Vector3(-90,0,0),
		Vector3(m_fScale,m_fScale,1),m_pkTexMan->Load(m_sFogTexture.c_str(),T_NOMIPMAPPING));

	glPopAttrib();
}

void P_FogRender::Explore(float x,float y,float r)
{
 
	x+=m_fScale/2;
	y+=m_fScale/2;	
	
	//apply fog scale
	x*=(FOG_TEXTURE_SIZE/m_fScale);
	y*=(FOG_TEXTURE_SIZE/m_fScale);
	r*=(FOG_TEXTURE_SIZE/m_fScale);

	m_pkTexMan->BindTexture(m_sFogTexture.c_str(),T_NOMIPMAPPING);
	
	
	for(int i=1;i<r;i++)
	{
		for(int j=0;j<360;j+=5)
		{
			float xx=sin((float)j)*i;
			float yy=cos((float)j)*i;
			
			xx+=x;
			yy+=y;
			
			m_pkTexMan->PsetRGBA((int)xx,(int)yy,0,0,0,0);

			if(xx >= 0 && xx <255 &&
				yy >= 0 && yy <255)
				m_bExploredSqrs[(int)yy][(int)xx] = true;
		}
	}
	
	m_bHaveChanged = true;
}

void P_FogRender::UnExplore(float x,float y,float r)
{
 
	x+=m_fScale/2;
	y+=m_fScale/2;	
	
	//apply fog scale
	x*=(FOG_TEXTURE_SIZE/m_fScale);
	y*=(FOG_TEXTURE_SIZE/m_fScale);
	r*=(FOG_TEXTURE_SIZE/m_fScale);

	m_pkTexMan->BindTexture(m_sFogTexture.c_str(),T_NOMIPMAPPING);
	
	
	for(int i=1;i<r;i++)
	{
		for(int j=0;j<360;j+=5)
		{
			float xx=sin((float)j)*i;
			float yy=cos((float)j)*i;
			
			xx+=x;
			yy+=y;
			
			m_pkTexMan->PsetRGBA((int)xx,(int)yy,0,0,0,255);

			if(xx >= 0 && xx <255 &&
				yy >= 0 && yy <255)
				m_bExploredSqrs[(int)yy][(int)xx] = false;
		}
	}
	
	m_bHaveChanged = true;
}

void P_FogRender::UnExploreAll()
{
	m_pkTexMan->BindTexture(m_sFogTexture.c_str(),T_NOMIPMAPPING);
	
	float w = m_pkTexMan->GetImage()->w;
	float h = m_pkTexMan->GetImage()->h;
	
	for(int x=0;x<w;x++)
		for(int y=0;y<h;y++)
			m_pkTexMan->PsetRGBA(x,y,0,0,0,255);
		
	memset(m_bExploredSqrs, 0, sizeof(bool)*FOG_TEXTURE_SIZE*FOG_TEXTURE_SIZE);

	m_bHaveChanged = true;
}

void P_FogRender::ExploreAll()
{
	m_pkTexMan->BindTexture(m_sFogTexture.c_str(),T_NOMIPMAPPING);
	
	float w = m_pkTexMan->GetImage()->w;
	float h = m_pkTexMan->GetImage()->h;
	
	for(int x=0;x<w;x++)
		for(int y=0;y<h;y++)
			m_pkTexMan->PsetRGBA(x,y,0,0,0,0);
		
	memset(m_bExploredSqrs, 1, sizeof(bool)*FOG_TEXTURE_SIZE*FOG_TEXTURE_SIZE);

	m_bHaveChanged = true;
}

COMMON_API Property* Create_P_FogRender()
{
	return new P_FogRender();

}

