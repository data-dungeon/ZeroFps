#include "zshadersystem.h"
#include "glslprogram.h"
#include "res_texture.h"
#include "zvertexbuffer.h"

ZShaderSystem::ZShaderSystem() : ZFSubSystem("ZShaderSystem")
{
	m_iPushPop 							=	0;	
	
	//statistics
	m_iMaterialReloads 				=	0;
	m_iMaterialBinds 					=	0;
	m_iSavedReloads 					=	0;
	m_iGLupdates 						=	0;
	m_iTotalVertises 					=	0;
	m_iRenderedVBOs 					=	0;
	
	m_bCopyedData 						=	false;
	
	m_bSupportFBO 						=	false;	
	m_bSupportVertexProgram 		=	false;
	m_bSupportFragmentProgram 		= 	false;
	
	m_iCurrentVertexProgram 		=	-1;
	m_iCurrentFragmentProgram 		= 	-1;	
	m_bSupportGLSLProgram 			=	false;
	m_iCurrentGLSLProgramID 		=	0;
	m_bForceDisableGLSL				=	false;
	m_bSupportVertexBuffers 		=	false;	
	m_pkDefaultGLSLProgram			=	NULL;
	m_bUseDefaultGLSLProgram 		= 	false;
	
	m_bOcclusion 						=	false;
	m_iOcQuery 							=	0;
	
	m_bFogSetting 						=	true;	
	m_eDrawMode							=	TRIANGLES_MODE;
	m_iGLDrawMode						=	GL_TRIANGLES;

	m_kEyePosition						=	Vector3(0,0,0);
	m_iShadowmapWidth					=	1024;
	m_iShadowmapHeight				=	1024;
		
	m_fExposure							= 1.0;
	m_bSupportHDR						= false;
		
	m_aiCurrentTextures[0]			= 0;
	m_aiCurrentTextures[1]			= 0;
	m_aiCurrentTextures[2]			= 0;
	m_aiCurrentTextures[3]			= 0;
	
	//force settings
	m_iForceColorMask = 	FORCE_DEFAULT;
	m_iForceAlphaTest =	FORCE_DEFAULT;
	m_iForceLighting	=	LIGHT_MATERIAL;
	m_bDisableTU3 		=	false;
	m_iForceBlend		=	BLEND_MATERIAL;
		
	
	//register console commands
	Register_Cmd("setgamma",FID_SETGAMMA);		
	
	m_kbUseGLSL.Register(this, 	"r_useglsl","0");
	m_kbUseHDR.Register(this, 		"r_usehdr",	"1");
	m_kfRedGamma.Register(this,	"r_gammar", "1");
	m_kfGreenGamma.Register(this,	"r_gammag", "1");
	m_kfBlueGamma.Register(this, 	"r_gammab", "1");
	
	m_kbSpecMap.Register(this, 	"r_specmap", "1", "");
	m_kbNormalMap.Register(this, 	"r_normalmap", "0", "");	
	
	
	
	//reset all pointers
	ResetPointers();
};

bool ZShaderSystem::StartUp()
{
	//register material script interface
	RegisterSI_Material();

 	m_pkTexMan	= static_cast<TextureManager*>(GetSystem().GetObjectPtr("TextureManager"));
 	m_pkLight	= static_cast<ZSSLight*>(GetSystem().GetObjectPtr("ZSSLight"));

	SetupOpenGL();
	BindMaterial(NULL);
	
	//create a default shader resource
	//m_pkDefaultGLSLProgram	= new ZFResourceHandle();	
	
	//check for vertex and fragment program support
	if(!(m_bSupportVertexProgram = HaveExtension("GL_ARB_vertex_program")))
		//Printf("ZSHADER: No vertex program support");
		cerr<<"ZSHADER: No vertex program support"<<endl;
	if(!(m_bSupportFragmentProgram = HaveExtension("GL_ARB_fragment_program")))
		//Printf("ZSHADER: No fragment program support");
		cerr<<"ZSHADER: No fragment program support"<<endl;
	
	//check for glsl support
	m_bSupportGLSLProgram  =HaveExtension("GL_ARB_shader_objects") &&
									HaveExtension("GL_ARB_shading_language_100") &&
									HaveExtension("GL_ARB_vertex_shader") &&
									HaveExtension("GL_ARB_fragment_shader") && m_kbUseGLSL.GetBool();
	
	if(!m_bSupportGLSLProgram)
		//Printf("ZSHADER: No GLSL program support");
		cerr<<"ZSHADER: No GLSL program support"<<endl;
											
	//check for framebuffer object support
	m_bSupportFBO = HaveExtension("GL_EXT_framebuffer_object");
	if(!m_bSupportFBO)
		//Printf("ZSHADER: No Framebuffer object support");
		cerr<<"ZSHADER: No Framebuffer object support"<<endl;
											
	//check for vertexbuffer support	
	if(!(m_bSupportVertexBuffers = HaveExtension("GL_ARB_vertex_buffer_object")))
		//Printf("ZSHADER: No vertexbuffer support");
		cerr<<"ZSHADER: No vertexbuffer support"<<endl;	
		
	//setup ucculusion
	if(!(m_bOcclusion = HaveExtension("GL_ARB_occlusion_query")))
		//Printf("ZSHADER: No Occlusion support");
		cerr<<"ZSHADER: No Occlusion support ( THIS IS BAD! ecspect none working occuluction culling and flares )"<<endl;
	else
		SetupOcculusion();
	
	m_bSupportARBTC = HaveExtension("ARB_texture_compression");
	if(!m_bSupportARBTC)
		//Printf("ZSHADER: No arb texturecompression support");
		cerr<<"ZSHADER: No arb texturecompression support"<<endl;
			
	//hdr requires glsl support
	m_bSupportHDR = m_bSupportGLSLProgram;
			
			
	//set gamma after its been loaded
	SetGamma(m_kfRedGamma.GetFloat(),m_kfGreenGamma.GetFloat(),m_kfBlueGamma.GetFloat());				
		
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

	if(m_bSupportGLSLProgram)
	{
		glUseProgramObjectARB(0);			
		m_iCurrentGLSLProgramID = 0 ;
	}
	
	//load empty material
	BindMaterial(&m_kEmptyMaterial);

	
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
	
	m_iPushPop--;
}




