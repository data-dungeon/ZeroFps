#ifndef _TCS_H_
#define _TCS_H_

//#include "../propertys/p_tcs.h"
#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"



class P_Tcs;

class ENGINE_SYSTEMS_API Tcs_collission
{
	public:
	
		P_Tcs*	pkBody1;
		P_Tcs*	pkBody2;

		Vector3	kPos;					//collission point
		Vector3	kNormal;				//pkBody2's normal
		Vector3	kRelVel;				//pkBody2's normal		
	
		float		fAtime;					//alpha time when collission occured	
};

class ENGINE_SYSTEMS_API Tcs : public ZFSubSystem
{
	private:
		ZeroFps*				m_pkZeroFps;
		Render*				m_pkRender;
	
		vector<P_Tcs*>					m_kBodys;		
		vector<Tcs_collission>		m_kCollissions;		
		
		Matrix4				m_kModelMatrix;
		Vector3				m_kLastLineTestColPos;
		Vector3				m_kLastTestPos;
		Vector3				m_kLastTestNormal;		
		int					m_iMaxTests;
		
		P_Tcs*					m_pkBodyCopy1;
		P_Tcs*					m_pkBodyCopy2;		
		
		//main funktions
		void UpdateForces();
		void UpdateAllVelnPos(float fAtime);
		void UpdateBodyVelnPos(P_Tcs* pkBody,float fAtime);		
		void UpdateCollissions(float fAtime);
		void SyncEntitys();
		void SyncBodys();		
		void HandleCollission(Tcs_collission* pkCol);
		
		//help funktions
		Tcs_collission* FindNextCollission();
	
		
		//collission funktions
		void TestSphereVsSphere(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime);
		bool CollideSphereVSSphere(P_Tcs* pkBody1,P_Tcs* pkBody2);			
		
		void TestSphereVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime);
		bool CollideSphereVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh);			

		//line tests
		void UpdateLineTests();		
		void HandleCharacterCollission(P_Tcs* pkCharacter,P_Tcs* pkBody);
		
		P_Tcs* TestLine(Vector3 kStart,Vector3 kDir,P_Tcs* pkTester);
		bool TestLineVSMesh(Vector3 kStart,Vector3 kDir,P_Tcs* pkB);
		bool TestLineVSSphere(Vector3 kP1,Vector3 kP2,P_Tcs* pkB);

		void GenerateModelMatrix(P_Tcs* pkMesh);		
		
		
		bool TestSphereVSPolygon(Vector3* kVerts,P_Tcs* pkSphere);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR);
		
		void GenerateModelMatrix();

		
	public:
		
		Tcs();
		~Tcs();
		
		void Update(float fAlphaTime);
		
		void AddBody(P_Tcs* pkPTcs);
		void RemoveBody(P_Tcs* pkPTcs);		


		bool StartUp();
		bool ShutDown();
		bool IsValid();

};



#endif