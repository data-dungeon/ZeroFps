#ifndef _ENGINE_CAMERA_H_
#define _ENGINE_CAMERA_H_


#include "../basic/vector3.h"
#include "../basic/matrix4.h"
#include "../render/frustum.h"
#include "engine_x.h"

class ENGINE_API Camera {
	private:		
		Matrix4 m_kCamProjectionMatrix;
		Matrix4 m_kCamModelViewMatrix;
		Vector3 m_kPos;
		Vector3 m_kRot;	
		Matrix4 m_kRotM;
			
		
		
		bool m_bViewChange;
		bool m_bViewPortChange;				
		
		float m_fX;
		float m_fY;
		float m_fWidth;
		float m_fHeight;

		string	m_strName;		// Is used in GetCameraDesc.

		float	m_fFov;
		float	m_fAspect;
		float	m_fNear;
		float	m_fFar;
		
	public:
		Frustum m_kFrustum;
		
		Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar);
		void Update(int iWidth,int iHeight);
		void UpdateAll(int iWidth,int iHeight);
		void SetView(float fFov,float fAspect,float fNear,float fFar);
		void SetViewPort(float iX,float iY,float iW,float iH);
		void ClearViewPort();
		
		void SetRotM(Matrix4 kRotM) { m_kRotM = kRotM;};
		void RotateV(Vector3 kRot) { m_kRotM.Rotate(kRot);};
		void MultRotM(Matrix4 kRotM) { m_kRotM = kRotM * m_kRotM ;};
		
		Matrix4 GetRotM() { return m_kRotM;};
		void SetPos(Vector3 kPos){ m_kPos=kPos;};
		void SetRot(Vector3 kRot){ m_kRot=kRot;};
		Vector3 &GetPos(){return m_kPos;};
		Vector3 &GetRot(){return m_kRot;};
		Matrix4& GetModelViewMatrix() {return m_kCamModelViewMatrix;};
		Matrix4& GetProjectionMatrix() {return m_kCamProjectionMatrix;};		

		void SetFov(float fFov);
		void SetName(string strName) { m_strName = strName; } 
		string GetName( ) { return m_strName; }

		string GetCameraDesc();
};

#endif




