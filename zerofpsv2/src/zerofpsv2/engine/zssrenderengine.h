#ifndef _ZSSRENDERINGINE_H_
#define _ZSSRENDERINGINE_H_

#include "../basic/zfsystem.h"
#include "../basic/vector2.h"
#include "../basic/vector3.h"
#include "../basic/vector4.h"
#include "../render/zmaterial.h"
#include "../render/light.h"
#include "../render/zshadersystem.h"
#include <vector>
#include "../render/zocculusionquery.h"
#include "renderplugin_factory.h"

class RenderPackage;
class RenderState;
class ZSSRenderEngine;
class Entity;
class Property;

enum RENDER_TARGET
{
	RENDER_TARGET_SCREEN,
	RENDER_TARGET_TEXTURE,
};



/*
	collect render packages
	
	Stages 1 - 3 can be done by using help function ZSSRenderEngine::SetupView	

	1: setup projection matrix
	2: apply camera orientation to modelview matrix
	3: setup renderstate frustum		
	4: collect renderpackages 
	5: process renderpackages (sort by distance etc)
	6: call DoRender() in render engine
	7: return view matrises to previus state
*/
class ENGINE_API PreRenderPlugin : public Plugin
{
	private:
		string	m_strName;
		int		m_iOrder;
		
	public:
		PreRenderPlugin(const string& strName,int iOrder);
		virtual ~PreRenderPlugin()	{};
		
		const string& GetName()	{	return m_strName;	};
		int GetOrder()				{	return m_iOrder;	};
		
		virtual bool Call(ZSSRenderEngine& kRenderEngine,RenderState& kRenderState) = 0;
		virtual bool Validate(const RenderState& kRenderState) {	return true;};
				
		friend class RenderState;
};


/*
	Called when all rendering is done, 
	
	used to apply fullscreen post effects, like bloom etc
*/
class ENGINE_API PostRenderPlugin : public Plugin
{
	private:
		string	m_strName;
		int		m_iOrder;
		
	public:
		PostRenderPlugin(const string& strName,int iOrder);
		virtual ~PostRenderPlugin()	{};
		
		const string& GetName()	{	return m_strName;	};
		int GetOrder()				{	return m_iOrder;	};
		
		virtual bool Call(RenderState& kRenderState) = 0;
		virtual bool Validate(const RenderState& kRenderState) {	return true;};		
		
		friend class RenderState;
};


/*
	render the supplied renderpackage.
	
	1: Apply orientation to modelview matrix
	2: Bind material
	3: Setup pointers
	4: Render
*/
class ENGINE_API RenderPlugin : public Plugin
{
	private:
		string	m_strName;
		int		m_iOrder;
		
	public:
		RenderPlugin(const string& strName,int iOrder);
		virtual ~RenderPlugin()	{};
		
		const string& GetName()	{	return m_strName;	};		
		int GetOrder()				{	return m_iOrder;	};
		
		virtual bool Call(ZSSRenderEngine& kRenderEngine,RenderPackage& kRenderPackage, const RenderState& kRenderState) = 0;		
		virtual bool Validate(const RenderState& kRenderState) {	return true;};
		
		friend class RenderState;
};


class ENGINE_API RenderState
{
	private:
		static int			m_iNextStateID;	//next renderstateID		
		int					m_iStateID;			//rendetstate id

		//plugins 
		vector<PreRenderPlugin*>	m_kPreRenderPlugins;
		vector<RenderPlugin*>		m_kRenderPlugins;
		vector<PostRenderPlugin*>	m_kPostRenderPlugins;
		
