#include "zshadersystem.h"

ZShaderSystem::ZShaderSystem() : ZFSubSystem("ZShaderSystem")
{
	
};

bool ZShaderSystem::StartUp()
{
 	m_pkTexMan	= static_cast<TextureManager*>(GetSystem().GetObjectPtr("TextureManager"));
 	m_pkLight	= static_cast<Light*>(GetSystem().GetObjectPtr("Light"));

	BindMaterial(NULL);
	
	m_iPushPop = 				0;
	
	m_iMaterialReloads = 	0;
	m_iMaterialBinds = 		0;
	m_iSavedReloads = 		0;
	m_iGLupdates =				0;
	m_iTotalVertises = 		0;
	
	m_bCopyedData =			false;
	
	return true;
}

bool ZShaderSystem::ShutDown()
{
	
	return true;
}



void ZShaderSystem::Push(const char* czNote)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	//push matrises
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//disable all texture units
	glActiveTextureARB(GL_TEXTURE3_ARB);	
	glClientActiveTextureARB(GL_TEXTURE3_ARB);		
	glDisable(GL_TEXTURE_2D);	

	glActiveTextureARB(GL_TEXTURE2_ARB);	
	glClientActiveTextureARB(GL_TEXTURE2_ARB);		
	glDisable(GL_TEXTURE_2D);	

	glActiveTextureARB(GL_TEXTURE1_ARB);	
	glClientActiveTextureARB(GL_TEXTURE1_ARB);		
	glDisable(GL_TEXTURE_2D);	

	glActiveTextureARB(GL_TEXTURE0_ARB);	
	glClientActiveTextureARB(GL_TEXTURE0_ARB);		
	glEnable(GL_TEXTURE_2D);	
	
	
	m_iPushPop ++;		
}

void ZShaderSystem::Pop()
{
	if(m_iPushPop <= 0)
		return;
	
	glPopAttrib();
	
	//pop matrises
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


	ReloadMaterial();
	
	m_iPushPop--;
}

void ZShaderSystem::MatrixMode(const int& iMode)
{
	switch(iMode)
	{
		case MATRIX_MODE_MODEL:
			glMatrixMode(GL_MODELVIEW);
			return;
		case MATRIX_MODE_PROJECTION:
			glMatrixMode(GL_PROJECTION);
			return;
		case MATRIX_MODE_TEXTURE:
			glMatrixMode(GL_TEXTURE);
			return;
		case MATRIX_MODE_COLOR:
			glMatrixMode(GL_COLOR);
			return;	
	}
}


void ZShaderSystem::BindMaterial(ZMaterial* pkMaterial)
{
	if(pkMaterial == NULL)
	{
		m_pkCurrentMaterial = NULL;
		return;
	}
	
	m_iMaterialBinds++;
	
	//compare IDS of materials	if its the same , return
	if(m_pkCurrentMaterial != NULL)
		if(pkMaterial->m_iID == m_pkCurrentMaterial->m_iID)
		{
			m_iSavedReloads++;
			return;
		}

	//set corrent material
	m_pkCurrentMaterial = pkMaterial;

	//reload material
	ReloadMaterial();
}

void ZShaderSystem::ReloadMaterial()
{
	if(m_pkCurrentMaterial == NULL)
		return;
		
	m_iMaterialReloads++;

	//setup global material settings
	SetupGlobalSettings();

	if(m_pkCurrentMaterial->GetNrOfPasses() == 1)
		SetupPass(0);
		
}

void ZShaderSystem::SetupArrayClientStates()
{
	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_NORMAL_ARRAY);	
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