void ZShaderSystem::BindMaterial(const ZMaterial* pkMaterial,bool bForceReload)
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
	
	if(m_b2DVertexPointer)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2,GL_FLOAT,0,m_pk2DVertexPointer);
	}	
	if(m_bVertexPointer)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,m_pkVertexPointer);
	}	
	if(m_bNormalPointer)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,0,m_pkNormalPointer);		
	}
	if(m_bIndexPointer)
	{
		glEnableClientState(GL_INDEX_ARRAY);
		glIndexPointer(GL_INT,0,m_pkIndexPointer);
	}
	if(m_bColorPointer)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT,0,m_pkColorPointer);
	}
}


void ZShaderSystem::SetupPass(int iPass)
{
	if(m_pkCurrentMaterial->m_kPasses.size() <= iPass)
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
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);		
	glColor4fv(&(pkSettings->m_kVertexColor.x));		
	glEnable(GL_COLOR_MATERIAL);	

	if( !pkSettings->m_bColorMaterial )
		glDisable(GL_COLOR_MATERIAL);
		
	//line width
	glLineWidth(pkSettings->m_fLineWidth);
	glPointSize(pkSettings->m_fLineWidth);

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
		
	
	//this will disable fog if wanted, but it will not enable fog if the default setting is disabled
	if(pkSettings->m_bFog)
	{
		if(m_bFogSetting)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);
	}
	else
		glDisable(GL_FOG);
	
	//enable/disable colormask
	bool bColorMask = pkSettings->m_bColorMask;
	if(m_iForceColorMask == FORCE_DISABLE)
		bColorMask = false;
	else if(m_iForceColorMask == FORCE_ENABLE)
		bColorMask = true;		
	
	if(bColorMask)
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	else
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//enable/disable depthmask
	if(pkSettings->m_bDepthMask)
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);
			
	
	//enable /disable blending
	if( (pkSettings->m_bBlend && m_iForceBlend == BLEND_MATERIAL) || m_iForceBlend == BLEND_FORCE_TRANSPARENT)
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
	if(m_iForceAlphaTest == FORCE_ENABLE || (m_iForceAlphaTest == FORCE_DEFAULT && pkSettings->m_bAlphaTest))
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL, pkSettings->m_fAlphaTreshold);	
	}
	else if(m_iForceAlphaTest == FORCE_DISABLE || (m_iForceAlphaTest == FORCE_DEFAULT && !pkSettings->m_bAlphaTest))
	{
		glDisable(GL_ALPHA_TEST);
	
	}
	else if(m_iForceAlphaTest == FORCE_OTHER)
	{
 		glEnable(GL_ALPHA_TEST);
  		glAlphaFunc(GL_GREATER, 0.1);	
	}

		
	//setup tus
	for(int i = 3;i>=0;i--)
		SetupTU(pkSettings,i);
		

	//want TU 0 to be active when exiting
	
	//setup vertex program
	SetupVertexProgram(pkSettings);

	//setup fragment program
	SetupFragmentProgram(pkSettings);	
	
}

void ZShaderSystem::PushTexture()
{
	//get current texture unit
	GLint iTU = GL_TEXTURE0_ARB;
	int iTUID = 0;	
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB,&iTU);
	switch(iTU)
	{
		case GL_TEXTURE0_ARB:	iTUID = 0;	break;
		case GL_TEXTURE1_ARB:	iTUID = 1;	break;	
		case GL_TEXTURE2_ARB:	iTUID = 2;	break;
		case GL_TEXTURE3_ARB:	iTUID = 3;	break;
	}

	//push current texture
	m_kTextureStacks[iTUID].push(m_aiCurrentTextures[iTUID]);
}

void ZShaderSystem::PopTexture()
{
	//get current texture unit
	GLint iTU = GL_TEXTURE0_ARB;
	int iTUID = 0;	
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB,&iTU);
	switch(iTU)
	{
		case GL_TEXTURE0_ARB:	iTUID = 0;	break;
		case GL_TEXTURE1_ARB:	iTUID = 1;	break;	
		case GL_TEXTURE2_ARB:	iTUID = 2;	break;
		case GL_TEXTURE3_ARB:	iTUID = 3;	break;
	}

	if(m_kTextureStacks[iTUID].empty())
		return;

	//get top texture
	GLuint iTexture = m_kTextureStacks[iTUID].top();
	m_kTextureStacks[iTUID].pop();

	//bind texture if not bound already
	if(m_aiCurrentTextures[iTUID] != iTexture)
	{
		glBindTexture(GL_TEXTURE_2D,iTexture);
		m_aiCurrentTextures[iTUID] = iTexture;
	}
}

void ZShaderSystem::BindTexture(const ResTexture* pkTexture)
{
	//get texture id
	GLuint iGLTexID;	
	if(pkTexture)
	{
		iGLTexID = pkTexture->m_iOpenGLID;
	}
	else
		iGLTexID = 0;	

	//get current texture unit
	GLint iTU = GL_TEXTURE0_ARB;
	int iTUID = 0;	
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB,&iTU);
	switch(iTU)
	{
		case GL_TEXTURE0_ARB:	iTUID = 0;	break;
		case GL_TEXTURE1_ARB:	iTUID = 1;	break;	
		case GL_TEXTURE2_ARB:	iTUID = 2;	break;
		case GL_TEXTURE3_ARB:	iTUID = 3;	break;
	}

	if(m_aiCurrentTextures[iTUID] != iGLTexID)
	{
		glBindTexture(GL_TEXTURE_2D,iGLTexID);
		m_aiCurrentTextures[iTUID] = iGLTexID;
	}

}