		struct PreRenderSortS : public binary_function<PreRenderPlugin*, PreRenderPlugin*, bool> 	{
			bool operator()(PreRenderPlugin* x, PreRenderPlugin* y) { return x->GetOrder() < y->GetOrder(); };
		} PreRenderSort;		
		struct RenderSortS : public binary_function<RenderPlugin*, RenderPlugin*, bool> 	{
			bool operator()(RenderPlugin* x, RenderPlugin* y) { return x->GetOrder() < y->GetOrder(); };
		} RenderSort;		
		struct PostRenderSortS : public binary_function<PostRenderPlugin*, PostRenderPlugin*, bool> 	{
			bool operator()(PostRenderPlugin* x, PostRenderPlugin* y) { return x->GetOrder() < y->GetOrder(); };
		} PostRenderSort;		
		
	public:
	
		//target type
		RENDER_TARGET	m_eRenderTarget;	
		
		//viewport
		Vector2			m_kViewPortSize;
		Vector2			m_kViewPortPos;
		int				m_iClearViewPort;
		
		//frustum		
		Frustum			m_kFrustum;

		//root entity in screengraph
		Entity*			m_pkRoot;
		
		//camera origentation
		Vector3			m_kCameraPosition;
		Matrix4			m_kCameraRotation;
		Matrix4			m_kCameraProjectionMatrix;
		
		//fog and clearcolor
		Vector4			m_kClearColor;
		Vector4			m_kFogColor;
		float				m_fFogNear;
		float				m_fFogFar;
		bool				m_bFogEnabled;
		float				m_fExposure;			//HDR exposure
				
		int GetStateID() const												{	return m_iStateID;	};
				
		RenderState();
		~RenderState();
		
		//plugin handling
		bool AddPlugin(const string& strName);		
		bool RemovePlugin(const string& strName);
		Plugin* GetPlugin(const string& strName);
		vector<string> GetPluginList();	

		friend class ZSSRenderEngine;
};

class ENGINE_API DataPointer
{
	public:
		int				m_iType;
		const void*		m_pkPointer;
		unsigned int	m_iElements;
		
		DataPointer(int iType,const void* pkPointer)
		{
			m_iType 			= iType;
			m_pkPointer 	= pkPointer;
			m_iElements 	= 0;
		}

		DataPointer(int iType,const void* pkPointer,int iElements)
		{
			m_iType 		= iType;
			m_pkPointer = pkPointer;
			m_iElements = iElements;
		}
		
};

class ENGINE_API MeshData
{
	public:
		//if VBO is sett it will override everything else in the mesh
		ZVertexBuffer*			m_pkVBO;								
		
		//plygon mode for this mesh
		DRAW_MODE				m_ePolygonMode;		
		
		//nr of vertises/indexes  must always be set
		unsigned int			m_iNrOfDataElements;
		
		//mesh data pointers
		vector<DataPointer>	m_kDataPointers;		
		
		//mesh data, will override pointers if used
		vector<Vector2>		m_kVertises2D;
		vector<Vector3>		m_kVertises;
		vector<Vector3>		m_kNormals;
		vector<Vector2>		m_kTexture[4];
		vector<Vector4>		m_kColors;
		vector<Vector3>		m_kTangents;
		vector<Vector3>		m_kBiTangents;
		vector<unsigned int>	m_kIndexs;
		
		MeshData()
		{
			m_pkVBO 					=	NULL;
			m_ePolygonMode 		= 	TRIANGLES_MODE;
			m_iNrOfDataElements	=	0;
		}	
		
		//Assignment operator 
		MeshData& operator=(const MeshData &kOther)
		{
			m_pkVBO 					= kOther.m_pkVBO;
			m_ePolygonMode 		= kOther.m_ePolygonMode;
			m_iNrOfDataElements 	= kOther.m_iNrOfDataElements;
			m_kDataPointers 		= kOther.m_kDataPointers;
			m_kVertises2D 			= kOther.m_kVertises2D;
			m_kVertises 			= kOther.m_kVertises;
			m_kNormals 				= kOther.m_kNormals;
			m_kTexture[0] 			= kOther.m_kTexture[0];
			m_kTexture[1] 			= kOther.m_kTexture[1];
			m_kTexture[2] 			= kOther.m_kTexture[2];
			m_kTexture[3] 			= kOther.m_kTexture[3];
			m_kColors 				= kOther.m_kColors;
			m_kTangents 			= kOther.m_kTangents;
			m_kBiTangents 			= kOther.m_kBiTangents;
			m_kIndexs 				= kOther.m_kIndexs;
			return *this;
		}
};

