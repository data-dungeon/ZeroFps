#include "zshadersystem.h"

ZShaderSystem::ZShaderSystem() : ZFSubSystem("ZShaderSystem")
{
	
};

bool ZShaderSystem::StartUp()
{
 	m_pkTexMan	= static_cast<TextureManager*>(GetSystem().GetObjectPtr("TextureManager"));
 	m_pkLight	= static_cast<Light*>(GetSystem().GetObjectPtr("Light"));

	SetupOpenGL();
	BindMaterial(NULL);
	
	m_iPushPop = 				0;
	
	m_iMaterialReloads = 	0;
	m_iMaterialBinds = 		0;
	m_iSavedReloads = 		0;
	m_iGLupdates =				0;
	m_iTotalVertises = 		0;
	
	m_bCopyedData =			false;
	
	m_bSupportVertexProgram = 		false;
	m_bSupportFragmentProgram = 	false;
	m_iCurrentVertexProgram = 		-1;
	m_iCurrentFragmentProgram = 	-1;
	
	m_bOcclusion = 					false;
	m_iOcQuery =						0;
	
	m_bSupportVertexBuffers =		false;
	
	//check for vertex and fragment program support
	if(!(m_bSupportVertexProgram = HaveExtension("GL_ARB_vertex_program")))
		cout<<"ZSHADER: No vertex program support"<<endl;
	if(!(m_bSupportFragmentProgram = HaveExtension("GL_ARB_fragment_program")))
		cout<<"ZSHADER: No fragment program support"<<endl;
	
	//check for vertexbuffer support
	if(!(m_bSupportVertexBuffers = HaveExtension("GL_ARB_vertex_buffer_object")))
		cout<<"ZSHADER: No vertexbuffer support"<<endl;
	
	//setup ucculusion
	if(!(m_bOcclusion = HaveExtension("GL_ARB_occlusion_query")))
		cout<<"ZSHADER: No Occlusion support"<<endl;
	else
		SetupOcculusion();
		
	return true;
}

bool ZShaderSystem::ShutDown()
{
	
	return true;
}

