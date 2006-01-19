#include "zssrenderengine.h"
#include "../render/light.h"
#include "entitymanager.h"
#include "zerofps.h"
#include "../render/zmaterial.h"

class ZMaterial;


class ENGINE_API SkyLayer
{
	public:
		RenderPackage	m_kRenderPackage[6];
		ZMaterial		m_kMaterials[6];
		Vector3			m_kRotation;
		
		bool				m_bSidesOnly;
};

enum SKY_HDR
{
	NO_HDR		=0,
	SQUARE_HDR	=1,
	EXTERNAL_HDR=2,
	LINEAR_HDR	=3,
};

class ENGINE_API SkyRender : public PreRenderPlugin
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSLight*			m_pkLight;
		ZSSZeroFps*			m_pkZeroFps;
		Application*		m_pkApplication;
		ZSSEntityManager*	m_pkEntityManager;
	
		vector<SkyLayer*>	m_kMaterials;
	
	public:
		SkyRender();
		~SkyRender();
		
		void Clear();
		void AddTexture(const string& strName,SKY_HDR eHDR = NO_HDR,const Vector3& kColor = Vector3::ZERO,bool bOnlySides = false);
		bool Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState);
};
Plugin* Create_SkyRender();


class ENGINE_API InterfaceRender : public PreRenderPlugin
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



enum ShadowmapPointers
{
	SHADOWMAP_POINTER	= 1,
};

class ENGINE_API ShadowMap
{
	public:
		ResTexture	m_kTexture;
		Matrix4		m_kTextureMatrix;
};

class ENGINE_API ShadowmapPlugin : public PreRenderPlugin
{	
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSEntityManager*	m_pkEntityManager;
		ZSSZeroFps*			m_pkZeroFps;
		ZSSRender*			m_pkRender;
		ZSSLight*			m_pkLight;
		
		int 			m_iShadowTexWidth;
		int 			m_iShadowTexHeight;
		bool 			m_bEnabled;
		float			m_fShadowArea;
		
		Vector3		m_kLastShadowPos;
		Vector3		m_kLastShadowRot;
				
				
		GLuint		m_iShadowFBO;
		GLuint		m_iShadowRBOcolor;				
		Matrix4		m_kLightViewMatrix;
		Matrix4		m_kLightProjMatrix;
		
		ShadowMap	m_kShadowmap;
		
		RenderState	m_kShadowRenderState;
		
		int   GetMaxSize(int iRes);
		void  MakeShadowTexture(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState,const Vector3& kLightPos,const Vector3& kCenter);
		
	public:
		ShadowmapPlugin();
		~ShadowmapPlugin();
		
		bool Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState);
		
};
Plugin* Create_ShadowmapPlugin();

enum BonePointers
{
	BONEMATRIX_POINTER 	= 32,
	BONEINDEX_POINTER,
};		
		
class ENGINE_API DebugRenderPlugin : public RenderPlugin
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
		
		bool Call(ZSSRenderEngine& kRenderEngine,RenderPackage& kRenderPackage, const RenderState& kRenderState);
};
Plugin* Create_DebugRenderPlugin();
		
class ENGINE_API DepthMapRenderer : public RenderPlugin
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSLight*			m_pkLight;
		ZSSZeroFps*			m_pkZeroFps;
		ZSSEntityManager*	m_pkEntityManager;
	
		ZMaterial*			m_pkDepthMaterial;
	
		void DoBoneTransformation(const RenderPackage& kRenderPackage,const DataPointer& kBoneMatrises,const DataPointer& kBoneIndexes);
	
	public:
					
		DepthMapRenderer();
		~DepthMapRenderer();
	
		bool Call(ZSSRenderEngine& kRenderEngine,RenderPackage& kRenderPackage, const RenderState& kRenderState);
};
Plugin* Create_DepthMapRendererPlugin();		

class ENGINE_API DefaultRenderPlugin : public RenderPlugin
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSLight*			m_pkLight;
		ZSSZeroFps*			m_pkZeroFps;
		ZSSEntityManager*	m_pkEntityManager;
	
		void DoBoneTransformation(const RenderPackage& kRenderPackage,const DataPointer& kBoneMatrises,const DataPointer& kBoneIndexes);
		void SetupShadowmap(ShadowMap* kShadowmap);
	
	public:
					
		DefaultRenderPlugin() : RenderPlugin("Render",0)
		{
			m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
			m_pkLight			= static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));
			m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
			m_pkEntityManager = static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));			
		}
	
		bool Call(ZSSRenderEngine& kRenderEngine,RenderPackage& kRenderPackage, const RenderState& kRenderState);
};
Plugin* Create_DefaultRenderPlugin();

class ENGINE_API AttachToJoint : public RenderPlugin
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSLight*			m_pkLight;
		ZSSZeroFps*			m_pkZeroFps;
		ZSSEntityManager*	m_pkEntityManager;
	
	
	public:
					
		AttachToJoint() : RenderPlugin("AttachToJoint",-10)
		{
			m_pkZShaderSystem = static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));
			m_pkLight			= static_cast<ZSSLight*>(g_ZFObjSys.GetObjectPtr("ZSSLight"));
			m_pkZeroFps			= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
			m_pkEntityManager = static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));			
		}
	
		bool Call(ZSSRenderEngine& kRenderEngine,RenderPackage& kRenderPackage, const RenderState& kRenderState);
};
Plugin* Create_AttachToJointPlugin();



class ENGINE_API DefaultPreRenderPlugin : public PreRenderPlugin
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
		~DefaultPreRenderPlugin();
		
		bool Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState);
		
};
Plugin* Create_DefaultPreRenderPlugin();

class ENGINE_API ExposureCalculator : public PostRenderPlugin
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
		
		bool Call(RenderState& kRenderState);

		
};
Plugin* Create_ExposureCalculator();

class ENGINE_API BloomPostPlugin : public PostRenderPlugin
{	
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSRender*			m_pkRender;
	
		int 			m_iFSSTextureWidth;
		int 			m_iFSSTextureHeight;
		Matrix4		m_kFSSProjMatrix;
		
		ZMaterial*	m_pkBloomMaterial1;
		ZMaterial*	m_pkBloomMaterial2;
		ZMaterial*	m_pkBloomMaterial3;
		
		ResTexture	m_kBloomTexture;
		ResTexture	m_kBloomTexture2;
		
		int   GetMinSize(int iRes);
	public:
		BloomPostPlugin();
		~BloomPostPlugin();
		
		bool Call(RenderState& kRenderState);
		bool Validate(const RenderState& kRenderState);
		
};
Plugin* Create_BloomPostPlugin();

