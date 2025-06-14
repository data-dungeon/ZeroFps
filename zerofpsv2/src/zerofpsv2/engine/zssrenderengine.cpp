#include "zssrenderengine.h"
#include "render_plugins.h"

//initiate nextstateid
int		RenderState::m_iNextStateID = 0;


//------- PreRenderPlugin
PreRenderPlugin::PreRenderPlugin(const string& strName,int iOrder)
{
	m_strName = strName;
	m_iOrder = iOrder;
}

//------ PostRenderPlugin
PostRenderPlugin::PostRenderPlugin(const string& strName,int iOrder)
{
	m_strName = strName;
	m_iOrder = iOrder;
}

//------ RenderPlugin
RenderPlugin::RenderPlugin(const string& strName,int iOrder)
{
	m_strName = strName;
	m_iOrder = iOrder;
}


//------- RenderState

RenderState::RenderState()
{
	m_iStateID = m_iNextStateID++;

	m_eRenderTarget = RENDER_TARGET_SCREEN;
	
	m_kViewPortSize.Set(640,480);
	m_kViewPortPos.Set(0,0);
	
	m_iClearViewPort = COLOR_BUFFER|DEPTH_BUFFER;
	
	m_kCameraPosition.Set(0,0,0);
	m_kCameraRotation.Identity();
	m_kCameraProjectionMatrix.Identity();
	
	m_kClearColor.Set(0.5,0.5,0.5,1.0);
	m_kFogColor.Set(0.5,0.5,0.5,1.0);
	
	m_fFogNear 		= 10;
	m_fFogFar 		= 100;
	m_bFogEnabled	= false;
	m_fExposure		= 1.0;
	
	m_pkRoot			= NULL;
}

RenderState::~RenderState()
{
	for(int i = 0;i<m_kPreRenderPlugins.size();i++)
		delete m_kPreRenderPlugins[i];		
	
	for(int i = 0;i<m_kRenderPlugins.size();i++)
		delete m_kRenderPlugins[i];		
	
	for(int i = 0;i<m_kPostRenderPlugins.size();i++)
		delete m_kPostRenderPlugins[i];		
		
}

bool RenderState::AddPlugin(const string& strName)
{
	if(ZSSRenderEngine* pkRenderEngine = static_cast<ZSSRenderEngine*>(g_ZFObjSys.GetObjectPtr("ZSSRenderEngine")))
	{
		//create plugin
		if(Plugin* pkPlugin = pkRenderEngine->m_kPluginFactory.CreatePluginInstans(strName))
		{
			//check witch type of plugin it is
		
			if(dynamic_cast<PreRenderPlugin*>(pkPlugin))
			{
				//check if plugin already added
				for(vector<PreRenderPlugin*>::iterator it = m_kPreRenderPlugins.begin();it!=m_kPreRenderPlugins.end();it++)
					if((*it)->GetName() == static_cast<PreRenderPlugin*>(pkPlugin)->GetName())
					{
						cerr<<"WARNING: Plugin "<<((PreRenderPlugin*)pkPlugin)->GetName()<<" already added"<<endl;
						delete pkPlugin;
						return false;
					}

				//validate plugin
				if(!((PreRenderPlugin*)pkPlugin)->Validate(*this))
				{
						cerr<<"WARNING: Plugin "<<((PreRenderPlugin*)pkPlugin)->GetName()<<" did not validate"<<endl;
						delete pkPlugin;
						return false;				
				}

				//add plugin
				m_kPreRenderPlugins.push_back((PreRenderPlugin*)pkPlugin);
				
				//sort
				sort(m_kPreRenderPlugins.begin(),m_kPreRenderPlugins.end(),PreRenderSort);				
				
				return true;
			}
			
			if(dynamic_cast<RenderPlugin*>(pkPlugin))
			{
				//check if plugin already added
				for(vector<RenderPlugin*>::iterator it = m_kRenderPlugins.begin();it!=m_kRenderPlugins.end();it++)
					if((*it)->GetName() == static_cast<RenderPlugin*>(pkPlugin)->GetName())
					{
						cerr<<"WARNING: Plugin "<<((RenderPlugin*)pkPlugin)->GetName()<<" already added"<<endl;
						delete pkPlugin;
						return false;
					}
				
				//validate plugin
				if(!((RenderPlugin*)pkPlugin)->Validate(*this))
				{
						cerr<<"WARNING: Plugin "<<((RenderPlugin*)pkPlugin)->GetName()<<" did not validate"<<endl;
						delete pkPlugin;
						return false;				
				}				
				
				//add plugin
				m_kRenderPlugins.push_back((RenderPlugin*)pkPlugin);				
				
				//sort
				sort(m_kRenderPlugins.begin(),m_kRenderPlugins.end(),RenderSort);
				
				return true;
			}
			
			if(dynamic_cast<PostRenderPlugin*>(pkPlugin))
			{
				//check if plugin already added
				for(vector<PostRenderPlugin*>::iterator it = m_kPostRenderPlugins.begin();it!=m_kPostRenderPlugins.end();it++)
					if((*it)->GetName() == static_cast<PostRenderPlugin*>(pkPlugin)->GetName())
					{
						cerr<<"WARNING: Plugin "<<((PostRenderPlugin*)pkPlugin)->GetName()<<" already added"<<endl;
						delete pkPlugin;
						return false;
					}

				//validate plugin
				if(!((PostRenderPlugin*)pkPlugin)->Validate(*this))
				{
						cerr<<"WARNING: Plugin "<<((PostRenderPlugin*)pkPlugin)->GetName()<<" did not validate"<<endl;
						delete pkPlugin;
						return false;				
				}

				//add plugin			
				m_kPostRenderPlugins.push_back((PostRenderPlugin*)pkPlugin);
				
				//sort
				sort(m_kPostRenderPlugins.begin(),m_kPostRenderPlugins.end(),PostRenderSort);
				return true;
			}
			
			cerr<<"WARNING: plugin is of unkown type "<<strName<<endl;			
		}
	}

	cerr<<"WARNING: could not add plugin "<<strName<<endl;

	return false;
}

