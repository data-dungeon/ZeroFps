#include "zshader.h"

ZShader::ZShader() : ZFObject("ZShader")
{
	
}

bool ZShader::StartUp()
{
 	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
 	m_pkFrustum = static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
 	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light")); 		
 	
	Reset();
			
	return true;
} 

void ZShader::Reset()
{
	m_pkCurentMaterial = NULL;
	m_iNrOfVertexs = 		0;
	
	m_pkVertexPointer =	NULL;
	m_pkNormalPointer =	NULL;	
	m_pkTexturePointer = NULL;
	m_pkIndexPointer = 	NULL;
	m_pkColorPointer = 	NULL;	

	SetDrawMode(POLYGON_MODE);
}

void ZShader::SetPointer(int iType,void* pkPointer)
{
	switch(iType)
	{
		case VERTEX_POINTER:
			m_pkVertexPointer = (Vector3*)pkPointer;
			break;
		case NORMAL_POINTER:
			m_pkNormalPointer = (Vector3*)pkPointer;
			break;
		case TEXTURE_POINTER:
			m_pkTexturePointer = (Vector2*)pkPointer;
			break;
		case INDEX_POINTER:
			m_pkIndexPointer = (int*)pkPointer;
			break;
		case COLOR_POINTER:
			m_pkColorPointer = (Vector4*)pkPointer;
			break;
			
	}
	
	SetupClientStates();
}

void ZShader::SetNrOfVertexs(int iNr)
{
	m_iNrOfVertexs = iNr;
}

void ZShader::SetDrawMode(int iDrawMode)
{
	switch(iDrawMode)
	{
		case POINTS_MODE:
			m_iDrawMode = GL_POINTS;
			break;
		case LINES_MODE:
			m_iDrawMode = GL_LINES;
			break;
		case LINESTRIP_MODE:
			m_iDrawMode = GL_LINE_STRIP;
			break;		
		case LINELOOP_MODE:
			m_iDrawMode = GL_LINE_LOOP;
			break;
		case TRIANGLES_MODE:
			m_iDrawMode = GL_TRIANGLES;
			break;
		case TRIANGLESTRIP_MODE:
			m_iDrawMode = GL_TRIANGLE_STRIP;
			break;
		case TRIANGLEFAN_MODE:
			m_iDrawMode = GL_TRIANGLE_FAN;
			break;
		case QUADS_MODE:
			m_iDrawMode = GL_QUADS;
			break;
		case QUADSTRIP_MODE:
			m_iDrawMode = GL_QUAD_STRIP;
			break;
		case POLYGON_MODE:
			m_iDrawMode = GL_POLYGON;
			break;	
	}

}


void ZShader::SetupClientStates()
{
	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_NORMAL_ARRAY);	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
	glDisableClientState(GL_INDEX_ARRAY);	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);	
	
	if(m_pkVertexPointer)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,m_pkVertexPointer);
	}	
	if(m_pkNormalPointer)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,0,m_pkNormalPointer);		
	}
	if(m_pkTexturePointer)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer);		
	}
	if(m_pkIndexPointer)
	{
		glEnableClientState(GL_INDEX_ARRAY);
		glIndexPointer(GL_INT,0,m_pkIndexPointer);
	}
	if(m_pkColorPointer)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT,0,m_pkColorPointer);
	}
}


void ZShader::BindMaterial(ZMaterial* pkMaterial)
{
	m_pkCurentMaterial = pkMaterial;
}


void ZShader::SetupRenderStates(ZMaterialSettings* pkSettings)
{
	//polygon mode settings	
	glPolygonMode(GL_FRONT, pkSettings->m_iPolygonModeFront);
	glPolygonMode(GL_BACK, pkSettings->m_iPolygonModeBack);	

	//lighting setting
	if(pkSettings->m_bLighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
		
	//cullface setting
	if(pkSettings->m_bLighting)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);	
	
	//setup TU 1
	glActiveTextureARB(GL_TEXTURE0_ARB);
	if(pkSettings->m_iTUs[0] > 0)
	{	
		glEnable(GL_TEXTURE_2D);
		m_pkTexMan->BindTexture(pkSettings->m_iTUs[0]);
		
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	
			
	}
	
	//setup TU 2
	glActiveTextureARB(GL_TEXTURE1_ARB);
	if(pkSettings->m_iTUs[1] > 0)
	{	
		glEnable(GL_TEXTURE_2D);
		m_pkTexMan->BindTexture(pkSettings->m_iTUs[0]);
	
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);		
	}


}

void ZShader::Draw()
{	
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	//go trough all passes of material
	for(int i=0;i<m_pkCurentMaterial->GetNrOfPasses();i++)
	{
		ZMaterialSettings* pkSettings = m_pkCurentMaterial->GetPass(i);
	
		SetupRenderStates(pkSettings);
		
		glDrawArrays(m_iDrawMode,0,m_iNrOfVertexs);
	}
	
	glPopMatrix();
	glPopAttrib();
}