void ZShaderSystem::SetupOpenGL()
{
	//fog stuff
	glHint(GL_FOG_HINT,GL_NICEST);		
	glFogi(GL_FOG_MODE,GL_LINEAR);
	glFogi(FOG_DISTANCE_MODE_NV,EYE_RADIAL_NV);
	
	glEnable(GL_NORMALIZE);
	glEnable(GL_SCISSOR_TEST);

	glShadeModel(GL_SMOOTH);
	glClearColor(0,0,0,0);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	
	SetFog(Vector4(1,1,1,0),10,100,true);
	SetClearColor(Vector4(1,1,1,0));
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
	
	//disable gpu programs
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
	glDisable(GL_VERTEX_PROGRAM_ARB);
	
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




void ZShaderSystem::BindMaterial(ZMaterial* pkMaterial,bool bForceReload)
{
	if(pkMaterial == NULL)
	{
		m_pkCurrentMaterial = NULL;
		return;
	}
	
	m_iMaterialBinds++;
	
	//compare IDS of materials	if its the same , return
	if(!bForceReload)
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
	
	if(m_pk2DVertexPointer)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2,GL_FLOAT,0,m_pk2DVertexPointer);
	}	
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
	
	
	//material light propertys
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,	&pkSettings->m_kMatAmbient.x);	
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,	&pkSettings->m_kMatDiffuse.x);	
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,	&pkSettings->m_kMatSpecular.x);
	glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,	&pkSettings->m_kMatEmission.x);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,	&pkSettings->m_fShininess);
	
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

	
	//enable/disable stenciltest
   if ( pkSettings->m_bStencilTest )
	{
		//enable stencil test
      glEnable (GL_STENCIL_TEST);
		
		//setup stencil operation
		int iFail;		
		int iZFail;
		int iZPass;		
		switch(pkSettings->m_iStencilOpFail)
		{
			case STENCILOP_KEEP:
				iFail = GL_KEEP;
				break;
			case STENCILOP_ZERO:
				iFail = GL_ZERO;
				break;
			case STENCILOP_REPLACE:
				iFail = GL_REPLACE;
				break;
			case STENCILOP_INCR:
				iFail = GL_INCR;
				break;
			case STENCILOP_DECR:
				iFail = GL_DECR;
				break;		
			case STENCILOP_INVERT:
				iFail = GL_INVERT;
				break;				
		};

		switch(pkSettings->m_iStencilOpZFail)
		{
			case STENCILOP_KEEP:
				iZFail = GL_KEEP;
				break;
			case STENCILOP_ZERO:
				iZFail = GL_ZERO;
				break;
			case STENCILOP_REPLACE:
				iZFail = GL_REPLACE;
				break;
			case STENCILOP_INCR:
				iZFail = GL_INCR;
				break;
			case STENCILOP_DECR:
				iZFail = GL_DECR;
				break;		
			case STENCILOP_INVERT:
				iZFail = GL_INVERT;
				break;		
		};
		
		switch(pkSettings->m_iStencilOpZPass)
		{
			case STENCILOP_KEEP:
				iZPass = GL_KEEP;
				break;
			case STENCILOP_ZERO:
				iZPass = GL_ZERO;
				break;
			case STENCILOP_REPLACE:
				iZPass = GL_REPLACE;
				break;
			case STENCILOP_INCR:
				iZPass = GL_INCR;
				break;
			case STENCILOP_DECR:
				iZPass = GL_DECR;
				break;		
			case STENCILOP_INVERT:
				iZPass = GL_INVERT;
				break;					
		};
		
		glStencilOp(iFail,iZFail,iZPass);		
		
		//setup stencil function
		int iStencilFunc;
		switch(pkSettings->m_iStencilFunc)
		{
			case STENCILFUNC_NEVER:
				iStencilFunc = GL_NEVER;
				break;
			case STENCILFUNC_LESS:
				iStencilFunc = GL_LESS;
				break;
			case STENCILFUNC_LEQUAL:
				iStencilFunc = GL_LEQUAL;
				break;
			case STENCILFUNC_GREATER:
				iStencilFunc = GL_GREATER;
				break;
			case STENCILFUNC_GEQUAL:
				iStencilFunc = GL_GEQUAL;
				break;
			case STENCILFUNC_EQUAL:
				iStencilFunc = GL_EQUAL;
				break;
			case STENCILFUNC_NOTEQUAL:
				iStencilFunc = GL_NOTEQUAL;
				break;
			case STENCILFUNC_ALWAYS:
				iStencilFunc = GL_ALWAYS;
				break;
		
		};
		
		glStencilFunc(iStencilFunc,pkSettings->m_iStencilFuncRef,pkSettings->m_iStencilFuncMask);
		
	}
   else
      glDisable (GL_STENCIL_TEST);		
		
		
	//depthtest
   if ( pkSettings->m_bDepthTest )
	{      
		glEnable (GL_DEPTH_TEST);
	
		//depthfunction settings
		switch(pkSettings->m_iDepthFunc)
		{
			case DEPTHFUNC_NEVER:
				glDepthFunc(GL_NEVER);			
				break;
			case DEPTHFUNC_LESS:
				glDepthFunc(GL_LESS);			
				break;	
			case DEPTHFUNC_EQUAL:
				glDepthFunc(GL_EQUAL);			
				break;	
			case DEPTHFUNC_LEQUAL:
				glDepthFunc(GL_LEQUAL);			
				break;	
			case DEPTHFUNC_GREATER:
				glDepthFunc(GL_GREATER);			
				break;	
			case DEPTHFUNC_NOTEQUAL:
				glDepthFunc(GL_NOTEQUAL);			
				break;	
			case DEPTHFUNC_GEQUAL:
				glDepthFunc(GL_GEQUAL);			
				break;		
			case DEPTHFUNC_ALWAYS:
				glDepthFunc(GL_ALWAYS);			
				break;			
		}	
	}
   else
      glDisable (GL_DEPTH_TEST);
		
	
	//enable /disable blending
	if(pkSettings->m_bFog)
		glEnable(GL_FOG);
	else
		glDisable(GL_FOG);
	
	//enable/disable colormask
	if(pkSettings->m_bColorMask)
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	else
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//enable/disable depthmask
	if(pkSettings->m_bDepthMask)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);
			
	
	//enable /disable blending
	if(pkSettings->m_bBlend)
	{
		//enable blending
		glEnable(GL_BLEND);
		
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
	else
		glDisable(GL_BLEND);
		

	
	//lighting setting
	if(pkSettings->m_bLighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);	
		
	//cullface setting
	switch(pkSettings->m_iCullFace)
	{
		case CULL_FACE_NONE:
			glDisable(GL_CULL_FACE);
			break;
		case CULL_FACE_BACK:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case CULL_FACE_FRONT:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case CULL_FACE_ALL:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT_AND_BACK);
			break;
	}
		
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
	
	//setup vertex program
	SetupVertexProgram(pkSettings);

	//setup fragment program
	SetupFragmentProgram(pkSettings);		
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

	m_pk2DVertexPointer =		NULL;
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
		case VERTEX2D_POINTER:
			m_pk2DVertexPointer = (Vector3*)pkPointer;
			break;		
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
	
	//update shader parameters
	UpdateFragmentProgramParameters();
	UpdateVertexProgramParameters();
	
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

