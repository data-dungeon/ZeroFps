#include "zshader.h"

ZShader::ZShader() : ZFSubSystem("ZShader")
{
	
}
 
bool ZShader::StartUp()
{
 	m_pkTexMan	= static_cast<TextureManager*>(GetSystem().GetObjectPtr("TextureManager"));
 	m_pkLight	= static_cast<Light*>(GetSystem().GetObjectPtr("Light"));


	m_iForceLighting = LIGHT_MATERIAL;
	m_iForceBledning = BLEND_MATERIAL;	
	m_kModelMatrix.Identity();


	//check if we have arb_vertex_program support
	if(HaveVertexProgramExt())
		m_bVertexProgram = true;
	else
	{
		cout<<"ARB Vertex program not supported"<<endl;
		m_bVertexProgram = false;
	}

	//check if we have arb_fragment_program support
	if(HaveFragmentProgramExt())
		m_bFragmentProgram = true;
	else
	{
		cout<<"ARB Fragment program not supported"<<endl;
		m_bFragmentProgram = false;
	}	
	
		
	//initiate GPU states
	SetVertexProgram(NO_VPROGRAM);
	SetFragmentProgram(NO_FPROGRAM);

		
	return true;
}

bool ZShader::ShutDown()	{ return true; }
bool ZShader::IsValid()		{ return true; }


void ZShader::Reset()
{
	m_pkCurentMaterial = 		NULL;
	m_iNrOfVertexs = 				0;
	m_iNrOfIndexes = 				0;

	m_pkVertexPointer =			NULL;
	m_pkNormalPointer =			NULL;
	m_pkTexturePointer0 = 		NULL;
	m_pkTexturePointer1 = 		NULL;
	m_pkTexturePointer2 = 		NULL;
	m_pkTexturePointer3 = 		NULL;
	m_pkIndexPointer = 			NULL;
	m_pkColorPointer = 			NULL;

	m_pkBakupVertexPointer = 	NULL;
	m_pkBakupNormalPointer =	NULL;
	m_pkBakupTexturePointer0 = NULL;
	m_pkBakupTexturePointer1 = NULL;
	m_pkBakupTexturePointer2 = NULL;
	m_pkBakupTexturePointer3 = NULL;
	m_pkBakupIndexPointer = 	NULL;
	m_pkBakupColorPointer = 	NULL;

	m_bCopyedData =				false;

	SetDrawMode(POLYGON_MODE);

	//SetVertexOffset(Vector3(0,0,0));
	//m_kModelMatrix.Identity();
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
		
	if(m_pkCurentMaterial->m_bTextureOffset)
		TextureOffset();

	if(m_pkCurentMaterial->m_bRandomMovements)
		RandomVertexMovements();

	if(m_pkCurentMaterial->m_bWaves)
		Waves();


	

}

void ZShader::SetupVertexProgram(ZMaterialSettings* pkSettings)
{
	if(!SupportVertexProgram())
		return;
	
	int iVP;
	ZVProgram* pkRt = (ZVProgram*)pkSettings->m_pkVP->GetResourcePtr();

	
	if(!pkRt)
		iVP = NO_VPROGRAM;
	else
		iVP = pkRt->m_iId;
		

	SetVertexProgram(iVP);
}

void ZShader::SetupFragmentProgram(ZMaterialSettings* pkSettings)
{
	if(!SupportFragmentProgram())
		return;

	int iFP;
	ZFProgram* pkRt = (ZFProgram*)pkSettings->m_pkFP->GetResourcePtr();

	
	if(!pkRt)
		iFP = NO_FPROGRAM;
	else
		iFP = pkRt->m_iId;
		
	//set current frament program
	SetFragmentProgram(iFP);
	
	//update fragment program parameters
	UpdateFragmentProgramParameters();
}

