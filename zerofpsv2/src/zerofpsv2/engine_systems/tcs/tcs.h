#ifndef _TCS_H_
#define _TCS_H_

#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"
#include "../propertys/p_tcs.h"


class P_Tcs;

class ENGINE_SYSTEMS_API Tcs : public ZFSubSystem
{
	private:
		ZeroFps*				m_pkZeroFps;
		Render*				m_pkRender;
	
		vector<P_Tcs*>		m_kBodys;		
		Matrix4				m_kModelMatrix;


		void UpdateVel();
		void UpdateMotion();
		void UpdateCollissions();

		void GenerateModelMatrix(P_Tcs* pkMesh);
		void CalcMotionSpheres();
		
		Vector3 GetNewPos(P_Tcs* pkBody);
		
		bool TestMotionSpheres(P_Tcs* pkB1,P_Tcs* pkB2);
		bool TestMotionSphereVSMesh(P_Tcs* pkB1,P_Tcs* pkB2);
		
		
		bool MeshVSSphereTest(P_Tcs* pkMesh,P_Tcs* pkSphere);
		bool TestSphereVSPolygon(Vector3* kVerts,P_Tcs* pkSphere);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR);
	public:
		
		Tcs();
		~Tcs();
		
		void Update();
		
		void AddBody(P_Tcs* pkPTcs);
		void RemoveBody(P_Tcs* pkPTcs);		


		bool StartUp();
		bool ShutDown();
		bool IsValid();

};



#endif