void ZShaderSystem::SetupPass(int iPass)
{
	if(m_pkCurrentMaterial->m_kPasses.size() < (iPass + 1))
		return;
		
	m_iGLupdates++;
		
	ZMaterialSettings* pkSettings = m_pkCurrentMaterial->m_kPasses[iPass];
	

	
	//enable color material if wanted
	if(pkSettings->m_bColorMaterial)
	{
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);		
		glEnable(GL_COLOR_MATERIAL);	
		glColor4fv(&(pkSettings->m_kVertexColor.x));		
		glDisable(GL_COLOR_MATERIAL);	
		
	}
	else
	{
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);		
		glEnable(GL_COLOR_MATERIAL);	
		glColor4f(1,1,1,1);		
		glDisable(GL_COLOR_MATERIAL);	
	}

	
		

	//line width
	glLineWidth(pkSettings->m_fLineWidth);

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

	//depthtest
   if ( pkSettings->m_bDepthTest )
      glEnable (GL_DEPTH_TEST);
   else
      glDisable (GL_DEPTH_TEST);
	
		
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
	if(pkSettings->m_bFog)
		glEnable(GL_FOG);
	else
		glDisable(GL_FOG);
	
	
	
	//enable /disable blending
	if(pkSettings->m_bBlend)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
		
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
		glAlphaFunc(GL_GEQUAL, 0.1);
	}
	else
		glDisable(GL_ALPHA_TEST);
		
	//setup tus
	for(int i = 3;i>=0;i--)
		SetupTU(pkSettings,i);
		

	//want TU 0 to be active when exiting

	
	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);			
	//setup vertex program
	//SetupVertexProgram(pkSettings);

	//setup fragment program
	//SetupFragmentProgram(pkSettings);		
}

void ZShaderSystem::SetupTU(ZMaterialSettings* pkSettings,int iTU)
{
	switch(iTU)
	{
		case 0:		
			glActiveTextureARB(GL_TEXTURE0_ARB);	
			break;
		case 1:		
			glActiveTextureARB(GL_TEXTURE1_ARB);	
			break;
		case 2:		
			glActiveTextureARB(GL_TEXTURE2_ARB);	
			break;
		case 3:		
			glActiveTextureARB(GL_TEXTURE3_ARB);	
			break;
		
		default:
			return;
	}
	

	if(pkSettings->m_kTUs[iTU]->IsValid())
	{		
		glEnable(GL_TEXTURE_2D);
		m_pkTexMan->BindTexture(((ResTexture*)pkSettings->m_kTUs[iTU]->GetResourcePtr())->m_iTextureID);
		
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	
			
		switch(pkSettings->m_iTUTexCords[iTU])
		{
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
		glDisable(GL_TEXTURE_2D);	
	}
}


void ZShaderSystem::SetupTUClientStates(const int& iPass)
{
	if(m_pkCurrentMaterial->m_kPasses.size() < (iPass + 1))
		return;
		
	ZMaterialSettings* pkSettings = m_pkCurrentMaterial->m_kPasses[iPass];	
	
	//setup tus
	for(int iTU = 3;iTU>=0;iTU--)
	{	
		switch(iTU)
		{
			case 3:
				glClientActiveTextureARB(GL_TEXTURE3_ARB);	
				break;
			case 2:
				glClientActiveTextureARB(GL_TEXTURE2_ARB);	
				break;
			case 1:
				glClientActiveTextureARB(GL_TEXTURE1_ARB);	
				break;
			case 0:
				glClientActiveTextureARB(GL_TEXTURE0_ARB);	
				break;
		}
	
		//no valid texutre
		if(!pkSettings->m_kTUs[iTU]->IsValid())
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			continue;
		}
		
		switch(pkSettings->m_iTUTexCords[iTU])
		{
			case CORDS_FROM_ARRAY_0:
				if(m_pkTexturePointer0)
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer0);						
				}
				break;						
			case CORDS_FROM_ARRAY_1:
				if(m_pkTexturePointer1)
				{			
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer1);						
				}
				break;			
			case CORDS_FROM_ARRAY_2:
				if(m_pkTexturePointer2)
				{			
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer2);						
				}
				break;			
			case CORDS_FROM_ARRAY_3:
				if(m_pkTexturePointer3)
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer3);						
				}					
				break;			
			
			default:
				//disable cord array
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				break;

		}
	}
}