void ZShader::SetupTU(ZMaterialSettings* pkSettings,int iTU)
{
	//cout<<"trying to enable tu: "<<iTU<<endl;

	int iTexture;
	ResTexture* pkRt = (ResTexture*)pkSettings->m_kTUs[iTU]->GetResourcePtr();

	
	if(!pkRt)
		iTexture = -1;
	else
		iTexture = (pkRt)->m_iTextureID;
		
	if( iTexture >= 0)
	{	
		glEnable(GL_TEXTURE_2D);
		m_pkTexMan->BindTexture(iTexture);
		
		//cout<<"TU: "<<iTU<<" using texture: "<<pkRt->strTextureName<<endl;
		
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
		
			case CORDS_SPHERE_MAP:
				glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
				glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);				
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);				
				break;			
			case CORDS_EYE_LINEAR:
				glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
				glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);				
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);				
				break;			
			case CORDS_OBJECT_LINEAR:
				glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
				glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);				
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);				
				break;			
		
		}
	}
	else
	{
		//cout<<"disabl*:ASD: "<<iTU<<endl;
		glDisable(GL_TEXTURE_2D);	
	}
}

void ZShader::SetupRenderStates(ZMaterialSettings* pkSettings)
{
	//run effects
	if(pkSettings->m_iTextureColorEffect != -1)
		ColorEffect(pkSettings);

	//enable color material if wanted
	if(pkSettings->m_bColorMaterial)
	{
		glEnable(GL_COLOR_MATERIAL);	
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	}
	else
		glDisable(GL_COLOR_MATERIAL);	
	
	//set vertex color	
	glColor4fv(&pkSettings->m_kVertexColor.x);


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

   if ( pkSettings->m_bDepthTest )
      glEnable (GL_DEPTH_TEST);
   else
      glDisable (GL_DEPTH_TEST);
	
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
	
	
	//enable /disable blending
	if(m_iForceBledning == BLEND_MATERIAL)				//default using material settings
	{	
		if(pkSettings->m_bBlend)
		{
			glEnable(GL_BLEND);
			//glDepthMask(GL_FALSE);
		}
		else
		{
			glDisable(GL_BLEND);
			//glDepthMask(GL_TRUE);
		}
		
		//setup blending factors
		int iblendsrc;
		int iblenddst;
	
		//setup src blend factor
		switch(pkSettings->m_iBlendSrc)
		{
			case ZERO_BLEND_SRC:
				iblendsrc = GL_ZERO;
				break;
			case ONE_BLEND_SRC:
				iblendsrc = GL_ONE;
				break;
			case DST_COLOR_BLEND_SRC:
				iblendsrc = GL_DST_COLOR;
				break;
			case ONE_MINUS_DST_COLOR_BLEND_SRC:
				iblendsrc = GL_ONE_MINUS_DST_COLOR;
				break;	
			case SRC_ALPHA_BLEND_SRC:
				iblendsrc = GL_SRC_ALPHA;
				break;
			case ONE_MINUS_SRC_ALPHA_BLEND_SRC:
				iblendsrc = GL_ONE_MINUS_SRC_ALPHA;
				break;
			case DST_ALPHA_BLEND_SRC:
				iblendsrc = GL_DST_ALPHA;
				break;
			case ONE_MINUS_DST_ALPHA_BLEND_SRC:
				iblendsrc = GL_ONE_MINUS_DST_ALPHA;
				break;
			case SRC_ALPHA_SATURATE_BLEND_SRC:
				iblendsrc = GL_SRC_ALPHA_SATURATE;
				break;
			default:
				iblendsrc = GL_ONE;
		}
	
		//setup dst blend factor	
		switch(pkSettings->m_iBlendDst)
		{
			case ZERO_BLEND_DST:
				iblenddst = GL_ZERO;
				break;
			case ONE_BLEND_DST:
				iblenddst = GL_ONE;
				break;
			case SRC_COLOR_BLEND_DST:
				iblenddst = GL_SRC_COLOR;
				break;
			case ONE_MINUS_SRC_COLOR_BLEND_DST:
				iblenddst = GL_ONE_MINUS_SRC_COLOR;
				break;	
			case SRC_ALPHA_BLEND_DST:
				iblenddst = GL_SRC_ALPHA;
				break;
			case ONE_MINUS_SRC_ALPHA_BLEND_DST:
				iblenddst = GL_ONE_MINUS_SRC_ALPHA;
				break;
			case DST_ALPHA_BLEND_DST:
				iblenddst = GL_DST_ALPHA;
				break;
			case ONE_MINUS_DST_ALPHA_BLEND_DST:
				iblenddst = GL_ONE_MINUS_DST_ALPHA;
				break;
			default:
				iblenddst = GL_ZERO;
		}
	
		//finally set opengl blend function
		glBlendFunc(iblendsrc, iblenddst);
	}
	else if(	m_iForceBledning == BLEND_FORCE_TRANSPARENT)		//force transparent blend
	{
		glEnable(GL_BLEND);
		glBlendFunc( GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1,1,1,0.3);

		//float black[4]={0.0,0.0,0.0,0.0};
		//glMaterialfv(GL_FRONT,GL_AMBIENT,black);
		//glMaterialfv(GL_FRONT,GL_SPECULAR,black);
		glEnable(GL_COLOR_MATERIAL);				
	}
	
	
	//lighting setting
	if(m_iForceLighting == LIGHT_MATERIAL)
	{
		if(pkSettings->m_bLighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
	}
	else if(m_iForceLighting == LIGHT_ALWAYS_ON)
		glEnable(GL_LIGHTING);
	else if(m_iForceLighting == LIGHT_ALWAYS_OFF)
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
		glAlphaFunc(GL_GEQUAL, 0.1);
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

	//setup vertex program
	SetupVertexProgram(pkSettings);

	//setup fragment program
	SetupFragmentProgram(pkSettings);

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
	delete []m_pkVertexPointer;	
	delete []m_pkNormalPointer;		
	delete []m_pkTexturePointer0;		
	delete []m_pkTexturePointer1;			
	delete []m_pkTexturePointer2;			
	delete []m_pkTexturePointer3;			
	delete []m_pkIndexPointer;		
	delete []m_pkColorPointer;		
	
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

	//glMultMatrixf( &m_kModelMatrix.data[0] );

	SetupPrerenderStates();

	//we have to reset opengls data pointers
	SetupClientStates();

	//go trough all passes of material
	for(int i=0;i<m_pkCurentMaterial->GetNrOfPasses();i++)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		SetupRenderStates(m_pkCurentMaterial->GetPass(i));

		if(m_pkIndexPointer)
			glDrawElements(m_iDrawMode,m_iNrOfIndexes,GL_UNSIGNED_INT,m_pkIndexPointer);
		else
			glDrawArrays(m_iDrawMode,0,m_iNrOfVertexs);


		glPopAttrib();
	}

	if(m_bCopyedData)
		CleanCopyedData();

	glPopMatrix();


	//strange reason this has to be here, push_attrib does not take it =(
	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
}


