#ifndef _ENGINE_CAMERA_H_
#define _ENGINE_CAMERA_H_

#include "../basic/vector3.h"
#include "../basic/matrix4.h"
#include "../render/frustum.h"
#include "engine_x.h"
#include "../render/zshadersystem.h"

#include "entitymanager.h"
#include "zerofps.h"
#include "application.h"
#include "../engine_systems/common/zshadow.h"

#include "i_camera.h"

class Render;

/** \brief	A Camera (ViewPoint) in ZeroFPS
	 \ingroup Engine
*/
class ENGINE_API Camera : public I_Camera
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

		EntityManager*	m_pkEntityMan;
		Render*			m_pkRender;
		ZShaderSystem*	m_pkZShaderSystem;
		ZeroFps*			m_pkZeroFps;
		ZShadow*			m_pkZShadow;
		
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
		
		//viewport
		Vector3	m_kViewPortCorner;
		Vector3	m_kViewPortSize;

		//camera name
		string	m_strName;							// Is used in GetCameraDesc.

		//projection stuff
		float		m_fFov;
		float		m_fAspect;
		float		m_fNear;
		float		m_fFar;
		
		//mode
		CamMode	m_eMode;

		float		m_fAppWidth;
		float		m_fAppHeight;

		Frustum	m_kFrustum;

		Vector3	m_kOrthoAxisX;
		Vector3	m_kOrthoAxisY;
		Vector3	m_kOrthoAxisZ;

		Vector3	m_kOrthoSize;
		
		//camera settings
		bool		m_bRender;							// False if no need to render from this camera (not visible on screen).
		bool		m_bSelected;
		int		m_iRenderTarget;
		bool		m_bClearViewPort;
		
		
		//if theres a p_cam attached to this camera , this is its entity id
		int		m_iEntity;							// ID Of entity that this camera is connected to.
		int		m_iRootEntity;		
		bool		m_bRootOnly;
		
		
		
		void		DrawGrid();
		
	public:
		bool				m_bForceFullScreen;
		static bool		m_bDrawOrthoGrid;
		static float	m_fGridSpace;
		static bool		m_bGridSnap;

				
		Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar);
	
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

		
		void		SetFog(const Vector4& kColor,float fStart,float fStop,bool bEnabled);
		void		SetClearColor(const Vector4& kColor)	{	m_kClearColor = kColor;	}
		
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
		
		Frustum*	GetFrustum()						{	return &m_kFrustum;			}
		bool	   IsRenderOn()						{	return m_bRender;				}
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

		friend	class P_Camera;
};

#endif