void ZShaderSystem::SetupTU(ZMaterialSettings* pkSettings,int iTU)
{
	//reserved for shadow's 
	if(m_bDisableTU3)
		if(iTU == 3)
			return;
		

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
	

	if(pkSettings->m_pkTUs[iTU]->IsValid() )
	{		
		glEnable(GL_TEXTURE_2D);
		BindTexture((ResTexture*)pkSettings->m_pkTUs[iTU]->GetResourcePtr());
		
		switch(pkSettings->m_iTUTexEnvMode[iTU])
		{
			case TEXENV_MODE_MODULATE: glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);	break;
			case TEXENV_MODE_DECAL: 	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);		break;
			case TEXENV_MODE_BLEND: 	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND);		break;
			case TEXENV_MODE_REPLACE:	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);	break;
		}

			
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
			{
				static float s[] = {1,0,1,0};
				static float t[] = {0,1,1,0};

				s[0] = pkSettings->m_fTUTexGenScale[iTU];
				s[2] = pkSettings->m_fTUTexGenScale[iTU];
				t[1] = pkSettings->m_fTUTexGenScale[iTU];
				t[2] = pkSettings->m_fTUTexGenScale[iTU];							
			
				
				glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
 				glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);				
				glEnable(GL_TEXTURE_GEN_S);
 				glEnable(GL_TEXTURE_GEN_T);				

  				glTexGenfv(GL_S,GL_OBJECT_PLANE,s);				 								
  				glTexGenfv(GL_T,GL_OBJECT_PLANE,t);				 								
				
				break;			
			}
			
			default:	//if no automatic generation, make sure its disabled
				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
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
	if(m_pkCurrentMaterial->m_kPasses.size() <= iPass)
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
		if(!pkSettings->m_pkTUs[iTU]->IsValid())
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			continue;
		}
		
		switch(pkSettings->m_iTUTexCords[iTU])
		{
			case CORDS_FROM_ARRAY_0:
				if(m_bTexturePointer0)
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer0);						
				}
				else
				{
					cerr<<"ERROR: trying to use UV array 0 in tu "<<iTU<<" ,but pointer is missing"<<endl;
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				break;						
			case CORDS_FROM_ARRAY_1:
				if(m_bTexturePointer1)
				{			
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer1);						
				}
				else
				{
					cerr<<"ERROR: trying to use UV array 1 in tu "<<iTU<<" ,but pointer is missing"<<endl;
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}				
				break;			
			case CORDS_FROM_ARRAY_2:
				if(m_bTexturePointer2)
				{			
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer2);						
				}
				else
				{
					cerr<<"ERROR: trying to use UV array 2 in tu "<<iTU<<" ,but pointer is missing"<<endl;
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				break;			
			case CORDS_FROM_ARRAY_3:
				if(m_bTexturePointer3)
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2,GL_FLOAT,0,m_pkTexturePointer3);						
				}					
				else
				{
					cerr<<"ERROR: trying to use UV array 3 in tu "<<iTU<<" ,but pointer is missing"<<endl;				
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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

	//reset buffert pointers
	m_pk2DVertexPointer =		NULL;
	m_pkVertexPointer =			NULL;
	m_pkNormalPointer =			NULL;
	m_pkTexturePointer0 = 		NULL;
	m_pkTexturePointer1 = 		NULL;
	m_pkTexturePointer2 = 		NULL;
	m_pkTexturePointer3 = 		NULL;
	m_pkIndexPointer = 			NULL;
	m_pkColorPointer = 			NULL;
	m_pkTangentPointer =			NULL;
	m_pkBiTangentPointer =		NULL;
	
	//reset bakup buffert pointers
	m_pkBakup2DVertexPointer =	NULL;
	m_pkBakupVertexPointer =	NULL;
	m_pkBakupNormalPointer =	NULL;
	m_pkBakupTexturePointer0 = NULL;
	m_pkBakupTexturePointer1 = NULL;
	m_pkBakupTexturePointer2 = NULL;
	m_pkBakupTexturePointer3 = NULL;
	m_pkBakupIndexPointer = 	NULL;
	m_pkBakupColorPointer = 	NULL;	
	m_pkBakupTangentPointer =	NULL;
	m_pkBakupBiTangentPointer =NULL;
	
	
	m_b2DVertexPointer =			false;
	m_bVertexPointer =			false;
	m_bNormalPointer =			false;
	m_bTexturePointer0 = 		false;
	m_bTexturePointer1 = 		false;
	m_bTexturePointer2 = 		false;
	m_bTexturePointer3 = 		false;
	m_bIndexPointer = 			false;
	m_bColorPointer = 			false;	
	m_bTangentPointer = 			false;
	m_bBiTangentPointer =		false;
}

const void* ZShaderSystem::GetPointer(POINTER_TYPE eType)
{
	switch(eType)
	{
		case VERTEX2D_POINTER: 	return (void*)m_pk2DVertexPointer;
		case VERTEX_POINTER:		return (void*)m_pkVertexPointer;
		case NORMAL_POINTER: 	return (void*)m_pkNormalPointer;
		case TEXTURE_POINTER0: 	return (void*)m_pkTexturePointer0;
		case TEXTURE_POINTER1: 	return (void*)m_pkTexturePointer1;
		case TEXTURE_POINTER2: 	return (void*)m_pkTexturePointer2;
		case TEXTURE_POINTER3: 	return (void*)m_pkTexturePointer3;								
		case INDEX_POINTER:		return (void*)m_pkIndexPointer;
		case COLOR_POINTER: 		return (void*)m_pkColorPointer;
		case TANGENT_POINTER: 	return (void*)m_pkTangentPointer;
		case BITANGENT_POINTER: return (void*)m_pkBiTangentPointer;
										
		default:
			cerr<<"ERROR: ZShaderSystem::GetPointer , invalid pointer id "<<eType<<endl;
			break;
	}	

	return NULL;
}