bool ZShader::HaveVertexProgramExt()
{
	unsigned char* pcExt = const_cast<unsigned char*>(glGetString(GL_EXTENSIONS));		

	if(strstr((const char*)pcExt,"GL_ARB_vertex_program") != NULL)
	{
		return true;	
	}
	
	return false;
}

bool ZShader::HaveFragmentProgramExt()
{
	unsigned char* pcExt = const_cast<unsigned char*>(glGetString(GL_EXTENSIONS));		
	
	if(strstr((const char*)pcExt,"GL_ARB_fragment_program") != NULL)
	{
		return true;	
	}
	
	return false;
}


void ZShader::SetVertexProgram(int iVPID)
{
		
	if(iVPID == NO_VPROGRAM)
	{
		glDisable(GL_VERTEX_PROGRAM_ARB);
	}
	else
	{
		glEnable(GL_VERTEX_PROGRAM_ARB);

		if(m_iCurrentVertexProgram != iVPID)
			glBindProgramARB(GL_VERTEX_PROGRAM_ARB, iVPID);
	}


	m_iCurrentVertexProgram=iVPID;
}

void ZShader::SetFragmentProgram(int iFPID)
{
    
	if(iFPID == NO_FPROGRAM)
	{
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
	}
	else 
	{
		glEnable(GL_FRAGMENT_PROGRAM_ARB);

		if(m_iCurrentFragmentProgram != iFPID)
			glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, iFPID);
	}


	m_iCurrentFragmentProgram=iFPID;

}