bool RenderState::RemovePlugin(const string& strName)
{
	for(vector<PreRenderPlugin*>::iterator it = m_kPreRenderPlugins.begin();it != m_kPreRenderPlugins.end(); it++)
		if( (*it)->GetName() == strName)
		{
			delete *it;
			m_kPreRenderPlugins.erase(it);
			return true;
		}

	for(vector<RenderPlugin*>::iterator it = m_kRenderPlugins.begin();it != m_kRenderPlugins.end(); it++)
		if( (*it)->GetName() == strName)
		{
			delete *it;
			m_kRenderPlugins.erase(it);
			return true;
		}

	for(vector<PostRenderPlugin*>::iterator it = m_kPostRenderPlugins.begin();it != m_kPostRenderPlugins.end(); it++)
		if( (*it)->GetName() == strName)
		{
			delete *it;
			m_kPostRenderPlugins.erase(it);
			return true;
		}
		
	return false;
}


Plugin* RenderState::GetPlugin(const string& strName)
{
	for(int i =0;i<m_kPreRenderPlugins.size();i++)
		if(m_kPreRenderPlugins[i]->GetName() == strName)
			return m_kPreRenderPlugins[i];

	for(int i =0;i<m_kRenderPlugins.size();i++)
		if(m_kRenderPlugins[i]->GetName() == strName)
			return m_kRenderPlugins[i];

	for(int i =0;i<m_kPostRenderPlugins.size();i++)
		if(m_kPostRenderPlugins[i]->GetName() == strName)
			return m_kPostRenderPlugins[i];

	return NULL;
}

vector<string> RenderState::GetPluginList()
{
	vector<string> kTemp;
	
	for(int i =0;i<m_kPreRenderPlugins.size();i++)
		kTemp.push_back(m_kPreRenderPlugins[i]->GetName());

	for(int i =0;i<m_kRenderPlugins.size();i++)
		kTemp.push_back(m_kRenderPlugins[i]->GetName());

	for(int i =0;i<m_kPostRenderPlugins.size();i++)
		kTemp.push_back(m_kPostRenderPlugins[i]->GetName());

	return kTemp;
}


//------ RenderPackage

RenderPackage::RenderPackage()
{
	m_pkProperty			=	NULL;

	m_pkMaterial			=	NULL;
	m_pkLightProfile		=	NULL;
	m_fRadius				=	0;
	m_kCenter				=	Vector3(0,0,0);
	
	m_kAABBMin.Set(0,0,0);
	m_kAABBMax.Set(0,0,0);
	
	m_bOcculusionTest		=	false;
	m_bBlendSort			=	true;
	m_pkOcculusionParent	=	NULL;
	
	m_bStatic				=	true;
	
	m_iAttachToEntityID	=	-1;
	m_strAttachToJointName=	"";
	
	m_kModelMatrix.Identity();
	
}


//-------- RenderEngine

ZSSRenderEngine::ZSSRenderEngine() : ZFSubSystem("ZSSRenderEngine")
{
	m_pkZShaderSystem = NULL;
	m_pkZeroFps			= NULL;
	
	m_kRenderEngineDebug.Register(this, "r_renderenginedebug","0");
}

