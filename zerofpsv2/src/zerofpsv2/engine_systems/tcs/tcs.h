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
		vector<Vector3> kTangents;
		vector<Vector3> kRelVels;
		
		Vector3	kPos;					//collission point
		Vector3	kNormal;				//pkBody2's normal
		Vector3	kRelVel;				//pkBody2's normal
		Vector3	kTangent;		
	
		float		fAtime;					//alpha time when collission occured	
		
		void Clear()
		{
			pkBody1 = NULL;
			pkBody2 = NULL;
			
			fAtime = 0;
			
			kPositions.clear();
			kNormals.clear();
			kTangents.clear();
			kRelVels.clear();		
		}
};

class ENGINE_SYSTEMS_API Tcs : public ZFSubSystem
{
	private:
		ZeroFps*				m_pkZeroFps;
		Render*				m_pkRender;
	
		vector<P_Tcs*>					m_kBodys;		
		vector<Tcs_collission*>		m_kCollissions;		
		
		Vector3				m_kLastLineTestColPos;
		Vector3				m_kLastTestPos;
		Vector3				m_kLastTestNormal;
		float					m_fLastTestDist;
		
		float					m_fSleepVel;
		float					m_fMaxDelay;
		float					m_fMinTime;
		float					m_fMinForce;
		float					m_fAlmostZero;
		
		P_Tcs*				m_pkBodyCopy1;
		P_Tcs*				m_pkBodyCopy2;		
				
		int					m_iNrOfCollissions;
		
		//settings
		int 				m_iHandleCollission;
		int				m_iDebugGraph;
		
		
		//main funktions
		void UpdateForces();
		void UpdateAllVelnPos(float fAtime);
		void UpdateBodyVelnPos(P_Tcs* pkBody,float fAtime);		
		void UpdateCollissions(float fAtime);
		void SyncEntitys();
		void SyncBodys();		
		void HandleCollission(Tcs_collission* pkCol);
		void ResetForces();
		void ClearCollissions();
		void TryToSleep(P_Tcs* pkBody1,P_Tcs* pkBody2);
						
		//help funktions
		Tcs_collission* FindNextCollission();
		
		//collission funktions
		void TestSphereVsSphere(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime);
		bool CollideSphereVSSphere(P_Tcs* pkBody1,P_Tcs* pkBody2);			
		
		void TestSphereVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime);
		bool CollideSphereVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh);			

		void TestMeshVsMesh(P_Tcs* pkBody1,P_Tcs* pkBody2,float fAtime);
		bool CollideMeshVSMesh(P_Tcs* pkSphere,P_Tcs* pkMesh,Tcs_collission* pkTempCol);			
		bool CollideMeshVSMesh2(P_Tcs* pkSphere,P_Tcs* pkMesh);			
		
		//line tests
		void UpdateLineTests();		
		void HandleCharacterCollission(P_Tcs* pkCharacter,P_Tcs* pkBody);
		
		P_Tcs* TestLine(Vector3 kStart,Vector3 kDir,P_Tcs* pkTester);
		bool TestLineVSMesh(Vector3 kStart,Vector3 kDir,P_Tcs* pkB);
		bool TestLineVSSphere(Vector3 kP1,Vector3 kP2,P_Tcs* pkB);

		bool TestSphereVSPolygon(Vector3* kVerts,P_Tcs* pkSphere);
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos,float fR);
		bool TestLineVSPolygon(Vector3* pkPolygon,Vector3* pkPos1,Vector3* pkPos2,Plane* pkPlane);
		
		bool TestPolygonVSPolygon(Vector3* pkPolgyon1,Vector3* pkPolygon2);
		float LineInsidePolygon(Vector3* pkPolgyon,Plane* pkPlane,Vector3* pkLinePos1,Vector3* pkLinePos2,Vector3* pkColPos);
		
		//void GenerateModelMatrix();
		//void GenerateModelMatrix(P_Tcs* pkMesh);		

		
		
	public:		
		
		Tcs();
		~Tcs();
		
		void Update(float fAlphaTime);
		
		void AddBody(P_Tcs* pkPTcs);
		void RemoveBody(P_Tcs* pkPTcs);		

		int GetNrOfCollissions() {return m_iNrOfCollissions;};

		bool StartUp();
		bool ShutDown();
		bool IsValid();

};



#endif