void ZShader::UpdateFragmentProgramParameters()
{
	if(m_iCurrentFragmentProgram != NO_FPROGRAM)
	{
		float fTime = (float(SDL_GetTicks()) /1000.0);		
		
		// parameter 0  (time,time,0,0)
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,0,
											fTime	, fTime, 0 ,0);

		// parameter 1  (time,time,time,time)
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,1,
											fTime	, fTime, fTime ,fTime);
	
											
		// parameter 2  (sin(time),sin(time),sin(time),sin(time))
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,2,
											sin(fTime)	, sin(fTime), sin(fTime) ,sin(fTime));
																																		
	}
}

// SOFTWARE SHADERS
void ZShader::TextureOffset()
{
	if(m_pkTexturePointer0)
	{
		for(int i=0;i<m_iNrOfVertexs;i++)
		{
			m_pkTexturePointer0[i].x += SDL_GetTicks() * m_pkCurentMaterial->m_faTextureOffset[0];
			m_pkTexturePointer0[i].y += SDL_GetTicks() * m_pkCurentMaterial->m_faTextureOffset[1];
		}
	}
}

void ZShader::RandomVertexMovements()
{
	for(int i=0;i<m_iNrOfVertexs;i++)
	{
		m_pkVertexPointer[i] += Vector3( float((rand()%500 /1000.0) -0.25), 
			float((rand()%500 /1000.0) -0.25),
			float((rand()%500 /1000.0) -0.25));

	}
	
}

void ZShader::Waves()
{
	for(int i=0;i<m_iNrOfVertexs;i++)
	{
		float offset = Clamp(m_pkVertexPointer[i].x + m_pkVertexPointer[i].y + m_pkVertexPointer[i].z,0,4);
		float bla = float( cos(SDL_GetTicks()/500.0 + offset)*0.1 );
		m_pkVertexPointer[i] += Vector3(bla,bla,bla);

	}
}

void ZShader::ColorEffect(ZMaterialSettings* pkSettings)
{
	switch(pkSettings->m_iTextureColorEffect)
	{
		case 1:	
		{
			//float fV = ((rand() % 500)  + 500)  / 1000.0  ;			
			float fV = (rand() % 500)  / float(1000.0) + float(0.2);
 
			pkSettings->m_kVertexColor.x = fV;
			pkSettings->m_kVertexColor.y = fV;
			pkSettings->m_kVertexColor.z = fV;	
			break;
		}
 
		case 2:
		{

			float fV = sin((float)SDL_GetTicks() / float(300.0)) * float(0.25) + float(0.2);
			//float fV = ((rand() % 500)  + 500)  / 1000.0  ;

			pkSettings->m_kVertexColor.x = fV;
			pkSettings->m_kVertexColor.y = fV;
			pkSettings->m_kVertexColor.z = fV;				
		
			break;
		}
	}

}

// Matrix Stuff

void ZShader::MatrixPush()
{
	m_kModelMatrixStack.push(m_kModelMatrix);
}

void ZShader::MatrixPop()
{
	if(!m_kModelMatrixStack.empty())
	{
		m_kModelMatrix = m_kModelMatrixStack.top();
		m_kModelMatrixStack.pop();
	}
}
