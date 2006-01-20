#ifndef _ENGINE_CAMERA_H_
#define _ENGINE_CAMERA_H_

#include "../basic/math.h"
#include "../render/frustum.h"
#include "engine_x.h"
#include "../render/zshadersystem.h"

#include "entitymanager.h"
#include "zerofps.h"
#include "application.h"
#include "../engine_systems/common/zshadow.h"
#include "../engine_systems/propertys/p_camera.h"
#include "zssrenderengine.h"

#include "i_camera.h"

#include "../basic/autolist.h"

class ZSSRender;
class P_Camera;


enum ENGINE_API RENDERMODE
{
	RENDER_NONE				=0,
	RENDER_CASTSHADOW		=1,
	RENDER_SHADOWED		=2,
	RENDER_NOSHADOWFIRST	=4,
	RENDER_NOSHADOWLAST	=8,
};

/** \brief	A Camera (ViewPoint) in ZeroFPS
	 \ingroup Engine
*/
class ENGINE_API Camera : public I_Camera , public AutoList<Camera>
{
	public:
		enum CamMode 
		{
			CAMMODE_PERSP,
			CAMMODE_ORTHO_TOP,
			CAMMODE_ORTHO_FRONT,
			CAMMODE_ORTHO_LEFT,
			CAMMODE_ORTHO_BOT,
			CAMMODE_ORTHO_BACK,
			CAMMODE_ORTHO_RIGHT,
		};
	
		enum RENDERTARGET
		{
			RENDERTARGET_SCREEN,
			RENDERTARGET_TEXTURE,
			RENDERTARGET_NONE,		
		};


		
	private:	

		ZSSEntityManager*	m_pkEntityMan;
		ZSSRender*			m_pkRender;
		ZShaderSystem*		m_pkZShaderSystem;
		ZSSZeroFps*			m_pkZeroFps;
		ZShadow*				m_pkZShadow;
		ZSSLight*			m_pkLight;
		TextureManager* 	m_pkTexMan;
		ZSSRenderEngine*	m_pkZSSRenderEngine;
		
		
		//matrises and position
		Matrix4	m_kCamProjectionMatrix;
		Matrix4	m_kCamModelViewMatrix;
		Vector3	m_kPos;
		Matrix4	m_kRotM;
		Vector3	m_kRenderPos;
		
		//gl stuff  , clear color and fog
		Vector4	m_kClearColor;
		Vector4	m_kFogColor;
		float		m_fFogNear;
		float		m_fFogFar;
		bool		m_bFogEnabled;
		
		int		m_iForceLighing;
		
		//viewport
		Vector2	m_kViewPortCorner;
		Vector2	m_kViewPortSize;

		//camera name
		string	m_strName;							// Is used in GetCameraDesc.

		//projection stuff
		float		m_fFov;
		float		m_fAspect;
		float		m_fNear;
		float		m_fFar;
		
		//mode
		CamMode	m_eMode;								// The camera mode this camera is in.

		float		m_fAppWidth;						// Width of application (OpenGl) window in pixels.
		float		m_fAppHeight;						// Height of application (OpenGl) window in pixels.

		Frustum	m_kFrustum;

		/* OrthoAxis maps the local screen coo to world. Used to draw grid and move ortho views around. */
		Vector3	m_kOrthoAxisX;							
		Vector3	m_kOrthoAxisY;
		Vector3	m_kOrthoAxisZ;

		Vector3	m_kOrthoSize;						// How many units the camera cover in ortho view. z not used. 
		
		//camera settings
		bool		m_bRender;							// False if no need to render from this camera (not visible on screen).
		bool		m_bSelected;
		int		m_iRenderTarget;
		bool		m_bClearViewPort;
		bool		m_bDrawInterface;
		bool		m_bShadowMap;
		
		Vector3	m_kLastShadowPos;
		Vector3	m_kLastShadowLightPos;
		
		//if theres a p_cam attached to this camera , this is its entity id
		int			m_iEntity;						// ID Of entity that this camera is connected to (if any).
		int			m_iRootEntity;					// ID of entity to render from. 	
		bool			m_bRootOnly;					// Render only rootentity. 
		P_Camera*	m_pkCameraProp;				
		
		//fullscreen shader
		ResTexture		m_kFSSTexture;
		Matrix4			m_kFSSProjMatrix;
		ZMaterial*		m_pkFSSMaterial;
		bool				m_bFSSEnabled;		
		int				m_iFSSTextureWidth;
		int				m_iFSSTextureHeight;
		
		//shadow shaders		
		ZFResourceHandle* m_pkDefaultShadowmapShader;
		ZFResourceHandle* m_pkDefaultShadowmapShaderPPL;
		ZFResourceHandle* m_pkDefaultFastShadowmapShader;
		
		//bloom
		ResTexture		m_kBloomTexture;
		bool				m_bBloomEnabled;
		ZMaterial*		m_pkBloomMaterial1;
		ZMaterial*		m_pkBloomMaterial2;
		
		//HDR STUFF
		float				m_fExposureFactor;
		
		//camera shake effect		
		float 			m_fShakeAmount;									///< camera shake magnitude, 0 means no shake at all
		Vector3			m_kCurrentShake;
		
		//shadow hack
		ResTexture		m_kShadowTexture;
		int				m_iShadowTexWidth;
		int				m_iShadowTexHeight;
		float				m_fShadowArea;
		int				m_iCurrentRenderMode;
		
		Matrix4			m_kLightProjMatrix;
		Matrix4			m_kLightViewMatrix;
		