void ZShaderSystem::DrawVertexBuffer(ZVertexBuffer* pkBuffer)
{
	if(m_pkCurrentMaterial == NULL)
	{
		cout<<"WARNING: no material bound"<<endl;
		return;
	}

	if(pkBuffer->m_iBufferID == 0)
	{
		cout<<"warning tried to draw incomplete vertex buffer"<<endl;
		return;
	}

	//reset all pointers
	ResetPointers();
	
	//disable all client states
	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_NORMAL_ARRAY);	
	glDisableClientState(GL_INDEX_ARRAY);	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_EDGE_FLAG_ARRAY);		

	//set vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER_ARB,pkBuffer->m_iBufferID);
	
			
	if(pkBuffer->m_iVertexType == VERTEXTYPE_3)
	{
		m_pkVertexPointer = 0;
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,m_pkVertexPointer);		
	}	

	//update shader parameters
	UpdateFragmentProgramParameters();
	UpdateVertexProgramParameters();

	
	if(m_pkCurrentMaterial->GetNrOfPasses() == 1)
	{
		SetupTUClientStates(0);
		
		if(m_pkIndexPointer)
		{
			m_iTotalVertises += m_iNrOfIndexes;
			glDrawElements(pkBuffer->m_iDrawMode,m_iNrOfIndexes,GL_UNSIGNED_INT,m_pkIndexPointer);
		}
		else
		{			
			m_iTotalVertises += pkBuffer->m_iVertises;
			glDrawArrays(pkBuffer->m_iDrawMode,0,pkBuffer->m_iVertises);
			//cout<<"bah"<<endl;
		}
	}
		
	glBindBuffer(GL_ARRAY_BUFFER_ARB,0);		
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
	if(!m_kVerties.empty())
	{
		SetPointer(VERTEX_POINTER,&m_kVerties[0]);
		SetNrOfVertexs(m_kVerties.size());
	}
	else if(!m_kVerties2D.empty())
	{
		SetPointer(VERTEX2D_POINTER,&m_kVerties2D[0]);
		SetNrOfVertexs(m_kVerties2D.size());
	}
	else
	{
		cout<<"WARNING: ZShaderSystem::DrawGeometry() called whiout any geometry"<<endl;
		return;
	}
	
	//normals
	if(!m_kNormals.empty())
		SetPointer(NORMAL_POINTER,&m_kNormals[0]);
		
	//setup texture cordinats
	if(!m_kTexture[0].empty())
		SetPointer(TEXTURE_POINTER0,&(m_kTexture[0])[0]);
	
	if(!m_kTexture[1].empty())
		SetPointer(TEXTURE_POINTER1,&(m_kTexture[1])[0]);
		
	if(!m_kTexture[2].empty())
		SetPointer(TEXTURE_POINTER2,&(m_kTexture[2])[0]);
		
	if(!m_kTexture[3].empty())
		SetPointer(TEXTURE_POINTER3,&(m_kTexture[3])[0]);
		
		
	
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
	
	m_pkBakup2DVertexPointer = m_pk2DVertexPointer;
	m_pkBakupVertexPointer = m_pkVertexPointer;
	m_pkBakupNormalPointer = m_pkNormalPointer;	
	m_pkBakupTexturePointer0 = m_pkTexturePointer0;
	m_pkBakupTexturePointer1 = m_pkTexturePointer1;		
	m_pkBakupTexturePointer2 = m_pkTexturePointer2;		
	m_pkBakupTexturePointer3 = m_pkTexturePointer3;		
	m_pkBakupIndexPointer = m_pkIndexPointer;
	m_pkBakupColorPointer = m_pkColorPointer;
	
	if(m_pk2DVertexPointer)
		CopyData((void**)&m_pk2DVertexPointer,sizeof(Vector2)*m_iNrOfVertexs);	
	
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
	m_kVerties2D.clear();
	m_kVerties.clear();
	m_kNormals.clear();
	m_kTexture[0].clear();
	m_kTexture[1].clear();
	m_kTexture[2].clear();
	m_kTexture[3].clear();
}


