#ifndef _CAMERA_H_
#define _CAMERA_H_


#include "../basic/basic.pkg"
#include "engine.pkg"

class Camera {
	private:		
		Matrix4 m_kCamMatrix;
		Vector3 m_kPos;
		Vector3 m_kRot;
		
		int m_iX;
		int m_iY;
		int m_iWidth;
		int m_iHeight;
			
		bool m_bViewChange;
	public:
		
		Camera(Vector3 kPos,Vector3 kRot,float fFov,float fAspect,float fNear,float fFar);
		void Update();
		void UpdateAll();
		void SetView(float fFov,float fAspect,float fNear,float fFar);
		void SetPos(Vector3 kPos){ m_kPos=kPos;};
		void SetRot(Vector3 kRot){ m_kRot=kRot;};
		Vector3 &GetPos(){return m_kPos;};
		Vector3 &GetRot(){return m_kRot;};
};

#endif