void ZShaderSystem::SetPointer(POINTER_TYPE eType,const void* pkPointer)
{
	switch(eType)
	{
		case VERTEX2D_POINTER:
			m_pk2DVertexPointer = (Vector2*)pkPointer;
			m_b2DVertexPointer = true;
			break;		
		case VERTEX_POINTER:
			m_pkVertexPointer = (Vector3*)pkPointer;
			m_bVertexPointer = true;
			break;
		case NORMAL_POINTER:
			m_pkNormalPointer = (Vector3*)pkPointer;
			m_bNormalPointer = true;
			break;
		case TEXTURE_POINTER0:
			m_pkTexturePointer0 = (Vector2*)pkPointer;
			m_bTexturePointer0 = true;
			break;
		case TEXTURE_POINTER1:
			m_pkTexturePointer1 = (Vector2*)pkPointer;
			m_bTexturePointer1 = true;
			break;
		case TEXTURE_POINTER2:
			m_pkTexturePointer2 = (Vector2*)pkPointer;
			m_bTexturePointer2 = true;
			break;
		case TEXTURE_POINTER3:
			m_pkTexturePointer3 = (Vector2*)pkPointer;
			m_bTexturePointer3 = true;
			break;		
		case INDEX_POINTER:
			m_pkIndexPointer = (unsigned int*)pkPointer;
			m_bIndexPointer = true;
			break;
		case COLOR_POINTER:
			m_pkColorPointer = (Vector4*)pkPointer;
			m_bColorPointer = true;
			break;
		case TANGENT_POINTER:
			m_pkTangentPointer = (Vector3*)pkPointer;
			m_bTangentPointer = true;
			break;
		case BITANGENT_POINTER:
			m_pkBiTangentPointer = (Vector3*)pkPointer;
			m_bBiTangentPointer = true;
			break;
	}	
}


void ZShaderSystem::SetDrawMode(DRAW_MODE eDrawMode)
{
	m_eDrawMode = eDrawMode;

	switch(eDrawMode)
	{
		case POINTS_MODE:
			m_iGLDrawMode = GL_POINTS;
			break;
		case LINES_MODE:
			m_iGLDrawMode = GL_LINES;
			break;
		case LINESTRIP_MODE:
			m_iGLDrawMode = GL_LINE_STRIP;
			break;		
		case LINELOOP_MODE:
			m_iGLDrawMode = GL_LINE_LOOP;
			break;
		case TRIANGLES_MODE:
			m_iGLDrawMode = GL_TRIANGLES;
			break;
		case TRIANGLESTRIP_MODE:
			m_iGLDrawMode = GL_TRIANGLE_STRIP;
			break;
		case TRIANGLEFAN_MODE:
			m_iGLDrawMode = GL_TRIANGLE_FAN;
			break;
		case QUADS_MODE:
			m_iGLDrawMode = GL_QUADS;
			break;
		case QUADSTRIP_MODE:
			m_iGLDrawMode = GL_QUAD_STRIP;
			break;
		case POLYGON_MODE:
			m_iGLDrawMode = GL_POLYGON;
			break;	
	}

}

void ZShaderSystem::DrawArray(DRAW_MODE eDrawMode)
{	
	SetDrawMode(eDrawMode);
	DrawArray();
}

void ZShaderSystem::DrawArray()
{
	if(m_pkCurrentMaterial == NULL)
	{
		cout<<"WARNING: no material bound"<<endl;
		return;
	}

	StartProfileTimer("ZShader::DrawArray");	

	//do software vertex transformations
	VertexTransform();
	
	//update ARB shader parameters
	//UpdateFragmentProgramParameters();
	//UpdateVertexProgramParameters();
	
	//we have to reset opengls data pointers
	SetupArrayClientStates();
	
	if(m_pkCurrentMaterial->GetNrOfPasses() == 1)
	{						
		SetupTUClientStates(0);
			
		//setup glsl program
		SetupGLSLProgram(m_pkCurrentMaterial->GetPass(0) );
		
		//update glsl program parameter information if theres any glslprogram bound
// 		if(m_iCurrentGLSLProgramID != NO_GLSLPROGRAM)
			UpdateGLSLProgramParameters(0);
		
			
		if(m_bIndexPointer)
		{
			m_iTotalVertises += m_iNrOfIndexes;
			glDrawElements(m_iGLDrawMode,m_iNrOfIndexes,GL_UNSIGNED_INT,m_pkIndexPointer);
		}
		else
		{			
			m_iTotalVertises += m_iNrOfVertexs;
			glDrawArrays(m_iGLDrawMode,0,m_iNrOfVertexs);
		}
	}
	else
	{	
		//go trough all passes of material
		for(int i=0;i<m_pkCurrentMaterial->GetNrOfPasses();i++)
		{					
			//load pass settings
			SetupPass(i);
						
			SetupTUClientStates(i);
	
			
			//setup glsl program
			SetupGLSLProgram(m_pkCurrentMaterial->GetPass(i));
			
			//update glsl program parameter information if theres any glslprogram bound
// 			if(m_iCurrentGLSLProgramID != NO_GLSLPROGRAM)
				UpdateGLSLProgramParameters(i);
			
			
			if(m_bIndexPointer)
			{
				m_iTotalVertises += m_iNrOfIndexes;
				glDrawElements(m_iGLDrawMode,m_iNrOfIndexes,GL_UNSIGNED_INT,m_pkIndexPointer);
			}
			else
			{
				m_iTotalVertises += m_iNrOfVertexs;
				glDrawArrays(m_iGLDrawMode,0,m_iNrOfVertexs);
			}	
		}
	}
	
	//if vertextransform has done any vertex copies , delete the copys and restore pointers
 	if(m_bCopyedData)
 		CleanCopyedData();
 		
	StopProfileTimer("ZShader::DrawArray");	 		
}

