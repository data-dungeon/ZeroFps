#include "zshader.h"

ZShader::ZShader() : ZFSubSystem("ZShader")
{
	
}

bool ZShader::StartUp()
{
 	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
 	m_pkFrustum = static_cast<Frustum*>(g_ZFObjSys.GetObjectPtr("Frustum"));
 	m_pkLight = static_cast<Light*>(g_ZFObjSys.GetObjectPtr("Light")); 		
 	
//	Reset();
			
	return true;
} 

void ZShader::Reset()
{
	m_pkCurentMaterial = NULL;
	m_iNrOfVertexs = 		0;
	m_iNrOfIndexes = 		0;	
	
	m_pkVertexPointer =	NULL;
	m_pkNormalPointer =	NULL;	
	m_pkTexturePointer0 = NULL;
	m_pkTexturePointer1 = NULL;	
	m_pkTexturePointer2 = NULL;	
	m_pkTexturePointer3 = NULL;	
	m_pkIndexPointer = 	NULL;
	m_pkColorPointer = 	NULL;	

	m_pkBakupVertexPointer = NULL;
	m_pkBakupNormalPointer = NULL;	
	m_pkBakupTexturePointer0 = NULL;
	m_pkBakupTexturePointer1 = NULL;		
	m_pkBakupTexturePointer2 = NULL;		
	m_pkBakupTexturePointer3 = NULL;		
	m_pkBakupIndexPointer = NULL;
	m_pkBakupColorPointer = NULL;
				
	m_bCopyedData =		false;
	
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
		case TEXTURE_POINTER0:
			m_pkTexturePointer0 = (Vector2*)pkPointer;
			break;
		case TEXTURE_POINTER1:
			m_pkTexturePointer1 = (Vector2*)pkPointer;
			break;
		case TEXTURE_POINTER2:
			m_pkTexturePointer2 = (Vector2*)pkPointer;
			break;
		case TEXTURE_POINTER3:
			m_pkTexturePointer3 = (Vector2*)pkPointer;
			break;		
		case INDEX_POINTER:
			m_pkIndexPointer = (unsigned int*)pkPointer;
			break;
		case COLOR_POINTER:
			m_pkColorPointer = (Vector4*)pkPointer;
			break;
			
	}
	
}

void ZShader::SetNrOfVertexs(int iNr)
{
	m_iNrOfVertexs = iNr;
}

void ZShader::SetNrOfIndexes(int iNr)
{
	m_iNrOfIndexes = iNr;
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
	glDisableClientState(GL_INDEX_ARRAY);	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);	
	
	glClientActiveTextureARB(GL_TEXTURE3_ARB);	
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
	glClientActiveTextureARB(GL_TEXTURE2_ARB);	
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTextureARB(GL_TEXTURE1_ARB);	
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
	glClientActiveTextureARB(GL_TEXTURE0_ARB);	
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);		
	
	
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
	
/*	//tus
	if(m_pkTexturePointer0)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB);	
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer0);		
	}
	if(m_pkTexturePointer1)
	{
		glActiveTextureARB(GL_TEXTURE1_ARB);	
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer1);		
		glActiveTextureARB(GL_TEXTURE0_ARB);				
	}
	if(m_pkTexturePointer2)
	{
		glActiveTextureARB(GL_TEXTURE2_ARB);	
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer2);		
		glActiveTextureARB(GL_TEXTURE0_ARB);							
	}
	if(m_pkTexturePointer3)
	{
		glActiveTextureARB(GL_TEXTURE3_ARB);	
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer3);		
		glActiveTextureARB(GL_TEXTURE0_ARB);					
	}
	// -- tus
*/
}


void ZShader::SetMaterial(ZMaterial* pkMaterial)
{
	m_pkCurentMaterial = pkMaterial;
}

void ZShader::SetupPrerenderStates()
{
	if(m_pkCurentMaterial->m_bCopyData)
		CopyVertexData();		
		
	if(m_pkCurentMaterial->m_bRandomMovements)
		RandomVertexMovements();

	if(m_pkCurentMaterial->m_bWaves)
		Waves();

}

void ZShader::RandomVertexMovements()
{
	for(int i=0;i<m_iNrOfVertexs;i++)
	{
		m_pkVertexPointer[i] += Vector3( (rand()%500 /1000.0) -0.25,(rand()%500 /1000.0) -0.25,(rand()%500 /1000.0) -0.25);

	}
	
}