void ZShaderSystem::AddLineV(const Vector3& kPos1,const Vector3& kPos2)
{
	m_kVerties.push_back(kPos1);
	m_kVerties.push_back(kPos2);
}

void ZShaderSystem::AddLineV(const Vector2& kPos1,const Vector2& kPos2)
{
	m_kVerties2D.push_back(kPos1);
	m_kVerties2D.push_back(kPos2);
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

void ZShaderSystem::AddTriangleV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3)
{
	m_kVerties2D.push_back(kPos1);
	m_kVerties2D.push_back(kPos2);
	m_kVerties2D.push_back(kPos3);
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

void ZShaderSystem::AddQuadV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const Vector2& kPos4)
{
	m_kVerties2D.push_back(kPos1);
	m_kVerties2D.push_back(kPos2);
	m_kVerties2D.push_back(kPos3);
	m_kVerties2D.push_back(kPos4);
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
		float off = m_pkVertexPointer[i].x * m_pkVertexPointer[i].y * m_pkVertexPointer[i].z;
		float bla = float( sin((SDL_GetTicks()/200.0 + off )/10.0)*0.1 );		
		m_pkVertexPointer[i] += Vector3(bla,bla,bla);

	}
}

bool ZShaderSystem::HaveExtension(const string& strExt)
{
	unsigned char* pcExt = const_cast<unsigned char*>(glGetString(GL_EXTENSIONS));		

	if(strstr((const char*)pcExt,strExt.c_str()) != NULL)
	{
		return true;	
	}
	
	return false;
}

int ZShaderSystem::GetStencilBits()
{
	int iBits = 0;
	glGetIntegerv(GL_STENCIL_BITS, &iBits);
	
	return iBits;
}

