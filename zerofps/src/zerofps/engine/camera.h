#ifndef _CAMERA_H_
#define _CAMERA_H_


#include "../basic/basic.pkg"
#include "engine.pkg"

class ENGINE_API Camera {
	private:		
		Matrix4 m_kCamMatrix;
		Vector3 m_kPos;
		Vector3 m_kRot;
		
			
		bool m_bViewChange;
		bool m_bViewPortChange;				
		
		float m_fX;
		float m_fY;
		float m_fWidth;
		float m_fHeight;

	public:
		
		Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar);
		void Update(int iWidth,int iHeight);
		void UpdateAll(int iWidth,int iHeight);
		void SetView(float fFov,float fAspect,float fNear,float fFar);
		void SetViewPort(float iX,float iY,float iW,float iH);
		void ClearViewPort();
		void SetPos(Vector3 kPos){ m_kPos=kPos;};
		void SetRot(Vector3 kRot){ m_kRot=kRot;};
		Vector3 &GetPos(){return m_kPos;};
		Vector3 &GetRot(){return m_kRot;};
};

#endif