		GLuint			m_iShadowFBO;
		GLuint			m_iShadowRBOcolor;
		
 		void		MakeShadowTexture(const Vector3& kLightPos,const Vector3& kCenter, ResTexture* pkTexture);
		int		GetMaxSize(int iRes);
		int		GetMinSize(int iRes);

		void		DrawShadowedScene();		
		void		DrawGrid();		
		void		DrawWorld();
		
		void		FullScreenShader();
		void		MakeBloom();
		
		float		GetExposureFactor();
		
		void		SetupRenderState();
		
	public:
		bool				m_bForceFullScreen;	// Ignore own settings and render to fullscreen.
		static bool		m_bDrawOrthoGrid;		// Draw grid when rendering from ortho cameras.
		static float	m_fGridSpace;			// Distance between each grid line.
		static bool		m_bGridSnap;			// Snap postions to grid when use Camera::SnapToGrid

		//render state
		RenderState	m_kRenderState;
				
				
		Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar);
		~Camera();
		
		void 	InitView();	
		void	RenderView();	
		void 	SetView(float fFov,float fAspect,float fNear,float fFar);
	
		void 	SetViewMode(CamMode eMode);
		void 	SetViewMode(string strName);

		void 	SetOrthoView();
		void 	SetViewPort(float iX,float iY,float iW,float iH);
		void 	ClearViewPort(bool bColor);
		void 	SetFov(float fFov);

		void 		OrthoZoom(float fZoom);
		void 		OrthoMove(Vector3 kMove);
		Vector3 	GetOrthoMove(Vector3 kMove);
		Vector3 	SnapToGrid(Vector3 kPos);
		
		void		SetShakeAmount(float fShake)				{	m_fShakeAmount = fShake;	}
		
		void		SetFog(const Vector4& kColor,float fStart,float fStop,bool bEnabled);
		void		SetClearColor(const Vector4& kColor)	{	m_kClearColor = kColor;		}
		
		void		SetForceLighting(int iForce)				{	m_iForceLighing = iForce;	}
		int		GetForceLighting()							{	return m_iForceLighing;		}
		
		void 		SetRotM(const Matrix4& kRotM);
		void 		RotateV(const Vector3& kRot);
		void 		MultRotM(const Matrix4& kRotM);
		
		Matrix4	GetRotM()							{	return m_kRotM;						}
		void 		SetPos(const Vector3& kPos)	{	m_kPos=kPos;							}
		void 		SetRot(const Vector3& kRot)	{	m_kRotM.Identity();
																m_kRotM.Rotate(kRot);				}
		Vector3  GetRenderPos()						{	return m_kRenderPos;					}																
		Vector3	GetPos()								{	return m_kPos;							}
		Matrix4	GetModelViewMatrix()				{	return m_kCamModelViewMatrix;		}
		Matrix4	GetProjectionMatrix()			{	return m_kCamProjectionMatrix;	}		
		void 		SetName(const string& strName){	m_strName = strName;					} 
		string 	GetName( )							{	return m_strName;						}
		Camera::CamMode GetViewMode()				{	return m_eMode;						}
		int		GetEntityID()						{	return m_iEntity;						}
		int		GetRootEntityID()					{	return m_iRootEntity;				}		
		void		SetRootEntityID(int iEntityID){	m_iRootEntity = iEntityID;			}
		void		SetRootOnly(bool bRoot)			{	m_bRootOnly = bRoot;					}
		bool		GetRootOnly()						{	return m_bRootOnly;					}
		void		SetClearViewPort(bool bClear)	{	m_bClearViewPort = bClear;			}
		bool		GetClearViewPort()				{	return m_bClearViewPort;			}
		
		void		SetDrawInterface(bool bDebug)	{	m_bDrawInterface = bDebug;			}
		bool		GetDrawInterface()				{	return m_bDrawInterface;			}
		
		Frustum*	GetFrustum()						{	return &m_kFrustum;			}
		bool	   IsRenderOn()						{	return m_bRender;				}
		
		int		GetCurrentRenderMode()			{	return m_iCurrentRenderMode;}
		
		void		SetShadowMap(bool bSM)			{	m_bShadowMap = bSM;			}		
		bool		GetShadowMap()						{	return m_bShadowMap;			}
		bool	   IsSelected()						{	return m_bRender;				}
		void		SetRender( bool bRenderOn )	{	m_bRender = bRenderOn;		}
		void		SetSelected( bool bSelected )	{	m_bSelected = bSelected;	}
		Vector3	GetOrthoAxisX()					{	return m_kOrthoAxisX;		}
		Vector3	GetOrthoAxisY()					{	return m_kOrthoAxisY;		}
		Vector3	GetOrthoAxisZ()					{	return m_kOrthoAxisZ;		}
		Vector3	GetOrthoSize()						{	return m_kOrthoSize;			}

		string	GetCameraDesc();
		Vector3	GetViewPortSize();
		Vector3	GetViewPortCorner();
		Vector3	Get3DCursorDir(float x,float y,bool bMouse=true);
		Vector3  Get3DCursorPos(float x,float y,bool bMouse=true);

		//fullscreen shader stuff		
		void SetBloomEnabled(bool bEn)			{	m_bBloomEnabled = bEn;		}
		void SetFSSEnabled(bool bEn)				{	m_bFSSEnabled = bEn;			}
		void SetFSSGLSLShader(const string& strShader);
		ZMaterial* GetFSSMaterial()				{	return m_pkFSSMaterial;		}
		
		
		friend	class P_Camera;
};

#endif