bool ZSSRenderEngine::StartUp()
{
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(GetSystem().GetObjectPtr("ZShaderSystem"));
	m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));

	// REGISTER DEFAULT PLUGINS
	m_kPluginFactory.RegisterPlugin("Render",Create_DefaultRenderPlugin);
	m_kPluginFactory.RegisterPlugin("PreRender",Create_DefaultPreRenderPlugin);
	m_kPluginFactory.RegisterPlugin("HdrExposure",Create_ExposureCalculator);
	m_kPluginFactory.RegisterPlugin("Bloom",Create_BloomPostPlugin);
	m_kPluginFactory.RegisterPlugin("InterfaceRender",Create_InterfaceRender);
	m_kPluginFactory.RegisterPlugin("DebugRender",Create_DebugRenderPlugin);
	m_kPluginFactory.RegisterPlugin("Shadowmap",Create_ShadowmapPlugin);
	m_kPluginFactory.RegisterPlugin("DepthMapRender",Create_DepthMapRendererPlugin);
	m_kPluginFactory.RegisterPlugin("SkyRender",Create_SkyRender);
	m_kPluginFactory.RegisterPlugin("AttachToJoint",Create_AttachToJointPlugin);


	return true;
}

bool ZSSRenderEngine::ShutDown()
{

	return true;
}

bool ZSSRenderEngine::Render(RenderState& kRenderState)
{
	//validate renderstate
	if(m_kRenderEngineDebug.GetBool())
		if(!ValidateRenderState(kRenderState))
		{
			return false;
		}
	
	/*
		sets upp framebuffer  and clears it
	*/
	SetupFramebuffer(kRenderState);
	
	
	/* call pre render plugins
	
		collects render data and sets upp render parameters
	*/
	for(int i = 0;i<kRenderState.m_kPreRenderPlugins.size();i++)
	{	
		if(!kRenderState.m_kPreRenderPlugins[i]->Call(*this,kRenderState))
			cerr<<"WARNING: PreRenderPlugin "<<kRenderState.m_kPreRenderPlugins[i]->GetName()<<" failed"<<endl;
				
	}	
	

	/*call post render plugins
	
		Applys after effects like bloom and other framebuffer based effects
	*/
	for(int i = 0;i<kRenderState.m_kPostRenderPlugins.size();i++)
	{
		if(!kRenderState.m_kPostRenderPlugins[i]->Call(kRenderState))
			cerr<<"WARNING: PreRenderPlugin "<<kRenderState.m_kPreRenderPlugins[i]->GetName()<<" failed"<<endl;
	}	

	
	//clear parameters
	m_kParameters.clear();	
	
	return true;
}

bool ZSSRenderEngine::ValidateRenderState(const RenderState& kRenderState)
{

// 	if(kRenderState.m_kViewPortSize == Vector2(0,0)) return false;
	
	
// 	if(kRenderState.m_kPreRenderPlugins.empty())
// 	{
// 		cerr<<"ERROR: no pre render plugins in render state"<<endl;
// 		return false;
// 	}
// 	
// 	if(kRenderState.m_kRenderPlugins.empty()) 
// 	{
// 		cerr<<"ERROR: no render plugins in render state"<<endl;		
// 		return false;
// 	}
		
	return true;
}

bool ZSSRenderEngine::ValidateRenderPackages(const vector<RenderPackage*>& kRenderPackages)
{
	int iPackages = kRenderPackages.size();
	for(int i = 0;i<iPackages;i++)
	{
		const RenderPackage& kRenderPackage = *kRenderPackages[i];
	
		if(!kRenderPackage.m_pkMaterial)
		{	
			cerr<<"ERROR: missing material"<<endl;
			return false;		
		}
		
		if(kRenderPackage.m_pkMaterial->GetPass(0))
		{
			if(kRenderPackage.m_pkMaterial->GetPass(0)->m_bLighting && !kRenderPackage.m_pkLightProfile)
			{
				cerr<<"WARNING: lighting enabled on material "<<kRenderPackage.m_pkMaterial->GetName()<<"but no lightprofile in renderpackage"<<endl;
				return false;
			}					
		}
		
		
		if(!kRenderPackage.m_kMeshData.m_pkVBO && kRenderPackage.m_kMeshData.m_iNrOfDataElements == 0)
		{
				cerr<<"ERROR: num of elements in renderpackage is 0"<<endl;
				return false;		
		}
				
		if(!kRenderPackage.m_kMeshData.m_pkVBO &&
			kRenderPackage.m_kMeshData.m_kDataPointers.empty() && 
			kRenderPackage.m_kMeshData.m_kVertises.empty() &&
			kRenderPackage.m_kMeshData.m_kVertises2D.empty())
		{
				cerr<<"WARNING: missing vertex array"<<endl;
				return false;		
		}
		
		if(kRenderPackage.m_pkOcculusionParent)
			if(kRenderPackage.m_pkOcculusionParent == &kRenderPackage)
			{
				cerr<<"WARNING: occulusion parent set to self"<<endl;
				return false;
			}
	}
		
		
	return true;
}