void ZShaderSystem::DrawVertexBuffer(ZVertexBuffer* pkBuffer)
{
	if(pkBuffer == NULL)
	{
		cout<<"not a valid vertex buffer object"<<endl;
		return;
	
	}

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

	//set vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER_ARB,pkBuffer->m_iBufferID);


	//reset all pointers
	ResetPointers();
	
	
 	if(pkBuffer->m_b2DVertexPointer)
  		SetPointer(VERTEX2D_POINTER, (void*)pkBuffer->m_i2DVertexPointer);		
 	if(pkBuffer->m_bVertexPointer)
 		SetPointer(VERTEX_POINTER, (void*)pkBuffer->m_iVertexPointer);				
 	if(pkBuffer->m_bNormalPointer)
 		SetPointer(NORMAL_POINTER, (void*)pkBuffer->m_iNormalPointer);	
 	if(pkBuffer->m_bTexturePointer0)
 		SetPointer(TEXTURE_POINTER0, (void*)pkBuffer->m_iTexturePointer0);
 	if(pkBuffer->m_bTexturePointer1)
 		SetPointer(TEXTURE_POINTER1, (void*)pkBuffer->m_iTexturePointer1);		
 	if(pkBuffer->m_bTexturePointer2)
 		SetPointer(TEXTURE_POINTER2, (void*)pkBuffer->m_iTexturePointer2);		
 	if(pkBuffer->m_bTexturePointer3)
 		SetPointer(TEXTURE_POINTER3, (void*)pkBuffer->m_iTexturePointer3);	
  	if(pkBuffer->m_bColorPointer)
  		SetPointer(COLOR_POINTER, (void*)pkBuffer->m_iColorPointer);		
  	if(pkBuffer->m_bTangentPointer)
  		SetPointer(TANGENT_POINTER, (void*)pkBuffer->m_iTangentPointer);		
  	if(pkBuffer->m_bBiTangentPointer)
  		SetPointer(BITANGENT_POINTER, (void*)pkBuffer->m_iBiTangentPointer);		


		
 	if(pkBuffer->m_bIndexPointer)
	{
		//set index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB,pkBuffer->m_iIndexBufferID);		
 		SetPointer(INDEX_POINTER, 0);		
	}	


	SetNrOfVertexs(pkBuffer->m_iNrOfVertexs);	
	SetNrOfIndexes(pkBuffer->m_iNrOfIndexes);	
			
  	DrawArray(pkBuffer->m_eDrawMode);
 	glBindBuffer(GL_ARRAY_BUFFER_ARB,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
	
	
	m_iRenderedVBOs++;
}

void ZShaderSystem::DrawGeometry(DRAW_MODE eDrawMode)
{	
	SetDrawMode(eDrawMode);
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
	
	
	//tangents
	if(!m_kTangents.empty())
		SetPointer(TANGENT_POINTER,&m_kTangents[0]);
	
	//bitangents
	if(!m_kBiTangents.empty())
		SetPointer(BITANGENT_POINTER,&m_kBiTangents[0]);
		
	//colors
	if(!m_kColors.empty())
		SetPointer(COLOR_POINTER,&m_kColors[0]);
	
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
	if(m_pkCurrentMaterial->m_bTextureOffset)
		TextureOffset();		
}

void ZShaderSystem::CopyData(void** pkData,int iSize)
{
	void* pkNewData = new char[iSize];
	memcpy(pkNewData,*pkData,iSize);
	
	*pkData = pkNewData;	
}

void ZShaderSystem::CopyPointerData(int iPointer)
{
	m_bCopyedData = true;

	switch(iPointer)
	{
		case VERTEX_POINTER:
			m_pkBakupVertexPointer = m_pkVertexPointer;
			CopyData((void**)&m_pkVertexPointer,sizeof(Vector3)*m_iNrOfVertexs);
			break;
		case NORMAL_POINTER:
			m_pkBakupNormalPointer = m_pkNormalPointer;
			CopyData((void**)&m_pkNormalPointer,sizeof(Vector3)*m_iNrOfVertexs);
			break;			
		case TEXTURE_POINTER0:
			m_pkBakupTexturePointer0 = m_pkTexturePointer0;
			CopyData((void**)&m_pkTexturePointer0,sizeof(Vector2)*m_iNrOfVertexs);
			break;		
		case TEXTURE_POINTER1:
			m_pkBakupTexturePointer1 = m_pkTexturePointer1;
			CopyData((void**)&m_pkTexturePointer1,sizeof(Vector2)*m_iNrOfVertexs);
			break;	
		case TEXTURE_POINTER2:
			m_pkBakupTexturePointer2 = m_pkTexturePointer2;
			CopyData((void**)&m_pkTexturePointer2,sizeof(Vector2)*m_iNrOfVertexs);
			break;	
		case TEXTURE_POINTER3:
			m_pkBakupTexturePointer3 = m_pkTexturePointer3;
			CopyData((void**)&m_pkTexturePointer3,sizeof(Vector2)*m_iNrOfVertexs);
			break;	
		case INDEX_POINTER:
			m_pkBakupIndexPointer = m_pkIndexPointer;
			CopyData((void**)&m_pkIndexPointer,sizeof(unsigned int)*m_iNrOfIndexes);
			break;	
		case COLOR_POINTER:
			m_pkBakupColorPointer = m_pkColorPointer;
			CopyData((void**)&m_pkColorPointer,sizeof(Vector4)*m_iNrOfVertexs);
			break;	
		case VERTEX2D_POINTER:
			m_pkBakup2DVertexPointer = m_pk2DVertexPointer;
			CopyData((void**)&m_pk2DVertexPointer,sizeof(Vector2)*m_iNrOfVertexs);
			break;	
																								
	}
}

void ZShaderSystem::CleanCopyedData()
{
	if(m_pkBakup2DVertexPointer)
	{
		delete[] m_pk2DVertexPointer;
		m_pk2DVertexPointer =		m_pkBakup2DVertexPointer;
		m_pkBakup2DVertexPointer = NULL;
	}
	if(m_pkBakupVertexPointer)
	{
 		delete[] m_pkVertexPointer;
		m_pkVertexPointer =		m_pkBakupVertexPointer;
		m_pkBakupVertexPointer = NULL;
	}
	if(m_pkBakupNormalPointer)
	{
		delete[] m_pkNormalPointer;
		m_pkNormalPointer =		m_pkBakupNormalPointer;
		m_pkBakupNormalPointer = NULL;
	}	
	if(m_pkBakupTexturePointer0)
	{
		delete[] m_pkTexturePointer0;
		m_pkTexturePointer0 =		m_pkBakupTexturePointer0;
		m_pkBakupTexturePointer0 = NULL;
	}		
	if(m_pkBakupTexturePointer1)
	{
		delete[] m_pkTexturePointer1;
		m_pkTexturePointer1 =		m_pkBakupTexturePointer1;
		m_pkBakupTexturePointer1 = NULL;
	}
	if(m_pkBakupTexturePointer2)
	{
		delete[] m_pkTexturePointer2;
		m_pkTexturePointer2 =		m_pkBakupTexturePointer2;
		m_pkBakupTexturePointer2 = NULL;
	}		
	if(m_pkBakupTexturePointer3)
	{
		delete[] m_pkTexturePointer3;
		m_pkTexturePointer3 =		m_pkBakupTexturePointer3;
		m_pkBakupTexturePointer3 = NULL;
	}			
	if(m_pkBakupIndexPointer)
	{
		delete[] m_pkIndexPointer;
		m_pkIndexPointer =		m_pkBakupIndexPointer;
		m_pkBakupIndexPointer = NULL;
	}			
	if(m_pkBakupColorPointer)
	{
		delete[] m_pkColorPointer;
		m_pkColorPointer =		m_pkBakupColorPointer;
		m_pkBakupColorPointer = NULL;
	}				
	
		
	m_bCopyedData = false;
// 	delete []m_pk2DVertexPointer;	
// 	delete []m_pkVertexPointer;	
// 	delete []m_pkNormalPointer;		
// 	delete []m_pkTexturePointer0;		
// 	delete []m_pkTexturePointer1;			
// 	delete []m_pkTexturePointer2;			
// 	delete []m_pkTexturePointer3;			
// 	delete []m_pkIndexPointer;		
// 	delete []m_pkColorPointer;		
// 	
// 	m_pk2DVertexPointer =	m_pkBakup2DVertexPointer;	
// 	m_pkVertexPointer =		m_pkBakupVertexPointer;	
// 	m_pkNormalPointer =		m_pkBakupNormalPointer;		
// 	m_pkTexturePointer0 = 	m_pkBakupTexturePointer0;		
// 	m_pkTexturePointer1 = 	m_pkBakupTexturePointer1;			
// 	m_pkTexturePointer2 = 	m_pkBakupTexturePointer2;			
// 	m_pkTexturePointer3 = 	m_pkBakupTexturePointer3;			
// 	m_pkIndexPointer =		m_pkBakupIndexPointer;		
// 	m_pkColorPointer =		m_pkBakupColorPointer;
// 	
// 	m_bCopyedData = false;

}



void ZShaderSystem::ClearGeometry()
{
	m_kTangents.clear();
	m_kBiTangents.clear();
	m_kVerties2D.clear();
	m_kVerties.clear();
	m_kNormals.clear();
	m_kColors.clear();
	m_kTexture[0].clear();
	m_kTexture[1].clear();
	m_kTexture[2].clear();
	m_kTexture[3].clear();
}


void ZShaderSystem::AddPointV(const Vector3& kPos)
{
	m_kVerties.push_back(kPos);
}

void ZShaderSystem::AddPointN(const Vector3& kNormal)
{
	m_kNormals.push_back(kNormal);
}

void ZShaderSystem::AddPointC(const Vector4& kColor)
{
	m_kColors.push_back(kColor);
}

void ZShaderSystem::AddPointUV(const Vector2& kPos,const int& iTU)
{
	m_kTexture[iTU].push_back(kPos);
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

void ZShaderSystem::AddLineC(const Vector4& kColor1,const Vector4& kColor2)
{
	m_kColors.push_back(kColor1);
	m_kColors.push_back(kColor2);
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

void ZShaderSystem::AddTriangleC(const Vector4& kColor1,const Vector4& kColor2,const Vector4& kColor3)
{
	m_kColors.push_back(kColor1);
	m_kColors.push_back(kColor2);
	m_kColors.push_back(kColor3);
}

void ZShaderSystem::AddTriangleUV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const int& iTU)
{
	m_kTexture[iTU].push_back(kPos1);
	m_kTexture[iTU].push_back(kPos2);
	m_kTexture[iTU].push_back(kPos3);
}

void ZShaderSystem::AddTriangleT(const Vector3& kTangent1,const Vector3& kTangent2,const Vector3& kTangent3)
{
	m_kTangents.push_back(kTangent1);
	m_kTangents.push_back(kTangent2);
	m_kTangents.push_back(kTangent3);
}

void ZShaderSystem::AddTriangleBT(const Vector3& kTangent1,const Vector3& kTangent2,const Vector3& kTangent3)
{
	m_kBiTangents.push_back(kTangent1);
	m_kBiTangents.push_back(kTangent2);
	m_kBiTangents.push_back(kTangent3);
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

void ZShaderSystem::AddQuadC(const Vector4& kColor1,const Vector4& kColor2,const Vector4& kColor3,const Vector4& kColor4)
{
	m_kColors.push_back(kColor1);
	m_kColors.push_back(kColor2);
	m_kColors.push_back(kColor3);
	m_kColors.push_back(kColor4);
}

void ZShaderSystem:: AddQuadUV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const Vector2& kPos4,const int& iTU)
{
	m_kTexture[iTU].push_back(kPos1);
	m_kTexture[iTU].push_back(kPos2);
	m_kTexture[iTU].push_back(kPos3);
	m_kTexture[iTU].push_back(kPos4);
}


void ZShaderSystem::TextureOffset()
{	
	if(m_bTexturePointer0)
		CopyPointerData(TEXTURE_POINTER0);
	if(m_bTexturePointer1)
		CopyPointerData(TEXTURE_POINTER1);

	float fTicks = SDL_GetTicks() / 1000.0;

	for(int i=0;i<m_iNrOfVertexs;i++)
	{
		if(m_bTexturePointer0)
		{
			m_pkTexturePointer0[i].x += fTicks * m_pkCurrentMaterial->m_faTextureOffset[0];
			m_pkTexturePointer0[i].y += fTicks * m_pkCurrentMaterial->m_faTextureOffset[1];
		}
	
		if(m_bTexturePointer1)
		{
			m_pkTexturePointer1[i].x -= fTicks * m_pkCurrentMaterial->m_faTextureOffset[0];
			m_pkTexturePointer1[i].y -= fTicks * m_pkCurrentMaterial->m_faTextureOffset[1];	
		}
	
	}
}


bool ZShaderSystem::HaveExtension(const string& strExt)
{
	unsigned char* pcExt = const_cast<unsigned char*>(glGetString(GL_EXTENSIONS));		

	if(pcExt == NULL)
	{
		cout<<"ERROR: ZShaderSystem::HaveExtension : could not fetch extension list"<<endl;
		return false;	
	}
	
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

void ZShaderSystem::UpdateGLSLProgramParameters(int iPass)
{	
	//purge old vertex attribs	
	int iSize = m_kVertexAttribs.size();
	for(int i = 0;i<iSize;i++)
		glDisableVertexAttribArrayARB(m_kVertexAttribs[i]);
	m_kVertexAttribs.clear();	


	//IF theres no bound glslprogram skip rest
	if(m_iCurrentGLSLProgramID == NO_GLSLPROGRAM)
		return;

	ZMaterialSettings* pkSettings = m_pkCurrentMaterial->GetPass(iPass);

	//attribute for tangent lists
	GLuint iTangentArray = glGetAttribLocationARB(m_iCurrentGLSLProgramID,"g_kTangent");	
	if(m_bTangentPointer)
	{
		m_kVertexAttribs.push_back(iTangentArray);
		glEnableVertexAttribArrayARB(iTangentArray);
		glVertexAttribPointerARB(iTangentArray,3,GL_FLOAT,GL_FALSE,0,m_pkTangentPointer);
	}
	
	//attribute for bitangent lists
	GLuint iBiTangentArray= glGetAttribLocationARB(m_iCurrentGLSLProgramID,"g_kBiTangent");
	if(m_bBiTangentPointer)
	{
		m_kVertexAttribs.push_back(iBiTangentArray);
		glEnableVertexAttribArrayARB(iBiTangentArray);
		glVertexAttribPointerARB(iBiTangentArray,3,GL_FLOAT,GL_FALSE,0,m_pkBiTangentPointer);
	}
	


	//time
	glUniform1fARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_fTime"), (float(SDL_GetTicks()) /1000.0));

	//Textures
	glUniform1iARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_kTexture0") , 0);
	glUniform1iARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_kTexture1") , 1);
	glUniform1iARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_kTexture2") , 2);
	glUniform1iARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_kTexture3") , 3);

	
	//eye position
 	glUniform3fvARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_kEyePosition") ,1, &m_kEyePosition.x);
 	
 	//nr of active lights
//  	glUniform1iARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_iActiveLights") , m_pkLight->GetNrOfActiveLights());
 
 	//shadowmap quality	
 	glUniform1fARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_fShadowPixelWidth") , 1.0 / float(m_iShadowmapWidth) );
 	glUniform1fARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_fShadowPixelHeight") , 1.0 / float(m_iShadowmapHeight) );

 	//HDR exposure
 	if(!UseHDR()) m_fExposure = 1.0;
 	glUniform1fARB(glGetUniformLocationARB(m_iCurrentGLSLProgramID,"g_fExposure") , m_fExposure);

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

void ZShaderSystem::SetupGLSLProgram(ZMaterialSettings* pkSettings)
{
	if(!m_bSupportGLSLProgram)
		return;

	GLenum iProgram = 0;	
		
	if(!m_bForceDisableGLSL && pkSettings->m_bUseShader)
	{
		if(GLSLProgram* pkRt = (GLSLProgram*)pkSettings->m_pkSLP->GetResourcePtr())	
			iProgram = pkRt->m_iProgramIDs[m_pkLight->GetNrOfActiveLights()];
		else if(m_bUseDefaultGLSLProgram && m_pkDefaultGLSLProgram)
				if(GLSLProgram* pkRt = (GLSLProgram*)m_pkDefaultGLSLProgram->GetResourcePtr())
					iProgram = pkRt->m_iProgramIDs[m_pkLight->GetNrOfActiveLights()];
	}
		
		
 	if(iProgram == m_iCurrentGLSLProgramID)
 		return;
	
 		
//   	cout<<"bidning "<<iProgram<<"  "<<m_pkLight->GetNrOfActiveLights()<<endl;
 		
 	//clear gl errors before trying to bind the shader
	glGetError(); 		 		
 		
	//bind program
	glUseProgramObjectARB(iProgram);			
	
	//set current program
	m_iCurrentGLSLProgramID = iProgram;
	
	switch(glGetError())
	{
		case GL_INVALID_VALUE: cout<<"glUseProgramObjectARB: ivalid value"<<endl;break;
		case GL_INVALID_OPERATION: cout<<"glUseProgramObjectARB: ivalid operation"<<endl;break;
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
	if(iBuffer & COLOR_BUFFER)
		glClear(GL_COLOR_BUFFER_BIT);					
	
	if(iBuffer & DEPTH_BUFFER)
		glClear(GL_DEPTH_BUFFER_BIT);					
	
	if(iBuffer & ACCUM_BUFFER)
		glClear(GL_ACCUM_BUFFER_BIT);					

	if(iBuffer & STENCIL_BUFFER)
		glClear(GL_STENCIL_BUFFER_BIT);					

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

ZVertexBuffer* ZShaderSystem::CreateVertexBuffer(DRAW_MODE eDrawMode)
{
	SetDrawMode(eDrawMode);
	return CreateVertexBuffer();
}

ZVertexBuffer* ZShaderSystem::CreateVertexBuffer()
{
// 	return NULL;
	if(!m_bSupportVertexBuffers)
		return NULL;

		
	//check if theres any vertises =D
	if(!(m_b2DVertexPointer || m_bVertexPointer))
		return NULL;

	//create new zvertexbuffer object
	ZVertexBuffer* pkNewBuffer = new ZVertexBuffer;

	//generate a new vertexbuffer
	glGenBuffersARB(1,&(pkNewBuffer->m_iBufferID));
	
	//bind the new buffer
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, pkNewBuffer->m_iBufferID);

		
	//setup size of vbo
	unsigned int iSize =0;	
	
	if(m_b2DVertexPointer)
		iSize += m_iNrOfVertexs*sizeof(Vector2);	
	if(m_bVertexPointer)
		iSize += m_iNrOfVertexs*sizeof(Vector3);
	if(m_bNormalPointer)
		iSize += m_iNrOfVertexs*sizeof(Vector3);	
	if(m_bTexturePointer0)
		iSize += m_iNrOfVertexs*sizeof(Vector2);
	if(m_bTexturePointer1)
		iSize += m_iNrOfVertexs*sizeof(Vector2);
	if(m_bTexturePointer2)
		iSize += m_iNrOfVertexs*sizeof(Vector2);
	if(m_bTexturePointer3)
		iSize += m_iNrOfVertexs*sizeof(Vector2);
	if(m_bColorPointer)
		iSize += m_iNrOfVertexs*sizeof(Vector4);
	if(m_bTangentPointer)
		iSize += m_iNrOfVertexs*sizeof(Vector3);
	if(m_bBiTangentPointer)
		iSize += m_iNrOfVertexs*sizeof(Vector3);
				
			
	//allocate vbo
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, iSize, NULL, GL_STATIC_DRAW_ARB);	
	
	//upload data and save offsets
	unsigned int iPos = 0;		
	
	if(m_b2DVertexPointer)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector2),m_pk2DVertexPointer);		
		pkNewBuffer->m_i2DVertexPointer = iPos;
		pkNewBuffer->m_b2DVertexPointer = true;
		iPos += m_iNrOfVertexs*sizeof(Vector2);							
	}
	if(m_bVertexPointer)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector3),m_pkVertexPointer);		
		pkNewBuffer->m_iVertexPointer = iPos;
		pkNewBuffer->m_bVertexPointer = true;		
		iPos += m_iNrOfVertexs*sizeof(Vector3);			
	}
	if(m_bNormalPointer)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector3),m_pkNormalPointer);		
		pkNewBuffer->m_iNormalPointer = iPos;		
		pkNewBuffer->m_bNormalPointer = true;		
		iPos += m_iNrOfVertexs*sizeof(Vector3);			
	}
	if(m_bTexturePointer0)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector2),m_pkTexturePointer0);		
		pkNewBuffer->m_iTexturePointer0 = iPos;		
		pkNewBuffer->m_bTexturePointer0 = true;				
		iPos += m_iNrOfVertexs*sizeof(Vector2);			
	}	
	if(m_bTexturePointer1)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector2),m_pkTexturePointer1);		
		pkNewBuffer->m_iTexturePointer1 = iPos;	
		pkNewBuffer->m_bTexturePointer1 = true;				
		iPos += m_iNrOfVertexs*sizeof(Vector2);			
	}	
	if(m_bTexturePointer2)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector2),m_pkTexturePointer2);		
		pkNewBuffer->m_iTexturePointer2 = iPos;		
		pkNewBuffer->m_bTexturePointer2 = true;				
		iPos += m_iNrOfVertexs*sizeof(Vector2);			
	}	
	if(m_bTexturePointer3)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector2),m_pkTexturePointer3);		
		pkNewBuffer->m_iTexturePointer3 = iPos;	
		pkNewBuffer->m_bTexturePointer3 = true;				
		iPos += m_iNrOfVertexs*sizeof(Vector2);			
	}	
	if(m_bColorPointer)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector4),m_pkColorPointer);		
		pkNewBuffer->m_iColorPointer = iPos;		
		pkNewBuffer->m_bColorPointer = true;						
		iPos += m_iNrOfVertexs*sizeof(Vector4);			
	}		
	if(m_bTangentPointer)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector3),m_pkTangentPointer);		
		pkNewBuffer->m_iTangentPointer = iPos;		
		pkNewBuffer->m_bTangentPointer = true;						
		iPos += m_iNrOfVertexs*sizeof(Vector3);			
	}
	if(m_bBiTangentPointer)
	{
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,iPos,m_iNrOfVertexs*sizeof(Vector3),m_pkBiTangentPointer);		
		pkNewBuffer->m_iBiTangentPointer = iPos;		
		pkNewBuffer->m_bBiTangentPointer = true;						
		iPos += m_iNrOfVertexs*sizeof(Vector3);			
	}	
	
	//reset buffer binding
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

	
	//if indexed modell
 	if(m_bIndexPointer)
 	{
 		pkNewBuffer->m_bIndexPointer = true;			
		
		//generate a new vertexbuffer
		glGenBuffersARB(1,&(pkNewBuffer->m_iIndexBufferID));
		
		//bind the new buffer
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, pkNewBuffer->m_iIndexBufferID);

		//upload index data	
 		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,m_iNrOfIndexes*sizeof(unsigned int),m_pkIndexPointer,GL_STATIC_DRAW_ARB);		
		
		//reset buffer binding
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);		
 	}			
			
		
	//set drawmode
	pkNewBuffer->m_eDrawMode = m_eDrawMode;
	
	//set nr of vertiess
	pkNewBuffer->m_iNrOfVertexs = m_iNrOfVertexs;
	
	//set nr of indexes
	pkNewBuffer->m_iNrOfIndexes = m_iNrOfIndexes;
			
	
	
	return pkNewBuffer;
}