class ENGINE_API RenderPackage
{
	public:
		Property*						m_pkProperty;				//HACK override everything and do a ugly draw how the hell u want call
	
		ZMaterial*						m_pkMaterial;				//pointer to material, always needed
		LightProfile*					m_pkLightProfile;			//pointer to lightprofile, if lighting is to be used
		Matrix4							m_kModelMatrix;			//model orientation matrix
		Vector3							m_kCenter;					//center of mesh, used for culling etc
		float								m_fRadius;					//radius		
		Vector3							m_kAABBMin;					//local aabb min
		Vector3							m_kAABBMax;					//local aabb max		
		bool								m_bOcculusionTest;		//shuld an occulusion test be done
		RenderPackage*					m_pkOcculusionParent;	//if set, occulusion test will test occulusion on this package instead		
		bool								m_bBlendSort;				//blended sorting, is reversed ie closest object is rendered last
		bool								m_bStatic;					//this mesh is static in the world ( wont move or animated ), used for static shadowmaps etc
		
		MeshData							m_kMeshData;				//contains mesh infromation
		int								m_iAttachToEntityID;		//entity id of parent model, -1 is no attachment
		string							m_strAttachToJointName;	//joints name to attach to 
		
		//occulusion test map, one for each renderstate ID
		map< int, ZOcculusionTest>	m_kOcculusionTest;	
		
		//TEMP distance to camera
		float								m_fDistanceToCamera;
		
		
		RenderPackage();	
			
		//assignment operator	
		RenderPackage& operator=(const RenderPackage &kOther)
		{
			m_pkMaterial			= kOther.m_pkMaterial;		
			m_pkLightProfile		= kOther.m_pkLightProfile;		
			m_kModelMatrix			= kOther.m_kModelMatrix;		
			m_kCenter				= kOther.m_kCenter;		
			m_fRadius				= kOther.m_fRadius;		
			m_kAABBMin				= kOther.m_kAABBMin;		
			m_kAABBMax				= kOther.m_kAABBMax;		
			m_bOcculusionTest		= kOther.m_bOcculusionTest;		
			m_pkOcculusionParent = kOther.m_pkOcculusionParent;		
			m_bBlendSort			= kOther.m_bBlendSort;		
			m_kMeshData				= kOther.m_kMeshData;		
			m_fDistanceToCamera	= kOther.m_fDistanceToCamera;		
			return *this;
		}
};


class ENGINE_API ZSSRenderEngine : public ZFSubSystem
{
	private:
		ZSSZeroFps*			m_pkZeroFps;				
		ZShaderSystem*		m_pkZShaderSystem;			
		ConVar				m_kRenderEngineDebug;
			
			
		map<string,void*>	m_kParameters;
			
			
		void SetupFramebuffer(const RenderState& kRenderState);
		bool ValidateRenderState(const RenderState& kRenderState);	
		bool ValidateRenderPackages(const vector<RenderPackage*>& kRenderPackages);	

	public:
		PluginFactory	m_kPluginFactory;
	
		bool StartUp();
		bool ShutDown();
		bool IsValid() 	{return true;};		
		ZSSRenderEngine();
	
		bool Render(RenderState& kRenderState);
		void DoRender(const vector<RenderPackage*>&	kRenderPackages,const RenderState& kRenderState);
		
		//used to paste parameters between renderplugins, will be cleared after each render call
		void  SetParameter(const string& strName,void* pkValue);
		void* GetParameter(const string& strName);
		
		//help functions
		void SetupView(RenderState& kRenderState);
		bool OcculusionTest(RenderPackage& kRenderPackage, const RenderState& kRenderState);
		
};

#endif