void ZShaderSystem::SetupGlobalSettings()
{
/*
	if(m_pkCurentMaterial->m_bCopyData)
		CopyVertexData();		
		
	if(m_pkCurentMaterial->m_bTextureOffset)
		TextureOffset();

	if(m_pkCurentMaterial->m_bRandomMovements)
		RandomVertexMovements();

	if(m_pkCurentMaterial->m_bWaves)
		Waves();
*/		
}

void ZShaderSystem::ResetPointers()
{
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
}

void ZShaderSystem::SetPointer(int iType,void* pkPointer)
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


void ZShaderSystem::SetDrawMode(const int& iDrawMode)
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

void ZShaderSystem::DrawArray(const int& iDrawMode)
{	
	SetDrawMode(iDrawMode);
	DrawArray();
}

void ZShaderSystem::DrawArray()
{
	if(m_pkCurrentMaterial == NULL)
	{
		cout<<"WARNING: no material bound"<<endl;
		return;
	}

	//do software vertex transformations
	VertexTransform();
	
	//we have to reset opengls data pointers
	SetupArrayClientStates();
	
	if(m_pkCurrentMaterial->GetNrOfPasses() == 1)
	{
		SetupTUClientStates(0);
		
		if(m_pkIndexPointer)
		{
			m_iTotalVertises += m_iNrOfIndexes;
			glDrawElements(m_iDrawMode,m_iNrOfIndexes,GL_UNSIGNED_INT,m_pkIndexPointer);
		}
		else
		{
			m_iTotalVertises += m_iNrOfVertexs;
			glDrawArrays(m_iDrawMode,0,m_iNrOfVertexs);
		}
	}
	else
	{	
		//go trough all passes of material
		for(int i=0;i<m_pkCurrentMaterial->GetNrOfPasses();i++)
		{
			SetupPass(i);
			SetupTUClientStates(i);
	
			if(m_pkIndexPointer)
			{
				m_iTotalVertises += m_iNrOfIndexes;
				glDrawElements(m_iDrawMode,m_iNrOfIndexes,GL_UNSIGNED_INT,m_pkIndexPointer);
			}
			else
			{
				m_iTotalVertises += m_iNrOfVertexs;
				glDrawArrays(m_iDrawMode,0,m_iNrOfVertexs);
			}	
		}
	}
	
	//if vertextransform has done any vertex copies , delete the copys and restore pointers
	if(m_bCopyedData)
		CleanCopyedData();
}

void ZShaderSystem::DrawGeometry(const int& iDrawMode)
{	
	SetDrawMode(iDrawMode);
	DrawGeometry();
}

void ZShaderSystem::DrawGeometry()
{
	ResetPointers();

	
	//always set vertex pointer
	SetPointer(VERTEX_POINTER,&m_kVerties[0]);
	
	//normals
	if(!m_kNormals.empty())
		SetPointer(NORMAL_POINTER,&m_kNormals[0]);
		
	//setup texture cordinats
	if(!m_kTexture[0].empty())
	{
		if(m_kVerties.size() != m_kTexture[0].size())
			cout<<"error: tus and vertises size mismatch "<<m_kTexture[0].size()<< " "<<m_kVerties.size()<<endl;
		
		SetPointer(TEXTURE_POINTER0,&(m_kTexture[0])[0]);
	}
	
	if(!m_kTexture[1].empty())
		SetPointer(TEXTURE_POINTER1,&(m_kTexture[1])[0]);
		
	if(!m_kTexture[2].empty())
		SetPointer(TEXTURE_POINTER2,&(m_kTexture[2])[0]);
		
	if(!m_kTexture[3].empty())
		SetPointer(TEXTURE_POINTER3,&(m_kTexture[3])[0]);
		
		
	//set number of vertises
	SetNrOfVertexs(m_kVerties.size());
	
	//do the final drawing
	DrawArray();
}

