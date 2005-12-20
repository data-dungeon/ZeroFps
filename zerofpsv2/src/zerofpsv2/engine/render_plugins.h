#include "zssrenderengine.h"
#include "../render/light.h"
#include "entitymanager.h"
#include "zerofps.h"


class InterfaceRender : public PreRenderPlugin
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSLight*			m_pkLight;
		ZSSZeroFps*			m_pkZeroFps;
		Application*		m_pkApplication;
		ZSSEntityManager*	m_pkEntityManager;
	
		ZMaterial*			m_pkLine;	
		RenderPackage		m_kAxesIcon;
	
	public:
		InterfaceRender();
		~InterfaceRender();
	
		bool Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState);
};
Plugin* Create_InterfaceRender();



enum BonePointers
{
	BONEMATRIX_POINTER 	= 32,
	BONEINDEX_POINTER,
};		
		
class DebugRenderPlugin : public RenderPlugin
{	
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSLight*			m_pkLight;
		ZSSRender*			m_pkRender;
		ZSSZeroFps*			m_pkZeroFps;
		ZMaterial*			m_pkLineMaterial;
	
		void DoBoneTransformation(int iVertises,const DataPointer& kBoneMatrises,const DataPointer& kBoneIndexes,const Vector3** pkVertises,const Vector3** pkNormals);
	
	public:					
		DebugRenderPlugin();	
		~DebugRenderPlugin();
		
		bool Call(RenderPackage& kRenderPackage, const RenderState& kRenderState);
};
Plugin* Create_DebugRenderPlugin();
		
class DefaultRenderPlugin : public RenderPlugin
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSLight*			m_pkLight;
		ZSSZeroFps*			m_pkZeroFps;
	
		void DoBoneTransformation(const RenderPackage& kRenderPackage,const DataPointer& kBoneMatrises,const DataPointer& kBoneIndexes);
	
	public:
					
		DefaultRenderPlugin() : RenderPlugin("Render")
		{
			m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
			m_pkLight			= static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));
			m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
		}
	
		bool Call(RenderPackage& kRenderPackage, const RenderState& kRenderState);
};
Plugin* Create_DefaultRenderPlugin();



class DefaultPreRenderPlugin : public PreRenderPlugin
{	
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSEntityManager*	m_pkEntityManager;
		ZSSZeroFps*			m_pkZeroFps;

		//shadow shaders		
		ZFResourceHandle* m_pkDefaultShader;
		ZFResourceHandle* m_pkDefaultShaderPPL;
		ZFResourceHandle* m_pkDefaultFastShadowmapShader;		

		//sort funktion for renderpackage sorting
		struct PackageSort : public binary_function<RenderPackage*, RenderPackage*, bool> 
		{
			bool operator()(RenderPackage* x, RenderPackage* y) 
			{ 		
				int iBlend = 	int(x->m_pkMaterial->GetPass(0)->m_bBlend && x->m_bBlendSort) + 
									int(y->m_pkMaterial->GetPass(0)->m_bBlend && y->m_bBlendSort)*2;
								
				switch(iBlend)
				{
					case 3:return x->m_fDistanceToCamera > y->m_fDistanceToCamera; 
					case 1:return false;
					case 2:return true;
					case 0:return x->m_fDistanceToCamera < y->m_fDistanceToCamera;
				}
				
				return false;
			};
		} Less_RenderPackage;
		
	public:
		DefaultPreRenderPlugin();
		
		bool Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState);
		
};
Plugin* Create_DefaultPreRenderPlugin();

class ExposureCalculator : public PostRenderPlugin
{	
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSRender*			m_pkRender;
		ZSSZeroFps*			m_pkZeroFps;
			
		float					m_fExposureFactor;		
		int 					m_iFSSTextureWidth;
		int 					m_iFSSTextureHeight;
		ResTexture* 		m_pkTex;
		
		float GetExposureFactor(const RenderState& kRenderState);			
		int   GetMinSize(int iRes);
	public:
		ExposureCalculator();
		~ExposureCalculator();
		
		bool Call(const RenderState& kRenderState);

		
};
Plugin* Create_ExposureCalculator();

class BloomPostPlugin : public PostRenderPlugin
{	
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSRender*			m_pkRender;
	
		int 			m_iFSSTextureWidth;
		int 			m_iFSSTextureHeight;
		Matrix4		m_kFSSProjMatrix;
		
		ZMaterial*	m_pkBloomMaterial1;
		ZMaterial*	m_pkBloomMaterial2;
		
		ResTexture	m_kBloomTexture;
		ResTexture	m_kBloomTexture2;
		
		int   GetMinSize(int iRes);
	public:
		BloomPostPlugin();
		~BloomPostPlugin();
		
		bool Call(const RenderState& kRenderState);

		
};
Plugin* Create_BloomPostPlugin();