void ZShader::Waves()
{	
	for(int i=0;i<m_iNrOfVertexs;i++)
	{
		float offset = m_pkVertexPointer[i].x + m_pkVertexPointer[i].y + m_pkVertexPointer[i].z;
		float bla = sin(SDL_GetTicks()/500.0 + offset)*0.1;
		m_pkVertexPointer[i] += Vector3(bla,bla,bla);


//		m_pkTexturePointer0[i].x += sin(SDL_GetTicks()/500.0 + i*0.1);
//		m_pkTexturePointer0[i].y += sin(SDL_GetTicks()/500.0 + i*0.1);		
	}
}

void ZShader::SetupTU(ZMaterialSettings* pkSettings,int iTU)
{
	if(pkSettings->m_iTUs[iTU] >= 0)
	{	
		glEnable(GL_TEXTURE_2D);
		m_pkTexMan->BindTexture(pkSettings->m_iTUs[iTU]);
		
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	
			
		switch(pkSettings->m_iTUTexCords[iTU])
		{
			case CORDS_FROM_ARRAY_0:
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer0);						
				break;						
			case CORDS_FROM_ARRAY_1:
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer1);						
				break;			
			case CORDS_FROM_ARRAY_2:
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer2);						
				break;			
			case CORDS_FROM_ARRAY_3:
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer3);						
				break;			
		
		}
	}
	else
		glDisable(GL_TEXTURE_2D);	

}

void ZShader::SetupRenderStates(ZMaterialSettings* pkSettings)
{

	//polygon mode settings		front
	switch(pkSettings->m_iPolygonModeFront)
	{
		case FILL_POLYGON:
			glPolygonMode(GL_FRONT, GL_FILL);
			break;
		case LINE_POLYGON:
			glPolygonMode(GL_FRONT, GL_LINE);
			break;			
		case POINT_POLYGON:
			glPolygonMode(GL_FRONT, GL_POINT);
			break;						
	}
	
	//polygon mode settings		back	
	switch(pkSettings->m_iPolygonModeBack)
	{
		case FILL_POLYGON:
			glPolygonMode(GL_BACK, GL_FILL);
			break;
		case LINE_POLYGON:
			glPolygonMode(GL_BACK, GL_LINE);
			break;			
		case POINT_POLYGON:
			glPolygonMode(GL_BACK, GL_POINT);
			break;							
	}
	
	//depthfunction settings
	switch(pkSettings->m_iDepthFunc)
	{
		case NEVER_DEPTH:
			glDepthFunc(GL_NEVER);			
			break;
		case LESS_DEPTH:
			glDepthFunc(GL_LESS);			
			break;	
		case EQUAL_DEPTH:
			glDepthFunc(GL_EQUAL);			
			break;	
		case LEQUAL_DEPTH:
			glDepthFunc(GL_LEQUAL);			
			break;	
		case GREATER_DEPTH:
			glDepthFunc(GL_GREATER);			
			break;	
		case NOTEQUAL_DEPTH:
			glDepthFunc(GL_NOTEQUAL);			
			break;	
		case GEQUAL_DEPTH:
			glDepthFunc(GL_GEQUAL);			
			break;		
		case ALWAYS_DEPTH:
			glDepthFunc(GL_ALWAYS);			
			break;			
	}

	glDepthFunc(pkSettings->m_iDepthFunc);
	
	//lighting setting
	if(pkSettings->m_bLighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
		
	//cullface setting
	if(pkSettings->m_bCullFace)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	
	//alphatest setting
	if(pkSettings->m_bAlphaTest)
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL, 0.5);
	}
	else
		glDisable(GL_ALPHA_TEST);
		
	
	//setup TU 3
	glActiveTextureARB(GL_TEXTURE3_ARB);	
	glClientActiveTextureARB(GL_TEXTURE3_ARB);
	SetupTU(pkSettings,3);
	
	//setup TU 2
	glActiveTextureARB(GL_TEXTURE2_ARB);		
	glClientActiveTextureARB(GL_TEXTURE2_ARB);
	SetupTU(pkSettings,2);
	
	//setup TU 1
	glActiveTextureARB(GL_TEXTURE1_ARB);		
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	SetupTU(pkSettings,1);
	
	//setup TU 0
	glActiveTextureARB(GL_TEXTURE0_ARB);		
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	SetupTU(pkSettings,0);
	

	//want TU 0 to be active when exiting

}