void ZShaderSystem::SetFog(const Vector4& kColor,float fStart,float fStop,bool bEnabled)
{
	if(bEnabled)
	{
		m_bFogSetting = true;		
		glEnable(GL_FOG);
		glFogfv(GL_FOG_COLOR,&kColor.x);
		glFogf(GL_FOG_START,fStart);
		glFogf(GL_FOG_END,fStop);	
	} 
	else 
	{
		m_bFogSetting = false;
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

bool ZShaderSystem::SetGamma(float fGamma)
{
	return SetGamma(fGamma,fGamma,fGamma);
}


bool ZShaderSystem::SetGamma(float fRed,float fGreen,float fBlue)
{
	if( SDL_SetGamma(fRed, fGreen,fBlue)  == -1)
		return false;
	else
	{
		m_kfRedGamma.SetFloat(fRed);
		m_kfGreenGamma.SetFloat(fGreen);
		m_kfBlueGamma.SetFloat(fBlue);		
	
		return true;
	}
}

void ZShaderSystem::RunCommand(int cmdid, const ConCommandLine* kCommand)
{
	switch(cmdid) 
	{
		case FID_SETGAMMA:
			if(kCommand->m_kSplitCommand.size() <= 1) 
				return;
																			
			float fGamma;
			fGamma = atof(kCommand->m_kSplitCommand[1].c_str());
			
			SetGamma(fGamma);				
			break;			
	}
}