void ZShaderSystem::UpdateFragmentProgramParameters()
{
	if(m_iCurrentFragmentProgram != -1)
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

void ZShaderSystem::UpdateVertexProgramParameters()
{
	if(m_iCurrentVertexProgram != -1)
	{
		//float fTime = (float(SDL_GetTicks()) /1000.0);		
																																				
	}
}

void ZShaderSystem::SetupVertexProgram(ZMaterialSettings* pkSettings)
{
	if(!m_bSupportVertexProgram)
		return;
	
	ZVProgram* pkRt = (ZVProgram*)pkSettings->m_pkVP->GetResourcePtr();
	
	if(!pkRt)
		SetVertexProgram(-1);
	else
		SetVertexProgram(pkRt->m_iId);		
}

void ZShaderSystem::SetupFragmentProgram(ZMaterialSettings* pkSettings)
{
	if(!m_bSupportFragmentProgram)
		return;

	ZFProgram* pkRt = (ZFProgram*)pkSettings->m_pkFP->GetResourcePtr();

	
	if(!pkRt)
		SetFragmentProgram(-1);
	else
		SetFragmentProgram(pkRt->m_iId);
			
}

void ZShaderSystem::SetVertexProgram(const int& iVPID)
{
		
	if(iVPID == -1)
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

void ZShaderSystem::SetFragmentProgram(const int& iFPID)
{
    
	if(iFPID == -1)
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


void ZShaderSystem::ClearBuffer(const int& iBuffer)
{
	switch(iBuffer)
	{
		case COLOR_BUFFER:
			glClear(GL_COLOR_BUFFER_BIT);					
			break;
		case DEPTH_BUFFER:
			glClear(GL_DEPTH_BUFFER_BIT);					
			break;
		case ACCUM_BUFFER:
			glClear(GL_ACCUM_BUFFER_BIT);						
			break;
		case STENCIL_BUFFER:			
			glClear(GL_STENCIL_BUFFER_BIT);			
			break;
	}
} 


void ZShaderSystem::MatrixSave(Matrix4* pkMatrix)
{
	int iMatrixMode;
	glGetIntegerv(GL_MATRIX_MODE,&iMatrixMode);
	
	switch(iMatrixMode)
	{
		case GL_MODELVIEW:
			glGetFloatv(GL_MODELVIEW_MATRIX,pkMatrix->data);
			return;
		case GL_PROJECTION:
			glGetFloatv(GL_PROJECTION_MATRIX,pkMatrix->data);
			return;
		case GL_TEXTURE:
			glGetFloatv(GL_TEXTURE_MATRIX,pkMatrix->data);
			return;	
		case GL_COLOR:
			glGetFloatv(GL_COLOR_MATRIX,pkMatrix->data);
			return;
			
		default:
			cout<<"ERROR: ZShaderSystem::MatrixSave : illigal matrix mode"<<endl;
			return;			
	}
}

void ZShaderSystem::MatrixLoad(const Matrix4* pkMatrix)
{
	glLoadMatrixf(pkMatrix->data);
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

ZVertexBuffer* ZShaderSystem::CreateVertexBuffer(const int& iDrawMode)
{
	SetDrawMode(iDrawMode);
	return CreateVertexBuffer();
}

ZVertexBuffer* ZShaderSystem::CreateVertexBuffer()
{
	if(!m_bSupportVertexBuffers)
		return NULL;

		
	//check if theres any vertises =D
	if(!(m_pk2DVertexPointer || m_pkVertexPointer))
		return NULL;

	//create new zvertexbuffer object
	ZVertexBuffer* pkNewBuffer = new ZVertexBuffer;

	//generate a new vertexbuffer
	glGenBuffersARB(1,&(pkNewBuffer->m_iBufferID));
	
	//bind the new buffer
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, pkNewBuffer->m_iBufferID);
	
	//save vertex data
	if(m_pkVertexPointer)
	{
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_iNrOfVertexs*sizeof(Vector3), m_pkVertexPointer, GL_STATIC_DRAW_ARB);
		pkNewBuffer->m_iVertexType = VERTEXTYPE_3;
	}
		
		
	//set drawmode
	pkNewBuffer->m_iDrawMode = m_iDrawMode;
	
	//set nr of vertiess
	pkNewBuffer->m_iVertises = m_iNrOfVertexs;
		
	//reset buffer binding
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
	
	//cout<<"+VB:"<<pkNewBuffer->m_iBufferID<<endl;
	
	return pkNewBuffer;
}

void ZShaderSystem::SetFog(const Vector4& kColor,float fStart,float fStop,bool bEnabled)
{
	if(bEnabled)
	{
		glEnable(GL_FOG);
		glFogfv(GL_FOG_COLOR,&kColor.x);
		glFogf(GL_FOG_START,fStart);
		glFogf(GL_FOG_END,fStop);	
	} 
	else 
	{
		glDisable(GL_FOG);
	}	
}


void ZShaderSystem::SetClearColor(const Vector4& kColor)
{
	glClearColor(kColor.x, kColor.y,kColor.z, kColor.w);
}

unsigned int ZShaderSystem::GetDepth(int iX,int iY)
{
	unsigned int iDepth;

	glReadPixels(iX,iY,1,1,GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,&iDepth);
	
	return iDepth;
}



void ZShaderSystem::SetupOcculusion()
{
	//generate one occulusion query
	glGenQueriesARB(1,&m_iOcQuery);
	
}

void ZShaderSystem::OcculusionBegin()
{
	if(!m_bOcclusion)
		return;

	glBeginQueryARB(GL_SAMPLES_PASSED_ARB, m_iOcQuery);
}

unsigned int ZShaderSystem::OcculusionEnd()
{
	if(!m_bOcclusion)
		return 0;

	glEndQueryARB(GL_SAMPLES_PASSED_ARB);
	
	unsigned int iResult;
	glGetQueryObjectuivARB(m_iOcQuery, GL_QUERY_RESULT_ARB,&iResult);
	
	return iResult;
}

