#ifndef _TCS_H_
#define _TCS_H_

#include "../propertys/p_tcs.h"
#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"



class P_Tcs;

class ENGINE_SYSTEMS_API Tcs_collission
{
	public:
	
		P_Tcs*	pkBody1;
		P_Tcs*	pkBody2;

		vector<Vector3> kPositions;
		vector<Vector3> kNormals;
		
		float		fAtime;					//alpha time when collission occured	
		
		void Clear()
		{
			pkBody1 = NULL;
			pkBody2 = NULL;
			
			fAtime = -1;
			
			kPositions.clear();
			kNormals.clear();
		}
};

class ENGINE_SYSTEMS_API Tcs : public ZFSubSystem
{
	private:
		ZeroFps*				m_pkZeroFps;
		Render*				m_pkRender;
	
		vector<P_Tcs*>		m_kBodys;		
		Tcs_collission		m_kLastCollission;
		
		Vector3				m_kLastLineTestColPos;
		Vector3				m_kLastTestPos;
		Vector3				m_kLastTestNormal;
		float					m_fLastTestDist;
		
		float					m_fSleepLinVel;
		float					m_fSleepRotVel;
		float					m_fMaxDelay;
		float					m_fMinTime;
		float					m_fMinForce;
		float					m_fMaxVel;
		float					m_fAlmostZero;
		float					m_fTimeSlice;
		float					m_fGravity;
		
		P_Tcs*				m_pkBodyCopy1;
		P_Tcs*				m_pkBodyCopy2;		
				
		int					m_iNrOfCollissions;			//nr of collisions last frame
		int					m_iNrOfTests;					//nr of tests last frame
		int					m_iNrOfActiveBodies;			//nr of active bodies last frame
		
		//settings
		int 					m_iHandleCollission;
		int					m_iDebugGraph;
		
		
		//main funktions
		void UpdateForces();
		void UpdateAllVelnPos(float fAtime);
		void UpdateBodyVelnPos(P_Tcs* pkBody,float fAtime);		
		void UpdateCollissions(float fAtime);
		void SyncEntitys();
		void SyncBodys();		
		void HandleCollission(Tcs_collission* pkCol);
		void ResetForces();
		void TryToSleep(P_Tcs* pkBody1,P_Tcs* pkBody2);
						
		
		//collission funktions
		void TestSphereVsSphere(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime,Tcs_collission* pkCollission);
		bool CollideSphereVSSphere(P_Tcs* pkBody1,P_Tcs* pkBody2);			
		
		void TestSphereVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime,Tcs_collission* pkCollission);
		bool CollideSphereVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh);			

		void TestMeshVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime,Tcs_collission* pkCollission);
		bool CollideMeshVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh,Tcs_collission* pkTempCol);			
		bool CollideMeshVSMesh2(P_Tcs* pkSphere,P_Tcs* pkMesh);			
		bool CollideMeshVSMesh3(P_Tcs* pkSphere,P_Tcs* pkMesh,Tcs_collission* pkTempCol);			
		
		//line tests
		void UpdateLineTests(float fAlphaTime);		
		void HandleCharacterCollission(P_Tcs* pkCharacter,P_Tcs* pkBody);
		
		P_Tcs* TestLine(Vector3 kStart,Vector3 kDir,P_Tcs* pkTester);
		bool TestLineVSMesh(Vector3 kStart,Vector3 kDir,P_Tcs* pkB);
		bool TestLineVSSphere(Vector3 kP1,Vector3 kP2,P_Tcs* pkB);

		bool TestSphereVSPolygon(Vector3* kVerts,P_Tcs* pkSphere);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,const Vector3&);
		bool TestLineVSPolygon(Vector3* pkPolygon,Vector3* pkPos1,Vector3* pkPos2,Plane* pkPlane,const Vector3& pkNormal1,const Vector3& pkNormal2);
		bool TestLineVSPolygonNoNormal(Vector3* pkPolygon,Vector3* pkPos1,Vector3* pkPos2,Plane* pkPlane);

		Vector3 GetNeighbourFaceNormal(const Vector3& kVert1,const Vector3& kVert2,const int& iCurrentFace ,P_Tcs* pkBody);
				
		bool TestPolygonVSPolygon(Vector3* pkPolgyon1,Vector3* pkPolygon2);
		float LineInsidePolygon(Vector3* pkPolgyon,Plane* pkPlane,Vector3* pkLinePos1,Vector3* pkLinePos2,Vector3* pkColPos);
		
		bool TestMotionSpheres(P_Tcs* pkBody1,P_Tcs* pkBody2,const float& fAtime);
		
		
	public:		
		
		Tcs();
		~Tcs();
		
		void Update(float fAlphaTime);
		
		void AddBody(P_Tcs* pkPTcs);
		void RemoveBody(P_Tcs* pkPTcs);		

		
		void SetGravity(float fGravity)	{ m_fGravity=fGravity			;};
		int GetNrOfCollissions() 			{ return m_iNrOfCollissions	;};
		int GetNrOfTests() 					{ return m_iNrOfTests			;};
		int GetNrOfActiveBodies() 			{ return m_iNrOfActiveBodies	;};
		
		int GetDebugGraph()					{ return m_iDebugGraph			;};
		
		bool StartUp();
		bool ShutDown();
		bool IsValid();

};



#endif






