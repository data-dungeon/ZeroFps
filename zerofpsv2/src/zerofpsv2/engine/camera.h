#ifndef _ENGINE_CAMERA_H_
#define _ENGINE_CAMERA_H_


#include "../basic/vector3.h"
#include "../basic/matrix4.h"
#include "../render/frustum.h"
#include "engine_x.h"
#include "../gui/zguiwindow.h"

/** \brief	A Camera (ViewPoint) in ZeroFPS
	 \ingroup Engine
*/
class ENGINE_API Camera 
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
	
	ZGuiWnd*	m_pkWnd;

	private:	

		Matrix4 m_kCamProjectionMatrix;
		Matrix4 m_kCamModelViewMatrix;
		Vector3 m_kPos;
		Vector3 m_kRot;	
		Matrix4 m_kRotM;
			


		bool m_bViewChange;
		bool m_bViewPortChange;				
		
		//float m_fX;
		//float m_fY;
		//float m_fWidth;
		//float m_fHeight;
		Vector3	m_kViewPortCorner;
		Vector3	m_kViewPortSize;

		string	m_strName;		// Is used in GetCameraDesc.

		float	m_fFov;
		float	m_fAspect;
		float	m_fNear;
		float	m_fFar;
		
		//bool	m_bPerspective;
		CamMode	m_eMode;

		float m_fAppWidth;
		float m_fAppHeight;


		void	DrawGrid();
		

	public:
		Frustum m_kFrustum;

		Vector3	m_kOrthoAxisX;
		Vector3	m_kOrthoAxisY;
		Vector3	m_kOrthoAxisZ;

		Vector3	m_kOrthoSize;
		bool		m_bRender;			// False if no need to render from this camera (not visible on screen).
		bool		m_bSelected;

		int		m_iEntity;			// ID Of entity that this camera is connected to.
		bool		m_bForceFullScreen;

		Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar);
		void Update(int iWidth,int iHeight);
		void UpdateAll(int iWidth,int iHeight);
		void SetView(float fFov,float fAspect,float fNear,float fFar);
	
		void SetViewMode(CamMode eMode);
		void SetViewMode(string strName);
		Camera::CamMode GetViewMode()	{ return m_eMode; }

		void SetOrthoView();
		void SetViewPort(float iX,float iY,float iW,float iH);
		//void SetViewPortPerc(float iX,float iY,float iW,float iH);
		void ClearViewPort();
		void SetFov(float fFov);
		
		Vector3 GetViewPortSize();
		Vector3 GetViewPortCorner();

		void OrthoZoom(float fZoom);
		void OrthoMove(Vector3 kMove);
		Vector3 GetOrthoMove(Vector3 kMove);
		Vector3 SnapToGrid(Vector3 kPos);

		void SetRotM(Matrix4 kRotM);
		void RotateV(Vector3 kRot);
		void MultRotM(Matrix4 kRotM);
		
		Matrix4 GetRotM()						{	return m_kRotM;	};
		void SetPos(Vector3 kPos)			{	m_kPos=kPos;		};
		void SetRot(Vector3 kRot)			{	m_kRot=kRot;		};
		Vector3 &GetPos()						{	return m_kPos;		};
		Vector3 &GetRot()						{	return m_kRot;		};
		Matrix4& GetModelViewMatrix()		{	return m_kCamModelViewMatrix;};
		Matrix4& GetProjectionMatrix()	{	return m_kCamProjectionMatrix;};		
		void SetName(string strName)		{	m_strName = strName; } 
		string GetName( )						{	return m_strName; }

		string GetCameraDesc();

		static bool		m_bDrawOrthoGrid;
		static float	m_fGridSpace;
		static bool		m_bGridSnap;

};

#endif