void ZSSRenderEngine::SetupView(RenderState& kRenderState)
{
	//load projection matrix
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_PROJECTION);
	m_pkZShaderSystem->MatrixLoad(&kRenderState.m_kCameraProjectionMatrix);
	
	//reset modelview matrix and setup the newone
	m_pkZShaderSystem->MatrixMode(MATRIX_MODE_MODEL);
  	m_pkZShaderSystem->MatrixLoad(&kRenderState.m_kCameraRotation);		
			
	//do camera translation
	m_pkZShaderSystem->MatrixTranslate(-kRenderState.m_kCameraPosition);	 	

	static Matrix4 kModelViewMatrix;
	m_pkZShaderSystem->MatrixSave(&kModelViewMatrix);			
			
	//setup frustum
	kRenderState.m_kFrustum.GetFrustum(kRenderState.m_kCameraProjectionMatrix,kModelViewMatrix);

	//set eye position in shadersystem
	m_pkZShaderSystem->SetEyePosition(kRenderState.m_kCameraPosition);	

}

void ZSSRenderEngine::SetupFramebuffer(const RenderState& kRenderState)
{
	//setup viewport
	glScissor(	GLint(kRenderState.m_kViewPortPos.x), 		GLint(kRenderState.m_kViewPortPos.y),	
					GLsizei(kRenderState.m_kViewPortSize.x), 	GLsizei(kRenderState.m_kViewPortSize.y) );					
	glViewport(	GLint(kRenderState.m_kViewPortPos.x), 		GLint(kRenderState.m_kViewPortPos.y),	
					GLsizei(kRenderState.m_kViewPortSize.x), 	GLsizei(kRenderState.m_kViewPortSize.y) );					
	
	//last but not least setup fog and clear color
	m_pkZShaderSystem->SetClearColor(kRenderState.m_kClearColor);
	m_pkZShaderSystem->SetFog(kRenderState.m_kFogColor,kRenderState.m_fFogNear,kRenderState.m_fFogFar,kRenderState.m_bFogEnabled);
	
	//set exposure
	m_pkZShaderSystem->SetExposure(kRenderState.m_fExposure);
	
	
	m_pkZShaderSystem->ClearBuffer(kRenderState.m_iClearViewPort);
}


void ZSSRenderEngine::DoRender(const vector<RenderPackage*>&	kRenderPackages,const RenderState& kRenderState)
{
	//validate package if debug is enabled
	if(m_kRenderEngineDebug.GetBool())
		if(!ValidateRenderPackages(kRenderPackages))
			return;

	//draw all packages
	int iRenderPlugins = kRenderState.m_kRenderPlugins.size();
	int iPackages = kRenderPackages.size();
		
	for(int iR = 0 ; iR < iRenderPlugins;iR++)
	{	
		for(int i = 0;i<iPackages;i++)
		{
			kRenderState.m_kRenderPlugins[iR]->Call(*this,*kRenderPackages[i],kRenderState);
		}
	}	
}

void  ZSSRenderEngine::SetParameter(const string& strName,void* pkValue)
{
	m_kParameters[strName] = pkValue;
}

void* ZSSRenderEngine::GetParameter(const string& strName)
{
	static map<string,void*>::iterator it;
	
	it = m_kParameters.find(strName);
	if(it != m_kParameters.end())
		return (*it).second;

	return NULL;
}




bool ZSSRenderEngine::OcculusionTest(RenderPackage& kRenderPackage, const RenderState& kRenderState)
{
	//do occulusion test
	if(m_pkZeroFps->GetOcculusionCulling() && kRenderPackage.m_bOcculusionTest)
	{
		//test if camera is inside the object
		if(!(	kRenderState.m_kCameraPosition > (kRenderPackage.m_kAABBMin + kRenderPackage.m_kCenter) &&
				kRenderState.m_kCameraPosition < (kRenderPackage.m_kAABBMax + kRenderPackage.m_kCenter)) )
 		{	

			//if we have an occulusion parent test parent
			if(kRenderPackage.m_pkOcculusionParent)
			{
				if(!kRenderPackage.m_pkOcculusionParent->m_kOcculusionTest[kRenderState.GetStateID()].GetCurrentResult())
				{	
					return false;									
				}			
			}
			
			//else test this object
			else if(!kRenderPackage.m_kOcculusionTest[kRenderState.GetStateID()].Visible(	kRenderPackage.m_kAABBMin + kRenderPackage.m_kCenter,
																													kRenderPackage.m_kAABBMax + kRenderPackage.m_kCenter))
			{
				m_pkZeroFps->m_iOcculedObjects++;
				return false;					
			}
			else
				m_pkZeroFps->m_iNotOcculedObjects++;
 		}
	}	


	return true;
}