void ZShaderSystem::VertexTransform()
{
	if(m_pkCurrentMaterial->m_bCopyData)
		CopyVertexData();		

	if(m_pkCurrentMaterial->m_bWaves)
		Waves();
}

void ZShaderSystem::CopyVertexData()
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

void ZShaderSystem::CopyData(void** pkData,int iSize)
{
	void* pkNewData = new char[iSize];
	memcpy(pkNewData,*pkData,iSize);
	
	*pkData = pkNewData;
	
}

void ZShaderSystem::CleanCopyedData()
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



void ZShaderSystem::ClearGeometry()
{
	m_kVerties.clear();
	m_kNormals.clear();
	m_kTexture[0].clear();
	m_kTexture[1].clear();
	m_kTexture[2].clear();
	m_kTexture[3].clear();
}

void ZShaderSystem::SetColorMask(const bool& bColorMask)
{
	if(bColorMask)
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	else
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
};

void ZShaderSystem::SetDepthMask(const bool& bDepthMask)
{
	if(bDepthMask)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);	
}

void ZShaderSystem::AddLineV(const Vector3& kPos1,const Vector3& kPos2)
{
	m_kVerties.push_back(kPos1);
	m_kVerties.push_back(kPos2);
}

void ZShaderSystem::AddLineN(const Vector3& kNormal1,const Vector3& kNormal2)
{
	m_kNormals.push_back(kNormal1);
	m_kNormals.push_back(kNormal2);
}

void ZShaderSystem::AddLineUV(const Vector2& kPos1,const Vector2& kPos2,const int& iTU)
{
	m_kTexture[iTU].push_back(kPos1);
	m_kTexture[iTU].push_back(kPos2);
}


void ZShaderSystem::AddTriangleV(const Vector3& kPos1,const Vector3& kPos2,const Vector3& kPos3)
{
	m_kVerties.push_back(kPos1);
	m_kVerties.push_back(kPos2);
	m_kVerties.push_back(kPos3);
}

void ZShaderSystem::AddTriangleN(const Vector3& kNormal1,const Vector3& kNormal2,const Vector3& kNormal3)
{
	m_kNormals.push_back(kNormal1);
	m_kNormals.push_back(kNormal2);
	m_kNormals.push_back(kNormal3);
}

void ZShaderSystem::AddTriangleUV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const int& iTU)
{
	m_kTexture[iTU].push_back(kPos1);
	m_kTexture[iTU].push_back(kPos2);
	m_kTexture[iTU].push_back(kPos3);
}

void ZShaderSystem::AddQuadV(const Vector3& kPos1,const Vector3& kPos2,const Vector3& kPos3,const Vector3& kPos4)
{
	m_kVerties.push_back(kPos1);
	m_kVerties.push_back(kPos2);
	m_kVerties.push_back(kPos3);
	m_kVerties.push_back(kPos4);
}

void ZShaderSystem::AddQuadN(const Vector3& kNormal1,const Vector3& kNormal2,const Vector3& kNormal3,const Vector3& kNormal4)
{
	m_kNormals.push_back(kNormal1);
	m_kNormals.push_back(kNormal2);
	m_kNormals.push_back(kNormal3);
	m_kNormals.push_back(kNormal4);
}

void ZShaderSystem:: AddQuadUV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const Vector2& kPos4,const int& iTU)
{
	m_kTexture[iTU].push_back(kPos1);
	m_kTexture[iTU].push_back(kPos2);
	m_kTexture[iTU].push_back(kPos3);
	m_kTexture[iTU].push_back(kPos4);
}


void ZShaderSystem::Waves()
{
	for(int i=0;i<m_iNrOfVertexs;i++)
	{
		//float offset = Clamp(m_pkVertexPointer[i].x + m_pkVertexPointer[i].y + m_pkVertexPointer[i].z,0,4);
		float off = m_pkVertexPointer[i].x ;
		float bla = float( sin((SDL_GetTicks()/200.0 + off )/10.0)*4.0 );		
		m_pkVertexPointer[i] += Vector3(0,bla,0);

	}
}