void ZShader::CopyVertexData()
{
	m_bCopyedData = true;	
	
	m_pkBakupVertexPointer = m_pkVertexPointer;
	m_pkBakupNormalPointer = m_pkNormalPointer;	
	m_pkBakupTexturePointer0 = m_pkTexturePointer0;
	m_pkBakupTexturePointer1 = m_pkTexturePointer1;		
	m_pkBakupTexturePointer2 = m_pkTexturePointer2;		
	m_pkBakupTexturePointer3 = m_pkTexturePointer3;		
	m_pkBakupIndexPointer = m_pkIndexPointer;
	m_pkBakupColorPointer = m_pkColorPointer;
	
	if(m_pkVertexPointer)
		CopyData((void**)&m_pkVertexPointer,sizeof(Vector3)*m_iNrOfVertexs);

	if(m_pkNormalPointer)
		CopyData((void**)&m_pkNormalPointer,sizeof(Vector3)*m_iNrOfVertexs);
	
	if(m_pkIndexPointer)
		CopyData((void**)&m_pkIndexPointer,sizeof(unsigned int)*m_iNrOfVertexs);
	
	if(m_pkColorPointer)
		CopyData((void**)&m_pkColorPointer,sizeof(Vector4)*m_iNrOfVertexs);
	
	
	if(m_pkTexturePointer0)
		CopyData((void**)&m_pkTexturePointer0,sizeof(Vector2)*m_iNrOfVertexs);
	if(m_pkTexturePointer1)
		CopyData((void**)&m_pkTexturePointer1,sizeof(Vector2)*m_iNrOfVertexs);
	if(m_pkTexturePointer2)
		CopyData((void**)&m_pkTexturePointer2,sizeof(Vector2)*m_iNrOfVertexs);
	if(m_pkTexturePointer3)
		CopyData((void**)&m_pkTexturePointer3,sizeof(Vector2)*m_iNrOfVertexs);
	
}

void ZShader::CopyData(void** pkData,int iSize)
{
	void* pkNewData = new char[iSize];
	memcpy(pkNewData,*pkData,iSize);
	
	*pkData = pkNewData;
	
}

void ZShader::CleanCopyedData()
{
	delete m_pkVertexPointer;	
	delete m_pkNormalPointer;		
	delete m_pkTexturePointer0;		
	delete m_pkTexturePointer1;			
	delete m_pkTexturePointer2;			
	delete m_pkTexturePointer3;			
	delete m_pkIndexPointer;		
	delete m_pkColorPointer;		
	
	m_pkVertexPointer =	m_pkBakupVertexPointer;	
	m_pkNormalPointer =	m_pkBakupNormalPointer;		
	m_pkTexturePointer0 = m_pkBakupTexturePointer0;		
	m_pkTexturePointer1 = m_pkBakupTexturePointer1;			
	m_pkTexturePointer2 = m_pkBakupTexturePointer2;			
	m_pkTexturePointer3 = m_pkBakupTexturePointer3;			
	m_pkIndexPointer =	m_pkBakupIndexPointer;		
	m_pkColorPointer =	m_pkBakupColorPointer;		
	
	m_bCopyedData = false;
	
}


void ZShader::Draw()
{	
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	SetupPrerenderStates();

	//we have to reset opengls data pointers
	SetupClientStates();

	//go trough all passes of material
	for(int i=0;i<m_pkCurentMaterial->GetNrOfPasses();i++)
	{	
		SetupRenderStates(m_pkCurentMaterial->GetPass(i));		
		
		if(m_pkIndexPointer)
			glDrawElements(m_iDrawMode,m_iNrOfIndexes,GL_UNSIGNED_INT,m_pkIndexPointer);
		else
			glDrawArrays(m_iDrawMode,0,m_iNrOfVertexs);	
	}
	
	if(m_bCopyedData)
		CleanCopyedData();
	
	
	glPopMatrix();
	glPopAttrib();
	
}

bool ZShader::ShutDown() { return true; }
bool ZShader::IsValid()	{ return true; }

